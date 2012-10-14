#include <main/kstdlib.h>
#include <main/global.h>

void __debugbreak() {}

void panic(const char *msg)
{
  kprintf(msg);
  kprintf("\nPanicking!\n");
  kprintf("Hanging indefinitely\n");
  __debugbreak(); // In case the console isn't initialized...

  while(1);
}

void __DASSERT(int bCond, const char *pszCond, const char *pszFile, int iLine)
{
  static bool bDieOnAssert = false;
  static bool bBreakOnAssert = true;

  if(!bCond)
  {
    kprintf("\n%s:%d: Assertion Failed: %s\n", pszFile, iLine, pszCond);
    if(bBreakOnAssert) __debugbreak();
    if(bDieOnAssert) panic();
  }
}
