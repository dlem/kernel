#include "precomp.h"
#include "x86/thread.h"
#include "thread.h" // CBaseThread

class CX86Thread : CBaseThread
{
public:
  void Dispatch(CBaseThread *pOldThread);
};


void CX86Thread::Dispatch(CBaseThread *pOldThread)
{

}

CBaseThread *Arch_GetNewThread()
{
  return KNew CX86Thread;
}

void Arch_Idle()
{
  asm_idle();
}
