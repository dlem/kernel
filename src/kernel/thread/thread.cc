#include <thread/thread.h>
#include <main/global.h>

// TODO: Implement zombie threads. This may involve threads being some sort of
// smart pointer. But that's just one option. In the current system, any exiting
// thread will cause a memory leak (of the thread object).

KRESULT Init_Scheduler()
{
  DASSERT(!Global::pGScheduler);
  Global::pGScheduler = KNew CQueueScheduler;
  if(!Global::pGScheduler) return E_OUTOFMEMORY;

  KRESULT kr = Global::pGScheduler->Init();
  if(FAILED(kr))
  {
    KDelete Global::pGScheduler;
    Global::pGScheduler = NULL;
  }

  return kr;
}

////////////////////////////////////////////////////////////////////////////////
// The base scheduler doesn't implement thread selection; a derived class needs
// to do that.

KRESULT CBaseScheduler::Init()
{
  m_pCurrentThread = Arch_GetNewThread();
  m_pIdleThread = Arch_GetNewThread();
  KRESULT hr;
  if(!(m_pCurrentThread && m_pIdleThread))
  {
    kr = E_OUTOFMEMORY;
  }
  if(SUCCEEDED(hr))
  {
    m_pCurrentThread->Init();
    m_pIdleThread->Init(Arch_Idle);
  }
}

void CBaseScheduler::NewCurrentThread(CBaseThread *pThread)
{
  m_pCurrentThread = pThread;
}

void CBaseScheduler::Yield()
{
  // Add the current thread back to the pool first so that the selector won't
  // be forced to return NULL if this is the only thread.
  CBaseThread *pYielder = GetCurrentThread();
  AddThread(pYielder);
  Dispatch();
}

KRESULT CBaseScheduler::Spawn(ICallable *pCallable, const char *pszName)
{
  CBaseThread *pThread = Arch_GetNewThread();
  if(!pThread)
  {
    return E_OUTOFMEMORY;
  }

  KRESULT kr = pThread->Init(pCallable, pszName);

  if(SUCCEEDED(kr))
  {
    kr = AddThread(pThread);
  }

  if(FAILED(kr))
  {
    KDelete pThread;
    pThread = NULL;
  }
}

void Start()
{
  // When this method is called, there is no current thread.
  // If we just tell some thread to start running, we should spend the rest
  // of the time either running a queued thread or idling. In other words, the
  // Dispatch call should never return.
  Dispatch();
  DASSERT(false); // We should never return from the above call.
}

void Exit(CBaseThread *pThread)
{
  if(!pThread) { pThread = GetCurrentThread(); }

  // We can't really delete the exiting thread, because other threads could
  // in theory have references to it, and at some point parents will want to
  // get exit codes or whatever.
  //
  // Also, we wouldn't have a currently running thread, and I don't like the
  // ramifications of that (what if another thread needs information?). Of
  // course, I'm getting way ahead of myself wrt multiprocessing here.
  //
  // Unfortunately, that means that we've got a memory leak here until I,
  // I don't know, implement smart pointers or vectors or something.

  Dispatch();

  panic("Z-day is upon us! Undead thread!"); // We should never get here, because I just removed the thread
                                             // from circulation.
}

void Sleep(time_t iSeconds, CBaseThread *pThread)
{
  if(!pThread) { pThread = GetCurrentThread(); }
  DASSERT(false); // not currently implemented, as that would involve clock
                  // callbacks and whatnot -- a bunch of infrastructure.
}

void CBaseScheduler::Dispatch(CBaseThread *pThread)
{
  if(!pThread) { pThread = GetNextThread(); }
  DASSERT(pThread);
  CBaseThread *pOldThread = GetCurrentThread();
  NewCurrentThread(pThread);
  pThread->Dispatch(pOldThread);
}

CBaseThread *CBaseScheduler::GetNextThread()
{
  // If the override returns a thread, run it; else, idle.
  CBaseThread *pThread = GetNextThreadDerived();
  return pThread ? pThread : m_pIdleThread;
}

////////////////////////////////////////////////////////////////////////////////
// CQueueScheduler
#define CQS_MAXTHREADS 32

