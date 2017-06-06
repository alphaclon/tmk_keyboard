
#include "color_cycle_all.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static uint8_t offset;

void set_animation_color_cycle_all()
{
	dprintf("color_cycle_all\n");

    animation.delay_in_ms = FPS_TO_DELAY(20);    // 50ms = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &color_cycle_all_animation_start;
    animation.animationStop = &color_cycle_all_animation_stop;
    animation.animationLoop = &color_cycle_all_animation_loop;
    animation.animation_typematrix_row = &color_cycle_all_typematrix_row;
}

void color_cycle_all_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	offset = 255 - offset;
}

void color_cycle_all_animation_start()
{
    offset = 0;
    animation_prepare(true);
}

void color_cycle_all_animation_stop()
{
    animation_postpare();
}

void color_cycle_all_animation_loop()
{
	HSV hsv = {.h = offset, .s = animation.hsv.s, .v = animation.hsv.v};

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
        	hsv.h = offset;
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset++;
}
