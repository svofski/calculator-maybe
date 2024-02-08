#pragma once

#ifdef __AVR__
#include "avr/pgmspace.h"
#else
#define PROGMEM

#define pgm_read_dword_near(x) (*(x))
#define pgm_read_byte(x) (*(x))

#endif
