#pragma once

class IHardwareManager {
public:
  virtual ~IHardwareManager() {}
  virtual void Report() = 0;
};
