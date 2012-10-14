#pragma once

#include "intno.h" // INTERRUPT_COUNT

class ISR {
  static const char *intNames[INTERRUPT_COUNT];
  static KRESULT (*intHandlers[INTERRUPT_COUNT])(STACKFRAME *);

public:
  static KRESULT Init();
  static const char *GetInterruptName(int intNo);
  static const void *GetInterruptHandler(int intNo);
};
