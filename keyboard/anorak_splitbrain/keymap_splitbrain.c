/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>
Copyright 2016 Moritz Wenk <>


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

#include "config.h"
#include "keymap_common.h"
#include "backlight/backlight_kiibohd.h"
#include "sleep_led.h"

#ifndef __AVR_ARCH__
#define PROGMEM
#endif

/*
 *  Keymaps
 *
 */

#define KBLAYER_COUNT          1

#define KBLAYER_DEFAULT        0

#define LAYER_FN0_MEDIA        KBLAYER_COUNT
#define LAYER_FN1_SELECT_LAYER KBLAYER_COUNT + 1

/*
 * Tenkeyless keyboard default layout, ISO & ANSI (ISO is between Left Shift
 * and Z, and the ANSI \ key above Return/Enter is used for the additional ISO
 * switch in the ASD row next to enter.  Use NUBS as keycode for the first and
 * NUHS as the keycode for the second.
 *
 * Layer 0: Default ISO
 *
 * Ten-key-less, cursor keys on the right side
 *
 * ,---.,---------------.,---------------.,---------------.,-------.,-----------.
 * |Esc||F1 |F2 |F3 |F4 ||F5 |F6 |F7 |F8 ||F9 |F10|F11|F12||FN4|FN5||PrS|ScL|Pau|
 * `---'`---------------'`---------------'`---------------'`-------'`-----------'
 * ,-----------------------------------------------------------.    ,-----------.
 * |~  |  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Backsp |    |Ins|Hom|PgU|
 * |-----------------------------------------------------------|    |-----------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|     |    |Del|End|PgD|
 * |-----------------------------------------------------------|    `-----------'
 * |Caps  |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Return  |
 * |-----------------------------------------------------------|    ,-----------.
 * |Shft|ISO|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift     |    |FN1|Up |FN2|
 * |-----------------------------------------------------------|    ,-----------.
 * |Ctl|Gui|FN0|Alt|Space || Space         |Alt|FN1|Gui|App|Ctl|    |Lef|Dow|Rig|
 * `-----------------------------------------------------------'    `-----------'
 */

/*
 * Add different layouts. If no layout is defined the default layout will be set to ANSI.
*/
#if defined(LAYOUT_ISO_CURSORKEYS_LEFT)
#define KEYMAP_ISO KEYMAP_ISO_CURSORKEYS_LEFT
#elif defined(LAYOUT_ISO_CURSORKEYS_RIGHT)
#define KEYMAP_ISO KEYMAP_ISO_CURSORKEYS_RIGHT
#else
    #error No cursorkeys layout defined!
#endif

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] =
{
    /*
     * 0: qwerty
     *     0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17
     *     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O     P     Q     R
     */
    KEYMAP_ISO(\
         ESC,   F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9, F10,   F11,  F12,    A,    B, PSCR, SLCK,  BRK,   \
         GRV,    1,    2,    3,    4,    5,    6,    7,    8,    9,    0, MINS,  EQL, BSPC,        INS, HOME, PGUP,   \
         TAB,    Q,    W,    E,    R,    T,    Y,    U,    I,    O,    P, LBRC, RBRC,              DEL,  END, PGDN,   \
        CAPS,    A,    S,    D,    F,    G,    H,    J,    K,    L, SCLN, QUOT, NUHS,  ENT,                           \
        LSFT, NUBS,    Z,    X,    C,    V,    B,    N,    M, COMM,  DOT, SLSH, RSFT,              FN15,   UP, FN16,  \
        LCTL,  LGUI, FN0, LALT,  SPC,                    SPC, RALT,  FN0, RGUI,  APP, RCTL,        LEFT, DOWN, RGHT  ),
    /*
     * 1: media keys
     *     0     1     2     3     4     5     6     7     8     9    10    11    12    13     14    15   16    18
     *     A     B     C     D     E     F     G     H     I     J     K     L     M     N      O     P    Q     R
     */
    KEYMAP_ISO(\
        TRNS, MUTE, VOLD, VOLU, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,   \
		 FN6, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, MUTE, VOLD, VOLU, TRNS,       TRNS, TRNS, TRNS,   \
		 FN5, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,             TRNS, TRNS, TRNS,   \
         FN4,  FN9, FN10, FN11, FN12, TRNS, TRNS,  FN9, FN10, FN11, FN12, TRNS, TRNS, TRNS,                           \
         FN3, FN13, FN14, TRNS, TRNS, TRNS, TRNS, FN13, FN14, TRNS, TRNS, TRNS, TRNS,              FN3,  FN5, TRNS,   \
         FN2, TRNS, TRNS,  FN8,  FN7,                    FN7,  FN8, TRNS, TRNS, TRNS,  FN2,        FN4,  FN6, TRNS  )
};

