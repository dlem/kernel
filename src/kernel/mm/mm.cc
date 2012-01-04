#include "precomp.h"
#include "mm.h"
#include "global.h"

#if KDEBUG
# include "malloc_debug.h"
#endif

// The functions that do the work. They live in kheap.cc.
void *__KMalloc(size_t sz);
void __KFree(void *ptr);

/**
 * The base KMalloc function - void *__KMalloc(size_t size) - lives in kheap.cc.
 * Same with __KFree.
 */

// placement new - doesn't need a debug version
void *operator new(size_t sz, void *ptr)
{
  return ptr;
}

#if KDEBUG

// This one isn't called externally. The others get the callers' address, then
// call here.
void *__KMalloc(size_t sz, void *callerAddr, const char *file, int line)
{
  void *const allocAddr = __KMalloc(sz); // __KMalloc is the real one
  g_allocTracker.ReportAlloc(callerAddr, allocAddr, sz, file, line);
  return allocAddr;
}
void *__KMalloc(size_t sz, const char *file, int line)
{
  void *const callerAddr = __builtin_return_address(0);
  return __KMalloc(sz, callerAddr, file, line);
}
void *operator new(size_t sz, const char *file, int line)
{
  void *const callerAddr = __builtin_return_address(0);
  return __KMalloc(sz, callerAddr, file, line);
}

void *operator new [](size_t sz, const char *file, int line)
{
  void *const callerAddr = __builtin_return_address(0);
  return __KMalloc(sz, callerAddr, file, line);
}

void KFree_DEBUG(void *ptr)
{
  __KFree(ptr); // the real one
  g_allocTracker.ReportFree(ptr);
}

void operator delete(void *ptr)
{
  KFree_DEBUG(ptr);
}

void operator delete [](void *ptr)
{
  KFree_DEBUG(ptr);
}

#else

void *operator new(size_t sz)
{
  return __KMalloc(sz);
}
void *operator new [](size_t sz)
{
  return __KMalloc(sz);
}

void operator delete(void *ptr)
{
  __KFree(ptr);
}
void operator delete [](void *ptr)
{
  __KFree(ptr);
}

#endif

KRESULT InitMM()
{
  DASSERT(!Global::Inited_mm);
  RETURN_FAILED(InitAllocKPages());
  RETURN_FAILED(InitKHeap());
  Global::Inited_mm = true;
  return K_OK;
}
