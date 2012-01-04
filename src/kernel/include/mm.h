#include "precomp.h"

#define MM_PAGESIZE               4096  // update the other defines if you update this
#define MM_PAGESHIFT              12    // must be lg(MM_PAGESIZE)
#define MM_MAXSUPERPAGEORDER      8     // that's 1GB worth of pages for each top-level entry
#define MM_MAXMEMORY              (MM_PAGEADDR(0xFFFFFFFF)) // that's one page less than 4GB
#define MM_PAGEADDR(addr)         ((addr_t)(addr) & 0xFFFFF000)
#define MM_ALIGN(addr, align)     ((addr_t)(addr) + (addr_t)(align) - 1 - (((addr_t)(addr) + (addr_t)(align) - 1) % (addr_t)(align)))

#define MM_NCACHES                9     // powers of two, 4 through 1024

// __kmalloc and __kfree are declared in kdebug.h

KRESULT InitMM();
KRESULT InitKHeap();
KRESULT InitAllocKPages();
KRESULT AllocKPages(size_t nPages, void **pptr);
void FreeKPages(void *ptr, size_t nPages);
KRESULT AllocKPage(void **pptr);
void FreeKPage(void *ptr);
size_t CountPages();
KRESULT ClaimKPage(void *ptr);
