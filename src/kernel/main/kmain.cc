#include <main/global.h>
#include <main/kstdlib.h>
#include <arch/clock.h>

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
      cls();
      kprintf("%d\n", seconds);
    }
  }
}
