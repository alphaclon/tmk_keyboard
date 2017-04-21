
#include "color_cycle_left_right.h"
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

void set_animation_color_cycle_left_right()
{
	dprintf("color_cycle_left_right\n");

    animation.delay_in_ms = 50;    // 50ms = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &color_cycle_left_right_animation_start;
    animation.animationStop = &color_cycle_left_right_animation_stop;
    animation.animationLoop = &color_cycle_left_right_animation_loop;
    animation.animation_typematrix_row = 0;
}

void color_cycle_left_right_typematrix_row(uint8_t row_number, matrix_row_t row)
{
}

void color_cycle_left_right_animation_start()
{
    offset = 0;
    animation_prepare(true);
    // TODO: Initialize with random colors?
}

void color_cycle_left_right_animation_stop()
{
    animation_postpare();
}

void color_cycle_left_right_animation_loop()
{
	HSV hsv = {.h = 0, .s = 255, .v = animation.hsv.v};

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            hsv.h = key_col + offset;
            draw_hsv_pixel(&issi, key_col, key_row, hsv);
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset++;
}

/*
void backlight_effect_cycle_left_right(void)
{
	uint8_t offset = g_tick & 0xFF;
	HSV hsv = { .h = 0, .s = 255, .v = g_config.brightness };
	RGB rgb;
	Point point;
	for ( int i=0; i<72; i++ )
	{
		uint16_t offset2 = g_key_hit[i]<<2;
		// stabilizer LEDs use spacebar hits
		if ( i == 36+6 || i == 54+13 || // LC6, LD13
				( g_config.use_7u_spacebar && i == 54+14 ) ) // LD14
		{
			offset2 = g_key_hit[36+0]<<2;
		}
		offset2 = (offset2<=63) ? (63-offset2) : 0;

		map_led_to_point( i, &point );
		// Relies on hue being 8-bit and wrapping
		hsv.h = point.x + offset + offset2;
		rgb = hsv_to_rgb( hsv );
		backlight_set_color( i, rgb.r, rgb.g, rgb.b );
	}
}
*/
