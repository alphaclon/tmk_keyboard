
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

void set_animation_type_o_matic(void)
{
	dprintf("type_o_matic\n");

    animation.delay_in_ms = FPS_TO_DELAY(2);
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_matic_animation_start;
    animation.animationStop = &type_o_matic_animation_stop;
    animation.animationLoop = &type_o_matic_animation_loop;
    animation.animation_typematrix_row = &type_o_matic_typematrix_row;
}

void type_o_matic_typematrix_row(uint8_t row_number, matrix_row_t row_columns)
{
    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    IS31FL3733_RGB *device;

    uint8_t key_row = row_number;

	for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
	{
		if (getLedPosByMatrixKey(key_row, key_col, &device_number, &row, &col))
		{
			if (matrix_is_on(key_row, key_col))
			{
				device = DEVICE_BY_NUMBER(issi, device_number);
				is31fl3733_rgb_set_pwm(device, col, row, animation.rgb);
			}
		}
	}

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void type_o_matic_animation_start()
{
	animation_prepare(true);
}

void type_o_matic_animation_stop()
{
	animation_postpare();
}

void type_o_matic_animation_loop()
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
            	RGB rgb = is31fl3733_rgb_get_pwm(device, col, row);

				if (rgb.r && rgb.g && rgb.b)
					continue;

                is31fl3733_rgb_set_pwm(device, col, row, animation.rgb);
            }
            else
            {
                RGB color = is31fl3733_rgb_get_pwm(device, col, row);

                hsv = rgb_to_hsv(color);
                dprintf("v=%u", hsv.v);
                hsv.v = decrement(hsv.v, 1, 0, 255);

                is31fl3733_hsv_set_pwm(device, col, row, hsv);
            }
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}
