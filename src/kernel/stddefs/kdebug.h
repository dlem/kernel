#pragma once

// make sure KDEBUG is defined
#ifndef KDEBUG
#error KDEBUG is not defined
#endif

// declarations

void
__DASSERT(int iCond, const char *pszCond, const char *pszFile, int iLine);

template<bool t>
  struct __CASSERT_STRUCT
  {
    enum
    {
      N = 1 - 2 * int(!t)
    };
    // 1 if t is true, -1 if t is false.
    static char A[N];
  };

template<bool t>
  char __CASSERT_STRUCT<t>::A[N];

#define _CASSERT(line, cond) extern __CASSERT_STRUCT< (cond) >  __CASSERT_DUMMY##line
#define CASSERT(cond) _CASSERT(__LINE__, cond)

void panic(const char *msg = NULL);

/**
 * Note that the main kmalloc function is actually named __kmalloc, so that I
 * can #define kmalloc in debug mode. The alternative would be to undef kmalloc
 * in a few places - I prefer the first option, though there are advantages and
 * disadvantages to both.
 */

#if KDEBUG // debug mode enabled

#define DASSERT(cond) (__DASSERT((int)(cond), #cond, __FILE__, __LINE__))
#define VERIFY(cond) (__DASSERT((int)(cond), #cond, __FILE__, __LINE__))
typedef __KRESULT KRESULT;

// memory allocation stuff

#define KNew new(__FILE__, __LINE__)
#define KMalloc(sz) __KMalloc(sz, __FILE__, __LINE__)

void *operator new(unsigned int sz, const char *file, int line) __noinline;
void *operator new [](unsigned int sz, const char *file, int line) __noinline;
void *__KMalloc(size_t sz, const char *file, int line) __noinline;

#define KFree(ptr) KFree_DEBUG(ptr)
void KFree_DEBUG(void *ptr);

#else // debug mode not enabled

#define DASSERT(cond)
#define VERIFY(cond) (void)(cond)
typedef int KRESULT;

// memory allocation stuff

#define KNew new
#define KMalloc(sz) __KMalloc(sz)

void *operator new(size_t sz);
void *operator new [](size_t sz);
void * __KMalloc(size_t sz);

#define KFree(ptr) __KFree(ptr)
void __KFree(void *ptr);

#endif

#define KDelete delete

void operator delete(void *ptr);
void operator delete [](void *ptr);
