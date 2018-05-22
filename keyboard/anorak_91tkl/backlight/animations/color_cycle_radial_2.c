
#include "color_cycle_radial_2.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "map_led_to_point_polar.h"
#include "timer.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static uint8_t offset;

void color_cycle_radial_2_animation_loop(void)
{
	uint8_t row;
	uint8_t col;
	HSV hsv = {.h = 0, .s = animation.hsv.h, .v = animation.hsv.v};

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
        	map_led_to_point_polar( key_row * MATRIX_COLS + key_col, &row, &col );
    		uint8_t offset2 = offset + col;
    		if ( offset2 & 0x80 )
    		{
    			offset2 = ~offset2;
    		}
    		offset2 = offset2 >> 2;
    		hsv.h = animation.hsv.h + offset2;
    		hsv.s = 127 + ( row >> 1 );
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset++;
}

void set_animation_color_cycle_radial_2()
{
	dprintf("color_cycle_radial_2\n");

	offset = 0;

    animation.delay_in_ms = FPS_TO_DELAY(20);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &color_cycle_radial_2_animation_loop;
    animation.animation_typematrix_row = 0;
}

/*

void backlight_effect_cycle_radial2(void)
{
	uint8_t offset = ( g_tick << g_config.effect_speed ) & 0xFF;

	HSV hsv = { .h = 0, .s = g_config.color_1.s, .v = g_config.brightness };
	RGB rgb;
	Point point;
	for ( int i=0; i<72; i++ )
	{
		map_led_to_point_polar( i, &point );
		uint8_t offset2 = offset + point.x;
		if ( offset2 & 0x80 )
		{
			offset2 = ~offset2;
		}
		offset2 = offset2 >> 2;
		hsv.h = g_config.color_1.h + offset2;
		hsv.s = 127 + ( point.y >> 1 );
		rgb = hsv_to_rgb( hsv );
		backlight_set_color( i, rgb.r, rgb.g, rgb.b );
	}
}

*/
