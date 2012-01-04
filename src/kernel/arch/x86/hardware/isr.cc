#include "precomp.h"
#include "kstdlib.h"

#include "x86/system.h"
#include "x86/pic.h"
#include "x86/intno.h"
#include "x86/handlers.h"
#include "x86/isr.h"

const char *ISR::intNames[INTERRUPT_COUNT] = {NULL};
KRESULT (*ISR::intHandlers[INTERRUPT_COUNT])(STACKFRAME *) = {NULL};

const char *ISR::GetInterruptName(int intNo)
{
  const char *undef = "undefined interrupt";
  if(intNo < 0 || intNo > 255)
  {
    DASSERT(false);
    return undef;
  }
  if(intNo >= NUMELMS(intNames)) return undef; // no string for it
  return intNames[intNo] ? intNames[intNo] : undef;
}

const void *ISR::GetInterruptHandler(int intNo)
{
  if(intNo < 0 || intNo >= 256)
  {
    DASSERT(false);
    return NULL;
  }
  if(intNo >= NUMELMS(intHandlers)) return NULL;
  return (void *)intHandlers[intNo];
}

KRESULT ISR::Init()
{
#define DEFINE_INTERRUPT(intNo, intName, intHandler) \
  DASSERT(intNo >= 0);\
  DASSERT(intNo < INTERRUPT_COUNT);\
  intHandlers[intNo] = intHandler;\
  intNames[intNo] = intName;\

  DEFINE_INTERRUPT(INT_DIVISIONBYZERO,            "division by zero exception",             NULL);
  DEFINE_INTERRUPT(INT_DEBUG,                     "debug exception",                        NULL);
  DEFINE_INTERRUPT(INT_NONMASKABLEINTERRUPT,      "not maskable interrupt exception",       NULL);
  DEFINE_INTERRUPT(INT_BREAKPOINT,                "breakpoint exception",                   NULL);
  DEFINE_INTERRUPT(INT_OVERFLOW,                  "overflow exception",                     NULL);
  DEFINE_INTERRUPT(INT_OUTOFBOUNDS,               "out of bounds exception",                NULL);
  DEFINE_INTERRUPT(INT_INVALIDOPCODE,             "invalid opcode exception",               NULL);
  DEFINE_INTERRUPT(INT_NOCOPROCESSOR,             "no coprocessor exception",               NULL);
  DEFINE_INTERRUPT(INT_DOUBLEFAULT,               "double fault exception",                 NULL);
  DEFINE_INTERRUPT(INT_COPROCESSORSEGMENTOVERRUN, "coprocessor segment overrun exception",  NULL);
  DEFINE_INTERRUPT(INT_BADTSS,                    "bad TSS exception",                      NULL);
  DEFINE_INTERRUPT(INT_SEGMENTNOTPRESENT,         "segment not present exception",          NULL);
  DEFINE_INTERRUPT(INT_STACKFAULT,                "stack fault exception",                  NULL);
  DEFINE_INTERRUPT(INT_GENERALPROTECTIONFAULT,    "general protection fault",               NULL);
  DEFINE_INTERRUPT(INT_PAGEFAULT,                 "page fault",                             NULL);
  DEFINE_INTERRUPT(INT_UNKNOWNINTERRUPT,          "unknown interrupt exception",            NULL);
  DEFINE_INTERRUPT(INT_COPROCESSORFAULT,          "coprocessor fault",                      NULL);
  DEFINE_INTERRUPT(INT_ALIGNMENTCHECK,            "alignment check exception",              NULL);
  DEFINE_INTERRUPT(INT_MACHINECHECK,              "machine check exception",                NULL);

  DEFINE_INTERRUPT(IRQ_TIMER,                     "timer IRQ",                              Handler_Timer);
  DEFINE_INTERRUPT(IRQ_KEYBOARD,                  "keyboard IRQ",                           NULL);
  DEFINE_INTERRUPT(IRQ_REDIRECTED,                "redirected IRQ",                         NULL);
  DEFINE_INTERRUPT(IRQ_SERIALCOMMS0,              "serial comms 2 and 4 IRQ",               NULL);
  DEFINE_INTERRUPT(IRQ_SERIALCOMMS1,              "serial comms 1 and 3 IRQ",               NULL);
  DEFINE_INTERRUPT(IRQ_SOUNDCARD,                 "sound card IRQ",                         NULL);
  DEFINE_INTERRUPT(IRQ_FLOPPYCONTROLLER,          "floppy controller IRQ",                  NULL);
  DEFINE_INTERRUPT(IRQ_PARALLELCOMMS,             "parallel comms IRQ",                     NULL);
  DEFINE_INTERRUPT(IRQ_REALTIMECLOCK,             "realtime clock IRQ",                     NULL);
  DEFINE_INTERRUPT(IRQ_REDIRECTEDIRQ2,            "redirected IRQ 2 IRQ",                   NULL);
  DEFINE_INTERRUPT(IRQ_PS2MOUSE,                  "PS/2 mouse IRQ",                         NULL);
  DEFINE_INTERRUPT(IRQ_MATHCOPROCESSOR,           "math coprocessor IRQ",                   NULL);
  DEFINE_INTERRUPT(IRQ_HARDDISK,                  "hard disk drive IRQ",                    NULL);

  return K_OK;

#undef DEFINE_INTERRUPT
}

/**
 * This is the general interrupt handler routine called by the stub in isr.asm.
**/
extern "C" __cdecl void _fault_handler(STACKFRAME *psf)
{
  KRESULT (*intHandler)(STACKFRAME *) = (KRESULT (*)(STACKFRAME *))ISR::GetInterruptHandler(psf->int_no);
  const char *intName = ISR::GetInterruptName(psf->int_no);
  bool handled = false;
  KRESULT kr = K_OK;

  if(NULL == intHandler)
  {
    kprintf("No handler for interrupt number %d: %s\n", psf->int_no, intName);
  }
  else
  {
    handled = true;
    kr = intHandler(psf);
  }

  const bool isISR = (psf->int_no >= 32) & (psf->int_no <= 47);
  if(!handled || FAILED(kr))
  {
    if(!isISR)
    {
      if(psf->cs == KERNEL_CODE_SEGMENT) // exception happened in kernel
      {
        char buf[256];
        if(SUCCEEDED(sprintfn(buf, NUMELMS(buf), "Fatal exception %d in kernel: %s", psf->int_no, intName)))
        {
          panic(buf);
        }
        else
        {
          panic("Fatal exception in kernel"); // I'm being extra safe here
        }
      }
      else
      {
        // TODO: kill user program
      }
    }
    else // isISR
    {
      // an isr failed - not good, but possibly not the end of the world
    }
  }

  // if this was an IRQ, we need to tell the PIC that it's been handled
  if(isISR)
  {
    PIC_IRQ_T irq = (PIC_IRQ_T)(psf->int_no - 32);
    PIC::EOI(irq);
  }
}
