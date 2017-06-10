
#include "jellybean_raindrops.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

void jellybean_raindrops_animation_loop(void)
{
    // Change one LED every tick
    uint8_t row_to_change = rand() % MATRIX_ROWS;
    uint8_t col_to_change = rand() % MATRIX_COLS;

    HSV hsv;
    hsv.h = rand() & 0xff;
    hsv.s = rand() & 0xff;
    // Override brightness with global brightness control
    hsv.v = animation.hsv.v;

    draw_direct_keymatrix_hsv_pixel(&issi, row_to_change, col_to_change, hsv);

    /*
    // TODO: optimize: write rgb values directly to device by ignoring the buffer
    is31fl3733_91tkl_update_led_pwm(&issi);
    */
}

void set_animation_jellybean_raindrops()
{
	dprintf("jellybean_raindrops\n");

    animation.delay_in_ms = FPS_TO_DELAY(5);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &jellybean_raindrops_animation_loop;
    animation.animation_typematrix_row = 0;
}

/*
void backlight_effect_jellybean_raindrops( bool initialize )
{
	HSV hsv;
	RGB rgb;

	// Change one LED every tick
	uint8_t led_to_change = ( g_tick & 0x000 ) == 0 ? rand() % 72 : 255;

	for ( int i=0; i<72; i++ )
	{
		// If initialize, all get set to random colors
		// If not, all but one will stay the same as before.
		if ( initialize || i == led_to_change )
		{
			hsv.h = rand() & 0xFF;
			hsv.s = rand() & 0xFF;
			// Override brightness with global brightness control
			hsv.v = g_config.brightness;;

			rgb = hsv_to_rgb( hsv );
			backlight_set_color( i, rgb.r, rgb.g, rgb.b );
		}
	}
}
*/
