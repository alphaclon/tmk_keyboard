
#include "sector_led_masks.h"

/*
 * Ten-key-less
 *
  * ,------. ,---------------.,---------------.,---------------.  ,-----------.
 * |Esc|Key| |F1 |F2 |F3 |F4 ||F5 |F6 |F7 |F8 ||F9 |F10|F11|F12|  |PrS|ScL|Pau|
 * `-------' `---------------'`---------------'`---------------'  `-----------'
 * ,-----------------------------------------------------------.  ,-----------.
 * |~  |  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Backsp |  |Ins|Hom|PgU|
 * |-----------------------------------------------------------|  |-----------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|     |  |Del|End|PgD|
 * |-----------------------------------------------------------|  `-----------'
 * |Caps  |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Return  |
 * |-----------------------------------------------------------|  ,-----------.
 * |Shft|ISO|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift     |  |FN1|Up |FN2|
 * |-----------------------------------------------------------|  ,-----------.
 * |Ctl|Gui|FN0|Alt|Space || Space         |Alt|FN1|Gui|App|Ctl|  |Lef|Dow|Rig|
 * `-----------------------------------------------------------'  `-----------'
 *
 */

/*
 * 12 rows and 16 columns
 *
 *
 *
 */

uint8_t const LedMaskFull_Top[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff,
		0xff, 0xff
};
uint8_t const LedMaskOthr_Top[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};
uint8_t const LedMaskWASD_Top[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};
uint8_t const LedMaskCtrl_Top[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};
uint8_t const LedMaskCurs_Top[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};

uint8_t const LedMaskFull_Bottom[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};
uint8_t const LedMaskOthr_Bottom[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};
uint8_t const LedMaskWASD_Bottom[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};
uint8_t const LedMaskCtrl_Bottom[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};
uint8_t const LedMaskCurs_Bottom[IS31FL3733_LED_ENABLE_SIZE] PROGMEM = {0};



extern PGM_P LedMasksTop[] = {LedMaskWASD_Top, LedMaskCtrl_Top, LedMaskCurs_Top, LedMaskOthr_Top};
extern PGM_P LedMasksBottom[] = {LedMaskWASD_Bottom, LedMaskCtrl_Bottom, LedMaskCurs_Bottom, LedMaskOthr_Bottom};
