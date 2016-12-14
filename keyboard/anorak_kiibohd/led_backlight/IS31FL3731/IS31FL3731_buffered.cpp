
#include "IS31FL3731_buffered.h"
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

    clear();
}

IS31FL3731Buffered::~IS31FL3731Buffered()
{
	free(_pwm_buffer);
}

void IS31FL3731Buffered::clear()
{
	memset(_pwm_buffer, 0, _pwm_buffer_size);
}

void IS31FL3731Buffered::drawPixel(int16_t x, int16_t y, uint16_t color)
{
#ifdef IS31FL3731_SUPPORT_ROTATION
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
#endif

    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;
#ifdef IS31FL3731_DO_CHECKS
    if (color > 255)
        color = 255; // PWM 8bit max
#endif

    _pwm_buffer[x + y * 16] = color;
}

uint8_t IS31FL3731Buffered::getPixel(int16_t x, int16_t y)
{
#ifdef IS31FL3731_SUPPORT_ROTATION
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
#endif

    if ((x < 0) || (x >= 16))
        return 0;
    if ((y < 0) || (y >= 9))
        return 0;
#ifdef IS31FL3731_DO_CHECKS
    if (color > 255)
        color = 255; // PWM 8bit max
#endif

    return _pwm_buffer[x + y * 16];
}


void IS31FL3731Buffered::blitToFrame(uint8_t frame)
{
	setLedsBrightness(_pwm_buffer, frame);
}
