global asm_spinlock_acquire;
global asm_spinlock_tryacquire;

; asm_spinlock_acquire(BYTE *pLock);
; assumes the lock is free iff it's nonzero
asm_spinlock_acquire:
  mov al, 0
  mov ebx, [esp - 4]

.loop:
  xchg al, [ebx]
  and eax, eax
  jz .loop

  ret

; BYTE asm_spinlock_acquire(void *pLock);
asm_spinlock_tryacquire:
  mov al, 0
  mov ebx, [esp - 4]
  xchg al, [ebx]
  ret
