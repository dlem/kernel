#include <main/kmain.h>
#include "system.h"

extern "C" __cdecl void kentry(void *pMultibootInfo)
{
  KRESULT kr = Arch_Init(pMultibootInfo); // do uber-important initialization stuff
  if(FAILED(kr)) panic("Low-level system initialization failed");
  kmain();
  Arch_Shutdown();
}
