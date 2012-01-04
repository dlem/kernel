#pragma once

#include "precomp.h"


class IConsole {
 public:
  virtual ~IConsole() {}
  virtual KRESULT Write(const char *pszText) = 0;
  virtual KRESULT WriteLine(const char *pszLine) = 0;
  virtual KRESULT PutChar(const char szChar) = 0;
  virtual KRESULT Clear() = 0;
  virtual KRESULT Seek(int iPosition) = 0;
  virtual int     GetWidth() const = 0;
  virtual int     GetHeight() const = 0;
  virtual bool    IsValid() const = 0;
};
