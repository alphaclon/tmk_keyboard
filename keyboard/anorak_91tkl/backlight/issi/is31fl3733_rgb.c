
#include "is31fl3733_rgb.h"

#ifdef DEBUG_ISSI
#include "debug.h"
#else
#include "nodebug.h"
#endif

void is31fl3733_rgb_init(IS31FL3733_RGB *device)
{
    device->offsets.r = 1;
    device->offsets.g = 0;
    device->offsets.b = 2;

    is31fl3733_init(device->device);
}

void is31fl3733_rgb_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, RGB color)
{
    for (uint8_t c = 0; c < 3; ++c)
        is31fl3733_set_pwm(device->device, x, (y * 3) + device->offsets.color[c], color.rgb[c]);
}

void is31fl3733_rgb_direct_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, RGB color)
{
    for (uint8_t c = 0; c < 3; ++c)
        is31fl3733_direct_set_pwm(device->device, x, (y * 3) + device->offsets.color[c], color.rgb[c]);
}

void is31fl3733_hsv_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, HSV color)
{
    RGB color_rgb = hsv_to_rgb(color);
    is31fl3733_rgb_set_pwm(device, x, y, color_rgb);
}

void is31fl3733_hsv_direct_set_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y, HSV color)
{
    RGB color_rgb = hsv_to_rgb(color);
    is31fl3733_rgb_direct_set_pwm(device, x, y, color_rgb);
}

RGB is31fl3733_rgb_get_pwm(IS31FL3733_RGB *device, uint8_t x, uint8_t y)
{
    RGB color;
    for (uint8_t c = 0; c < 3; ++c)
        color.rgb[c] = is31fl3733_get_pwm(device->device, x, (y * 3) + device->offsets.color[c]);
    return color;
}

void is31fl3733_fill_rgb_masked(IS31FL3733_RGB *device, RGB rgb)
{
    // for (uint8_t c = 0; c < 3; ++c)
    //	is31fl3733_fill_masked(device->device, color.rgb[device->offsets.color[c]]);

    uint8_t pos;
    uint8_t offset;
    uint8_t mask_bit;
    uint8_t color;

    // Set brightness level of all LED's.
    for (uint8_t c = 0; c < IS31FL3733_SW; ++c)
    {
        for (uint8_t i = 0; i < IS31FL3733_CS; i++)
        {
            pos = i + (IS31FL3733_CS * c);
            offset = pos / 8;
            mask_bit = (0x01 << (pos % 8));
            color = c % 3;

            if (device->device->mask[offset] & mask_bit)
            {
                device->device->pwm[pos] = rgb.rgb[device->offsets.color[color]];
            }
        }
    }
}

void is31fl3733_fill_hsv_masked(IS31FL3733_RGB *device, HSV color)
{
    RGB color_rgb = hsv_to_rgb(color);
    is31fl3733_fill_rgb_masked(device, color_rgb);
}
