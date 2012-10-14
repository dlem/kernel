extern _fault_handler       ; c-level handler routine

; creates and makes global an ISR meant to deal with exceptions which don't push
; an error code onto the stack; pushes a byte instead to make things consistent

%macro MAKE_ISR 1

global _isr%1
_isr%1:
  cli
  push byte 0
  push byte %1
  jmp isr_common_stub

%endmacro

; creates and makes global an ISR meant to deal with exceptions which /do/ push
; an error code onto the stack

%macro MAKE_ISR_ERR 1

global _isr%1
_isr%1:
  cli
  push byte %1
  jmp isr_common_stub

%endmacro

MAKE_ISR 0        ; divide by zero
MAKE_ISR 1        ; debug exception
MAKE_ISR 2        ; non maskable interrupt exception
MAKE_ISR 3        ; breakpoint exception
MAKE_ISR 4        ; into detected overflow exception
MAKE_ISR 5        ; out of bounds exception
MAKE_ISR 6        ; invalid opcode exception
MAKE_ISR 7        ; no coprocessor exception
MAKE_ISR_ERR 8    ; double fault exception
MAKE_ISR 9        ; coprocessor segment overrun exception
MAKE_ISR_ERR 10   ; bad TSS exception
MAKE_ISR_ERR 11   ; segment not present exception
MAKE_ISR_ERR 12   ; stack fault exception
MAKE_ISR_ERR 13   ; general protection fault exception
MAKE_ISR_ERR 14   ; page fault exception
MAKE_ISR 15       ; unknown interrupt exception
MAKE_ISR 16       ; coprocessor fault exception
MAKE_ISR 17       ; alignment check exception
MAKE_ISR 18       ; machine check exception
; 19 through 31 are reserved
MAKE_ISR 19
MAKE_ISR 20
MAKE_ISR 21
MAKE_ISR 22
MAKE_ISR 23
MAKE_ISR 24
MAKE_ISR 25
MAKE_ISR 26
MAKE_ISR 27
MAKE_ISR 28
MAKE_ISR 29
MAKE_ISR 30
MAKE_ISR 31

; 32 through 47 will be IRQs; but I'll use the same handler anyway 
MAKE_ISR 32
MAKE_ISR 33
MAKE_ISR 34
MAKE_ISR 35
MAKE_ISR 36
MAKE_ISR 37
MAKE_ISR 38
MAKE_ISR 39
MAKE_ISR 40
MAKE_ISR 41
MAKE_ISR 42
MAKE_ISR 43
MAKE_ISR 44
MAKE_ISR 45
MAKE_ISR 46
MAKE_ISR 47

; common stub for interrupts; saves thread context, gets kernel context, calls
; c-level handler routine, and restores thread context
isr_common_stub:
  pushad            ; push all registers onto stack
  push ds
  push es
  push fs
  push gs
  mov ax, 0x10      ; kernel data segment descriptor
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov eax, esp      ; pass the current stack as a pointer argument to the
  push eax          ; handler as the trap frame argument
  mov eax, _fault_handler
  call eax          ; absolute call
  pop eax
  pop gs
  pop fs
  pop es
  pop ds
  popad
  add esp, 8		; clean up error code and int number (not sure why it's 8)
  sti				; re-enable interrupts now that we've handled this one
            ; I think it's important that the iretd instruction come next
  iretd             ; pops cs, eip, eflags, cs, and esp

