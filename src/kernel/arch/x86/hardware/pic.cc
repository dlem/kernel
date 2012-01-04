#include "precomp.h"
#include "x86/pic.h"
#include "x86/system.h"

void PIC::EOI(PIC_IRQ_T irq)
{
  if(irq / 8)
  {
    outportb(PIC_OCW2_EOI, PIC2_COMMAND_AND_STATUS);
  }
  outportb(PIC_OCW2_EOI, PIC1_COMMAND_AND_STATUS);
}

bool PIC::IsIRQEnabled(PIC_IRQ_T irq)
{
  DASSERT(0 <= irq && irq < PIC_IRQ_COUNT);

  BYTE pbPort = irq / 8 ? PIC2_MASK_AND_DATA : PIC1_MASK_AND_DATA;
  const BYTE bMask = 0x1 << (irq % 8);

  const BYTE data = inportb(pbPort);
  return data & bMask;
}

void PIC::EnableIRQ(PIC_IRQ_T irq)
{
  DASSERT(0 <= irq && irq < PIC_IRQ_COUNT);

  BYTE port = irq / 8 ? PIC2_MASK_AND_DATA : PIC1_MASK_AND_DATA;
  const BYTE bMask = 0x1 << (irq % 8);

  const BYTE prev = inportb(port);
  outportb(port, prev & (!bMask));
}

void PIC::DisableIRQ(PIC_IRQ_T irq)
{
  DASSERT(0 <= irq && irq < PIC_IRQ_COUNT);

  BYTE port = irq / 8 ? PIC2_MASK_AND_DATA : PIC1_MASK_AND_DATA;
  const BYTE bMask = 0x1 << (irq % 8);

  const BYTE data = inportb(port);
  outportb(data|bMask, port);
}

void PIC::Remap(BYTE iP1Remap, BYTE iP2Remap, BYTE bSlaveComm)
{
  DASSERT(iP1Remap + 8 <= 256);
  DASSERT(iP2Remap + 8 <= 256);
  DASSERT(bSlaveComm < 8);

  const BYTE icw1 = PIC_ICW1_IC4|PIC_ICW1_INITIALIZE;
  const BYTE bSCMask = 0x1 << bSlaveComm;
  const BYTE icw4 = PIC_ICW4_UPM;

  // send icw1
  outportb(icw1, PIC1_COMMAND_AND_STATUS);
  outportb(icw1, PIC2_COMMAND_AND_STATUS);

  // send icw2
  outportb(iP1Remap, PIC1_MASK_AND_DATA);
  outportb(iP2Remap, PIC2_MASK_AND_DATA);

  // send icw3
  outportb(bSCMask, PIC1_MASK_AND_DATA);
  outportb(bSlaveComm, PIC2_MASK_AND_DATA);

  // send icw4
  outportb(icw4, PIC1_MASK_AND_DATA);
  outportb(icw4, PIC2_MASK_AND_DATA);

  // send null BYTE
  outportb(0, PIC1_MASK_AND_DATA);
  outportb(0, PIC2_MASK_AND_DATA);
}
