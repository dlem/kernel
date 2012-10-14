#pragma once

class MultibootInfo {
  struct MULTIBOOT_INFO;

  MULTIBOOT_INFO *m_pInfo;

public:

  MultibootInfo(void *pInfo);

#define MULTIBOOT_FLAGGED_ACCESSOR(name, type) KRESULT get_##name(type *name) const

  MULTIBOOT_FLAGGED_ACCESSOR(mem_lower,         uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(mem_upper,         uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(boot_device,       uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(cmdline,           uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(mods_count,        uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(mods_addr,         uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_tabsize,       uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_strsize,       uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_addr,          uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_reserved,      uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_num,           uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_size,          uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_addr_elf,          uint32_t);
  //MULTIBOOT_FLAGGED_ACCESSOR(sym_shndx,         uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(mmap_length,       uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(mmap_addr,         uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(drives_length,     uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(drives_addr,       uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(config_table,      uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(boot_loader_name,  uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(apm_table,         uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(vbe_control_info,  uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(vbe_mode_info,     uint32_t);
  MULTIBOOT_FLAGGED_ACCESSOR(vbe_mode,          uint16_t);
  MULTIBOOT_FLAGGED_ACCESSOR(vbe_interface_seg, uint16_t);
  MULTIBOOT_FLAGGED_ACCESSOR(vbe_interface_off, uint16_t);
  MULTIBOOT_FLAGGED_ACCESSOR(vbe_interface_len, uint16_t);

#undef MULTIBOOT_FLAGGED_ACCESSOR

};

#define MULTIBOOT_MMAPTYPE_AVAILABLE 1
struct MULTIBOOT_MMAPNODE {
  uint32_t size;
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
  const MULTIBOOT_MMAPNODE *GetNext(uint32_t baseAddr, uint32_t length) const
  {
    const MULTIBOOT_MMAPNODE *pNext = (const MULTIBOOT_MMAPNODE *)((const BYTE *)this + size + sizeof(uint32_t));
    if((const uint32_t)pNext >= baseAddr + length)
    {
      DASSERT((const uint32_t)pNext == baseAddr + length);
      return NULL;
    }
    return pNext;
  }
};

struct MULTIBOOT_MOD {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t mod_string;
  uint32_t mod_reserved;
} __packed;
