#include "precomp.h"
#include "x86/clock.h"

KRESULT Clock::Init(size_t nTicksPerSecond)
{
  m_nTicksPerSecond = nTicksPerSecond;
  m_ticks = m_seconds = 0;
  return K_OK;
}

size_t Clock::GetSeconds()
{
  return m_seconds;
}
size_t Clock::GetMilliseconds()
{
  return m_nTicksPerSecond ? m_ticks * 1000 / m_nTicksPerSecond : 0;
}
void Clock::Tick()
{
  m_ticks += 1;
  if(m_ticks == m_nTicksPerSecond)
  {
    m_ticks = 0;
    m_seconds += 1;
  }
}
size_t Clock::GetPrecision()
{
  return m_nTicksPerSecond;
}
