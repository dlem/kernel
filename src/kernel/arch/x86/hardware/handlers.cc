#include "precomp.h"
#include "x86/clock.h"
#include "x86/system.h"
#include "global.h"

KRESULT Handler_Timer(STACKFRAME *pFrame)
{
  static_cast<Clock *>(Global::pGClock)->Tick();
  return K_OK;
}