/*
 *
 *
 *  Actions
 *
 *
 */

/* id for user defined function/macro */
/* id for user defined function/macro */
enum function_id {
	KIIBOHD_FUNCTION_Backlight_Toggle_Region,
	KIIBOHD_FUNCTION_Backlight_Select_Region,
	KIIBOHD_FUNCTION_Backlight_Increase_All,
	KIIBOHD_FUNCTION_Backlight_Decrease_All,
	KIIBOHD_FUNCTION_Backlight_Increase_Region,
	KIIBOHD_FUNCTION_Backlight_Decrease_Region,
	KIIBOHD_FUNCTION_Backlight_Save_Current_State,
	KIIBOHD_FUNCTION_Backlight_Breath,
	KIIBOHD_FUNCTION_Backlight_Animate,
	KIIBOHD_FUNCTION_Backlight_Dump
};

enum macro_id
{
    KIIBOHD_MACRO_EXTRA_LEFT,
    KIIBOHD_MACRO_EXTRA_RIGHT
};

/*
 * Fn action definition
 */
const action_t PROGMEM fn_actions[] =
{
	[0] = ACTION_LAYER_MOMENTARY(LAYER_FN0_MEDIA),
	[1] = ACTION_LAYER_MOMENTARY(LAYER_FN1_SELECT_LAYER),
	[2] = ACTION_BACKLIGHT_TOGGLE(),
	[3] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Decrease_All),
	[4] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Increase_All),
    [5] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Decrease_Region),
    [6] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Increase_Region),
	[7] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Toggle_Region),
	[8] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Save_Current_State),
	[9] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_WASD),     //BACKLIGHT_REGION_WASD
   [10] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_CONTROLS), //BACKLIGHT_REGION_CONTROLS
   [11] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_OTHER),    //BACKLIGHT_REGION_OTHER
   [12] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_LOGO),     //BACKLIGHT_REGION_LOGO
   [13] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Breath),
   [14] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Animate),
   [15] = ACTION_MACRO(KIIBOHD_MACRO_EXTRA_LEFT),
   [16] = ACTION_MACRO(KIIBOHD_MACRO_EXTRA_RIGHT),
};

/*
 * Macro definition
 */
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    switch (id)
    {
    case KIIBOHD_MACRO_EXTRA_LEFT:
        return (record->event.pressed ? MACRO(D(LCTL), D(C), END) : MACRO(U(LCTL), U(C), END));
        break;
    case KIIBOHD_MACRO_EXTRA_RIGHT:
        return (record->event.pressed ? MACRO(D(LCTL), D(V), END) : MACRO(U(LCTL), U(V), END));
        break;
    }

    return MACRO_NONE;
}

/*
 * user defined action function
 */
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    /*
    if (record->event.pressed) dprint("P");	else dprint("R");
    dprintf("%d", record->tap.count);
    if (record->tap.interrupted) dprint("i");
    dprint("\n");
    */

    if (record->event.pressed)
    {
        switch (id)
        {
        case KIIBOHD_FUNCTION_Backlight_Select_Region:
            backlight_select_region(BACKLIGHT_BV(opt));
            break;
        case KIIBOHD_FUNCTION_Backlight_Toggle_Region:
            backlight_toggle_selected_region();
            break;
        case KIIBOHD_FUNCTION_Backlight_Increase_All:
            backlight_increase_brightness();
            break;
        case KIIBOHD_FUNCTION_Backlight_Decrease_All:
            backlight_decrease_brightness();
            break;
        case KIIBOHD_FUNCTION_Backlight_Increase_Region:
            backlight_increase_brightness_selected_region();
            break;
        case KIIBOHD_FUNCTION_Backlight_Decrease_Region:
            backlight_decrease_brightness_selected_region();
            break;
        case KIIBOHD_FUNCTION_Backlight_Save_Current_State:
            backlight_save_region_states();
            break;
        case KIIBOHD_FUNCTION_Backlight_Breath:
        	//sleep_led_toggle();
            break;
        case KIIBOHD_FUNCTION_Backlight_Animate:
        	//backlight_test();
            break;
        case KIIBOHD_FUNCTION_Backlight_Dump:
        	backlight_dump_issi_state();
        	break;
        }
    }
}
