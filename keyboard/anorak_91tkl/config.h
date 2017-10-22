/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

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

#ifndef CONFIG_H
#define CONFIG_H

#define VENDOR_ID       0x1209
#define PRODUCT_ID      0xAA91
#define DEVICE_VER      0x0204
#define MANUFACTURER    Anorak
#define PRODUCT         91tkl
#define DESCRIPTION     t.m.k. keyboard firmware for Anorak 91tkl
#define SERIALNUMBER    1234

// 55438313133351211180

/* matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 17

#define BACKLIGHT_LEVELS 8

/* key combination for command */
#define IS_COMMAND() (keyboard_report->mods == (MOD_BIT(KC_LGUI) | MOD_BIT(KC_LCTRL)))

/* period of tapping(ms) */
#define TAPPING_TERM    300
/* tap count needed for toggling a feature */
#define TAPPING_TOGGLE  5
/* Oneshot timeout(ms) */
#define ONESHOT_TIMEOUT 300

/* Boot Magic salt key: Space */
#define BOOTMAGIC_KEY_SALT      KC_SPACE

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */
/* disable debug print */
//#define NO_DEBUG
//#define NO_DEBUG_LEDS

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

#endif
