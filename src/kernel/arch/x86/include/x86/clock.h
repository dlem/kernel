#pragma once

#include "precomp.h"
#include "arch/clock.h"

class Clock : public IClock {
  DECLARE_NOCOPY(Clock);

  size_t m_nTicksPerSecond;
  size_t m_seconds;
  size_t m_ticks;

public:
  Clock() {}
  KRESULT Init(size_t nTicksPerSecond);
  size_t GetSeconds();
  size_t GetMilliseconds();
  size_t GetPrecision();
  void Tick();
};
