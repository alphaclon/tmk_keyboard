/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>
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

#include "config.h"
#include "keymap_common.h"
#include "sleep_led.h"
#include "backlight/animations/animation.h"
#include "backlight/backlight_91tkl.h"
#include "backlight/sector/sector_control.h"

/*
 *  Keymaps
 *
 */

#define KBLAYER_COUNT          1
#define KBLAYER_DEFAULT        0
#define LAYER_FN0_MEDIA        KBLAYER_COUNT


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
 *    0    1   2   3   4   5    6   7   8   9   10   11  12  13   14  15  16
 *
 * ,---.,---.,---------------.,---------------.,---------------.  ,-----------.
 * |Esc||Esc||F1 |F2 |F3 |F4 ||F5 |F6 |F7 |F8 ||F9 |F10|F11|F12|  |PrS|ScL|Pau|
 * `---'`---'`---------------'`---------------'`---------------'  `-----------'
 * ,-----------------------------------------------------------.  ,-----------.
 * |~  |  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Backsp |  |Ins|Hom|PgU|
 * |-----------------------------------------------------------|  |-----------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|     |  |Del|End|PgD|
 * |-----------------------------------------------------------|  `-----------'
 * |Caps  |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Return  |
 * |-----------------------------------------------------------|  ,-----------.
 * |Shft|ISO|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift     |  |FN1|Up |FN2|
 * |-----------------------------------------------------------|  ,-----------.
 * |Ctl|Gui|Alt|         Space               |AltGr|FN1|App|Ctl|  |Lef|Dow|Rig|
 * `-----------------------------------------------------------'  `-----------'
 */

/*
 * Add different layouts. If no layout is defined the default layout will be set to ANSI.
*/

//                            0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16
//                            A    B    C    D    E    F    G    H    I    J    K    L    M    N    O    P    Q

#define KEYMAP_ISO( \
                            K5A, K5B, K5C, K5D, K5E, K5F, K5G, K5H, K5I, K5J, K5K, K5L, K5M, K5N, K5O, K5P, K5Q,  \
                            K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H, K4I, K4J, K4K, K4L, K4M, K4N, K4O, K4P, K4Q,  \
                            K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H, K3I, K3J, K3K, K3L, K3M,      K3O, K3P, K3Q,  \
                            K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2I, K2J, K2K, K2L, K2M, K2N,                 \
                            K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1I, K1J, K1K, K1L, K1M,      K1O, K1P, K1Q,  \
                            K0A, K0B, K0C, K0D,                               K0K, K0L, K0M, K0N, K0O, K0P, K0Q ) \
                                                                                                                  \
     KEYMAP_91TKL(  \
                            K5A, K5B, K5C, K5D, K5E, K5F, K5G, K5H, K5I, K5J, K5K, K5L, K5M, K5N, K5O, K5P, K5Q,  \
                            K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H, K4I, K4J, K4K, K4L, K4M, K4N, K4O, K4P, K4Q,  \
                            K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H, K3I, K3J, K3K, K3L, K3M,  NO, K3O, K3P, K3Q,  \
                            K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2I, K2J, K2K, K2L, K2M, K2N,  NO,  NO,  NO,  \
                            K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1I, K1J, K1K, K1L, K1M,  NO, K1O, K1P, K1Q,  \
                            K0A, K0B, K0C, K0D,  NO,  NO,  NO,  NO,  NO,  NO, K0K, K0L, K0M, K0N, K0O, K0P, K0Q  )

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] =
{
    /*
     * 0: default layer
     *
     * FN0:
     * FN1:
     * FN2:
     * FN3:
     *
     *     0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
     *     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O     P     Q
     */
    KEYMAP_ISO(\
         ESC,  FN1,   F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,  F10,  F11,  F12,  PSCR, SLCK,  BRK,  \
         GRV,    1,    2,    3,    4,    5,    6,    7,    8,    9,    0, MINS,  EQL, BSPC,   INS, HOME, PGUP,  \
         TAB,    Q,    W,    E,    R,    T,    Y,    U,    I,    O,    P, LBRC, RBRC,         DEL,  END, PGDN,  \
        CAPS,    A,    S,    D,    F,    G,    H,    J,    K,    L, SCLN, QUOT, NUHS,  ENT,                     \
        LSFT, NUBS,    Z,    X,    C,    V,    B,    N,    M, COMM,  DOT, SLSH, RSFT,         FN2,   UP,  FN3,  \
        LCTL, LGUI, LALT,  SPC,                                     RALT,  FN0,  APP, RCTL,  LEFT, DOWN, RGHT  ),


    /*
     * 1: FN0-Layer, media keys
     *     0     1     2     3     4     5     6     7     8     9    10    11    12    13     14    15   16
     *     A     B     C     D     E     F     G     H     I     J     K     L     M     N      O     P    Q
     */
    KEYMAP_ISO(\
         TRNS, TRNS, TRNS, TRNS, TRNS,TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,  TRNS, TRNS, TRNS,   \
  	     TRNS, TRNS, TRNS, TRNS, TRNS,TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,        TRNS, TRNS, TRNS,   \
	     TRNS, TRNS, TRNS, TRNS, TRNS,TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,  TRNS, TRNS, TRNS,   \
         TRNS, TRNS, TRNS, TRNS, TRNS,TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,                      \
		 TRNS, TRNS, TRNS, TRNS, TRNS,TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,        TRNS,  TRNS, TRNS,  \
		 TRNS, TRNS, TRNS, TRNS,                                    TRNS, TRNS, TRNS, TRNS,  TRNS,  TRNS, TRNS  )
};

