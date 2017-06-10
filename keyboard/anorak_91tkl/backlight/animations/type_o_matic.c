
#include "type_o_matic.h"
#include "../key_led_map.h"
#include "../../utils.h"
#include "animation_utils.h"
#include "config.h"

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

void type_o_matic_typematrix_row(uint8_t row_number, matrix_row_t row_columns)
{
    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    IS31FL3733_RGB *device;

    for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
    {
        //if (matrix_is_on(row_number, key_col))
    	if (row_columns & ((matrix_row_t)1 << key_col))
        {
            if (getLedPosByMatrixKey(row_number, key_col, &device_number, &row, &col))
            {
                device = DEVICE_BY_NUMBER(issi, device_number);
                is31fl3733_rgb_set_pwm(device, col, row, animation.rgb);
            }
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void type_o_matic_animation_loop(void)
{
    // HSV hsv;
    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    IS31FL3733_RGB *device;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            if (!getLedPosByMatrixKey(key_row, key_col, &device_number, &row, &col))
                continue;

            device = DEVICE_BY_NUMBER(issi, device_number);

            if (matrix_is_on(key_row, key_col))
            {
                RGB rgb = is31fl3733_rgb_get_pwm(device, col, row);

                if (rgb.r && rgb.g && rgb.b)
                    continue;

                is31fl3733_rgb_set_pwm(device, col, row, animation.rgb);
            }
            else
            {
                RGB color = is31fl3733_rgb_get_pwm(device, col, row);

                color.r = decrement(color.r, 3, 0, 255);
                color.g = decrement(color.g, 3, 0, 255);
                color.b = decrement(color.b, 3, 0, 255);

                /*
                hsv = rgb_to_hsv(color);
                dprintf("v=%u", hsv.v);
                hsv.v = decrement(hsv.v, 1, 0, 255);
                */

                is31fl3733_rgb_set_pwm(device, col, row, color);
            }
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void set_animation_type_o_matic(void)
{
    dprintf("type_o_matic\n");

    animation.delay_in_ms = FPS_TO_DELAY(6);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &type_o_matic_animation_loop;
    animation.animation_typematrix_row = &type_o_matic_typematrix_row;
}
