#include <hardware/kb.h>
#include "kb_layout.h"

KB_KEY *rgLayouts[KB_LAYOUT_COUNT] =
{
    KB_MAPPING_US
};

KB_KEY MapKeystroke(int ixKey, KB_LAYOUT eLayout)
{
  DASSERT(eLayout >= 0);
  DASSERT(eLayout < KB_LAYOUT_COUNT);
  DASSERT(ixKey > 0);
  DASSERT(ixKey < KB_MAPSIZE);
  return rgLayouts[eLayout][ixKey];
}