CQueueScheduler::CQueueScheduler()
{
  m_pQueue = NULL;
  m_ixStart = m_cQueue = 0;
}

CQueueScheduler::~CQueueScheduler()
{
  DASSERT(0 == m_cQueue); // We're being destructed when there are still threads?
  if(m_pQueue) {
    KDelete [] m_pQueue;
    m_pQueue = NULL;
  }
}

KRESULT CQueueScheduler::InitDerived()
{
  DASSERT(!m_pQueue);
  m_pQueue = KNew CBaseThread *[CQS_MAXTHREADS];
  if(!m_pQueue) { return E_OUTOFMEMORY; }
  memzero(m_pQueue, sizeof(CBaseThread *) * CQS_MAXTHREADS);
  m_ixStart = m_cQueue = 0;
  return K_OK;
}

KRESULT CQueueScheduler::AddThread(CBaseThread *pThread)
{
  if(m_cQueue >= CQS_MAXTHREADS) { return E_BUFFERFULL; } // TODO
  const int ix = (m_ixStart + m_cQueue) % CQS_MAXTHREADS;
  DASSERT(NULL == m_pQueue[ix]);
  m_pQueue[ix] = pThread;
  m_cQueue += 1;
  return K_OK;
}

CBaseThread *CQueueScheduler::GetNextThreadDerived()
{
  if(m_cQueue <= 0) { return NULL; }
  CBaseThread *pThread = m_pQueue[m_ixStart];
  m_pQueue[m_ixStart] = NULL;
  m_cQueue -= 1;
  m_ixStart = (m_ixStart + 1) % CQS_MAXTHREADS;
  return pThread;
}

virtual void RemoveThread(CBaseThread *pThread)
{
  int ii;
  for(ii = 0; ii < m_cQueue; ii++)
  {
    const int ix = (m_ixStart + ii) % CQS_MAXTHREADS;
    if(m_pQueue[ix] == pThread) { break; }
  }
  if(ii == m_cQueue)
  {
    DASSERT(false); // Couldn't find thread.
  }
  else
  {
    for(; ii < m_cQueue - 1; ii++)
    {
      const int ix = (m_ixStart + ii) % CQS_MAXTHREADS;
      const int ixNext = (m_ixStart + ii + 1) % CQS_MAXTHREADS;
      m_pQueue[ix] = m_pQueue[ixNext];
    }
    m_pQueue[(m_ixStart + m_cQueue - 1) % CQS_MAXTHREADS] = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////
// CThreadBase -- declaration in arch/thread.h

CThreadBase::CThreadBase()
{
  m_pszName = NULL;
  m_eEntryMode = ENTRYMODE_NONE;
  m_uEntry.pVoid = NULL;
}

CThreadBase::~CThreadBase()
{
  if(m_pszName)
  {
    KDelete [] m_pszName;
    m_pszName = NULL;
  }
}

KRESULT Init(const char *pszName)
{
  m_pszName = strdup(pszName);
  if(!m_pszName) { return E_OUTOFMEMORY; }
  return K_OK;
}

KRESULT Init(const char *pszName, ICallable *pCallable)
{
  RETURN_FAILED(Init(pszName));
  m_eEntryMode = ENTRYMODE_CALLABLE;
  m_uEntry.pCallable = pCallable;
  return K_OK;
}

KRESULT Init(const char *pszName, void (*func)())
{
  RETURN_FAILED(Init(pszName));
  m_eEntryMode = ENTRYMODE_FUNC1;
  m_uEntry.func1 = func;
  return K_OK;
}

void Entry()
{
  DASSERT(m_eEntryMode > ENTRYMODE_NONE && m_eEntryMode < ENTRYMODE_COUNT);
  DASSERT(m_uEntry.pVoid);

  switch(m_eEntryMode)
  {

    case ENTRYMODE_CALLABLE:
      m_uEntry.pCallable->operator ()();
      break;

    case ENTRYMODE_FUNC1:
      m_uEntry.func1();
      break;

    default:
      DASSERT(false); // put new ones here
      break;

  }

  DASSERT(false); // We're not supposed to get here...
  Global::pGScheduler->Exit();
}
