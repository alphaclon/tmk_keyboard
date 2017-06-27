
#include "gradient_left_right.h"
#include "animation_utils.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static uint8_t offset = 0;

void gradient_left_right_animation_loop(void)
{
    int16_t h1 = animation.hsv.h;
    int16_t h2 = animation.hsv2.h;
    int16_t deltaH = h2 - h1;

    // Take the shortest path between hues
    if ( deltaH > 127 )
    {
        deltaH -= 256;
    }
    else if ( deltaH < -127 )
    {
        deltaH += 256;
    }
    // Divide delta by MATRIX_COLS, this gives the delta per column
    deltaH /= MATRIX_COLS;

    //int16_t s1 = animation.hsv.s;
    //int16_t s2 = animation.hsv2.s;
    //int16_t deltaS = ( s2 - s1 ) / MATRIX_COLS;

    HSV hsv = { .h = 0, .s = 255, .v = animation.hsv.v };
    //HSV hsv = { 0, 255, animation.hsv.v };

    /*

    Point point;
    for ( int i=0; i<72; i++ )
    {
        map_led_to_point( i, &point );
        // The y range will be 0..64, map this to 0..4
        uint8_t y = (point.y>>4);
        // Relies on hue being 8-bit and wrapping
        hsv.h = g_config.color_1.h + ( deltaH * y );
        hsv.s = g_config.color_1.s + ( deltaS * y );
        rgb = hsv_to_rgb( hsv );
        backlight_set_color( i, rgb.r, rgb.g, rgb.b );
    }

    */

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            // The y range will be 0..64, map this to 0..4
            //uint8_t y = ((key_row * key_col) >> 4);
            // Relies on hue being 8-bit and wrapping
            hsv.h = animation.hsv.h + ( deltaH * key_col ) + offset;
            //hsv.s = animation.hsv.s + ( deltaS * key_row );
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset++;
}

void set_animation_gradient_left_right()
{
    dprintf("gradient_left_right\n");

    animation.delay_in_ms = FPS_TO_DELAY(40);    // 50ms = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &gradient_left_right_animation_loop;
    animation.animation_typematrix_row = 0;
}
