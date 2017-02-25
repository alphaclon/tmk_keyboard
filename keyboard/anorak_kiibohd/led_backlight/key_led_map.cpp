
#include "key_led_map.h"
#include "config.h"
#include <avr/pgmspace.h>

#ifdef DEBUG_ANIMATION_KBDMAP
#include "debug.h"
#else
#include "nodebug.h"
#endif

const uint8_t PROGMEM keyledmap[MATRIX_ROWS][MATRIX_COLS] =
    KEY_TO_LED_MAP_KIIBOHD(
    		05, 04, 03, 02, 01, 00,
			15, 14, 13, 12, 11, 10,
			25, 24, 23, 22, 21, 20,
			35, 34, 33, 32, 31, 30,
			45, 44, 43, 42, 41, 40, 50);

void getLedPosByMatrixKey(uint8_t row, uint8_t col, uint8_t *led_row, uint8_t *led_col)
{
    uint8_t pos = pgm_read_byte(&keyledmap[row][col]);

    *led_row = (pos >> 4);
    *led_col = (pos & 0x0F);

    dprintf("r:%u, c:%u 0x%X -> %u %u\n", row, col, pos, *led_row, *led_col);
}
