#if KDEBUG

#include "precomp.h"
#include "malloc_debug.h"
#include "kstdlib.h"

// The tracking system has to bypass itself when it need to allocate memory;
// otherwise we'll have some serious recursion. Solution: give it direct
// access to the internal KMalloc and KFree functions. Not as pretty as it
// could be, but simple.

void *__KMalloc(size_t sz);
void __KFree(void *ptr);

#define KMalloc_NODEBUG(ptr) __KMalloc(ptr)
#define KFree_NODEBUG(ptr) __KFree(ptr)

/**
 * Keeps track of memory allocations so we can find memory leaks.
 *
 * TODO: this needs mutex
 * TODO: might be a good idea to use a hash table
**/

AllocTracker g_allocTracker;

struct AllocTracker::ALLOCATION {

  addr_t callerAddr;
  addr_t allocAddr;
  size_t nAllocated;
  const char *file;
  int line;
  ALLOCATION *pNext;
  void Init
  (
    addr_t _callerAddr,
    addr_t _allocAddr,
    size_t _nAllocated,
    const char *_file,
    int _line,
    ALLOCATION *_pNext
  )
  {
    callerAddr = _callerAddr;
    allocAddr = _allocAddr;
    nAllocated = _nAllocated;
    file = _file;
    line = _line;
    pNext = _pNext;
  }
};

AllocTracker::AllocTracker()
{
  m_pHead = NULL;
}

AllocTracker::~AllocTracker()
{
  ALLOCATION *pAl = m_pHead;
  if(pAl) { DASSERT(false); } // Memory leak detected.
  while(pAl)
  {
    kprintf("Memory leak: %db at %s:%d\n", pAl->nAllocated, pAl->file, pAl->line);
    ALLOCATION *pNext = pAl->pNext;
    KFree_NODEBUG(pAl);
    pAl = pNext;
  }
}

AllocTracker::ALLOCATION **AllocTracker::FindAl(addr_t allocAddr)
{
  ALLOCATION **al;
  for(al = &m_pHead; *al; al = &((*al)->pNext))
  {
    if((*al)->allocAddr == allocAddr)
    {
      break;
    }
  }
  return al;
}

void AllocTracker::ReportAlloc
(
  void *callerAddr,
  void *allocAddr,
  size_t sz,
  const char *file,
  int line
)
{
  ALLOCATION *pAl = (ALLOCATION *)KMalloc_NODEBUG(sizeof(ALLOCATION));
  if(!pAl)
  {
    DASSERT(false);
  }
  else
  {
    pAl->Init((addr_t)callerAddr, (addr_t)allocAddr, sz, file, line, m_pHead);
    m_pHead = pAl;
  }
}

void AllocTracker::ReportFree
(
  void *allocAddr
)
{
  ALLOCATION **al = FindAl((addr_t)allocAddr);

  if(!*al)
  {
    DASSERT(false); // Free on unallocated memory!
  }
  else
  {
    ALLOCATION *pNext = (*al)->pNext;
    KFree_NODEBUG(*al);
    *al = pNext;
  }
}
#endif

