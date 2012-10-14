#pragma once

#define TSS_TYPE_NOBUSY (0x9)
#define TSS_TYPE_BUSY   (0xB)

struct TSS_DESCRIPTOR {
  uint16_t limit_low;
  uint16_t base_low;
  BYTE base_mid;
  BYTE type:4;          // For TSS, 10B1 where B = busy flag (running or suspended)
  BYTE zero1:1;
  BYTE dpl:2;           // Descriptor privilege level.
  BYTE present:1;
  BYTE limit_high:4;
  BYTE avl:1;           // Available for use by system software.
  BYTE zero2:2;
  BYTE granularity:1;
  BYTE base_high;

  void Init(uint32_t iBase, uint32_t iLimit, bool fBusy)
  {
    StructZero(this);

    base_low = (uint16_t)(iBase & 0xFFFF);
    base_mid = (BYTE)((iBase >> 16) & 0xFF);
    base_high = (BYTE)((iBase >> 24) & 0xFF);

    limit_low = (uint16_t)(iLimit & 0xFFFF);
    limit_high = (BYTE)((iLimit >> 16) & 0xF);
    DASSERT(0 == (iLimit >> 20)); // The limit should have at most 20 bits.

    type = fBusy ? TSS_TYPE_BUSY : TSS_TYPE_NOBUSY;
  }
} __packed;
