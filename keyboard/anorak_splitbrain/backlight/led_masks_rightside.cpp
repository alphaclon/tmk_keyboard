
#include "led_masks.h"
#include <avr/pgmspace.h>

/*
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
 *
 *
 *
 *
 *              1   2   3   4   5   6   7   8    1  2  3
 * Row 0:      F7  F8  F9 F10 F11 F12 FN4 FN5   Pr Sc PA
 * Row 1:           7   8   9   0   ?   ´  BS   In Hm Pu
 * Row 2:       Z   U   I   O   P   Ü   + ---   De En Pd
 * Row 3:       H   J   K   L   Ö   Ä   #  Rt   -- -- --
 * Row 4:       N   M   ,   .   - ---  Sh ---   Fn Up Fn
 * Row 5:          Sp  Al  Fn  Gu  Ap  Ct ---   Le Do Ri
 *
 */


/* Full keyboard LED matrix
 *
 *              1   2   3   4   5   6   7   8    1  2  3
 * Row 0:      F7  F8  F9 F10 F11 F12 FN4 FN5   Pr Sc PA    11111111   00000111   C1-1 -> C1-16
 * Row 1:           7   8   9   0   ?   ´  BS   In Hm Pu    01111111   00000111   C2-1 -> C2-16
 * Row 2:       Z   U   I   O   P   Ü   + ---   De En Pd    01111110   00000111   C3-1 -> C3-16
 * Row 3:       H   J   K   L   Ö   Ä   #  Rt   -- -- --    11111111   --------   C4-1 -> C4-16
 * Row 4:               N   M   ,   .   -  Sh   Fn Up Fn    00111111   00000111   C5-1 -> C5-16
 * Row 5:              Sp  Al  Fn  Gu  Ap  Ct   Le Do Ri    00111111   00000111   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskFull_Right[ISSI_LED_MASK_SIZE] PROGMEM =
{
    0xFF, 0x07,
    0x7F, 0x07,
    0xFE, 0x07,
    0xFF, 0x07,
    0x3F, 0x07,
    0x3F, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};


/* Other keys LED matrix
 *
 *              1   2   3   4   5   6   7   8    1  2  3
 * Row 0:      F7  F8  F9 F10 F11 F12 FN4 FN5   Pr Sc PA    11111111   00000111   C1-1 -> C1-16
 * Row 1:           7   8   9   0   ?   ´  BS   In Hm Pu    01111111   00000111   C2-1 -> C2-16
 * Row 2:       Z   U   I   O   P   Ü   + ---   De En Pd    11111110   00000111   C3-1 -> C3-16
 * Row 3:       H   J   K   L   Ö   Ä   #  Rt   -- -- --    11111111   00000000   C4-1 -> C4-16
 * Row 4:       N   M   ,   .   - ---  Sh ---   Fn Up Fn    11111010   00000111   C5-1 -> C5-16
 * Row 5:          Sp  Al  Fn  Gu  Ap  Ct ---   Le Do Ri    01111110   00000111   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskOthr_Right[ISSI_LED_MASK_SIZE] PROGMEM =
{
	0x00, 0x00,
	0x7E, 0x00,
	0xFE, 0x00,
	0xFE, 0x00,
	0x3E, 0x00,
	0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};

/* WASD keys LED matrix
 *
 * nothing on the right side...
 *
 */

uint8_t const LedMaskWASD_Right[ISSI_LED_MASK_SIZE] PROGMEM =
{
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};

/* Control keys LED matrix
 *
 *              1   2   3   4   5   6   7   8    1  2  3
 * Row 0:      F7  F8  F9 F10 F11 F12 FN4 FN5   Pr Sc PA    11111111   00000111   C1-1 -> C1-16
 * Row 1:           7   8   9   0   ?   ´  BS   In Hm Pu    01111111   00000111   C2-1 -> C2-16
 * Row 2:       Z   U   I   O   P   Ü   + ---   De En Pd    01111110   00000111   C3-1 -> C3-16
 * Row 3:       H   J   K   L   Ö   Ä   #  Rt   -- -- --    11111111   00000000   C4-1 -> C4-16
 * Row 4:       N   M   ,   .   - ---  Sh ---   Fn Up Fn    11111010   00000111   C5-1 -> C5-16
 * Row 5:          Sp  Al  Fn  Gu  Ap  Ct ---   Le Do Ri    01111110   00000111   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskCtrl_Right[ISSI_LED_MASK_SIZE] PROGMEM =
{
	0xFF, 0x00,
	0x01, 0x00,
	0x00, 0x00,
	0x01, 0x00,
	0x01, 0x00,
	0x3F, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};

/* Cursor keys LED matrix
 *
 *              1   2   3   4   5   6   7   8    1  2  3
 * Row 0:      F7  F8  F9 F10 F11 F12 FN4 FN5   Pr Sc PA    11111111   00000111   C1-1 -> C1-16
 * Row 1:           7   8   9   0   ?   ´  BS   In Hm Pu    01111111   00000111   C2-1 -> C2-16
 * Row 2:       Z   U   I   O   P   Ü   + ---   De En Pd    01111110   00000111   C3-1 -> C3-16
 * Row 3:       H   J   K   L   Ö   Ä   #  Rt   -- -- --    11111111   00000000   C4-1 -> C4-16
 * Row 4:       N   M   ,   .   - ---  Sh ---   Fn Up Fn    11111010   00000111   C5-1 -> C5-16
 * Row 5:          Sp  Al  Fn  Gu  Ap  Ct ---   Le Do Ri    01111110   00000111   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskCurs_Right[ISSI_LED_MASK_SIZE] PROGMEM =
{
    0x00, 0x07,
    0x00, 0x07,
    0x00, 0x07,
    0x00, 0x07,
    0x00, 0x07,
    0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};

