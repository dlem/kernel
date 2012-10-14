/**
 * Much of this file is based on:
 * http://wiki.osdev.org/C%2B%2B
 *
 * It provides support for the way GCC handles dynamic shared objects and
 * global/static destructors. Global/static constructors are handled in
 * main.asm.
**/

#define ATEXIT_MAX_FUNCS 64

typedef struct tATEXIT_ENTRY {  // valid iff pvDestructor != NULL
  void (*pvDestructor)(void *); // pointer to a registered destructor function
  void *pvArg;                  // pointer to be passed when the dtor is called
  void *pvHandle;               // ptr to corresponding shared object, or NULL
  void Set(void (*pvDestructor)(void *), void *pvArg, void *pvHandle)
  {
    this->pvDestructor = pvDestructor;
    this->pvArg = pvArg;
    this->pvHandle = pvHandle;
  }
} ATEXIT_ENTRY;

ATEXIT_ENTRY g_rgTable[ATEXIT_MAX_FUNCS] = {{0}};
int g_cDestructors = 0;

// This function is called when a pure virtual function can't be run, eg.
// due the virtual function table being overridden.
extern "C" __cdecl void __cxa_pure_virtual()
{
  DASSERT(false);
}

// handle for a dynamic shared object to be destructed - not sure what it's for...
void *__dso_handle;

// This function is used by GCC to register global/static destructors at the
// time their constructors are run or to register the destructors of dynamic
// shared objects.
extern "C" __cdecl int __cxa_atexit(void (*destructor)(void *), void *arg, void *__dso_handle)
{
  if(g_cDestructors >= ATEXIT_MAX_FUNCS)
  {
    DASSERT(false);
    return 1;
  }
  g_rgTable[g_cDestructors].Set(destructor, arg, __dso_handle);
  g_cDestructors++;
  return 0;
}

// Tells us to execute to provided destructor, which should have been
// registered in __cxa_atexit previously. Note that we have to execute them
// in reverse order of registration.
//
// TODO: each time we destruct an object with a __dso_handle, we're getting rid
// of a link to a dynamic shared object - ideally, we'd keep track of them so
// we know when an object is no longer in use (?).
extern "C" __cdecl void __cxa_finalize(void (*destructor)(void *))
{
  if(!destructor) // We're supposed to execute all of them.
  {
    for(int ix = g_cDestructors - 1; ix >= 0; ix--)
    {
      g_rgTable[ix].pvDestructor(g_rgTable[ix].pvArg);
      g_rgTable[ix].pvDestructor = NULL;
    }
    g_cDestructors = 0;
  }
  else
  {
    for(int ix = g_cDestructors - 1; ix >= 0; ix--)
    {
      if(g_rgTable[ix].pvDestructor == destructor)
      {
        g_rgTable[ix].pvDestructor(g_rgTable[ix].pvArg);
        g_cDestructors--;
        for(int ii = ix; ii < g_cDestructors; ii++)
        {
          g_rgTable[ix] = g_rgTable[ix + 1];
        }
        g_rgTable[g_cDestructors].pvDestructor = NULL;
      }
    }
  }
}

/**
 * GCC puts mutex guards around calls to the constructors of local static
 * variables. That's what this shindig is about.
**/

namespace __cxxabivl
{
  typedef QWORD __guard;

  // TODO
  extern "C" int __cxa_guard_acquire(__guard *)
  {
    DASSERT(false); // not implemented
    return 1;
  }
  extern "C" void __cxa_guard_release(__guard *)
  {
    DASSERT(false); // not implemented
  }
  extern "C" void __cxa_guard_abort(__guard *)
  {
    DASSERT(false); // not implemented
  }
}
