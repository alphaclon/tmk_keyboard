/** ISSI IS31FL3733 RGB header file.
  */
#ifndef _IS31FL3733_RGB_H_
#define _IS31FL3733_RGB_H_

#include "is31fl3733.h"

enum IS31FL3733_RGB_ColorOrder { RGB, RBG };
enum IS31FL3733_RGB_Color { red, green, blue };

/** IS31FL3733_RGB structure.
  */
typedef struct
{
	IS31FL3733 *device;

	uint8_t red_offset;
	uint8_t green_offset;
	uint8_t blue_offset;
} IS31FL3733_RGB;

/// Init LED matrix for normal operation.
void IS31FL3733_RGB_Init(IS31FL3733_RGB *device, enum IS31FL3733_RGB_ColorOrder color_order);

/// Set brightness level for all LEDs.
void IS31FL3733_RGB_SetBrightnessForMasked(IS31FL3733_RGB *device, enum IS31FL3733_RGB_Color color, uint8_t brightness);

#endif /* _IS31FL3733_H_ */
