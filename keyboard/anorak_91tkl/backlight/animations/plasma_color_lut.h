#ifndef PLASMA_COLOR_LUT_H
#define PLASMA_COLOR_LUT_H

#include "animation_enable.h"
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifdef ANIMATION_ENABLE_FLOATING_PLASMA

extern const uint8_t PlasmaColorSpace[3 * 3 * 256] PROGMEM;

#endif
#endif
