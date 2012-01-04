#include "precomp.h"

class IHardwareManager {
public:
  virtual ~IHardwareManager() {}
  virtual void Report() = 0;
};
