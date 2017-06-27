
#include "gradient_full_flicker.h"
#include "animation_utils.h"
#include "../key_led_map.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static uint8_t offset = 0;
static int8_t leftright = 1;

void gradient_full_flicker_animation_loop(void)
{
    // Divide delta by MATRIX_COLS, this gives the delta per column
    int16_t deltaHc = 256 / MATRIX_COLS;
    HSV hsv = { .h = 0, .s = 255, .v = animation.hsv.v };
    //HSV hsv = { 0, 255, animation.hsv.v };

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            // Relies on hue being 8-bit and wrapping
            hsv.h = ( deltaHc * key_col ) + offset;
            //hsv.s = animation.hsv.s + ( deltaS * key_row );
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset += leftright;
}

void gradient_full_flicker_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	if (!row)
		return;

    uint8_t speed = (rand() & 0x07) + 1;
    leftright = speed * ((rand() & 0x01) ? -1 : 1);
}

void set_animation_gradient_full_flicker()
{
    dprintf("gradient_full_flicker\n");

    animation.delay_in_ms = FPS_TO_DELAY(100);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &gradient_full_flicker_animation_loop;
    animation.animation_typematrix_row = &gradient_full_flicker_typematrix_row;
}
