#ifndef KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_
#define KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_

#include <inttypes.h>
#include <stdbool.h>

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

//    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16
//    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O    P    Q
#define KEY_TO_LED_MAP_91TKL( \
	K5A, K5B, K5C, K5D, K5E, K5F, K5G, K5H, K5I, K5J, K5K, K5L, K5M, K5N, K5O, K5P, K5Q,  \
	K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H, K4I, K4J, K4K, K4L, K4M, K4N, K4O, K4P, K4Q,  \
	K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H, K3I, K3J, K3K, K3L, K3M, K2N, K3O, K3P, K3Q,  \
	K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2I, K2J, K2K, K2L, K2M,                      \
	K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1I, K1J, K1K, K1L, K1M,      K1O, K1P, K1Q,  \
	K0A, K0B, K0C, K0D,                               K0K, K0L, K0M, K0N, K0O, K0P, K0Q   \
) { \
  { 0x##K5A, 0x##K5B, 0x##K5C, 0x##K5D, 0x##K5E, 0x##K5F, 0x##K5G, 0x##K5H, 0x##K5I, 0x##K5J, 0x##K5K, 0x##K5L, 0x##K5M, 0x##K5N, 0x##K5O, 0x##K5P, 0x##K5Q }, \
  { 0x##K4A, 0x##K4B, 0x##K4C, 0x##K4D, 0x##K4E, 0x##K4F, 0x##K4G, 0x##K4H, 0x##K4I, 0x##K4J, 0x##K4K, 0x##K4L, 0x##K4M, 0x##K4N, 0x##K4O, 0x##K4P, 0x##K4Q }, \
  { 0x##K3A, 0x##K3B, 0x##K3C, 0x##K3D, 0x##K3E, 0x##K3F, 0x##K3G, 0x##K3H, 0x##K3I, 0x##K3J, 0x##K3K, 0x##K3L, 0x##K3M, 0x##K2N, 0x##K3O, 0x##K3P, 0x##K3Q }, \
  { 0x##K2A, 0x##K2B, 0x##K2C, 0x##K2D, 0x##K2E, 0x##K2F, 0x##K2G, 0x##K2H, 0x##K2I, 0x##K2J, 0x##K2K, 0x##K2L, 0x##K2M,    0xff,    0xff,    0xff,    0xff }, \
  { 0x##K1A, 0x##K1B, 0x##K1C, 0x##K1D, 0x##K1E, 0x##K1F, 0x##K1G, 0x##K1H, 0x##K1I, 0x##K1J, 0x##K1K, 0x##K1L, 0x##K1M,    0xff, 0x##K1O, 0x##K1P, 0x##K1Q }, \
  { 0x##K0A, 0x##K0B, 0x##K0C, 0x##K0D,    0xff,    0xff,    0xff,    0xff,    0xff,    0xff, 0x##K0K, 0x##K0L, 0x##K0M, 0x##K0N, 0x##K0O, 0x##K0P, 0x##K0Q }  \
}

bool getLedPosByMatrixKey(uint8_t key_row, uint8_t key_col, uint8_t* device_number, uint8_t *row, uint8_t *col);

#endif /* KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_ */
