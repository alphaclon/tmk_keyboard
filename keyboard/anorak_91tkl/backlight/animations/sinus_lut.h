#ifndef SINUS_LUT_H
#define SINUS_LUT_H

#include "animation_enable.h"
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifdef ANIMATION_ENABLE_FLOATING_PLASMA

extern const uint16_t sin_lut[256] PROGMEM;

#endif

#endif
