#include "precomp.h"
#include "mm.h"
#include "global.h"
#include "kstdlib.h"

#define ALLOCKPAGES_SLOW 1

#define AKP_ORDERSZ(order) (MM_MAXMEMORY / MM_PAGESIZE / (1 << order))

/**
 * This file handles kernel page allocations. It's used by the kmalloc and the
 * heap. Think of AllocKPages as a malloc that gives away memory in units of
 * pages instead of bytes.
 *
 * The system is based on Linux's - there are tiers of "superpages", each having
 * double the size of the previous, to reduce segmentation. Each page has a
 * "buddy", an adjacent page of the same size, and when both a page and its
 * buddy are free they're merged and put into the next tier (and erased from
 * the lower tier). The presence of a superpage in a tier is indicated by a set
 * bit in the bit array corresponding to the tier.
**/

/**
 * I'll allocate space for every bit array in the BSS segment. Since each
 * next highest tier will require exactly half the number of bits, we can think
 * of the total size in terms of a geometric sum.
 *
 * Assuming a maximum of 4GB, the highest possible number of pages is
 * 1024^3 / MM_PAGESIZE; multiply that by 2 (geometric sum) and divide by 8
 * (bit array) to get the highest possible memory requirement of the combined
 * superpage bit arrays.
**/
#define MM_SUPERPAGE_ARRAY_SIZE   (MM_MAXMEMORY / 4 / MM_PAGESIZE)

// large array with space for all of the order arrays
BYTE superpageArray[MM_SUPERPAGE_ARRAY_SIZE] = {0};

// map from order -> address of array in the superpageArray; Inited by
// InitAllocKPages
BYTE *pPageArrays[MM_MAXSUPERPAGEORDER + 1] = {NULL};

inline BYTE IsBitSet(BYTE *pArray, size_t n)
{
  return pArray[n / 8] & (1 << (n % 8));
}
inline void SetBit(BYTE *pArray, size_t n)
{
  pArray[n / 8] |= 1 << (n % 8);
}
inline void ClearBit(BYTE *pArray, size_t n)
{
  pArray[n / 8] &= ~(1 << (n % 8));
}
inline size_t GetBuddy(const size_t ixPage)
{
  return ixPage % 2 ? ixPage - 1 : ixPage + 1;
}

bool IsPresent(size_t ixPage)
{
  for(size_t order = 0; order <= MM_MAXSUPERPAGEORDER; order++)
  {
    if(IsBitSet(pPageArrays[order], ixPage)) return true;
    ixPage /= 2;
  }
  return false;
}

void FreePage(void *pPage)
{
  const addr_t pageAddr = (addr_t)pPage;
  DASSERT(pageAddr % MM_PAGESIZE == 0);
  size_t ixPage = (size_t)(pageAddr / MM_PAGESIZE);
#if ALLOCKPAGES_SLOW
  DASSERT(!IsPresent(ixPage));
#endif
  size_t order;
  for(order = 0; order < MM_MAXSUPERPAGEORDER; order++)
  {
    DASSERT(ixPage < AKP_ORDERSZ(order));
    DASSERT(!IsBitSet(pPageArrays[order], ixPage));
    const size_t ixBuddy = GetBuddy(ixPage);
    if(!IsBitSet(pPageArrays[order], ixBuddy)) break; // buddy isn't free; we're done

    // buddy is free; we can go up to the next tier
    ClearBit(pPageArrays[order], ixBuddy);
    ixPage /= 2;
  }
  // we may have hit the break statement, or we may have hit the for condition;
  // either way, this next line does the right thing
  DASSERT(!IsBitSet(pPageArrays[order], ixPage));
  SetBit(pPageArrays[order], ixPage);
}

/**
 * Allocates a single page of the specified order. Returns NULL if none can be
 * found.
**/

void *GetPageWithOrder(size_t order)
{
  // TODO: Make this more efficient by keeping track of how many pages are
  // free and/or the location of the first free one in each array

  // Try every order from the requested one to the highest possible order.
  // Break as soon as you find a page of any order.
  size_t orderAvail;
  size_t ixPage;
  for(orderAvail = order; orderAvail <= MM_MAXSUPERPAGEORDER; orderAvail++)
  {
    const size_t limit = MM_MAXMEMORY / MM_PAGESIZE / (1 << orderAvail);
    for(ixPage = 0; ixPage < limit; ixPage++)
    {
      if(IsBitSet(pPageArrays[orderAvail], ixPage)) goto found_page;
    }
  }
found_page:

  if(orderAvail > MM_MAXSUPERPAGEORDER) return NULL; // can't fulfull the request

  // We can use the page found in the loops above, but we may need to split it
  // into buddies and move it down a tier any number of times.
  ClearBit(pPageArrays[orderAvail], ixPage);
  while(orderAvail > order)
  {
    ixPage *= 2;
    orderAvail -= 1;
    const size_t ixBuddy = ixPage + 1;
    SetBit(pPageArrays[orderAvail], ixBuddy);
  }

  return (void *)(MM_PAGESIZE * ixPage * (1 << order));
}

