#pragma once

class ICallable
{
public:
  virtual void operator ()() = 0;
};

class CBaseThread
{
public:

  CBaseThread();
  virtual ~CBaseThread();

  KRESULT Init(const char *pszName); // The thread represents what's currently executing.
  KRESULT Init(const char *pszName, ICallable *pCallable);
  KRESULT Init(const char *pszName, void (*func)());
  const char *GetName() {return m_pszName;}

  virtual void Dispatch(CBaseThread *pOldThread) = 0;

protected:

  void Entry();

private:

  enum ENTRYMODE {
    ENTRYMODE_NONE,
    ENTRYMODE_CALLABLE,
    ENTRYMODE_FUNC1,

    ENTRYMODE_COUNT
  } m_eEntryMode;

  union {
    void *pVoid;
    ICallable *pCallable;
    void (*func1)();
  } m_uEntry;

  char *m_pszName;
};

class CBaseScheduler
{
public:

  CBaseScheduler() {}
  virtual ~CBaseScheduler() {}
  // If NULL, use the current thread.
  virtual KRESULT   Init();
  KRESULT   Spawn(ICallable *pCallable, const char *pName);
  void      Start();
  void      Yield();
  void      Exit(CBaseThread *pThread=NULL);
  void      Sleep(time_t iSeconds, CBaseThread *pThread=NULL);
  const CBaseThread *GetCurrentThread() const {return m_pCurrentThread;}
  CBaseThread *GetCurrentThread() {return m_pCurrentThread;}
  void NewCurrentThread(CBaseThread *pThread);

protected:

  // Add a thread to the thread pool.
  virtual KRESULT   AddThread(CBaseThread *pThread) = 0;

  // Remove a thread from the thread pool.
  virtual void      RemoveThread(CBaseThread *pThread) = 0;

  // Pick a thread from the thread pool (and remove it from the pool). Returning
  // NULL will cause the system to idle.
  virtual CBaseThread * GetNextThreadDerived() = 0;

private:

  void Dispatch(CBaseThread *pThread=NULL); // NULL means GetNextThread().
  CBaseThread *GetNextThread();

  CBaseThread *m_pIdleThread;
  CBaseThread *m_pCurrentThread;

};

////////////////////////////////////////////////////////////////////////////////
// Dumb scheduler that uses a queue to determine who goes next.

class CQueueScheduler : public CBaseScheduler
{
public:
  CQueueScheduler();
  virtual ~CQueueScheduler();

protected:
  virtual KRESULT   AddThread(CBaseThread *pThread);
  virtual void      RemoveThread(CBaseThread *pThread);
  virtual CBaseThread * GetNextThread();
  virtual KRESULT   InitDerived() {}

private:
  int m_cQueue;
  int m_ixStart;
  CBaseThread *m_pQueue;
};

KRESULT Init_Scheduler();
