#pragma once

#include "precomp.h"

class IClock {
public:
  virtual ~IClock() {}
  virtual size_t GetSeconds() = 0;
  virtual size_t GetMilliseconds() = 0;
  virtual size_t GetPrecision() = 0;
};
