
#include "../backlight/led_masks.h"

#include <avr/pgmspace.h>

#ifndef __AVR_ARCH__
#define PROGMEM
#endif

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
 *           1  2  3  4  5  6  7
 * Row 0:  ESC,F1,F2,F3,F4,F5,F6
 * Row 1:    ^, 1, 2, 3, 4, 5, 6
 * Row 2:  TAB, Q, W, E, R, T
 * Row 3: CAPS, A, S, D, F, G
 * Row 4: Shft, Y, X, C, V, B
 * Row 5: Ctrl, G, F, A, S
 *
 *
 */

/* Full keyboard LED matrix
 *
 *           1  2  3  4  5  6  7
 * Row 0:  ESC,F1,F2,F3,F4,F5,F6  01111111 N/A 00000000   C1-1 -> C1-16
 * Row 1:    ^, 1, 2, 3, 4, 5, 6  01111111 N/A 00000000   C2-1 -> C2-16
 * Row 2:  TAB, Q, W, E, R, T     00111111 N/A 00000000   C3-1 -> C3-16
 * Row 3: CAPS, A, S, D, F, G     00111111 N/A 00000000   C4-1 -> C4-16
 * Row 4: Shft, Y, X, C, V, B     00111111 N/A 00000000   C5-1 -> C5-16
 * Row 5: Ctrl, G, F, A, S        00011111 N/A 00000000   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskFull_Left[ISSI_LED_MASK_SIZE] PROGMEM =
{
    0x7F, 0x00,
    0x7F, 0x00,
    0x3F, 0x00,
    0x3F, 0x00,
    0x3F, 0x00,
    0x1F, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};


/* Other keys LED matrix
 *
 *           1  2  3  4  5  6  7
 * Row 0:  ESC,F1,F2,F3,F4,F5,F6  00000000 N/A 00000000   C1-1 -> C1-16
 * Row 1:    ^, 1, 2, 3, 4, 5, 6  01111111 N/A 00000000   C2-1 -> C2-16
 * Row 2:  TAB, Q, W, E, R, T     00010111 N/A 00000000   C3-1 -> C3-16
 * Row 3: CAPS, A, S, D, F, G     00000011 N/A 00000000   C4-1 -> C4-16
 * Row 4: Shft, Y, X, C, V, B     00011111 N/A 00000000   C5-1 -> C5-16
 * Row 5: Ctrl, G, F, A, S        00000000 N/A 00000000   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskOthr_Left[ISSI_LED_MASK_SIZE] PROGMEM =
{
	0x00, 0x00,
    0x7F, 0x00,
    0x17, 0x00,
    0x03, 0x00,
    0x1F, 0x00,
    0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};


/* WASD keys LED matrix
 *
 *           1  2  3  4  5  6  7
 * Row 0:  ESC,F1,F2,F3,F4,F5,F6  00000000 N/A 00000000   C1-1 -> C1-16
 * Row 1:    ^, 1, 2, 3, 4, 5, 6  00000000 N/A 00000000   C2-1 -> C2-16
 * Row 2:  TAB, Q, W, E, R, T     00001000 N/A 00000000   C3-1 -> C3-16
 * Row 3: CAPS, A, S, D, F, G     00011100 N/A 00000000   C4-1 -> C4-16
 * Row 4: Shft, Y, X, C, V, B     00000000 N/A 00000000   C5-1 -> C5-16
 * Row 5: Ctrl, G, F, A, S        00000000 N/A 00000000   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskWASD_Left[ISSI_LED_MASK_SIZE] PROGMEM =
{
    0x00, 0x00,
	0x00, 0x00,
    0x08, 0x00,
    0x1C, 0x00,
    0x00, 0x00,
    0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};


/* Control keys LED matrix
 *
 *           1  2  3  4  5  6  7
 * Row 0:  ESC,F1,F2,F3,F4,F5,F6  01111111 N/A 00000000   C1-1 -> C1-16
 * Row 1:    ^, 1, 2, 3, 4, 5, 6  01000000 N/A 00000000   C2-1 -> C2-16
 * Row 2:  TAB, Q, W, E, R, T     00100000 N/A 00000000   C3-1 -> C3-16
 * Row 3: CAPS, A, S, D, F, G     00100000 N/A 00000000   C4-1 -> C4-16
 * Row 4: Shft, Y, X, C, V, B     00100000 N/A 00000000   C5-1 -> C5-16
 * Row 5: Ctrl, G, F, A, S        00011111 N/A 00000000   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskCtrl_Left[ISSI_LED_MASK_SIZE] PROGMEM =
{
    0x7F, 0x00,
    0x40, 0x00,
    0x20, 0x00,
    0x20, 0x00,
    0x20, 0x00,
    0x1F, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
  //0x00, 0x00,
};

/* Logo LED matrix
 *
 *           1  2  3  4  5  6  7
 * Row 0:  ESC,F1,F2,F3,F4,F5,F6  01111111 N/A 00000000   C1-1 -> C1-16
 * Row 1:    ^, 1, 2, 3, 4, 5, 6  01111111 N/A 00000000   C2-1 -> C2-16
 * Row 2:  TAB, Q, W, E, R, T     00111111 N/A 00000000   C3-1 -> C3-16
 * Row 3: CAPS, A, S, D, F, G     00111111 N/A 00000000   C4-1 -> C4-16
 * Row 4: Shft, Y, X, C, V, B     00111111 N/A 00000000   C5-1 -> C5-16
 * Row 5: Ctrl, G, F, A, S        00011111 N/A 00000000   C6-1 -> C6-16
 *
 */

uint8_t const LedMaskLogo_Left[ISSI_LED_MASK_SIZE] PROGMEM =
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
