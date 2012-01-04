#include "precomp.h"
#include "kstdlib.h"
#include "x86/system.h"
#include "x86/pic.h"

struct IDT_ACCESS {
  uint8_t signature:5;    // always 01110
  uint8_t dpl:2;          // highest ring allowed to use interrupt
  uint8_t isPresent:1;    // does routine exist?
} __attribute__((packed));


struct IDT_ENTRY {
  uint16_t baseLo;    // lower half of routine address
  uint16_t selector;  // code segment to use
  uint8_t zero;       // always zero
  BYTE access; // access BYTE
  uint16_t baseHi;
} __attribute__((packed));

struct IDT_PTR {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

class IDT
{
  IDT_ENTRY table[256]; 
  IDT_PTR ptr;

 public:

  KRESULT Init();
  void Register(int iInterrupt, uint32_t addr);
  void Load();
};

#define DECLARE_ISR(n) extern "C" void _isr##n();
DECLARE_ISR(0);
DECLARE_ISR(1);
DECLARE_ISR(2);
DECLARE_ISR(3);
DECLARE_ISR(4);
DECLARE_ISR(5);
DECLARE_ISR(6);
DECLARE_ISR(7);
DECLARE_ISR(8);
DECLARE_ISR(9);
DECLARE_ISR(10);
DECLARE_ISR(11);
DECLARE_ISR(12);
DECLARE_ISR(13);
DECLARE_ISR(14);
DECLARE_ISR(15);
DECLARE_ISR(16);
DECLARE_ISR(17);
DECLARE_ISR(18);
DECLARE_ISR(19);
DECLARE_ISR(20);
DECLARE_ISR(21);
DECLARE_ISR(22);
DECLARE_ISR(23);
DECLARE_ISR(24);
DECLARE_ISR(25);
DECLARE_ISR(26);
DECLARE_ISR(27);
DECLARE_ISR(28);
DECLARE_ISR(29);
DECLARE_ISR(30);
DECLARE_ISR(31);
DECLARE_ISR(32);
DECLARE_ISR(33);
DECLARE_ISR(34);
DECLARE_ISR(35);
DECLARE_ISR(36);
DECLARE_ISR(37);
DECLARE_ISR(38);
DECLARE_ISR(39);
DECLARE_ISR(40);
DECLARE_ISR(41);
DECLARE_ISR(42);
DECLARE_ISR(43);
DECLARE_ISR(44);
DECLARE_ISR(45);
DECLARE_ISR(46);
DECLARE_ISR(47);

CASSERT(sizeof(IDT_ACCESS) == sizeof(uint8_t));
CASSERT(sizeof(IDT_ENTRY) == 8);

extern "C" __cdecl void asm_load_idt(IDT_PTR ptr); // in idt.asm

KRESULT IDT::Init()
{
  DASSERT(NUMELMS(table) == 256);
  for(int ix = 0; ix < NUMELMS(table); ix++)
  {
    const IDT_ENTRY nullEntry = {0};
    table[ix] = nullEntry;
    table[ix].access = 0x0E;
  }
  ptr.base = (uint32_t)table;
  ptr.limit = sizeof(table) - 1;

  return K_OK;
}

void IDT::Register(int iInterrupt, uint32_t addr)
{
  DASSERT(0 <= iInterrupt && iInterrupt < NUMELMS(table));
  const uint16_t baseHi = (uint16_t)((addr >> 16) & 0xFFFF);
  const uint16_t baseLo = (uint16_t)((addr) & 0xFFFF);
  IDT_ENTRY &entry = table[iInterrupt];
  entry.baseLo = baseLo;
  entry.baseHi = baseHi;
  entry.selector = 8;
  entry.zero = 0;
  entry.access = 0x8E;
}

__cdecl void test(IDT_PTR ptr)
{
  (void)ptr;
}
void IDT::Load()
{
  test(ptr);
  asm_load_idt(ptr);
}

IDT Idt;

#define IDT_REG_ISR(t, n) t.Register(n, (uint32_t)_isr##n)
#define IDT_REG_IRQ(t, n) IDT_REG_ISR(t, n)
KRESULT InitIDT()
{
  DASSERT(!InterruptsEnabled());
  RETURN_FAILED(Idt.Init());

  PIC::Remap((BYTE)0x20, (BYTE)0x28, (BYTE)2);


  IDT_REG_ISR(Idt, 0);
  IDT_REG_ISR(Idt, 1);
  IDT_REG_ISR(Idt, 2);
  IDT_REG_ISR(Idt, 3);
  IDT_REG_ISR(Idt, 4);
  IDT_REG_ISR(Idt, 5);
  IDT_REG_ISR(Idt, 6);
  IDT_REG_ISR(Idt, 7);
  IDT_REG_ISR(Idt, 8);
  IDT_REG_ISR(Idt, 9);
  IDT_REG_ISR(Idt, 10);
  IDT_REG_ISR(Idt, 11);
  IDT_REG_ISR(Idt, 12);
  IDT_REG_ISR(Idt, 13);
  IDT_REG_ISR(Idt, 14);
  IDT_REG_ISR(Idt, 15);
  IDT_REG_ISR(Idt, 16);
  IDT_REG_ISR(Idt, 17);
  IDT_REG_ISR(Idt, 18);
  IDT_REG_ISR(Idt, 19);
  IDT_REG_ISR(Idt, 20);
  IDT_REG_ISR(Idt, 21);
  IDT_REG_ISR(Idt, 22);
  IDT_REG_ISR(Idt, 23);
  IDT_REG_ISR(Idt, 24);
  IDT_REG_ISR(Idt, 25);
  IDT_REG_ISR(Idt, 26);
  IDT_REG_ISR(Idt, 27);
  IDT_REG_ISR(Idt, 28);
  IDT_REG_ISR(Idt, 29);
  IDT_REG_ISR(Idt, 30);
  IDT_REG_ISR(Idt, 31);

  IDT_REG_IRQ(Idt, 32);
  IDT_REG_IRQ(Idt, 33);
  IDT_REG_IRQ(Idt, 34);
  IDT_REG_IRQ(Idt, 35);
  IDT_REG_IRQ(Idt, 36);
  IDT_REG_IRQ(Idt, 37);
  IDT_REG_IRQ(Idt, 38);
  IDT_REG_IRQ(Idt, 39);
  IDT_REG_IRQ(Idt, 40);
  IDT_REG_IRQ(Idt, 41);
  IDT_REG_IRQ(Idt, 42);
  IDT_REG_IRQ(Idt, 43);
  IDT_REG_IRQ(Idt, 44);
  IDT_REG_IRQ(Idt, 45);
  IDT_REG_IRQ(Idt, 46);
  IDT_REG_IRQ(Idt, 47);

  Idt.Load();

  return K_OK;
}
