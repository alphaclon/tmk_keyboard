#include "keymap_common.h"
#include "backlight.h"
#include "config.h"

/*
 *
 *
 *  Keymaps
 *
 *
 */

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * 0: Default layer
     * ,-------------------------.
     * |Esc  |   1|  2|  3|  4|  5|
     * |--------------------------|
     * |Tab  |   T|  Q|  W|  E|  R|
     * |--------------------------|
     * |RCtrl|   G|  A|  S|  D|  F|
     * |--------------------------|
     * |Shift|   B|  Z|  X|  C|  V|
     * |--------------------------|
     * |LCtrl|LAlt|Meta|FN0|Taunt|DuckJump FN7|Jump|
     * `---------------------------------------'
     *
     */
	KEYMAP_KIIBOHD(
        ESC,  1,   2,   3,   4,   5, \
        TAB,  T,   Q,   W,   E,   R, \
        FN8,  G,   A,   S,   D,   F, \
        LSFT, B,   Z,   X,   C,   V, \
        LCTL, LALT, LGUI, FN0 , SPC, FN7, SPC),
    /*
     * 1: FN0 layer
     *
     */
    KEYMAP_KIIBOHD(
        PAUS,   6,   7,   8,   9,   0,   \
        TRNS,VOLD,VOLU,MUTE,EJCT,TRNS,  \
         FN1, FN2, FN3, FN4, FN5, FN6,  \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,  \
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
    NONE,
};

enum macro_id {
    KIIBOHD_MACRO_DUCK_JUMP,
};

/*
 * Fn action definition
 */
const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_BACKLIGHT_TOGGLE(),
    [2] = ACTION_BACKLIGHT_INCREASE(),
    [3] = ACTION_BACKLIGHT_DECREASE(),
    [4] = ACTION_BACKLIGHT_LEVEL(BACKLIGHT_ALL),
    [5] = ACTION_BACKLIGHT_LEVEL(BACKLIGHT_WASD),
    [6] = ACTION_BACKLIGHT_LEVEL(BACKLIGHT_JUMP),
    [7] = ACTION_MACRO(KIIBOHD_MACRO_DUCK_JUMP),
    //ACTION_MODS_KEY(MOD_LCTL, KC_SPC)
    [8] = ACTION_MODS_TAP_KEY(MOD_RCTL, KC_F12),
};


/*
 * Macro definition
 */
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
#if 0
    switch (id) {
        case KIIBOHD_MACRO_DUCK_JUMP:
            return (record->event.pressed ?
                    MACRO( D(SPC) D(LCTL), END ) :
                    MACRO( U(SPC), END ));
    }
#endif
    return MACRO_NONE;
}
