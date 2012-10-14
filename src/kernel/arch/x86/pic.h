/**
 * Stuff related to the Programmable Interrupt Controller.
 *
 * See http://www.brokenthorn.com/Resources/OSDevPic.html
 */

#pragma once

enum PIC_IRQ_T {
  PIC_IRQ_TIMER=0,
  PIC_IRQ_KEYBOARD,
  PIC_IRQ_SLAVE,
  PIC_IRQ_SERIALPORT2,
  PIC_IRQ_SERIALPORT1,
  PIC_IRQ_PARALLELPORT2,
  PIC_IRQ_DISKETTE,
  PIC_IRQ_PARALLELPORT1,
  PIC_IRQ_CMOSREALTIMECLOCK,
  PIC_IRQ_CGAVERTICALRETRACE,
  PIC_IRQ_RESERVED1,
  PIC_IRQ_RESERVED2,
  PIC_IRQ_RESERVED3,
  PIC_IRQ_FPU,
  PIC_IRQ_HARDDISKCONTROLLER,
  PIC_IRQ_RESERVED,

  PIC_IRQ_COUNT
};

#define PIC1_COMMAND_AND_STATUS ((BYTE)0x20)
#define PIC1_MASK_AND_DATA      ((BYTE)0x21)
#define PIC2_COMMAND_AND_STATUS ((BYTE)0xA0)
#define PIC2_MASK_AND_DATA      ((BYTE)0xA1)

// ICW1:
#define PIC_ICW1_IC4            0x01  // if it should expect ICW4
#define PIC_ICW1_SNGL           0x02  // if it's a single PIC (vs cascaded)
#define PIC_ICW1_ADI            0x04  // if call address interval is 4 vs 8; usually ignored
#define PIC_ICW1_LTIM           0x08  // level triggered mode (vs edge triggered mode)
#define PIC_ICW1_INITIALIZE     0x10  // is it being initialized?
// the remaining bits of the BYTE are ignored for x86

// ICW2: gives the interrupt number at which to start the IRQs for each PIC

// ICW3: gives the IRQ line to use for master-slave communication
// it's mask format for PIC1, and binary format for PIC2

// ICW4:
#define PIC_ICW4_UPM            0x01  // if it's x86 mode (vs MCS-80/86)
#define PIC_ICW4_AEOI           0x02  // automatically do EOI on last acknowledge pulse
#define PIC_ICW4_MS             0x04  // if it's buffer master (only use if BUF is set!)
#define PIC_ICW4_BUF            0x08  // operate in buffered mode
#define PIC_ICW4_SFNM           0x10  // special fully nested mode (large amount of controllers?)
// the rest are reserved; must be zero

// OCW2:
// first three bits determine interrupt level upon which it must react
// next two must be zero, reserved
#define PIC_OCW2_EOI            0x20  // if it's an EOI request
#define PIC_OCW2_SELECTION      0x80  // not sure
#define PIC_OCW2_ROTATION       0xF0  // not sure



class PIC {

public:
  static void EOI(PIC_IRQ_T irq);
  static bool IsIRQEnabled(PIC_IRQ_T irq);
  static void EnableIRQ(PIC_IRQ_T irq);
  static void DisableIRQ(PIC_IRQ_T irq);
  static void Remap(BYTE iP1Remap, BYTE iP2Remap, BYTE bSlaveComm);

};
