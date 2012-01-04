#include "precomp.h"
#include "global.h"

namespace Global {
  // global objects
  IConsole *pGConsole = NULL;
  IHardwareManager *pGHardwareManager = NULL;
  IClock *pGClock = NULL;
  CBaseScheduler *pGScheduler = NULL;

  // global flags
  bool Inited_mm = false;
}
