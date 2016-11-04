#include "keymap_common.h"
#include <avr/pgmspace.h>
#include "led_backlight/backlight_kiibohd.h"
#include "config.h"

#include "sleep_led.h"
#include "led_backlight/control.h"

/*
 *  Keymaps
 *
 */

#define KBLAYER_COUNT         3

#define KBLAYER_DEFAULT       0
#define KBLAYER_SPC_ENT       1
#define KBLAYER_SHIFTED_TQWER 2

#define LAYER_FN0_BACKGROURND  KBLAYER_COUNT + 2
#define LAYER_FN2_TF2FKEYS     KBLAYER_COUNT + 1
#define LAYER_F14_SELECT_LAYER KBLAYER_COUNT


const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * 0: Default layer (QWERT)
     *
     * ,-------------------------.
     * |Esc  |   1|  2|  3|  4|  5|
     * |--------------------------|
     * |Tab  |   Q|  W|  E|  R|  T|
     * |--------------------------|
     * |    M|   A|  S|  D|  F|  G|
     * |--------------------------|
     * |Shift|   Z|  X|  C|  V|  B|
     * |--------------------------|
     * |LCtrl|FN0|FN14|LAlt|FN2|FN3|SPC|
     * `-------------------------------'
     *
     */
    KEYMAP_KIIBOHD(
         ESC,    1,    2,    3,    4,  F12, \
         TAB,    Q,    W,    E,    R,    T, \
         FN1,    A,    S,    D,    F,    G, \
        LSFT,    Z,    X,    C,    V,    B, \
        LCTL,  FN0, FN14, LALT,  FN2,  FN3, SPC),
    /*
     * 1: layer 1, QWERT,
     *    normal space on middle thumb key, enter on left thumb key
     *    toggled by FN14-layer: FN16
     *
     * ,-------------------------.
     * |Esc  |   1|  2|  3|  4|  5|
     * |--------------------------|
     * |Tab  |   Q|  W|  E|  R|  T|
     * |--------------------------|
     * |    M|   A|  S|  D|  F|  G|
     * |--------------------------|
     * |Shift|   Z|  X|  C|  V|  B|
     * |--------------------------|
     * |LCtrl|FN0|FN14|LAlt|FN2|SPC|ENT|
     * `-------------------------------'
     *
     */
    KEYMAP_KIIBOHD(
         ESC,    1,    2,    3,    4,  F12, \
         TAB,    Q,    W,    E,    R,    T, \
         FN1,    A,    S,    D,    F,    G, \
        LSFT,    Z,    X,    C,    V,    B, \
        LCTL,  FN0, FN14, LALT,  FN2,  SPC, ENT),
    /*
     * 2: shifted layer (TQWER)
     *    toggled by FN14-layer: FN17
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
    KEYMAP_KIIBOHD(
         ESC,    1,    2,    3,    4,  F12, \
         TAB,    T,    Q,    W,    E,    R, \
         FN1,    G,    A,    S,    D,    F, \
        LSFT,    B,    Z,    X,    C,    V, \
        LCTL,  FN0, FN14, LALT,  FN2,  FN3, SPC),
    /*
     * 3: layer 3, the layer selection layer
     *    activated by FN14
     *
     */
    KEYMAP_KIIBOHD(
        PAUS, FN17, FN18, FN19, FN20, FN21, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, FN14, TRNS, TRNS, TRNS, TRNS),
    /*
     * 4: layer 4, TF2 F-key layer,
     *    activated by FN2 (leftmost thumb key)
     *
     */
    KEYMAP_KIIBOHD(
        TRNS,   F1,   F2,   F5,   F6, TRNS, \
          F1,   F2,   F3,   F4,   F5,   F6, \
          F7,   F8,   F9,  F10,  F11,  F12, \
           0,    5,    6,    7,    8,    9, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS),
    /*
     * 5: layer 5, media and backlight layer,
     *    activated by FN0
     *
     *    Fn5: CAPS inc all
     *    Fn6: LSFT dec all
     *    Fn4: LCTL toggle all
     *   Fn12: THUMB MID inc region
     *   Fn13: THUMB LFT dec region
     *   Fn15: THUMB RIG toggle region
     *   Fn16: LALT save
     *
     *    Fn7: A WASD
     *    Fn8: S JUMP
     *    Fn9: D CTRL
     *   Fn10: F LOGO
     *   Fn11: G OTHR
     *
     */
    KEYMAP_KIIBOHD(
        PAUS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
         FN5,  FN7,  FN8,  FN9, FN10, FN11, \
         FN6, VOLD, VOLU, MUTE, TRNS, TRNS, \
         FN4, TRNS, TRNS, FN16, FN12, FN13, FN15)
};

/*
 *
 *
 *  Actions
 *
 *
 */

/* id for user defined function/macro */
enum function_id
{
    KIIBOHD_FUNCTION_Backlight_Toggle_Region,
    KIIBOHD_FUNCTION_Backlight_Select_Region,
    KIIBOHD_FUNCTION_Backlight_Increase_All,
    KIIBOHD_FUNCTION_Backlight_Decrease_All,
    KIIBOHD_FUNCTION_Backlight_Increase_Region,
    KIIBOHD_FUNCTION_Backlight_Decrease_Region,
    KIIBOHD_FUNCTION_Backlight_Save_Current_State,
    KIIBOHD_FUNCTION_Backlight_Breath,
	KIIBOHD_FUNCTION_Backlight_Animate,
};

enum macro_id
{
    KIIBOHD_MACRO_DUCK_JUMP,
};

/*
 * Fn action definition
 */
const uint16_t PROGMEM fn_actions[] =
{
    [0] = ACTION_LAYER_MOMENTARY(LAYER_FN0_BACKGROURND),
    [1] = ACTION_MODS_TAP_KEY(KC_CAPS, KC_M),
    [2] = ACTION_LAYER_TAP_KEY(LAYER_FN2_TF2FKEYS, KC_SLSH),
    [3] = ACTION_MACRO(KIIBOHD_MACRO_DUCK_JUMP),
    [4] = ACTION_BACKLIGHT_TOGGLE(),
    [5] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Increase_All),
    [6] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Decrease_All),
    [7] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_WASD),     //BACKLIGHT_REGION_WASD
    [8] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_JUMP),     //BACKLIGHT_REGION_JUMP
    [9] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_CONTROLS), //BACKLIGHT_REGION_CONTROLS
   [10] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_LOGO),     //BACKLIGHT_REGION_LOGO
   [11] = ACTION_FUNCTION_OPT(KIIBOHD_FUNCTION_Backlight_Select_Region, BACKLIGHT_OTHER),    //BACKLIGHT_REGION_OTHER
   [12] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Decrease_Region),
   [13] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Increase_Region),
   [14] = ACTION_LAYER_MOMENTARY(LAYER_F14_SELECT_LAYER),
   [15] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Toggle_Region),
   [16] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Save_Current_State),
   [17] = ACTION_DEFAULT_LAYER_SET(KBLAYER_DEFAULT),
   [18] = ACTION_DEFAULT_LAYER_SET(KBLAYER_SPC_ENT),
   [19] = ACTION_DEFAULT_LAYER_SET(KBLAYER_SHIFTED_TQWER),
   [20] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Breath),
   [21] = ACTION_FUNCTION(KIIBOHD_FUNCTION_Backlight_Animate)
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
        	sleep_led_toggle();
            break;
        case KIIBOHD_FUNCTION_Backlight_Animate:

            break;

        }
    }
}
