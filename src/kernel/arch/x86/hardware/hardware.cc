#include "precomp.h"
#include "x86/hardware.h"
#include "kstdlib.h"
#include "kb.h"


KRESULT CHardwareManager::Init()
{
  KRESULT kr = K_OK;
  uint16_t iEquipmentList = 0;

  hardwareParams.fIPSLDisketteInstalled = iEquipmentList & 0x1;
  hardwareParams.fMathCoprocessor = (iEquipmentList >> 1) & 0x1;
  // not sure about the next two bits
  
  // better way to do this?
#if 0
  switch ((iEquipmentList >> 4) & 0x11)
  {
    case 0:
      hardwareParams.eInitialVideoMode = HARDWAREPARAMS::IVM_UNUSED;
      break;
    case 1:
      hardwareParams.eInitialVideoMode = HARDWAREPARAMS::IVM_4025COLOUR;
      break;
    case 2:
      hardwareParams.eInitialVideoMode = HARDWAREPARAMS::IVM_8025COLOUR;
      break;
    case 3:
      hardwareParams.eInitialVideoMode = HARDWAREPARAMS::IVM_8025MONOCHROME;
      break;
    default:
      DASSERT(false);
      break;
   }
#else
  hardwareParams.eInitialVideoMode = HARDWAREPARAMS::IVM_8025COLOUR;
#endif

  //hardwareParams.eInitialVideoMode = (iEquipmentList >> 4) & 0x11;
  hardwareParams.nDisketteDrives = ((iEquipmentList >> 6) & 0x11) + 1;
  hardwareParams.fDMAInstalled = !((iEquipmentList >> 8) & 0x1);
  hardwareParams.nSerialPorts = (iEquipmentList >> 9) & 0x111;
  hardwareParams.fGameAdapter = (iEquipmentList >> 12) & 0x1;
  hardwareParams.fInternalModem = (iEquipmentList >> 13) & 0x1;
  hardwareParams.nPrinterPorts = (iEquipmentList >> 14) & 0x11;

  return kr;
}

void CHardwareManager::Report()
{
  const char *rgBools[] = {"yes", "no"};
  const char *rgVideoModes[] = {"unused", "4025 colour", "8025 colour", "8025 monochrome"};

  kprintf("IPSL diskette installed: %s\n", rgBools[hardwareParams.fIPSLDisketteInstalled]);
  kprintf("Math coprocessor: %s\n", rgBools[hardwareParams.fMathCoprocessor]);
  kprintf("Video mode: %s\n", rgVideoModes[hardwareParams.eInitialVideoMode]);
  kprintf("# diskette drives: %d\n", hardwareParams.nDisketteDrives);
  kprintf("DMA installed: %s\n", rgBools[hardwareParams.fDMAInstalled]);
  kprintf("# serial ports: %d\n", hardwareParams.nSerialPorts);
  kprintf("Game adapter: %s\n", rgBools[hardwareParams.fGameAdapter]);
  kprintf("Internal modem: %s\n", rgBools[hardwareParams.fInternalModem]);
  kprintf("# printer ports: %d\n", hardwareParams.nPrinterPorts);

}
