#include "precomp.h"
#include "global.h"
#include "mm.h"

#include "x86/system.h"
#include "x86/multiboot.h"
#include "x86/pit.h"
#include "x86/isr.h"
#include "x86/clock.h"
#include "x86/hardware.h"
#include "x86/console.h"

extern "C" __cdecl int32_t asm_get_eflags(); // used by InterruptsEnabled()
extern int os_image_start;
extern int os_image_end; // Defined by the linker script.

KRESULT FreeInitialPages(const MultibootInfo &pInfo)
{
  uint32_t iMap;
  uint32_t mapLen;
  uint32_t iModCount;
  uint32_t _pModAddrs;
  bool fMods;
  RETURN_FAILED(pInfo.get_mmap_addr(&iMap));
  RETURN_FAILED(pInfo.get_mmap_length(&mapLen));

  fMods = SUCCEEDED(pInfo.get_mods_count(&iModCount));
  if(fMods)
  {
    RETURN_FAILED(pInfo.get_mods_addr(&_pModAddrs));
  }
  else
  {
    iModCount = 0;
  }

  uint64_t missed = 0;
  uint64_t wrongType = 0;
  uint64_t totalBytes = 0;
  uint64_t pagesAdded = 0;

  // because of the way the system works, we can just free the pages to register
  // them. Here I'm looping through the MULTIBOOT memmap structs and freeing
  // the corresponding pages one by one
  for(const MULTIBOOT_MMAPNODE *pMap = (const MULTIBOOT_MMAPNODE *)iMap; pMap; pMap = pMap->GetNext(iMap, mapLen))
  {
    totalBytes += pMap->length;

    if(pMap->type == MULTIBOOT_MMAPTYPE_AVAILABLE && pMap->base_addr) // Don't use addrs smaller than 1MB, they can
    {
      addr_t pageAddr = MM_ALIGN(pMap->base_addr, MM_PAGESIZE);
      DASSERT((uint64_t)pageAddr >= pMap->base_addr); // overflow?

      if(pMap->length > (uint64_t)pageAddr - pMap->base_addr)
      {
        missed += (uint64_t)pageAddr - pMap->base_addr;
        missed += (pMap->base_addr + pMap->length) - MM_PAGEADDR(pMap->base_addr + pMap->length);
      }
      else
      {
        missed += pMap->length;
      }
      uint64_t segOffset = (uint64_t)(pageAddr - pMap->base_addr);
      for(; segOffset <= pMap->length - MM_PAGESIZE; segOffset += MM_PAGESIZE)
      {
        if(pMap->base_addr + segOffset < 0x100000) continue; // Don't mess with memory below 1MB
        FreeKPage((void *)(pMap->base_addr + segOffset));
        pagesAdded += 1;
      }
    }
    else
    {
      wrongType += pMap->length;
    }
  }

  // Free all pages that the kernel is loaded into.
  // The relevant numbers are defined by the linker script.
  uint64_t iKernelPages = 0;
  for(addr_t addrPage = MM_PAGEADDR(os_image_start); addrPage <= MM_PAGEADDR(os_image_end); addrPage += MM_PAGESIZE)
  {
    VERIFY(SUCCEEDED(ClaimKPage((void *)addrPage)));
    iKernelPages += 1;
  }

  uint64_t iModPages = 0;

  MULTIBOOT_MOD *pModAddrs = (MULTIBOOT_MOD *)_pModAddrs;
  // Now I need to free all pages that kernel modules are loaded into.
  for(uint32_t ixMod = 0; ixMod < iModCount; ixMod++)
  {
    for(addr_t addrPage = (addr_t)MM_PAGEADDR(pModAddrs[ixMod].mod_start); addrPage <= pModAddrs[ixMod].mod_end; addrPage += MM_PAGESIZE)
    {
      VERIFY(SUCCEEDED(ClaimKPage((void *)addrPage)));
      iModPages += 1;
    }
  }

  // This takes a while, but I think it's worth it for now.
  DASSERT(pagesAdded - iModPages == CountPages());

  return K_OK;
}


