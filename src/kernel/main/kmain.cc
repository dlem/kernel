#include "precomp.h"
#include "global.h"
#include "kstdlib.h"
#include "arch/clock.h"

void kmain()
{
  IClock *pClock = Global::pGClock;
  cls();
  size_t seconds = -1;
  for(;;)
  {
    if(pClock->GetSeconds() != seconds)
    {
      seconds = pClock->GetSeconds();
      if(5 == seconds) { break; }
      cls();
      kprintf("%d\n", seconds);
    }
  }
}
