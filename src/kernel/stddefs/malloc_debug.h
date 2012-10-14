#pragma once

#if !defined(KDEBUG) || !KDEBUG
#   error "malloc_debug.h included during non-debug build!"
#endif

class AllocTracker {
  struct ALLOCATION;
  ALLOCATION *m_pHead;
  ALLOCATION **FindAl(addr_t addr);

public:
  AllocTracker();
  ~AllocTracker();
  void ReportAlloc
  (
    void *callerAddr,
    void *allocAddr,
    size_t sz,
    const char *file,
    int line
  );
  void ReportFree
  (
    void *allocAddr
  );
};

extern AllocTracker g_allocTracker;
