/** ISSI IS31FL3733 RGB header file.
  */
#ifndef _is31fl3733_91tkl_H_
#define _is31fl3733_91tkl_H_

#include <stdbool.h>
#include "is31fl3733_rgb.h"

/** IS31FL3733_RGB 91TKL structure.
  */
struct IS31FL3733_RGB_91TKL
{
	IS31FL3733_RGB *upper;
	IS31FL3733_RGB *lower;
};

typedef struct IS31FL3733_RGB_91TKL IS31FL3733_91TKL;

extern IS31FL3733_91TKL issi;

/// Init LED matrix for normal operation.
void is31fl3733_91tkl_init(IS31FL3733_91TKL *device);

void is31fl3733_91tkl_hardware_shutdown(IS31FL3733_91TKL *device, bool enabled);

/// Set brightness level for all enabled LEDs.
void is31fl3733_91tkl_fill_rgb_masked(IS31FL3733_91TKL *device, RGB color);

/// Set brightness level for one color all enabled LEDs.
void is31fl3733_91tkl_fill_hsv_masked(IS31FL3733_91TKL *device, HSV color);

// power target in milliampere
void is31fl3733_91tkl_power_target(IS31FL3733_91TKL *device, uint16_t milliampere);

bool is31fl3733_91tkl_initialized(void);

/// Update LED matrix with internal buffer values.
void is31fl3733_91tkl_update(IS31FL3733_91TKL *device);
/// Update LED matrix LED enable/disable states with internal buffer values.
void is31fl3733_91tkl_update_led_enable(IS31FL3733_91TKL *device);
/// Update LED matrix LED brightness values with internal buffer values.
void is31fl3733_91tkl_update_led_pwm(IS31FL3733_91TKL *device);

#endif /* _is31fl3733_91tkl_H_ */
