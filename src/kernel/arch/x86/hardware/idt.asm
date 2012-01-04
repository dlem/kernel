global asm_load_idt

; executes the lidt instruction with the IDT_PTR provided as an argument.
asm_load_idt:
 lidt [esp + 4] ; return address
 ret
