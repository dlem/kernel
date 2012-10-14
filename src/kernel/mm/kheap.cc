#include <mm/mm.h>
#include <main/kstdlib.h>

#define KHEAP_SLOW 1
#if KHEAP_SLOW
# define SLOW(stmnt) stmnt
#else
# define SLOW(stmnt) while(0)
#endif

struct KHEAP_FREELIST {
  KHEAP_FREELIST *pNext;
} __attribute__((packed));

struct KHEAP_SLABLIST {
  KHEAP_SLABLIST *pNext;
  unsigned long nFree;
  KHEAP_FREELIST freeList; // this should really be a pointer...
  void Init() {
    nFree = 0;
    freeList.pNext = NULL;
    pNext = NULL;
  }
};

struct SUPERPAGEALLOCATION {
  void *addr;
  unsigned long nPages;
  void Init(void *_addr, unsigned long _nPages)
  {
    addr = _addr;
    nPages = _nPages;
  }
  void Init()
  {
    addr = NULL;
    nPages = 0;
  }
};

struct SALIST {
  SALIST *pNext;
  unsigned long nFree;
  SUPERPAGEALLOCATION saList[(MM_PAGESIZE - sizeof(void *) - sizeof(unsigned long)) / sizeof(SUPERPAGEALLOCATION)];
  void Init()
  {
    nFree = NUMELMS(saList);
    memset(saList, 0, sizeof(saList));
    pNext = NULL;
  }
} __attribute__((packed));

class MemCache {
public:
  MemCache() {}
  void Init(unsigned long allocSz);
  // destructor might be useful for generality or debugging

  unsigned long GetAllocationSize() const {return m_allocSz;}
  void *Malloc();
  void Free(void *ptr, bool *pfFound);

private:
  DECLARE_NOCOPY(MemCache);
  KRESULT GetSlab(KHEAP_SLABLIST **pSlab);
  void *SubSlabMalloc(KHEAP_SLABLIST *pSlab);

  KHEAP_SLABLIST *m_pSlabList;
  unsigned long          m_allocSz;
  unsigned long          m_slabCapacity;
  unsigned long          m_effectiveSz;

};

void MemCache::Init(unsigned long allocSz)
{
  m_allocSz = allocSz;
  m_effectiveSz = MM_ALIGN(allocSz, sizeof(void *));
  m_slabCapacity = (MM_PAGESIZE - MM_ALIGN(sizeof(KHEAP_SLABLIST), sizeof(void *))) / m_effectiveSz;
}

void *MemCache::Malloc()
{
  KHEAP_SLABLIST **pSlabList = &m_pSlabList;
  while(*pSlabList && (*pSlabList)->nFree == 0)
  {
    pSlabList = &((*pSlabList)->pNext);
  }
  if(!*pSlabList) // no free pages on current slabs; get new slab
  {
    if(FAILED(GetSlab(pSlabList))) *pSlabList = NULL;
  }
  return *pSlabList ? SubSlabMalloc(*pSlabList) : NULL;
}

void *MemCache::SubSlabMalloc(KHEAP_SLABLIST *pSlab)
{
  DASSERT(pSlab->nFree > 0);
  DASSERT(pSlab->freeList.pNext);
  // always take the first available chunk
  void *retval = pSlab->freeList.pNext;
  pSlab->freeList.pNext = pSlab->freeList.pNext->pNext;
  pSlab->nFree -= 1;
  return retval;
}

void MemCache::Free(void *ptr, bool *pfFound)
{
  KHEAP_SLABLIST *pSlab = (KHEAP_SLABLIST *)MM_PAGEADDR((addr_t)ptr);;
  KHEAP_SLABLIST **pIter = &m_pSlabList;
  while(*pIter && *pIter != pSlab)
  {
    pIter = &((*pIter)->pNext);
  }
  if(!*pIter)
  {
    *pfFound = false;
    return;
  }
  else
  {
    *pfFound = true;
#if KHEAP_SLOW
    KHEAP_FREELIST *pFL = (*pIter)->freeList.pNext;
    while(pFL) {
      if(ptr == (void *)pFL)
      {
        DASSERT(false); // freed pointer is already in freelist
        break;
      }
      pFL = pFL->pNext;
    }
#endif
    *(KHEAP_FREELIST **)ptr = pSlab->freeList.pNext;
    pSlab->freeList.pNext = (KHEAP_FREELIST *)ptr;
    pSlab->nFree += 1;
    if(pSlab->nFree == m_slabCapacity)
    {
      FreeKPages(pSlab, 1);
    }
  }
}

KRESULT MemCache::GetSlab(KHEAP_SLABLIST **pSlab)
{
  RETURN_FAILED(AllocKPage((void **)pSlab));
  (*pSlab)->Init();
  KHEAP_FREELIST **pFreeList = &((*pSlab)->freeList.pNext);
  SLOW(unsigned long nChunks = 0);
  for(unsigned long offset = MM_ALIGN(sizeof(KHEAP_SLABLIST), sizeof(void *)); offset <= MM_PAGESIZE - m_effectiveSz; offset += m_effectiveSz)
  {
    *pFreeList = (KHEAP_FREELIST *)((BYTE *)*pSlab + offset);
    SLOW(nChunks += 1);
    pFreeList = &((*pFreeList)->pNext);
  }
  *pFreeList = NULL;
  SLOW(DASSERT(nChunks == m_slabCapacity));
  (*pSlab)->nFree = m_slabCapacity;
  return K_OK;
}

SALIST *pSuperpageAllocations = NULL;
MemCache memCaches[MM_NCACHES];

