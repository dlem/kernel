#pragma once

#include "precomp.h"

class SpinLock {
  DECLARE_NOCOPY(SpinLock);
public:
  SpinLock(unsigned int iVal=1);
  void Acquire();
  bool TryAcquire();
  void Release();
};

class OwnerLock {
  DECLARE_NOCOPY(OwnerLock);
public:
  OwnerLock();
  unsigned int Times() const;
  void Acquire();
  bool TryAcquire();
  void Release();
};

class CondLock {
  DECLARE_NOCOPY(CondLock);
public:
  CondLock();
  bool Empty();
  void Wait(OwnerLock &lk);
  void Signal();
  void Broadcast();
};
