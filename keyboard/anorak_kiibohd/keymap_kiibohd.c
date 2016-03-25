#include "keymap_common.h"
#include <avr/pgmspace.h>
#include "backlight_kiibohd.h"
#include "config.h"

/* Keymaps
 *
 *
 * ,-------------------------.
 * |Esc  |   1|  2|  3|  4|  5|
 * |--------------------------|
 * |Tab  |   T|  Q|  W|  E|  R|
 * |--------------------------|
 * |RCtrl|   G|  A|  S|  D|  F|
 * |--------------------------|
 * |Shift|   B|  Z|  X|  C|  V|
 * |--------------------------|
 * |LCtrl|FN0|Meta|LAlt|FN9|DuckJump FN7|Jump|
 * `---------------------------------------'
 *
 */

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * 0: Default layer
     *
     */
	KEYMAP_KIIBOHD(
        ESC,  1,   2,   3,   4,   5, \
        TAB,  T,   Q,   W,   E,   R, \
        FN1,  G,   A,   S,   D,   F, \
        LSFT, B,   Z,   X,   C,   V, \
        LCTL, FN0, LGUI, LALT, FN2, FN3, SPC),
    /*
     * 1: FN0 layer
     *
     */
    KEYMAP_KIIBOHD(
        PAUS,  F1,  F2,  F3,  F4,  F5,  \
		  F7,  F8,  F9, F10, F11, F12,  \
	     FN5, FN7, FN8, FN9,FN10,TRNS,  \
         FN6,VOLD,VOLU,MUTE,TRNS,TRNS,  \
         FN4,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS),
	/*
	 * 2: FN9 layer
	 *
	 */
	KEYMAP_KIIBOHD(
		PAUS,  F1,  F2,  F5,  F6,  F4,  \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,  \
		TRNS,TRNS,TRNS,   M,TRNS,TRNS,  \
		TRNS,   L,   Y,   N,COMM, DOT,  \
		TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS)
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
	KIIBOHD_FUNCTION_Backlight_WASD,
	KIIBOHD_FUNCTION_Backlight_JUMP,
	KIIBOHD_FUNCTION_Backlight_LOGO,
	KIIBOHD_FUNCTION_Backlight_ALL,
};

enum macro_id {
    KIIBOHD_MACRO_DUCK_JUMP,
};

/*
 * Fn action definition
 */
const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_MODS_TAP_KEY(MOD_RCTL, KC_F12),
    [2] = ACTION_LAYER_TAP_KEY(2, KC_H),
    [3] = ACTION_MACRO(KIIBOHD_MACRO_DUCK_JUMP),
    [4] = ACTION_BACKLIGHT_TOGGLE(),
    [5] = ACTION_BACKLIGHT_INCREASE(),
    [6] = ACTION_BACKLIGHT_DECREASE(),
    [7] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_WASD),
    [8] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_JUMP),
    [9] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_LOGO),
   [10] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_ALL),
};


/*
 * Macro definition
 */
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
	switch (id)
	{
	case KIIBOHD_MACRO_DUCK_JUMP:
		return (record->event.pressed ?
				MACRO(D(SPC), D(LCTL), END) :
				MACRO(U(SPC), U(LCTL), END));
		break;
	}
	return MACRO_NONE;
}

/*
 * user defined action function
 */
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
	if (record->event.pressed) dprint("P");	else dprint("R");
	dprintf("%d", record->tap.count);
	if (record->tap.interrupted) dprint("i");
	dprint("\n");

	if (record->event.pressed)
	{
	switch (id)
	{
	case KIIBOHD_FUNCTION_Backlight_WASD:
		backlight_set_region(BACKLIGHT_REGION_WASD);
		break;
	case KIIBOHD_FUNCTION_Backlight_JUMP:
		backlight_set_region(BACKLIGHT_REGION_JUMP);
		break;
	case KIIBOHD_FUNCTION_Backlight_LOGO:
		backlight_set_region(BACKLIGHT_REGION_LOGO);
		break;
	case KIIBOHD_FUNCTION_Backlight_ALL:
		backlight_set_region(BACKLIGHT_REGION_ALL);
		break;
	}
	}
}
