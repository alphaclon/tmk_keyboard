
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
static uint8_t offset2;

void color_cycle_all_animation_loop(void)
{
	HSV hsv = {.h = offset, .s = animation.hsv.s, .v = animation.hsv.v};

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
        	offset2 = key_was_pressed(key_row, key_col) << 2;
        	// Relies on hue being 8-bit and wrapping
        	hsv.h = offset + offset2;
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset++;
}

void set_animation_color_cycle_all()
{
	dprintf("color_cycle_all\n");

	offset = 0;
	offset2 = 0;

    animation.delay_in_ms = FPS_TO_DELAY(20);    // 50ms = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &color_cycle_all_animation_loop;
    animation.animation_typematrix_row = 0;
}
