#pragma once

#include "precomp.h"
#include "arch/hardware.h"

class CHardwareManager : public IHardwareManager {
  DECLARE_NOCOPY(CHardwareManager);

 public:
  CHardwareManager() {}

  struct HARDWAREPARAMS {
    unsigned char nPrinterPorts:2;
    unsigned char fInternalModem:1;
    unsigned char fGameAdapter:1;
    unsigned char nSerialPorts:3;
    unsigned char fDMAInstalled:1;
    unsigned char nDisketteDrives:3;
    enum { IVM_UNUSED=0, IVM_4025COLOUR, IVM_8025COLOUR, IVM_8025MONOCHROME } eInitialVideoMode;
    unsigned char fMathCoprocessor:1;
    unsigned char fIPSLDisketteInstalled:1;
  } hardwareParams;

  KRESULT Init();
  void Report();
 
};
