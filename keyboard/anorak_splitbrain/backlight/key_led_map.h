#ifndef KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_
#define KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_

#include <inttypes.h>

#define KEY_TO_LED_MAP_KIIBOHD( \
		K00, K01, K02, K03, K04, K05, \
		K10, K11, K12, K13, K14, K15, \
		K20, K21, K22, K23, K24, K25, \
		K30, K31, K32, K33, K34, K35, \
		K40, K41, K42, K43, K44, K45, K46 \
) { \
	{  0x##K00, 0x##K01, 0x##K02, 0x##K03, 0x##K04, 0x##K05, 0xFF    }, \
	{  0x##K10, 0x##K11, 0x##K12, 0x##K13, 0x##K14, 0x##K15, 0xFF    }, \
	{  0x##K20, 0x##K21, 0x##K22, 0x##K23, 0x##K24, 0x##K25, 0xFF    }, \
	{  0x##K30, 0x##K31, 0x##K32, 0x##K33, 0x##K34, 0x##K35, 0xFF    }, \
	{  0x##K40, 0x##K41, 0x##K42, 0x##K43, 0x##K44, 0x##K45, 0x##K46 }  \
}


void getLedPosByMatrixKey(uint8_t row, uint8_t col, uint8_t *led_row, uint8_t *led_col);

#endif /* KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_KEY_LED_MAP_H_ */
