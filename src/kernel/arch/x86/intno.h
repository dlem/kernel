#pragma once


#define INT_DIVISIONBYZERO                0
#define INT_DEBUG                         1
#define INT_NONMASKABLEINTERRUPT          2
#define INT_BREAKPOINT                    3
#define INT_OVERFLOW                      4
#define INT_OUTOFBOUNDS                   5
#define INT_INVALIDOPCODE                 6
#define INT_NOCOPROCESSOR                 7
#define INT_DOUBLEFAULT                   8
#define INT_COPROCESSORSEGMENTOVERRUN     9
#define INT_BADTSS                        10
#define INT_SEGMENTNOTPRESENT             11
#define INT_STACKFAULT                    12
#define INT_GENERALPROTECTIONFAULT        13
#define INT_PAGEFAULT                     14
#define INT_UNKNOWNINTERRUPT              15
#define INT_COPROCESSORFAULT              16
#define INT_ALIGNMENTCHECK                17
#define INT_MACHINECHECK                  18
// 19 through 31 are reserved for exceptions
// 32 through 47 are remapped IRQs
#define IRQ_TIMER                         32
#define IRQ_KEYBOARD                      33
#define IRQ_REDIRECTED                    34
#define IRQ_SERIALCOMMS0                  35 // 2 and 4
#define IRQ_SERIALCOMMS1                  36 // 1 and 3
#define IRQ_SOUNDCARD                     37
#define IRQ_FLOPPYCONTROLLER              38
#define IRQ_PARALLELCOMMS                 39
#define IRQ_REALTIMECLOCK                 40
#define IRQ_REDIRECTEDIRQ2                41
// 42 and 43 are reserved
#define IRQ_PS2MOUSE                      44
#define IRQ_MATHCOPROCESSOR               45
#define IRQ_HARDDISK                      46
// 77 is reserved
// the rest are for software interrupts; there are 256 in total, I think
#define INTERRUPT_COUNT                   47

