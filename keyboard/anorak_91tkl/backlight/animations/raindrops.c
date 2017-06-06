
#include "raindrops.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

void set_animation_raindrops()
{
	dprintf("raindrops\n");

    animation.delay_in_ms = FPS_TO_DELAY(20);    // = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &raindrops_animation_start;
    animation.animationStop = &raindrops_animation_stop;
    animation.animationLoop = &raindrops_animation_loop;
    animation.animation_typematrix_row = 0;
}

void raindrops_typematrix_row(uint8_t row_number, matrix_row_t row)
{
}

void raindrops_animation_start()
{
    animation_prepare(false);
    // TODO: Initialize with random colors?
}

void raindrops_animation_stop()
{
    animation_postpare();
}

void raindrops_animation_loop()
{
    // Change one LED every tick
    uint8_t row_to_change = rand() % MATRIX_ROWS;
    uint8_t col_to_change = rand() % MATRIX_COLS;

    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    IS31FL3733_RGB *device;

    HSV hsv;

    if (!getLedPosByMatrixKey(row_to_change, col_to_change, &device_number, &row, &col))
    	return;

    device = DEVICE_BY_NUMBER(issi, device_number);

    int16_t h1 = animation.hsv.h;
    int16_t h2 = animation.hsv2.h;
    int16_t deltaH = h2 - h1;
    deltaH /= 4;

    // Take the shortest path between hues
    if (deltaH > 127)
    {
        deltaH -= 256;
    }
    else if (deltaH < -127)
    {
        deltaH += 256;
    }

    int16_t s1 = animation.hsv.s;
    int16_t s2 = animation.hsv2.s;
    int16_t deltaS = (s2 - s1) / 4;

    hsv.h = h1 + (deltaH * (rand() & 0x03));
    hsv.s = s1 + (deltaS * (rand() & 0x03));
    // Override brightness with global brightness control
    hsv.v = animation.hsv.v;

    is31fl3733_hsv_set_pwm(device, row, col, hsv);

    // TODO: optimize: write rgb values directly to device by ignoring the buffer
    is31fl3733_91tkl_update_led_pwm(&issi);
}