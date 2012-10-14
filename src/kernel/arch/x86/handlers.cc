#include "clock.h"
#include "system.h"
#include <main/global.h>

KRESULT Handler_Timer(STACKFRAME *pFrame)
{
  static_cast<Clock *>(Global::pGClock)->Tick();
  return K_OK;
}
