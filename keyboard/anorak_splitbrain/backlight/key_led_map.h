#ifndef KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_
#define KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_

#include <inttypes.h>

#define RS(arg) (arg)
#define LS(arg) (arg | 0x80)
#define NO_LED (0xff)

// COLUMN
//    0    1    2    3    4    5    6      7    8    9   10   11   12   13   14   15   16   17
#define KEY_TO_LED_MAP_SPLITBRAIN( \
	K5A, K5B, K5C, K5D, K5E, K5F, K5G,   K5H, K5I, K5J, K5K, K5L, K5M, K5N, K5O, K5P, K5Q, K5R,  \
    K4A, K4B, K4C, K4D, K4E, K4F, K4G,   K4H, K4I, K4J, K4K, K4L, K4M, K4N,      K4P, K4Q, K4R,  \
	K3A, K3B, K3C, K3D, K3E, K3F,   K3G, K3H, K3I, K3J, K3K, K3L, K3M,           K3P, K3Q, K3R,  \
	K2A, K2B, K2C, K2D, K2E, K2F,   K2G, K2H, K2I, K2J, K2K, K2L, K2M, K2N,                      \
	K1A, K1B, K1C, K1D, K1E, K1F, K1G,   K1H, K1I, K1J, K1K, K1L, K1M,           K1P, K1Q, K1R,  \
	K0A, K0B, K0C, K0D, K0E,                  K0I, K0J, K0K, K0L, K0M, K0N,      K0P, K0Q, K0R   \
) { \
  { LS(0x##K5A), LS(0x##K5B), LS(0x##K5C), LS(0x##K5D), LS(0x##K5E), LS(0x##K5F), LS(0x##K5G), RS(0x##K5H), RS(0x##K5I), RS(0x##K5J), RS(0x##K5K), RS(0x##K5L), RS(0x##K5M), RS(0x##K5N), RS(0x##K5O), RS(0x##K5P), RS(0x##K5Q), RS(0x##K5R) }, \
  { LS(0x##K4A), LS(0x##K4B), LS(0x##K4C), LS(0x##K4D), LS(0x##K4E), LS(0x##K4F), LS(0x##K4G), RS(0x##K4H), RS(0x##K4I), RS(0x##K4J), RS(0x##K4K), RS(0x##K4L), RS(0x##K4M), RS(0x##K4N),      NO_LED, RS(0x##K4P), RS(0x##K4Q), RS(0x##K4R) }, \
  { LS(0x##K3A), LS(0x##K3B), LS(0x##K3C), LS(0x##K3D), LS(0x##K3E), LS(0x##K3F), RS(0x##K3G), RS(0x##K3H), RS(0x##K3I), RS(0x##K3J), RS(0x##K3K), RS(0x##K3L), RS(0x##K3M),      NO_LED,      NO_LED, RS(0x##K3P), RS(0x##K3Q), RS(0x##K3R) }, \
  { LS(0x##K2A), LS(0x##K2B), LS(0x##K2C), LS(0x##K2D), LS(0x##K2E), LS(0x##K2F), RS(0x##K2G), RS(0x##K2H), RS(0x##K2I), RS(0x##K2J), RS(0x##K2K), RS(0x##K2L), RS(0x##K2M), RS(0x##K2N),      NO_LED,      NO_LED,      NO_LED,      NO_LED }, \
  { LS(0x##K1A), LS(0x##K1B), LS(0x##K1C), LS(0x##K1D), LS(0x##K1E), LS(0x##K1F), LS(0x##K1G), RS(0x##K1H), RS(0x##K1I), RS(0x##K1J), RS(0x##K1K), RS(0x##K1L), RS(0x##K1M),      NO_LED,      NO_LED, RS(0x##K1P), RS(0x##K1Q), RS(0x##K1R) }, \
  { LS(0x##K0A), LS(0x##K0B), LS(0x##K0C), LS(0x##K0D), LS(0x##K0E),      NO_LED,      NO_LED,      NO_LED, RS(0x##K0I), RS(0x##K0J), RS(0x##K0K), RS(0x##K0L), RS(0x##K0M), RS(0x##K0N),      NO_LED, RS(0x##K0P), RS(0x##K0Q), RS(0x##K0R) }  \
}

void getLedPosByMatrixKey(uint8_t row, uint8_t col, uint8_t *led_row, uint8_t *led_col, bool* is_left_side);

#endif /* KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_ */
