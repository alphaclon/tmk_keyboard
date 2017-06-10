
#include "color_wave.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "timer.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static int8_t center_column;
static int8_t direction;

int16_t deltaH;
int16_t deltaS;

void color_wave_colors(void)
{
    int16_t h1 = animation.hsv.h;
    int16_t h2 = animation.hsv2.h;
    deltaH = (h2 - h1) / (MATRIX_COLS);

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
    deltaS = (s2 - s1) / (MATRIX_COLS);

    if (deltaS > 127)
    {
        deltaS -= 256;
    }
    else if (deltaS < -127)
    {
        deltaS += 256;
    }

    dprintf("dh: %d, ds: %d\n", deltaH, deltaS);
}

void color_wave_animation_start(void)
{
    animation_prepare(true);
    color_wave_colors();
}

void color_wave_animation_loop(void)
{
    HSV hsv = {.h = animation.hsv.h, .s = animation.hsv.s, .v = animation.hsv.v};

    center_column += direction;

    if (center_column >= MATRIX_COLS)
    {
        direction = -1;
        center_column = MATRIX_COLS - 1;
    }
    else if (center_column < 0)
    {
        direction = 1;
        center_column = 0;
    }



    int16_t sh = hsv.h - (center_column * deltaH);
    int16_t ss = hsv.s - (center_column * deltaS);

    dprintf("---\n");
    dprintf("cc: %d, s: %u\n", center_column);
    dprintf("h: %u, s: %u\n", hsv.h, hsv.s);
    dprintf("sh: %d, ss: %d\n", sh, ss);

    hsv.h = sh;
    hsv.s = ss;



	for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
	{
		dprintf("---\n");
		for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
		{
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);

            dprintf("c: h: %u, s: %u\n", key_col, hsv.h, hsv.s);

            if (key_col < center_column)
            {
            	hsv.h += deltaH;
            	hsv.s += deltaS;
            }
            else
            {
            	hsv.h -= deltaH;
            	hsv.s -= deltaS;
            }
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void set_animation_color_wave()
{
    dprintf("color_wave\n");

    center_column = rand() % MATRIX_COLS;
    direction = rand() % 2 ? 1 : -1;

    animation.delay_in_ms = FPS_TO_DELAY(1); // 50ms = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &color_wave_animation_start;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &color_wave_animation_loop;
    animation.animation_typematrix_row = 0;
}

/*
void backlight_effect_wave(void)
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
