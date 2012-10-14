#include "multiboot.h"

struct MultibootInfo::MULTIBOOT_INFO {
  DWORD flags;
  uint32_t mem_lower;     // amount of memory in [0,1M]
  uint32_t mem_upper;     // amount of memory in [1M,], not guaranteed
  uint32_t boot_device;
  uint32_t cmdline;       // address of null-terminated arg for the kernel
  uint32_t mods_count;
  uint32_t mods_addr;
  uint32_t sym1;
  uint32_t sym2;
  uint32_t sym3;
  uint32_t sym4;
  uint32_t mmap_length;
  uint32_t mmap_addr;
  uint32_t drives_length;
  uint32_t drives_addr;
  uint32_t config_table;
  uint32_t boot_loader_name;
  uint32_t apm_table;
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;
} __attribute__((packed));

MultibootInfo::MultibootInfo(void *pInfo) : m_pInfo((MULTIBOOT_INFO *)pInfo) {}

#define MULTIBOOT_FLAG_MEM      0x00000001
#define MULTIBOOT_FLAG_DEVICE   0x00000002
#define MULTIBOOT_FLAG_CMDLINE  0x00000004
#define MULTIBOOT_FLAG_MODS     0x00000008
#define MULTIBOOT_FLAG_SYMAOUT  0x00000010
#define MULTIBOOT_FLAG_SYMELF   0x00000020
#define MULTIBOOT_FLAG_MMAP     0x00000040
#define MULTIBOOT_FLAG_DRIVES   0x00000080
#define MULTIBOOT_FLAG_CONFIG   0x00000100
#define MULTIBOOT_FLAG_LOADER   0x00000200
#define MULTIBOOT_FLAG_APM      0x00000400
#define MULTIBOOT_FLAG_VBE      0x00000800

#define MULTIBOOT_FLAGGED_ACCESSOR(name, type, flag) \
  KRESULT MultibootInfo::get_##name(type *pOut) const\
  {\
    if(!(m_pInfo->flags & MULTIBOOT_FLAG_##flag)) return E_FIELDNOTSET;\
    *pOut = m_pInfo->name;\
    return K_OK;\
  }

MULTIBOOT_FLAGGED_ACCESSOR(mem_lower,         uint32_t, MEM);
MULTIBOOT_FLAGGED_ACCESSOR(mem_upper,         uint32_t, MEM);
MULTIBOOT_FLAGGED_ACCESSOR(boot_device,       uint32_t, DEVICE);
MULTIBOOT_FLAGGED_ACCESSOR(cmdline,           uint32_t, CMDLINE);
MULTIBOOT_FLAGGED_ACCESSOR(mods_count,        uint32_t, MODS);
MULTIBOOT_FLAGGED_ACCESSOR(mods_addr,         uint32_t, MODS);
//MULTIBOOT_FLAGGED_ACCESSOR(sym_tabsize,       uint32_t, SYMAOUT);
//MULTIBOOT_FLAGGED_ACCESSOR(sym_strsize,       uint32_t, SYMAOUT);
//MULTIBOOT_FLAGGED_ACCESSOR(sym_addr,          uint32_t, SYMAOUT);
//MULTIBOOT_FLAGGED_ACCESSOR(sym_num,           uint32_t, SYMELF);
//MULTIBOOT_FLAGGED_ACCESSOR(sym_size,          uint32_t, SYMELF);
//MULTIBOOT_FLAGGED_ACCESSOR(sym_addr,          uint32_t, SYMELF);
//MULTIBOOT_FLAGGED_ACCESSOR(sym_shndx,         uint32_t, SYMELF);
MULTIBOOT_FLAGGED_ACCESSOR(mmap_length,       uint32_t, MMAP);
MULTIBOOT_FLAGGED_ACCESSOR(mmap_addr,         uint32_t, MMAP);
MULTIBOOT_FLAGGED_ACCESSOR(drives_length,     uint32_t, DRIVES);
MULTIBOOT_FLAGGED_ACCESSOR(drives_addr,       uint32_t, DRIVES);
MULTIBOOT_FLAGGED_ACCESSOR(config_table,      uint32_t, CONFIG);
MULTIBOOT_FLAGGED_ACCESSOR(boot_loader_name,  uint32_t, LOADER);
MULTIBOOT_FLAGGED_ACCESSOR(apm_table,         uint32_t, APM);
MULTIBOOT_FLAGGED_ACCESSOR(vbe_control_info,  uint32_t, VBE);
MULTIBOOT_FLAGGED_ACCESSOR(vbe_mode_info,     uint32_t, VBE);
MULTIBOOT_FLAGGED_ACCESSOR(vbe_mode,          uint16_t, VBE);
MULTIBOOT_FLAGGED_ACCESSOR(vbe_interface_seg, uint16_t, VBE);
MULTIBOOT_FLAGGED_ACCESSOR(vbe_interface_off, uint16_t, VBE);
MULTIBOOT_FLAGGED_ACCESSOR(vbe_interface_len, uint16_t, VBE);

#undef MULTIBOOT_FLAGGED_ACCESSOR
