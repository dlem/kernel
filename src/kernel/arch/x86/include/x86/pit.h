/**
 * Stuff related to the programmable interrupt timer. The PIT is generally used
 * as a system clock.
 *
 * http://download.intel.com/design/archives/periphrl/docs/23124406.pdf
 * http://www.brokenthorn.com/Resources/OSDevPit.html
 */


#pragma once

#include "precomp.h"

#define PIT_PORT_C0                 0x40    // port for counter 0
#define PIT_PORT_C1                 0x41    // port for counter 1
#define PIT_PORT_C2                 0x42    // ...
#define PIT_PORT_CW                 0x43    // port for control word

#define PIT_CLK_RATE                1193182

#define PIT_OCW_BCD                 0x01    // binary code decimal, vs binary

#define PIT_OCW_OPMODE_INTORTERM    0x00    // interrupt or terminal count
#define PIT_OCW_OPMODE_ONESHOT      0x02    // programmable one-shot
#define PIT_OCW_OPMODE_RATEGEN      0x04    // rate generator
#define PIT_OCW_OPMODE_SQUAREWAVE   0x06    // square wave generator
#define PIT_OCW_OPMODE_SOFTSTROBE   0x08    // software triggered strobe
#define PIT_OCW_OPMODE_HARDSTROBE   0x0a    // hardware triggered strobe

#define PIT_OCW_RDMODE_ICR          0x00    // latched into internal control register at time of write
#define PIT_OCW_RDMODE_LSB          0x10    // read least significant byte only
#define PIT_OCW_RDMODE_MSB          0x20    // read most significant byte only
#define PIT_OCW_RDMODE_LSBMSB       0x30    // read least then most significant byte

#define PIT_OCW_COUNTER_0           0x00
#define PIT_OCW_COUNTER_1           0x40
#define PIT_OCW_COUNTER_2           0x80

class PIT {

public:
  static KRESULT SetCounter(BYTE bCounter, BYTE bMode, uint16_t iFreqHz);
  static uint16_t ReadCounter(BYTE bCounter);
};
