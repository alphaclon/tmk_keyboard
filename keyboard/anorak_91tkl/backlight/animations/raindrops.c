
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

void raindrops_typematrix_row(uint8_t row_number, matrix_row_t row)
{
}

void raindrops_animation_loop(void)
{
    HSV hsv;

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

    // Change one LED every tick
    uint8_t row_to_change = rand() % MATRIX_ROWS;
    uint8_t col_to_change = rand() % MATRIX_COLS;

    draw_direct_keymatrix_hsv_pixel(&issi, row_to_change, col_to_change, hsv);

#if 0

    uint8_t row;
    uint8_t col;
    uint8_t device_number;

    if (!getLedPosByMatrixKey(row_to_change, col_to_change, &device_number, &row, &col))
        return;

    IS31FL3733_RGB *device;
    device = DEVICE_BY_NUMBER(issi, device_number);

    // TODO: optimize: write rgb values directly to device by ignoring the buffer
    //is31fl3733_hsv_direct_set_pwm(device, col, row, hsv);

    is31fl3733_hsv_set_pwm(device, col, row, hsv);
    is31fl3733_91tkl_update_led_pwm(&issi);
#endif
}

void set_animation_raindrops()
{
    dprintf("raindrops\n");

    animation.delay_in_ms = FPS_TO_DELAY(10); // = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &raindrops_animation_loop;
    animation.animation_typematrix_row = 0;
}
