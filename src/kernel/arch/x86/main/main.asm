extern kentry               ; c++ entry point
extern gdt_spec             ; makeshift global descriptor table in gdt.asm
extern SYS_CODE_SEL
extern SYS_DATA_SEL

global main
global __test

null_idt: times 6 db 0        ; null idt pointer

; After GRUB does its thing, it will jump here. Our state will be:
;   - Interrupts are disabled
;   - We need to make our own GDT
;   - We need to make our own IDT before we enable interrupts
;   - The A20 line is enabled
;
; So I'll first load a GDT and the segment registers.

main:
          lgdt [gdt_spec]     ; load our makeshift gdt
          lidt [null_idt]

          ; I should really check eax for the magic multiboot value,
          ; but I'm too lazy right now.

          ; update segment registers with GDT information
          mov ax, SYS_DATA_SEL
          mov ds, ax
          mov es, ax
          mov ss, ax

          mov esp, 0x200000   ; that's a ton of stack space
          push ebx            ; pointer to multiboot info struct

          ; before jumping to the c++ entry point, we need to
          ; execute any constructors ourselves since no once else is
          ; going to do it

          ; these addresses should be defined by the linker script

extern start_ctors, end_ctors

          ; Note that it is important for the constructors to be executed in
          ; the order in which they appear because of object dependence

          mov eax, start_ctors
          jmp ctors_test

ctors_loop:
          push eax
          call [eax]
          pop eax
          add eax, 4

ctors_test:
          cmp eax, end_ctors
          jb ctors_loop

          ; now run the actual c++ os code

          call kentry
          add esp, 4 ; clean up arg

          ; we return here after kentry returns. We need to first execute all
          ; of the destructors, then halt. Execute the dtors by calling
          ; __cxa_finalize(0).
          extern __cxa_finalize

          push DWORD 0
          call __cxa_finalize
          add esp, 4 ; clean up arg

          ; TODO: shut down.

halt:
          hlt
          jmp halt
