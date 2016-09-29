#include "keymap_common.h"
#include <avr/pgmspace.h>
#include "led_backlight/backlight_kiibohd.h"
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
     * 0: Default layer (QWERT)
     *
     */
    KEYMAP_KIIBOHD(
         ESC,    1,    2,    3,    4,  F12, \
         TAB,    Q,    W,    E,    R,    T, \
         FN1,    A,    S,    D,    F,    G, \
        LSFT,    Z,    X,    C,    V,    B, \
        LCTL,  FN0, FN14, LALT,  FN2,  FN3, SPC),
    /*
     * 1: layer 1, the layer selection layer
     *    activated by FN14
     *
     */
    KEYMAP_KIIBOHD(
        PAUS, FN16, FN17, FN18, FN19, FN20, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, FN14, TRNS, TRNS, TRNS, TRNS),
    /*
     * 2: layer 2, media and backlight layer,
     *    activated by FN0
     *
     */
    KEYMAP_KIIBOHD(
        PAUS, TRNS, TRNS, TRNS, TRNS, TRNS, \
         FN5, TRNS, TRNS, TRNS, TRNS, TRNS, \
         FN6,  FN7,  FN8,  FN9, FN10, FN11, \
         FN4, VOLD, VOLU, MUTE, TRNS, TRNS, \
        FN15, TRNS, FN12, FN13, TRNS, TRNS, TRNS),
    /*
     * 3: layer 3, TF2 F-key layer,
     *    activated by FN2 (leftmost thumb key)
     *
     */
    KEYMAP_KIIBOHD(
        TRNS,   F1,   F2,   F5,   F6, TRNS, \
          F1,   F2,   F3,   F4,   F5,   F6, \
          F7,   F8,   F9,  F10,  F11,  F12, \
           0,    5,    6,    7,    8,    9, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS),
    /*
     * 4: layer 1, QWERT, normal space on middle thumb key
     *    toggled by FN14-layer: FN16
     *
     */
    KEYMAP_KIIBOHD(
         ESC,    1,    2,    3,    4,  F12, \
         TAB,    Q,    W,    E,    R,    T, \
         FN1,    A,    S,    D,    F,    G, \
        LSFT,    Z,    X,    C,    V,    B, \
        LCTL,  FN0, FN14, LALT,  FN2,  SPC, KC_ENT),
    /*
     * 5: shifted layer (TQWER)
     *    toggled by FN14-layer: FN17
     *
     */
    KEYMAP_KIIBOHD(
         ESC,    1,    2,    3,    4,  F12, \
         TAB,    T,    Q,    W,    E,    R, \
         FN1,    G,    A,    S,    D,    F, \
        LSFT,    B,    Z,    X,    C,    V, \
        LCTL,  FN0, FN14, LALT,  FN2,  FN3, SPC),
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
    KIIBOHD_FUNCTION_Backlight_Toggle_Region,
    KIIBOHD_FUNCTION_Backlight_Select_Region,
    KIIBOHD_FUNCTION_Backlight_Increase_All,
    KIIBOHD_FUNCTION_Backlight_Decrease_All,
    KIIBOHD_FUNCTION_Backlight_Increase_Region,
    KIIBOHD_FUNCTION_Backlight_Decrease_Region
};

enum macro_id {
    KIIBOHD_MACRO_DUCK_JUMP,
};

/*
 * Fn action definition
 */
const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(2),
    [1] = ACTION_MODS_TAP_KEY(KC_M, KC_I),
    [2] = ACTION_LAYER_TAP_KEY(3, KC_SLSH),
    [3] = ACTION_MACRO(KIIBOHD_MACRO_DUCK_JUMP),
    [4] = ACTION_BACKLIGHT_TOGGLE(),
    [5] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Increase_All),
    [6] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Decrease_All),
    [7] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_REGION_WASD),
    [8] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_REGION_JUMP),
    [9] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_REGION_CONTROLS),
   [10] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_REGION_LOGO),
   [11] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_REGION_OTHER),
   [12] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Increase_Region),
   [13] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Decrease_Region),
   [14] = ACTION_LAYER_MOMENTARY(1),
   [15] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Toggle_Region),
   [16] = ACTION_LAYER_TOGGLE(4),
   [17] = ACTION_LAYER_TOGGLE(5),
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
                MACRO(U(LCTL), U(SPC), END));
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
            backlight_select_region(opt);
            break;
        case KIIBOHD_FUNCTION_Backlight_Toggle_Region:
            backlight_toggle_selected_region();
            break;
        case KIIBOHD_FUNCTION_Backlight_Increase_All:
            backlight_increase_brightness(10);
            break;
        case KIIBOHD_FUNCTION_Backlight_Decrease_All:
            backlight_decrease_brightness(10);
            break;
        case KIIBOHD_FUNCTION_Backlight_Increase_Region:
            backlight_increase_brightness_selected_region(10);
            break;
        case KIIBOHD_FUNCTION_Backlight_Decrease_Region:
            backlight_decrease_brightness_selected_region(10);
            break;
        }
    }
}
