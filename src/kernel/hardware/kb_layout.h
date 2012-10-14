/**
 * On scancodes: http://www.quadibloc.com/comp/scan.htm
 */

#pragma once
#include <hardware/kb.h>

#undef SC
#undef BEGIN_MAPPING
#undef END_MAPPING

#define SC(k) KB_SPECIALKEY_##k // saves typing...

#define BEGIN_MAPPING(mapname)\
  KB_KEY KB_MAPPING_##mapname[KB_MAPSIZE] =\
  {

#define END_MAPPING(mapname)\
  };

////////////////////////////////////////////////////////////////////////////////
// Keyboard mappings
BEGIN_MAPPING(US)
    SC(NONE),
    SC(ESC),
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    SC(BACKSPACE),
    SC(TAB),
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '[',
    ']',
    SC(ENTER),
    SC(ENTERKP),
    SC(LCTRL),
    SC(RCTRL),
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ':',
    '\'',
    '~',
    SC(LSHIFT),
    '\\',
    SC(INT2),
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    ',',
    '.',
    '/',
    SC(FWSLASHKP),
    SC(RSHIFT),
END_MAPPING(US)

#undef BEGIN_MAPPING
#undef END_MAPPING
#undef SC
