#pragma once

class IConsole;
class IHardwareManager;
class IClock;
class CBaseScheduler;

namespace Global {
  // global objects
  extern IConsole *pGConsole;
  extern IHardwareManager *pGHardwareManager;
  extern IClock *pGClock;
  extern CBaseScheduler *pGScheduler;

  // global flags
  extern bool Inited_mm;
}