void *SuperpageAllocation(unsigned long sz)
{
  // round up to the nearest page
  const unsigned long nPages = (sz + MM_PAGESIZE - 1) / MM_PAGESIZE;
  DASSERT(nPages >= 1);
  void *retval;

  KRESULT kr = AllocKPages(nPages, &retval);
  if(FAILED(kr)) retval = NULL;

  if(SUCCEEDED(kr))
  {
    // find a slot in which to record the allocation
    SALIST **ppSAList = &pSuperpageAllocations;
    while(*ppSAList)
    {
      if((*ppSAList)->nFree > 0) break;
    }
    if(!*ppSAList) // no allocation list slabs with free slots; get a new slab
    {
      kr = AllocKPage((void **)ppSAList);
      if(FAILED(kr)) *ppSAList = NULL;
      else (*ppSAList)->Init();
    }
    if(SUCCEEDED(kr))
    {
      (*ppSAList)->nFree -= 1;
      for(int ii = 0; ii < NUMELMS((*ppSAList)->saList); ii++)
      {
        if(!((*ppSAList)->saList[ii].addr))
        {
          (*ppSAList)->saList[ii].addr = retval;
          (*ppSAList)->saList[ii].nPages = nPages;
          break;
        }
      }
    }
  }

  if(FAILED(kr) && retval)
  {
    FreeKPages(retval, nPages);
    retval = NULL;
  }
  return retval;
}

void SuperpageFree(void *ptr, bool *pfFound)
{
  if(!ptr) return;
  SALIST **ppSAList = &pSuperpageAllocations;
  while(*ppSAList)
  {
    for(int i = 0; i < NUMELMS((*ppSAList)->saList); i++)
    {
      if((*ppSAList)->saList[i].addr == ptr) {
        FreeKPages(ptr, (*ppSAList)->saList[i].nPages);
        (*ppSAList)->saList[i].Init();
        (*ppSAList)->nFree += 1;
        if((*ppSAList)->nFree == NUMELMS((*ppSAList)->saList))
        {
#if KHEAP_SLOW
          for(int ii = 0; ii < NUMELMS((*ppSAList)->saList); ii++)
          {
            DASSERT(!(*ppSAList)->saList[ii].addr);
          }
#endif
          SALIST *pNext = (*ppSAList)->pNext;
          FreeKPage(*ppSAList);
          *ppSAList = pNext;
        }
        *pfFound = true;
        return;
      }
    }
    ppSAList = &((*ppSAList)->pNext);
  }
  *pfFound = false; // if we got here, we didn't free it
}

void *SubpageAllocation(unsigned long sz)
{
  for(int ixCache = 0; ixCache < MM_NCACHES; ixCache++)
  {
    if(memCaches[ixCache].GetAllocationSize() >= sz)
    {
      return memCaches[ixCache].Malloc();
    }
  }
  DASSERT(false);
  return NULL;
}

/**
 * Here are the main events: the actual implementations of kmalloc and kfree.
 */
void *__KMalloc(unsigned int sz)
{
  // a zero-sized malloc is probably a bug, and it's awkward from the MM's
  // point of view, but I guess it's a valid argument. I'll just return the
  // address of the first page, since it's not NULL and it's aligned. The
  // allocator won't actually touch any memory if it's not buggy.
  if(!sz)
  {
    DASSERT(false);
    return (void *)MM_PAGESIZE;
  }
  // Figure out what kind of allocation to make. If the allocation size
  // is greater than the size of the entries in the biggest cache group, we
  // need to do a superpage allocation.
  if(sz > memCaches[MM_NCACHES - 1].GetAllocationSize())
  {
    return SuperpageAllocation(sz);
  }
  else
  {
    return SubpageAllocation(sz);
  }
}

void __KFree(void *ptr)
{
  // The easiest way to do this is to just iterate through all of the caches
  // until one of them deals with the free. It's not very general, as it
  // requires the caches to be able to tell whether the location is valid,
  // but that's not currently a problem.
  bool fFound = false;
  for(int ixCache = 0; ixCache < MM_NCACHES && !fFound; ixCache++)
  {
    memCaches[ixCache].Free(ptr, &fFound);
  }
  if(!fFound)
  {
    SuperpageFree(ptr, &fFound);
  }
  DASSERT(fFound); // invalid pointer freed?
}

KRESULT InitKHeap()
{
  int ixCache = 0;
  int prevCacheType = 3;

#define KHEAP_DECLARE_CACHETYPE(nBytes) \
  DASSERT(nBytes > prevCacheType);\
  DASSERT(ixCache < MM_NCACHES);\
  memCaches[ixCache].Init(nBytes);\
  prevCacheType = nBytes;\
  ixCache += 1;

  KHEAP_DECLARE_CACHETYPE(4);
  KHEAP_DECLARE_CACHETYPE(8);
  KHEAP_DECLARE_CACHETYPE(16);
  KHEAP_DECLARE_CACHETYPE(32);
  KHEAP_DECLARE_CACHETYPE(64);
  KHEAP_DECLARE_CACHETYPE(128);
  KHEAP_DECLARE_CACHETYPE(256);
  KHEAP_DECLARE_CACHETYPE(512);
  KHEAP_DECLARE_CACHETYPE(1024);
  // anything >= 2048 would be pointless because only one allocation would be
  // possible per page

#undef KHEAP_DECLARE_CACHETYPE

  DASSERT(ixCache == MM_NCACHES);

  return K_OK;
}
CASSERT(MM_NCACHES == 9);