/*
 *
 *
 *  Actions
 *
 *
 */

/* id for user defined function/macro */
enum function_id {
	FN91_Backlight_Sector_Toggle,
	FN91_Backlight_Sector_Select,
	FN91_Backlight_Sector_Increase_All,
	FN91_Backlight_Sector_Decrease_All,
	FN91_Backlight_Sector_Increase_Selected,
	FN91_Backlight_Sector_Decrease_Selected,
	FN91_Backlight_Sector_Save_State,
	FN91_Backlight_Sector_Next_Custom_Map,
	FN91_Backlight_Animate_Toggle,
	FN91_Backlight_Animate_Increase_Speed,
	FN91_Backlight_Animate_Decrease_Speed,
	FN91_Backlight_Animate_Next,
	FN91_Backlight_Animate_Prev,
	FN91_Backlight_Dump
};

enum macro_id
{
    FN91_Macro_Cursor_Extra_Left,
    FN91_Macro_Cursor_Extra_Right
};

/*
 * Fn action definition
 */
const action_t PROGMEM fn_actions[] =
{
	[0] = ACTION_LAYER_MOMENTARY(LAYER_FN0_MEDIA),
	[1] = ACTION_LAYER_MOMENTARY(LAYER_FN0_MEDIA),

	[2] = ACTION_MACRO(FN91_Macro_Cursor_Extra_Left),
	[3] = ACTION_MACRO(FN91_Macro_Cursor_Extra_Right),

    [4] = ACTION_BACKLIGHT_TOGGLE(),
	[5] = ACTION_FUNCTION(FN91_Backlight_Sector_Toggle),
	[6] = ACTION_FUNCTION(FN91_Backlight_Sector_Save_State),

	[7] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Increase_All, hue),
	[8] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Increase_All, saturation),
	[9] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Increase_All, value),

   [10] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Decrease_All, hue),
   [11] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Decrease_All, saturation),
   [12] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Decrease_All, value),

   [13] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Increase_Selected, hue),
   [14] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Increase_Selected, saturation),
   [15] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Increase_Selected, value),

   [16] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Decrease_Selected, hue),
   [17] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Decrease_Selected, saturation),
   [18] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Decrease_Selected, value),

   [19] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Select, WASDKeys),
   [20] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Select, ControlKeys),
   [21] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Select, FunctionKeys),
   [22] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Select, CursorKeys),
   [23] = ACTION_FUNCTION_OPT(FN91_Backlight_Sector_Select, OtherKeys),

   [24] = ACTION_FUNCTION(FN91_Backlight_Sector_Next_Custom_Map),

   [25] = ACTION_FUNCTION(FN91_Backlight_Animate_Toggle),
   [26] = ACTION_FUNCTION(FN91_Backlight_Animate_Prev),
   [27] = ACTION_FUNCTION(FN91_Backlight_Animate_Next),
   [28] = ACTION_FUNCTION(FN91_Backlight_Animate_Decrease_Speed),
   [29] = ACTION_FUNCTION(FN91_Backlight_Animate_Increase_Speed),

   [30] = ACTION_FUNCTION(FN91_Backlight_Dump),
};

/*
 * Macro definition
 */
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    switch (id)
    {
    case FN91_Macro_Cursor_Extra_Left:
        return (record->event.pressed ? MACRO(D(LCTL), D(C), END) : MACRO(U(LCTL), U(C), END));
        break;
    case FN91_Macro_Cursor_Extra_Right:
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
    if (record->event.pressed)
    {
        switch (id)
        {
        case FN91_Backlight_Sector_Select:
            sector_select(SECTOR_BV(opt));
            break;
        case FN91_Backlight_Sector_Toggle:
            sector_toggle_selected();
            break;
        case FN91_Backlight_Sector_Increase_All:
            sector_increase_hsv_color(opt);
            break;
        case FN91_Backlight_Sector_Decrease_All:
            sector_decrease_hsv_color(opt);
            break;
        case FN91_Backlight_Sector_Increase_Selected:
        	sector_selected_increase_hsv_color(opt);
            break;
        case FN91_Backlight_Sector_Decrease_Selected:
        	sector_selected_decrease_hsv_color(opt);
            break;
        case FN91_Backlight_Sector_Save_State:
            sector_save_state();
            break;
        case FN91_Backlight_Dump:
        	break;
        case FN91_Backlight_Animate_Toggle:
            animation_toggle();
            break;
        case FN91_Backlight_Animate_Increase_Speed:
            animation_increase_speed();
            break;
        case FN91_Backlight_Animate_Decrease_Speed:
            animation_decrease_speed();
            break;
        case FN91_Backlight_Animate_Next:
        	animation_next();
            break;
        case FN91_Backlight_Animate_Prev:
        	animation_previous();
            break;
        }
    }
}
