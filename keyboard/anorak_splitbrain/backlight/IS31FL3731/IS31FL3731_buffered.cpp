
#include "../../backlight/IS31FL3731/IS31FL3731_buffered.h"

#include <util/delay.h>

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

IS31FL3731Buffered::IS31FL3731Buffered(uint8_t x, uint8_t y) : IS31FL3731(x, y)
{
    _pwm_buffer_size = x * y;
    _pwm_buffer = (uint8_t *)malloc(_pwm_buffer_size);
}

IS31FL3731Buffered::~IS31FL3731Buffered()
{
}

void IS31FL3731Buffered::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    // check rotation, move pixel around if necessary
    switch (getRotation())
    {
    case 1:
        _swap_int16_t(x, y);
        x = 16 - x - 1;
        break;
    case 2:
        x = 16 - x - 1;
        y = 9 - y - 1;
        break;
    case 3:
        _swap_int16_t(x, y);
        y = 9 - y - 1;
        break;
    }

#ifdef IS31FL3731_DO_CHECKS
    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;
    if (color > 255)
        color = 255; // PWM 8bit max
#endif

    _pwm_buffer[x + y * 16] = color;
}

void IS31FL3731Buffered::blitToFrame(uint8_t frame)
{
	setLedsBrightness(_pwm_buffer, frame);
}
