
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

#define ISSI_POS(dev, row, col) (dev << 8 | row << 4 | col)
#define DL(row, col) (row << 4 | col)
#define DU(row, col) (0x80 | row << 4 | col)

//               0        1         2         3         4         5         6         7         8         9        10         11         12         13         14         15         16
const uint8_t PROGMEM keyledmap[MATRIX_ROWS][MATRIX_COLS] =
		KEY_TO_LED_MAP_91TKL(
			 //K5A,     K5B,      K5C,      K5D,      K5E,      K5F,      K5G,      K5H,      K5I,      K5J,      K5K,       K5L,       K5M,       K5N,       K5O,       K5P,       K5Q,
		   DU(0,0), DU(0,1),  DU(0,2),  DU(0,3),  DU(0,4),  DU(0,5),  DU(0,6),  DU(0,7),  DU(0,8),  DU(0,9), DU(0,10),  DU(0,11),  DU(0,12),  DU(0,13),  DU(0,14),  DU(0,15),  DL(2, 6),
			 //K4A,     K4B,      K4C,      K4D,      K4E,      K4F,      K4G,      K4H,      K4I,      K4J,      K4K,       K4L,       K4M,       K4N,       K4O,       K4P,       K4Q,
		   DU(1,0), DU(1,1),  DU(1,2),  DU(1,3),  DU(1,4),  DU(1,5),  DU(1,6),  DU(1,7),  DU(1,8),  DU(1,9), DU(1,10),  DU(1,11),  DU(1,12),  DU(1,13),  DU(1,14),  DU(1,15),  DL(2, 5),
			 //K3A,     K3B,      K3C,      K3D,      K3E,      K3F,      K3G,      K3H,      K3I,      K3J,      K3K,       K3L,       K3M,       K3N,       K3O,       K3P,       K3Q,
	       DU(2,0), DU(2,1),  DU(2,2),  DU(2,3),  DU(2,4),  DU(2,5),  DU(2,6),  DU(2,7),  DU(2,8),  DU(2,9), DU(2,10),  DU(2,11),  DU(2,12),  DU(2,13),  DU(2,14),  DU(2,15),  DL(2, 4),
		     //K2A,     K2B,      K2C,      K2D,      K2E,      K2F,      K2G,      K2H,      K2I,      K2J,      K2K,       K2L,       K2M,
	       DL(0,0), DL(0,1),  DL(0,2),  DL(0,3),  DL(0,4),  DL(0,5),  DL(0,6),  DL(0,7),  DL(0,8),  DL(0,9), DL(0,10),  DL(0,11),  DL(0,12),
			 //K1A,     K1B,     K1C,       K1D,      K1E,      K1F,      K1G,      K1H,       K1I,     K1J,      K1K,       K1L,       K1M,                 K1O,       K1P,       K1Q,
		   DL(1,0), DL(1,1),  DL(1,2),  DL(1,3),  DL(1,4),  DL(1,5),  DL(1,6),  DL(1,7),  DL(1,8),  DL(1,9), DL(1,10),  DL(1,11),  DL(1,12),            DL(1,13),  DL(1,14),  DL(1,15),
			 //K0A,     K0B,     K0C,       K0D,                                                                  K0K,       K0L,       K0M,       K0N,      K0O,       K0P,       K0Q,
		   DL(2,0), DL(2,1),  DL(2,2),  DL(2,3),                                                              DL(2,9),  DL(2,10),  DL(2,11),  DL(2,12), DL(2,13),  DL(2,14),  DL(2,15)
);

bool getLedPosByMatrixKey(uint8_t key_row, uint8_t key_col, uint8_t* device_number, uint8_t *row, uint8_t *col)
{
    uint8_t pos = pgm_read_byte(&keyledmap[key_row][key_col]);

    if (pos == NLED)
    	return false;

    *device_number = (pos & 0x80) >> 7;
    *row = (pos & 0x70) >> 4;
    *col = (pos & 0x0F);

    return true;
}
