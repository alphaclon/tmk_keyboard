
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

static uint8_t offset = 0;
static int8_t direction = 1;
static bool updown = true;

void color_wave_animation_loop(void)
{
    int16_t h1 = animation.hsv.h;
    int16_t h2 = animation.hsv2.h;
    int16_t deltaH = h2 - h1;

    // Take the shortest path between hues
    if (deltaH > 127)
    {
        deltaH -= 256;
    }
    else if (deltaH < -127)
    {
        deltaH += 256;
    }

    int16_t deltaHr = deltaH / MATRIX_ROWS;

    // Divide delta by MATRIX_COLS, this gives the delta per col
    deltaH /= MATRIX_COLS;

    HSV hsv = { .h = animation.hsv.h, .s = 255, .v = animation.hsv.v};
    //HSV hsv = {animation.hsv.h, 255, animation.hsv.v};

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            // Relies on hue being 8-bit and wrapping

            if (updown)
                hsv.h = animation.hsv.h + (deltaHr * key_row) + offset;
            else
                hsv.h = animation.hsv.h + (deltaH * key_col) + offset;

            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset += direction;
}

void color_wave_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	if (!row)
		return;

    uint8_t speed = (rand() & 0x07) + 1;
    direction = speed * ((rand() & 0x01) ? -1 : 1);
    updown = (rand() & 0x01);
}

void set_animation_color_wave()
{
    dprintf("color_wave\n");

    animation.delay_in_ms = FPS_TO_DELAY(50);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &color_wave_animation_loop;
    animation.animation_typematrix_row = &color_wave_typematrix_row;
}
