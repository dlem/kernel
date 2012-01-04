/**
 * Much of this script is copied from http://wiki.osdev.org/C%2B%2B_Bare_Bones
 *
**/

OUTPUT_FORMAT(elf32-i386)
ENTRY(main)

SECTIONS {
  . = 0x200000;
  os_image_start = .;
  .text ALIGN(4096) :
  {
    src/kernel/arch/x86/main/multiboot.asmobj(.text*) /* the multiboot header needs to come first */
    *(.text*)
    *(.gnu.linkonce.t*)
  } 
  
  .rodata ALIGN(4096) :
  {
    start_ctors = .;
    *(.ctor*)
    end_ctors = .;
    
    /* There should be no destructors if we're using a newer version of GCC */
    
    *(.rodata*)
    *(.gnu.linkonce.r*)
  } 
  
  .data ALIGN(4096) :
  {
    *(.data*)
    *(.gnu.linkonce.d*)
  } 
  
  .bss ALIGN(4096) :
  {
    *(.COMMON*)
    *(.bss*)
    *(.gnu.linkonce.b*)
  }
  
  os_image_end = .;
  
  /DISCARD/ :
  {
    *(.comment)
    *(.eh_frame)
  }
    
  /* stab debug sections */
  .stab          : { *(.stab) }
  .stabstr       : { *(.stabstr) }
  .stab.excl     : { *(.stab.excl) }
  .stab.exclstr  : { *(.stab.exclstr) }
  .stab.index    : { *(.stab.index) }
  .stab.indexstr : { *(.stab.indexstr) }
  .comment       : { *(.comment) }
  
  /**
   * dwarf debug sections; their offset is wrt the file, so we need to start
   * them at 0.
  **/
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  .debug_arranges   0 : { *(.debug_arranges) }
  .debug_pubnames   0 : { *(.debug_pubnames) }
  .debug_info       0 : { *(.debug_info) }
  .debug_abbrev     0 : { *(.debug_abbrev) }
  .debug_line       0 : { *(.debug_line) }
  .debug_frame      0 : { *(.debug_frame) }
  .debug_str        0 : { *(.debug_str) }
  .debug_loc        0 : { *(.debug_loc) }
  
}
