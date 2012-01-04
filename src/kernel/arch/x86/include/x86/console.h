#pragma once

#include "precomp.h"
#include "arch/console.h"


class CDMAConsole : public IConsole {
  DECLARE_NOCOPY(CDMAConsole);

  uint16_t *pScreen;
  int iWidth, iHeight;
  int iPosition;

  const static char bAttribute;
  const static uint16_t iBlack;

  void Scroll();

 public:
  CDMAConsole();
  KRESULT Init(void *pScreen, int iWidth, int iHeight);
  KRESULT Write(const char *pszText);
  KRESULT WriteLine(const char *pszLine);
  KRESULT PutChar(const char szChar);
  KRESULT Clear();
  KRESULT Seek(int iPosition);
  int GetWidth() const;
  int GetHeight() const;
  bool IsValid() const;
};
 
