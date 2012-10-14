#pragma once

#define KERNEL_CODE_SEGMENT         (0x8)
#define KERNEL_DATA_SEGMENT         (0x10)

/**
 * Represents a stack frame; used to declare pointer arguments to functions
 * where the argument is a memory address to stack frame saved onto the stack.
 * Eg, in our interrupt handler routine.
**/
struct STACKFRAME {
  uint32_t gs, fs, es, ds;                          // we pushed these in stub
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by pushad
  uint32_t int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;        // pushed by trap
} __attribute__((packed));

extern "C" __cdecl void outportb(BYTE data, BYTE port);
extern "C" __cdecl void outportw(WORD data, BYTE port);
extern "C" __cdecl void outportdw(DWORD data, BYTE port);
extern "C" __cdecl BYTE inportb(BYTE port);
extern "C" __cdecl WORD inportw(BYTE port);
extern "C" __cdecl DWORD inportdw(BYTE port);

extern "C" __cdecl void CLI();
extern "C" __cdecl void STI();

bool SPL(bool pl);
bool SPLHi();
bool SPLLo();
bool InterruptsEnabled();

KRESULT InitIDT();
KRESULT Arch_Init(void *pMultibootInfo);
void Arch_Shutdown();
