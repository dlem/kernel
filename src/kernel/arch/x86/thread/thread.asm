global asm_idle;

; The hlt instruction, um, halts, until the next external interrupt is fired.
; It's okay that this function never returns (I think) because any interrupt
; with a valid handler will essentially jump away by itself.
asm_idle:
          hlt
          ; I'm not sure, but I think it's possible to get here,
          ; assuming the thread is restarted after an interrupt. So:
          jmp asm_idle
