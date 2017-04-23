
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

void set_animation_type_o_raindrops(void)
{
	dprintf("type_o_raindrops\n");

    animation.delay_in_ms = FPS_TO_DELAY(10);
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_raindrops_animation_start;
    animation.animationStop = &type_o_raindrops_animation_stop;
    animation.animationLoop = &type_o_raindrops_animation_loop;
    animation.animation_typematrix_row = &type_o_raindrops_typematrix_row;
}

void type_o_raindrops_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    type_o_raindrops_animation_loop();
}

void type_o_raindrops_animation_start()
{
	animation_prepare(true);
}

void type_o_raindrops_animation_stop()
{
	animation_postpare();
}

void type_o_raindrops_animation_loop()
{
	HSV hsv;
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
                hsv.h = rand() & 0xff;
                hsv.s = rand() & 0xff;
                // Override brightness with global brightness control
                hsv.v = animation.hsv.v;

                is31fl3733_hsv_set_pwm(device, row, col, hsv);
            }
            else
            {
                RGB color = is31fl3733_rgb_get_pwm(device, row, col);

                hsv = rgb_to_hsv(color);
                hsv.v = decrement(hsv.v, 5, 0, 255);

                is31fl3733_hsv_set_pwm(device, row, col, hsv);
            }
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}