/**
 * This function should perform all of the low-level initialization that must
 * be done before the kernel can start acting like a normal program. It will be
 * be called very early on, when there's still only one thread.
**/
KRESULT Arch_Init(void *pInfo)
{
  DASSERT(!(Global::pGHardwareManager || Global::pGClock || Global::pGConsole));
  RETURN_FAILED(InitMM());
  RETURN_FAILED(FreeInitialPages(MultibootInfo(pInfo)));

  CHardwareManager *pHardwareManager = KNew CHardwareManager;
  CDMAConsole *pConsole = KNew CDMAConsole;
  Clock *pClock = KNew Clock;

  KRESULT kr = K_OK;
  if(!(pHardwareManager && pConsole && pClock)) kr = E_OUTOFMEMORY;

  if(SUCCEEDED(kr))
  {
    kr = pHardwareManager->Init();
    if(SUCCEEDED(kr))
    {
      switch(pHardwareManager->hardwareParams.eInitialVideoMode)
      {
        case CHardwareManager::HARDWAREPARAMS::IVM_4025COLOUR:
          kr = pConsole->Init((void *)0xB8000, 40, 25);
          break;
        case CHardwareManager::HARDWAREPARAMS::IVM_8025COLOUR:
          kr = pConsole->Init((void *)0xB8000, 80, 25);
          break;
        case CHardwareManager::HARDWAREPARAMS::IVM_8025MONOCHROME:
          kr = pConsole->Init((void *)0xB000, 80, 25);
          break;
        case CHardwareManager::HARDWAREPARAMS::IVM_UNUSED:
          kr = pConsole->Init((void *)0xB8000, 80, 25);
          break;
        default:
          DASSERT(false);
          kr = E_UNEXPECTED;
          break;
      }
    }

    if(SUCCEEDED(kr))
    {
      DASSERT(!InterruptsEnabled()); // now it's safer to do things like this...
      kr = InitIDT();
    }

    if(SUCCEEDED(kr))
    {
      const int nTicksPerSecond = 100;
      kr = PIT::SetCounter(PIT_OCW_COUNTER_0, PIT_OCW_OPMODE_SQUAREWAVE, (uint16_t)nTicksPerSecond);
      if(SUCCEEDED(kr)) kr = ISR::Init();
      if(SUCCEEDED(kr)) kr = pClock->Init((size_t)nTicksPerSecond);
    }
  }

  if(SUCCEEDED(kr))
  {
    Global::pGConsole = pConsole;
    Global::pGClock = pClock;
    Global::pGHardwareManager = pHardwareManager;
    STI();
  }
  else
  {
    if(pHardwareManager) KDelete pHardwareManager;
    if(pConsole) KDelete pConsole;
    if(pClock) KDelete pClock;
  }
  return kr;
}
void Arch_Shutdown()
{
  if(Global::pGHardwareManager) KDelete Global::pGHardwareManager;
  if(Global::pGConsole) KDelete Global::pGConsole;
  if(Global::pGClock) KDelete Global::pGClock;
  Global::pGHardwareManager = NULL;
  Global::pGConsole = NULL;
  Global::pGClock = NULL;
}
bool SPL(bool pl)
{
  bool plPrev = InterruptsEnabled();
  if(pl & (!plPrev))
  {
    STI();
  }
  else if((!pl) & plPrev)
  {
    CLI();
  }
  return plPrev;
}
bool SPLLo()
{
  bool plPrev = InterruptsEnabled();
  if(!plPrev)
  {
    STI();
  }
  return plPrev;
}
bool SPLHi()
{
  bool plPrev = InterruptsEnabled();
  if(plPrev)
  {
    CLI();
  }
  return plPrev;
}
bool InterruptsEnabled()
{
  return asm_get_eflags() & 0x00000100; // IF is 9th bit
}

