#include "console.h"

const char CDMAConsole::bAttribute = 0x07;
const uint16_t CDMAConsole::iBlack = 0x0000;

CDMAConsole::CDMAConsole() : pScreen(NULL) {}

void CDMAConsole::Scroll()
{
  DASSERT(IsValid());
  uint16_t *pBuf = (uint16_t *)pScreen;
  for(int ix = iWidth; ix < iWidth * iHeight; ix++)
  {
    pBuf[ix - iWidth] = pBuf[ix];
  }

  for(int ix = iWidth * (iHeight - 1); ix < iWidth * iHeight; ix++)
  {
    pBuf[ix] = iBlack;
  }

  iPosition -= iWidth;
}

KRESULT CDMAConsole::Init(void *pScreen, int iWidth, int iHeight)
{
  this->pScreen = (uint16_t *)pScreen;
  this->iWidth = iWidth;
  this->iHeight = iHeight;
  this->iPosition = 0;

  return K_OK;
}

KRESULT CDMAConsole::Write(const char *pszText)
{
  DASSERT(IsValid());
  if(!pszText) return E_POINTER;
  while(*pszText)
  {
    RETURN_FAILED(PutChar(*(pszText++)));
  }

  return K_OK;
}

KRESULT CDMAConsole::WriteLine(const char *pszLine)
{
  DASSERT(IsValid());
  RETURN_FAILED(Write(pszLine));
  RETURN_FAILED(PutChar('\n'));

  return K_OK;
}

KRESULT CDMAConsole::PutChar(char szChar)
{
  DASSERT(IsValid());
  if('\n' == szChar)
  {
    iPosition += iWidth - iPosition % iWidth;
  }


  if(iPosition >= iHeight * iWidth)
  {
    Scroll();
  }

  if('\n' != szChar)
  {

    BYTE *pbLoc = (BYTE *)(pScreen + iPosition);
    pbLoc[0] = (BYTE)szChar;
    pbLoc[1] = bAttribute;

    iPosition++;
  }

  return K_OK;
}

KRESULT CDMAConsole::Clear()
{
  DASSERT(IsValid());
  uint16_t *pBuf = pScreen;

  for(int ix = 0; ix < iWidth * iHeight; ix++)
  {
    pBuf[ix] = iBlack;
  }

  iPosition = 0;

  return K_OK;
}

KRESULT CDMAConsole::Seek(int iPosition)
{
  DASSERT(IsValid());
  if(iPosition >= iWidth * iHeight || iPosition < 0) return E_INVALIDARG;

  this->iPosition = iPosition;
  return K_OK;
}

int CDMAConsole::GetWidth() const
{
  return iWidth;
}

int CDMAConsole::GetHeight() const
{
  return iHeight;
}

bool CDMAConsole::IsValid() const
{
  return pScreen != NULL;
}
