
#include "key_led_map.h"
#include "config.h"
#include <avr/pgmspace.h>

//               0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17
const uint8_t PROGMEM keyledmap[MATRIX_ROWS][MATRIX_COLS] =
		KEY_TO_LED_MAP_SPLITBRAIN(
			 //K5A, K5B, K5C, K5D, K5E, K5F, K5G, K5H, K5I, K5J, K5K, K5L, K5M, K5N, K5O, K5P, K5Q, K5R,
				0E,  0D,  0C,  0B,  0A,  09,  08,  07,  06,  05,  04,  03,  02,  01,  00,  0A,  09,  08,
			 //K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H, K4I, K4J, K4K, K4L, K4M, K4N,      K4P, K4Q, K4R,
				1E,  1D,  1C,  1B,  1A,  19,  18,  16,  15,  14,  13,  12,  11,  10,       1A,  19,  18,
			 //K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H, K3I, K3J, K3K, K3L, K3M,           K3P, K3Q, K3R,
				2D,  2C,  2B,  2A,  29,  28,  27,  26,  25,  24,  23,  22,  21,            2A,  29,  28,
		     //K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2I, K2J, K2K, K2L, K2M, K2N,
				3D,  3C,  3B,  3A,  39,  38,  37,  36,  35,  34,  33,  32,  31,  30,
			 //K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1I, K1J, K1K, K1L, K1M,           K1P, K1Q, K1R,
				4D,  4C,  4B,  4A,  49,  48,  46,  45,  44,  43,  42,  41,  40,            4A,  49,  48,
			 //K0A, K0B, K0C, K0D, K0E,                K0I, K0J, K0K, K0L, K0M, K0N,      K0P, K0Q, K0R
			    5C,  5B,  5A,  59,  58,                 55,  54,  53,  52,  51,  50,       5A,  59,  58
);

void getLedPosByMatrixKey(uint8_t row, uint8_t col, uint8_t *led_row, uint8_t *led_col, bool* is_left_side)
{
    uint8_t pos = pgm_read_byte(&keyledmap[row][col]);

    *is_left_side = (pos & 0x80);
    *led_row = (pos & 0x70) >> 4;
    *led_col = (pos & 0x0F);
}
