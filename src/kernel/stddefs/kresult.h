#pragma once

#if KDEBUG

class __KRESULT {

 private:
  mutable bool fChecked;
  int iResult;

 public:  
  __KRESULT();
  __KRESULT(const __KRESULT &kr);
  __KRESULT(int iResult);
  ~__KRESULT();

  int __get() const;
  void operator=(const __KRESULT &kr);
  int operator=(int iResult);
  operator int() const;
  int GetErrorCode() const;
  
};

#define SUCCEEDED(kr) (kr.__get() == K_OK)
#define IGNOREKR(kr) (void)(kr.__get())

#else

#define SUCCEEDED(kr) (kr == K_OK)
#define IGNOREKR(kr) (void)(kr)

#endif

#define FAILED(kr) (!SUCCEEDED(kr))
#define RETURN_FAILED(kr) { if(FAILED(kr)) return kr; } 
