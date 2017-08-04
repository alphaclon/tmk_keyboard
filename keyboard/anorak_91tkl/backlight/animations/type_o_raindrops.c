
#include "type_o_raindrops.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include "../../utils.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

void type_o_raindrops_typematrix_row(uint8_t row_number, matrix_row_t row_columns)
{
    HSV hsv;
    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    IS31FL3733_RGB *device;
    bool changed = false;

    for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
    {
        // if (matrix_is_on(row_number, key_col))
        if (row_columns & ((matrix_row_t)1 << key_col))
        {
            if (getLedPosByMatrixKey(row_number, key_col, &device_number, &row, &col))
            {
                device = DEVICE_BY_NUMBER(issi, device_number);
                RGB rgb = is31fl3733_rgb_get_pwm(device, col, row);

                if (rgb.r && rgb.g && rgb.b)
                    continue;

                hsv.h = rand() & 0xff;
                hsv.s = rand() & 0xff;
                // Override brightness with global brightness control
                hsv.v = animation.hsv.v;

                is31fl3733_hsv_set_pwm(device, col, row, hsv);
                changed = true;
            }
        }
    }

    if (changed)
    	is31fl3733_91tkl_update_led_pwm(&issi);
}

void type_o_raindrops_animation_loop()
{
    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    bool changed = false;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            if (!getLedPosByMatrixKey(key_row, key_col, &device_number, &row, &col))
                continue;

            IS31FL3733_RGB *device;
            device = DEVICE_BY_NUMBER(issi, device_number);

            if (matrix_is_on(key_row, key_col))
            {
                RGB rgb = is31fl3733_rgb_get_pwm(device, col, row);

                if (rgb.r && rgb.g && rgb.b)
                    continue;

                HSV hsv;
                hsv.h = rand() & 0xff;
                hsv.s = rand() & 0xff;
                // Override brightness with global brightness control
                hsv.v = animation.hsv.v;

                is31fl3733_hsv_set_pwm(device, col, row, hsv);
                changed = true;
            }
            else
            {
                RGB color = is31fl3733_rgb_get_pwm(device, col, row);

                if (color.r == 0 && color.g == 0 && color.b == 0)
                    continue;

                color.r = decrement(color.r, 3, 0, 255);
                color.g = decrement(color.g, 3, 0, 255);
                color.b = decrement(color.b, 3, 0, 255);

                /*
                HSV hsv = rgb_to_hsv(color);
                hsv.v = decrement(hsv.v, 1, 0, 255);
                */

                // is31fl3733_hsv_set_pwm(device, col, row, hsv);
                is31fl3733_rgb_set_pwm(device, col, row, color);
                changed = true;
            }
        }
    }

    if (changed)
    	is31fl3733_91tkl_update_led_pwm(&issi);
}

void set_animation_type_o_raindrops(void)
{
    dprintf("type_o_raindrops\n");

    animation.delay_in_ms = FPS_TO_DELAY(6);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &type_o_raindrops_animation_loop;
    animation.animation_typematrix_row = &type_o_raindrops_typematrix_row;
}
