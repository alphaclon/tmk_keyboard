
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


/*
 *
 *

issi open 0

00: 0xFF 0x1F
01: 0xFF 0x1F
02: 0xFF 0x1F
03: 0xFF 0x5F
04: 0xFF 0x5F
05: 0xFF 0x5F
06: 0x7F 0xFE
07: 0x7F 0xFE
08: 0x7F 0xFE
09: 0x00 0x00
10: 0x00 0x00
11: 0x00 0x00

b11111111 b11111
b11111111 b11111
b11111111 b11111
b11111111 b1011111
b11111111 b1011111
b11111111 b1011111
b1111111 b11111110
b1111111 b11111110
b1111111 b11111110


issi open 1

00: 0xFF 0x1F
01: 0xFF 0x1F
02: 0xFF 0x1F
03: 0xFF 0x5F
04: 0xFF 0x5F
05: 0xFF 0x5F
06: 0x7F 0xFE
07: 0x7F 0xFE
08: 0x7F 0xFE
09: 0x00 0x00
10: 0x00 0x00
11: 0x00 0x00

b11111111 b11111
b11111111 b11111
b11111111 b11111
b11111111 b1011111
b11111111 b1011111
b11111111 b1011111
b01111111 b11111110
b01111111 b11111110
b01111111 b11111110
b0 b0
b0 b0
b0 b0

 */

uint8_t const LedMaskFull_Top[IS31FL3733_LED_ENABLE_SIZE] PROGMEM =
{
	/* CS    SW         */
    /* 01 */ 0xff, 0xff,
    /* 02 */ 0xff, 0xff,
    /* 03 */ 0xff, 0xff,
    /* 04 */ 0xff, 0xff,
    /* 05 */ 0xff, 0xff,
    /* 06 */ 0xff, 0xff,
    /* 07 */ 0xff, 0xff,
    /* 08 */ 0xff, 0xff,
    /* 09 */ 0xff, 0xff,
    /* 10 */ 0xff, 0xff,
    /* 11 */ 0xff, 0xff,
    /* 12 */ 0xff, 0xff
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

const uint8_t* LedMasksTop[] = {LedMaskWASD_Top, LedMaskCtrl_Top, LedMaskCurs_Top, LedMaskOthr_Top};
const uint8_t* LedMasksBottom[] = {LedMaskWASD_Bottom, LedMaskCtrl_Bottom, LedMaskCurs_Bottom, LedMaskOthr_Bottom};
