#ifndef _IS31FL3733_RGB_H_
#define _IS31FL3733_RGB_H_

#include "is31fl3733.h"
#include "../color.h"

/** IS31FL3733_RGB structure.
  */
struct IS31FL3733_RGB_Device
{
    IS31FL3733 *device;

    union {
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        uint8_t color[3];
    } offsets;
};

typedef struct IS31FL3733_RGB_Device IS31FL3733_RGB;

/// Init LED matrix for normal operation.
void is31fl3733_rgb_init(IS31FL3733_RGB *device);

/// Set LED brightness level.
void is31fl3733_rgb_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, RGB color);
/// Set LED brightness level.
void is31fl3733_rgb_direct_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, RGB color);

/// Get LED brightness level.
RGB is31fl3733_rgb_get_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y);

/// Set LED brightness level.
void is31fl3733_hsv_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, HSV color);
/// Set LED brightness level.
void is31fl3733_hsv_direct_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, HSV color);

/// Set brightness level for all enabled LEDs.
void is31fl3733_fill_rgb_masked(IS31FL3733_RGB *device, RGB color);

/// Set brightness level for one color all enabled LEDs.
void is31fl3733_fill_hsv_masked(IS31FL3733_RGB *device, HSV color);

#endif /* _IS31FL3733_RGB_H_ */
