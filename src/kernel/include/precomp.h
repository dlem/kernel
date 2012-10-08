#pragma once

// useful macros
#define NULL            0
#define NUMELMS(array)  ((int)(sizeof(array)/sizeof(array[0])))
#define NUMBYTES(x)     (sizeof(x) / sizeof(BYTE))
#define StructZero(x)   memzero(x, NUMBYTES(*x))

#define __cdecl     __attribute__((cdecl))
#define __noinline  __attribute__((noinline))
#define __packed    __attribute__((packed))

#define DECLARE_NOCOPY(classname) \
  classname(const classname &other);\
  const classname &operator=(const classname &other)

// header files included by default
#include "limits.h"
#include "types.h"
#include "kerrno.h"
#include "kdebug.h"
#include "kresult.h"

void memset(void *pMem, char val, size_t numBytes);
void memzero(void *pMem, size_t numBytes);
void *operator new(unsigned int sz, void *ptr); // placement new; regular new is declared in kdebug
