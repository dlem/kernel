#include "precomp.h"
#include "pit.h"
#include "x86/system.h"

KRESULT PIT::SetCounter
(
  BYTE bCounter,
  BYTE bMode,
  uint16_t iFreqHz
)
{
  // too lazy to check arguments - might be safer to use enums
  const size_t iCount = PIT_CLK_RATE / iFreqHz;

  if(iCount > 0xFFFF) return E_INVALIDARG; // frequency was too small
  if(iCount < 1) return E_INVALIDARG; // frequency was too big

  BYTE counterPort = NULL;
  switch(bCounter)
  {
  case PIT_OCW_COUNTER_0:
    counterPort = PIT_PORT_C0;
    break;
  case PIT_OCW_COUNTER_1:
    counterPort = PIT_PORT_C1;
    break;
  case PIT_OCW_COUNTER_2:
    counterPort = PIT_PORT_C2;
    break;
  default:
    return E_INVALIDARG;
    break;
  }

  const BYTE bCW = bCounter|bMode|PIT_OCW_RDMODE_LSBMSB;

  // send control word BYTE
  outportb(bCW, PIT_PORT_CW);

  // send LSB then MSB of 16-bit count
  outportb(iCount & 0xFF, counterPort);
  outportb((iCount >> 8) & 0xFF, counterPort);

  return K_OK;
}

uint16_t PIT::ReadCounter(BYTE bCounter)
{
  BYTE counterPort = NULL;
  switch(bCounter)
  {
  case PIT_OCW_COUNTER_0:
    counterPort = PIT_PORT_C0;
    break;
  case PIT_OCW_COUNTER_1:
    counterPort = PIT_PORT_C1;
    break;
  case PIT_OCW_COUNTER_2:
    counterPort = PIT_PORT_C2;
    break;
  default:
    DASSERT(false);
    return 0;
    break;
  }

  uint16_t iCount = 0;
  const BYTE bCW = bCounter|PIT_OCW_RDMODE_ICR;

  // send control word
  outportb(bCW, counterPort);

  // get LSB then MSB (assumes it's already been programmed by us)
  iCount |= (uint16_t)inportb(counterPort);
  iCount |= ((uint16_t)inportb(counterPort)) << 8;

  return iCount;
}
