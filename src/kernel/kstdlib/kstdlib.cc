#include "precomp.h"
#include "global.h"
#include "kstdlib.h"
#include "arch/console.h"
#include <cstdarg>

#define KSTDLIB_ASSERTUNSAFE 1

#if KSTDLIB_ASSERTUNSAFE
# define KSTDLIB_UNSAFE DASSERT(false)
#else
# define KSTDLIB_UNSAFE
#endif

bool putchar(const char c)
{ 
  if(!Global::pGConsole) return E_UNINITIALIZED;
  return SUCCEEDED(Global::pGConsole->PutChar(c));
}
bool cls()
{
  if(!Global::pGConsole) return E_UNINITIALIZED;
  return SUCCEEDED(Global::pGConsole->Clear());
}
int strlen(const char *psz)
{
  int ix = 0;
  while(psz[ix]) ix++;
  return ix;
}
int strcpy(char *dst, const char *src)
{
  KSTDLIB_UNSAFE;
  int ix;
  for(ix = 0; src[ix]; ix++)
  {
    dst[ix] = src[ix];
  }
  dst[ix] = '\0';
  return ix;
}
int strcat(char *dst, const char *src)
{
  KSTDLIB_UNSAFE;
  const int ixBegin = strlen(dst);
  int ix;
  for(ix = 0; src[ix]; ix++)
  {
    dst[ixBegin + ix] = src[ix];
  }
  dst[ixBegin + ix] = '\0';
  return ixBegin + ix;
}
const char rgszDigits[] = "0123456789ABCDEF";
char *itoa(int iVal, char *psz, int iBase)
{
  KSTDLIB_UNSAFE;
  int ix = 0;

  if(iVal < 0)
  {
    psz[ix] = '-';
    ix++;
    iVal = -iVal;
  }
  else if(0 == iVal)
  {
    psz[ix] = '0';
    ix++;
  }

  int iTst = 1;
  while(iTst <= iVal)
  {
    iTst *= iBase;
  }
  iTst /= iBase;

  while(iVal > 0)
  {
    while(iTst > iVal)
    {
      iTst /= iBase;
      psz[ix] = '0';
      ix++;
    }
    while(iVal >= iTst && iTst > 0)
    {
      int nTimes = iVal / iTst;
      iVal %= iTst;
      iTst /= iBase;
      psz[ix] = rgszDigits[nTimes];
      ix++;
    }
  }
  psz[ix] = '\0';
  ix++;
  return psz;
}
int kprintf(const char *pszFmt, ...)
{
  va_list vlist;
  int64_t iData;
  int ixBuf;
  int nPrinted = 0;
  char rgszBuf[256];

  va_start(vlist, pszFmt);

  for(int ix = 0; pszFmt[ix] != '\0';)
  {
    if(pszFmt[ix] == '%')
    {
      ix++;
      switch(pszFmt[ix])
      {
        case '\0':
          DASSERT(false);
          break;          // let the loop see it and end
        case '%':
          putchar('%');
          nPrinted++;
          ix++;
          break;
        case 'd':
          *(int *)&iData = va_arg(vlist, int);
          VERIFY(SUCCEEDED(itoan(*(int *)&iData, rgszBuf, 10, NUMELMS(rgszBuf), NULL)));
          ix++;
          for(ixBuf = 0; rgszBuf[ixBuf]; ixBuf++)
          {
            putchar(rgszBuf[ixBuf]);
            nPrinted++;
          }
          break;
        case 's':
          *(const char **)&iData = va_arg(vlist, const char *);
          ix++;
          for(ixBuf = 0; ((const char *)iData)[ixBuf]; ixBuf++)
          {
            putchar(((const char *)iData)[ixBuf]);
            nPrinted++;
          }
          break;
        default:
          DASSERT(false);
          ix++;
          break;
      }
    }
    else // not a format specifier
    {
      putchar(pszFmt[ix]);
      ix++;
      nPrinted++;
    }
  } // for over input string

  va_end(vlist);

  return nPrinted;
}
int sprintf(char *buffer, const char *pszFmt, ...)
{
  KSTDLIB_UNSAFE;
  va_list vlist;
  int64_t iData;
  int ixBuf;
  int nPrinted = 0;
  int nWritten;

  va_start(vlist, pszFmt);

  for(int ix = 0; pszFmt[ix] != '\0';)
  {
    if(pszFmt[ix] == '%')
    {
      ix++;
      switch(pszFmt[ix])
      {
        case '\0':
          DASSERT(false);
          break;          // let the loop see it and end
        case '%':
          buffer[nPrinted] = '%';
          nPrinted++;
          ix++;
          break;
        case 'd':
          *(int *)&iData = va_arg(vlist, int);
          nWritten = 0;
          VERIFY(SUCCEEDED(itoan(*(int *)&iData, buffer + nPrinted, 10, INT_MAX, &nWritten)));
          ix++;
          nPrinted += nWritten;
          break;
        case 's':
          *(const char **)&iData = va_arg(vlist, const char *);
          ix++;
          for(ixBuf = 0; ((const char *)iData)[ixBuf]; ixBuf++)
          {
            buffer[nPrinted] = ((const char *)iData)[ixBuf];
            nPrinted++;
          }
          break;
        default:
          DASSERT(false);
          ix++;
          break;
      }
    }
    else // not a format specifier
    {
      buffer[nPrinted] = pszFmt[ix];
      ix++;
      nPrinted++;
    }
  } // for over input string

  va_end(vlist);

  return nPrinted;
}
KRESULT strcpyn(char *dst, int bufferSz, const char *src)
{
  KRESULT kr = K_OK;
  int ix;
  for(ix = 0; ix < bufferSz && src[ix]; ix++)
  {
    dst[ix] = src[ix];
  }
  if(ix == bufferSz)
  {
    kr = E_BUFFERTOOSMALL;
  }
  else
  {
    dst[ix] = '\0';
  }
  return kr;
}
KRESULT strcatn(char *buffer, int bufferSize, const char *str)
{
  KRESULT kr = K_OK;
  int ixBegin;
  ixBegin = strlen(buffer);
  int ix;
  for(ix = ixBegin; str[ix - ixBegin] && ix < bufferSize; ix++)
  {
    buffer[ix] = str[ix - ixBegin];
  }
  if(ix == bufferSize)
  {
    kr = E_BUFFERTOOSMALL;
  }
  else
  {
    buffer[ix] = '\0';
  }
  return kr;
}
KRESULT sprintfn(char *buffer, int bufferSize, const char *pszFmt, ...)
{
  if(!(buffer && pszFmt)) return E_POINTER;
  KRESULT kr = K_OK;
  va_list vlist;
  int64_t iData;
  int ixBuf;
  int nPrinted = 0;
  int nWritten;

  va_start(vlist, pszFmt);

  for(int ix = 0; pszFmt[ix] && nPrinted < bufferSize && SUCCEEDED(kr);)
  {
    if(pszFmt[ix] == '%')
    {
      ix++;
      switch(pszFmt[ix])
      {
        case '\0':
          DASSERT(false);
          break;          // let the loop see it and end
        case '%':
          buffer[nPrinted] = '%';
          nPrinted++;
          ix++;
          break;
        case 'd':
          *(int *)&iData = va_arg(vlist, int);
          nWritten = 0;
          kr = itoan(*(int *)&iData, buffer + nPrinted, 10, bufferSize - nPrinted, &nWritten);
          ix++;
          nPrinted += nWritten;
          break;
        case 's':
          *(const char **)&iData = va_arg(vlist, const char *);
          ix++;
          for(ixBuf = 0; ((const char *)iData)[ixBuf] && nPrinted < bufferSize; ixBuf++)
          {
            buffer[nPrinted] = ((const char *)iData)[ixBuf];
            nPrinted++;
          }
          break;
        default:
          DASSERT(false);
          ix++;
          break;
      }
    }
    else // not a format specifier
    {
      buffer[nPrinted] = pszFmt[ix];
      ix++;
      nPrinted++;
    }
  } // for over input string

  va_end(vlist);

  if(SUCCEEDED(kr))
  {
    if(nPrinted >= bufferSize)
    {
      kr = E_BUFFERTOOSMALL;
    }
    else
    {
      buffer[nPrinted] = '\0';
    }
  }

  return kr;
}
KRESULT itoan(int iVal, char *psz, int iBase, int iSz, int *nWritten)
{
  if(iBase <= 0) return E_INVALIDARG;
  if(iBase > 16) return E_UNIMPLEMENTED;
  if(!psz) return E_POINTER;

  // first, find number of digits required
  int nDigits = iVal < 0 ? 2 : 1;
  int absVal = iVal < 0 ? -iVal : iVal;
  for(int iTst = iBase; iTst <= absVal; iTst *= iBase) nDigits++;

  if(nDigits >= iSz) return E_BUFFERTOOSMALL;
  if(nWritten) *nWritten = nDigits;

  if(iVal < 0) {
    nDigits--;
    psz[nDigits] = '-';
  }
  for(int ix = nDigits - 1; ix >= 0; ix--)
  {
    psz[ix] = '0' + (char)(absVal % iBase);
    absVal /= iBase;
  }
  psz[nDigits] = '\0';
  return K_OK;
}
void memcpy(void *buf, const void *src, size_t sz)
{
  for(size_t ix = 0; ix < sz; ix++)
  {
    ((char *)buf)[ix] = ((char *)src)[ix];
  }
}
void memset(void *buf, char val, size_t sz)
{
  for(size_t ix = 0; ix < sz; ix++)
  {
    ((char *)buf)[ix] = val;
  }
}
void memzero(void *buf, size_t sz)
{
  memset(buf, 0, sz);
}

char *strdup(const char *psz)
{
  const int iLen = strlen(psz) + 1;
  char *pszDup = KNew char[iLen];
  if(!pszDup) { return NULL; }
  memcpy(pszDup, psz, iLen * sizeof(char));
  return pszDup;
}
