/** ISSI IS31FL3733 RGB header file.
  */
#ifndef _IS31FL3733_RGB_91TKL_H_
#define _IS31FL3733_RGB_91TKL_H_

#include "is31fl3733_rbg.h"
#include <stdbool.h>

/** IS31FL3733_RGB 91TKL structure.
  */
typedef struct
{
	IS31FL3733_RGB *device1;
	IS31FL3733_RGB *device2;
} IS31FL3733_RGB_91TKL;

extern IS31FL3733_RGB_91TKL issi;

/// Init LED matrix for normal operation.
void IS31FL3733_RGB_91TKL_Init(IS31FL3733_RGB_91TKL *device, enum IS31FL3733_RGB_ColorOrder color_order);

/// Set brightness level for all LEDs.
void IS31FL3733_RGB_91TKL_SetBrightnessForMasked(IS31FL3733_RGB_91TKL *device, enum IS31FL3733_RGB_Color color, uint8_t brightness);

// power target in milli ampere
void IS31FL3731_RGB_91TKL_Power_Target(IS31FL3733_RGB_91TKL *device, uint16_t ma);

bool IS31FL3731_RGB_91TKL_Initialized(void);

/// Update LED matrix with internal buffer values.
void IS31FL3733_RGB_91TKL_Update(IS31FL3733_RGB_91TKL *device);
/// Update LED matrix LED enable/disable states with internal buffer values.
void IS31FL3733_RGB_91TKL_Update_LedEnableStates(IS31FL3733_RGB_91TKL *device);
/// Update LED matrix LED brightness values with internal buffer values.
void IS31FL3733_RGB_91TKL_Update_LedPwmValues(IS31FL3733_RGB_91TKL *device);

#endif /* _IS31FL3733_RGB_91TKL_H_ */