KRESULT AllocKPages(size_t nPages, void **pPages)
{
  if(!Global::Inited_mm)
  {
    if(pPages) *pPages = NULL;
    return E_UNINITIALIZED;
  }
  if(!pPages) return E_POINTER;

  KRESULT kr = K_OK;
  size_t order;
  for(order = 0; order <= MM_MAXSUPERPAGEORDER; order++)
  {
    if(((size_t)1 << order) >= nPages) break; // we've reached a big enough order to do the allocation
  }
  if(order > MM_MAXSUPERPAGEORDER) kr = E_TOOMANYPAGES;

  // what I do here is weird - I allocate a whole superpage of the requested
  // order, then free the pages from it that weren't needed
  if(SUCCEEDED(kr))
  {
    *pPages = GetPageWithOrder(order);
    if(!*pPages) kr = E_OUTOFMEMORY;
  }
  // free unneeded pages
  if(SUCCEEDED(kr))
  {
    const size_t nToFree = ((size_t)1 << order) - nPages;
    void *toFree = (BYTE *)*pPages + MM_PAGESIZE * (1 << order);
    for(size_t i = 0; i < nToFree; i++)
    {
      toFree = (BYTE *)toFree - MM_PAGESIZE;
      FreePage(toFree);
    }
  }
  if(FAILED(kr)) *pPages = NULL;
  return kr;
}

void FreeKPages(void *pPages, size_t nPages)
{
  for(size_t i = 0; i < nPages; i++)
  {
    FreePage((BYTE *)pPages + i * MM_PAGESIZE);
  }
}

KRESULT AllocKPage(void **ppPage)
{
  return AllocKPages(1, ppPage);
}

void FreeKPage(void *pPage)
{
  FreeKPages(pPage, 1);
}

// Like AllocKPage, but you choose the address. Used for making OS-occupied
// pages invalid.
KRESULT ClaimKPage(void *pPage)
{
  DASSERT(MM_PAGEADDR(pPage) == (addr_t)pPage);
  size_t iOrder;
  size_t ixPage;
  for(iOrder = 0; iOrder <= MM_MAXSUPERPAGEORDER; iOrder++)
  {
    ixPage = (size_t)pPage / (1 << iOrder);
    if(IsBitSet(pPageArrays[iOrder], ixPage))
    {
      ClearBit(pPageArrays[iOrder], ixPage);
      break;
    }
  }
  if(iOrder > MM_MAXSUPERPAGEORDER) return E_INVALIDARG; // Page doesn't seem to have been present.

  // We've found the set bit in a page array corresponding to the requested
  // page. But we only want to claim the lowest-level page. We could take this
  // into account by setting bits on the way up in the previous loop, but then
  // we'd have to unset all of the bits if the page weren't present.
  while(iOrder > 0)
  {
    iOrder -= 1;
    ixPage = (size_t)pPage / (1 << iOrder);
    size_t ixBuddy = GetBuddy(ixPage);
    DASSERT(!IsBitSet(pPageArrays[iOrder], ixBuddy));
    SetBit(pPageArrays[iOrder], ixBuddy);
  }
  return K_OK;
}

size_t CountPages()
{
  size_t iTotal = 0;
  for(int order = 0; order <= MM_MAXSUPERPAGEORDER; order++)
  {
    size_t cOrder = 1 << order;
    for(size_t ixPage = 0; ixPage < MM_MAXMEMORY / MM_PAGESIZE / (1 << order); ixPage++)
    {
      if(IsBitSet(pPageArrays[order], ixPage))
      {
        iTotal += cOrder;
      }
    }
  }
  return iTotal;
}

KRESULT InitAllocKPages()
{
  BYTE *pArray = superpageArray;
  DASSERT(MM_MAXMEMORY % MM_PAGESIZE == 0);
  for(int order = 0; order <= MM_MAXSUPERPAGEORDER; order++)
  {
    pPageArrays[order] = pArray;
    pArray += ((MM_MAXMEMORY / MM_PAGESIZE + (1 << order) - 1) / (1 << order) + 7) / 8;
  }
  return K_OK;
}
