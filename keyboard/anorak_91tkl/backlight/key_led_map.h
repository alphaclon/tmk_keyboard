#ifndef KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_
#define KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_

#include <inttypes.h>
#include <stdbool.h>

#define NLED 0xff

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

//    0      1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
//    A      B     C     D     E     F     G     H     I     J     K     L     M     N     O     P     Q
#define KEY_TO_LED_MAP_91TKL( \
     K5A,  K5B,  K5C,  K5D,  K5E,  K5F,  K5G,  K5H,  K5I,  K5J,  K5K,  K5L,  K5M,  K5N,  K5O,  K5P,  K5Q,  \
     K4A,  K4B,  K4C,  K4D,  K4E,  K4F,  K4G,  K4H,  K4I,  K4J,  K4K,  K4L,  K4M,  K4N,  K4O,  K4P,  K4Q,  \
     K3A,  K3B,  K3C,  K3D,  K3E,  K3F,  K3G,  K3H,  K3I,  K3J,  K3K,  K3L,  K3M,  K2N,  K3O,  K3P,  K3Q,  \
     K2A,  K2B,  K2C,  K2D,  K2E,  K2F,  K2G,  K2H,  K2I,  K2J,  K2K,  K2L,  K2M,                          \
     K1A,  K1B,  K1C,  K1D,  K1E,  K1F,  K1G,  K1H,  K1I,  K1J,  K1K,  K1L,  K1M,        K1O,  K1P,  K1Q,  \
     K0A,  K0B,  K0C,  K0D,                                K0J,  K0K,  K0L,  K0M,        K0O,  K0P,  K0Q   \
) { \
  {  K5A,  K5B,  K5C,  K5D,  K5E,  K5F,  K5G,  K5H,  K5I,  K5J,  K5K,  K5L,  K5M,  K5N,  K5O,  K5P,  K5Q }, \
  {  K4A,  K4B,  K4C,  K4D,  K4E,  K4F,  K4G,  K4H,  K4I,  K4J,  K4K,  K4L,  K4M,  K4N,  K4O,  K4P,  K4Q }, \
  {  K3A,  K3B,  K3C,  K3D,  K3E,  K3F,  K3G,  K3H,  K3I,  K3J,  K3K,  K3L,  K3M,  K2N,  K3O,  K3P,  K3Q }, \
  {  K2A,  K2B,  K2C,  K2D,  K2E,  K2F,  K2G,  K2H,  K2I,  K2J,  K2K,  K2L,  K2M, NLED, NLED, NLED, NLED }, \
  {  K1A,  K1B,  K1C,  K1D,  K1E,  K1F,  K1G,  K1H,  K1I,  K1J,  K1K,  K1L,  K1M, NLED,  K1O,  K1P,  K1Q }, \
  {  K0A,  K0B,  K0C,  K0D, NLED, NLED, NLED, NLED, NLED,  K0J,  K0K,  K0L,  K0M, NLED,  K0O,  K0P,  K0Q }  \
}

bool getLedPosByMatrixKey(uint8_t key_row, uint8_t key_col, uint8_t* device_number, uint8_t *row, uint8_t *col);

#endif /* KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_ */
