; This file needs to be as close as possible to the beginning of the text
; segment. I'll put it first, and jump to main just in case.
; See the multiboot specification here:
; http://www.gnu.org/software/grub/manual/multiboot/multiboot.html

extern main

        jmp main

        MULTIBOOT_FLAGS equ 0x7 ; bit 0: align modules to 4K boundaries
                                ; bit 1: include memory map information
                                ; bit 2: include video mode information

        MULTIBOOT_MAGIC equ 0x1BADB002


        [BITS 32]
        [SECTION .text]
        align 4, db 0
        dd MULTIBOOT_MAGIC
        dd MULTIBOOT_FLAGS
        dd -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
