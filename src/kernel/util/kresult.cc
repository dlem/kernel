#if KDEBUG

#include "precomp.h"
#include "kresult.h"
#include "kdebug.h"


// default constructor
__KRESULT::__KRESULT() : fChecked(true), iResult(E_UNSET)
{}

// copy constructor
__KRESULT::__KRESULT(const __KRESULT &kr) : fChecked(false)
{
  iResult = kr.__get();
}

__KRESULT::__KRESULT(int iResult) : fChecked(true), iResult(iResult)
{}

__KRESULT::~__KRESULT()
{
  DASSERT(fChecked); // you didn't check this returned KRESULT
}

int __KRESULT::operator=(int iResult)
{
  DASSERT(fChecked); // you're clobbering a returned KRESULT
  this->iResult = iResult;
  return iResult;
}


void __KRESULT::operator=(const __KRESULT &kr)
{
  DASSERT(fChecked); // you're clobbering a returned KRESULT
  this->iResult = kr.__get();
}

__KRESULT::operator int() const
{
  return __get();
}


int __KRESULT::__get() const
{
  DASSERT(iResult != E_UNSET); // you're checking a KRESULT that wasn't set
  fChecked = true;
  return iResult;
}

int __KRESULT::GetErrorCode() const
{
  return __get();
}

#endif // if KDEBUG
