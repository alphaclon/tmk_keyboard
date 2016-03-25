/*
Copyright 2016 Moritz Wenk <MoritzWenk@web.de>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _IS31FL3731DEFAULTS_H_
#define _IS31Fl3731DEFAULTS_H_

#include <inttypes.h>

#define ISSI_ADDR_DEFAULT 0x74

// TODO Needs to be defined per keyboard
#define IS31FL3731_TotalChannels 144
#define IS31FL3731_BufferLength 144

/*
 * LED Mask
 *
 */

extern const uint8_t ISSILedMask[];

#define ISSILedMaskList \
0xFF, 0xFF, /* C1-1 -> C1-16 */ \
0xFF, 0xFF, /* C2-1 -> C2-16 */ \
0xFF, 0xFF, /* C3-1 -> C3-16 */ \
0xFF, 0xFF, /* C4-1 -> C4-16 */ \
0xFF, 0xFF, /* C5-1 -> C5-16 */ \
0xFF, 0xFF, /* C6-1 -> C6-16 */ \
0xFF, 0xFF, /* C7-1 -> C7-16 */ \
0xFF, 0xFF, /* C8-1 -> C8-16 */ \
0xFF, 0xFF, /* C9-1 -> C9-16 */

/*
 * LED Default Brightness
 *
 * By default, all LEDs are set to max brightness
 * Each LED channel supports 256 levels (8-bit control)
 */

#define ISSILedDefaultBrightness 0x40

#endif
