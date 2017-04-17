
#include "key_led_map.h"
#include "config.h"
#include <avr/pgmspace.h>

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
 *
 *
 * HIGH row
 *  LOW column
 *
 *  0D --> row 0, column 13
 *
 */

#define MPOS(dev, row, col) (dev << 8 | row << 4 | col)

//               0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16
const uint8_t PROGMEM keyledmap[MATRIX_ROWS][MATRIX_COLS] =
		KEY_TO_LED_MAP_91TKL(
			 //K5A, K5B, K5C, K5D, K5E, K5F, K5G, K5H, K5I, K5J, K5K, K5L, K5M, K5N, K5O, K5P, K5Q,
				AF,  AE,  AD,  AC,  AB,  AA,  A9,  A8,  A7,  A6,  A5,  A4,  A3,  A2,  A1,  A0,  AA,
			 //K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H, K4I, K4J, K4K, K4L, K4M, K4N, K4O  K4P, K4Q,
				9F,  9E,  9D,  9C,  9B,  9A,  99,  98,  97,  96,  95,  94,  93,  92,  91,  90,  9A,
			 //K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H, K3I, K3J, K3K, K3L, K3M,      K3O, K3P, K3Q,
				8E,  8D,  8C,  8B,  8A,  89,  88,  87,  86,  85,  84,  83,  82,       81,  80,  8A,
		     //K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2I, K2J, K2K, K2L, K2M, K2N,
				2D,  2C,  2B,  2A,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,
			 //K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1I, K1J, K1K, K1L, K1M,      K1O, K1P, K1Q,
				1E,  1D, 1C,  1B,  1A,  19,  18,  17,  16,  15,  14,  13,  12,        11,  10,  19,
			 //K0A, K0B, K0C, K0D,                               K0K, K0L, K0M, K0N, K0O  K0P, K0Q,
				0D,  0C,  0B,  0A,                                05,  04,  03,  02,  01,  00,  0A
);

void getLedPosByMatrixKey(uint8_t row, uint8_t col, uint8_t *led_row, uint8_t *led_col)
{
    uint8_t pos = pgm_read_byte(&keyledmap[row][col]);

    *led_row = (pos & 0xF0) >> 4;
    *led_col = (pos & 0x0F);
}
