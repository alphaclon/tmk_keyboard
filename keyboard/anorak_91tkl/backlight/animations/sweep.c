
#include "sweep.h"
#include "config.h"
#include "matrix.h"
#include "../issi/is31fl3733_91tkl.h"
#include "animation_utils.h"
#include <avr/pgmspace.h>
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static uint8_t offset = 0;
static int8_t direction = 1;
static bool updown = true;

void sweep_animation_loop(void)
{
    uint8_t deltaV = (animation.hsv.v * 2) / MATRIX_COLS;
    HSV hsv = { .h = animation.hsv.h, .s = animation.hsv.s, .v = animation.hsv.v };
    //HSV hsv = { animation.hsv.h, animation.hsv.s, animation.hsv.v };

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        uint8_t c = 0;
        int8_t d = 1;

        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            uint8_t col = (key_col + offset) % MATRIX_COLS;

            hsv.v = (deltaV * c);
            draw_keymatrix_hsv_pixel(&issi, key_row, col, hsv);

            c += d;
            if (c >= MATRIX_COLS / 2)
                d = -1;
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset += direction;
}

void sweep_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	if (!row)
		return;

    uint8_t speed = (rand() & 0x01) + 1;
    direction = speed * ((rand() & 0x01) ? -1 : 1);
    updown = (rand() & 0x01);
}

void set_animation_sweep()
{
	dprintf("sweep\n");

    animation.delay_in_ms = FPS_TO_DELAY(10);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &sweep_animation_loop;
    animation.animation_typematrix_row = &sweep_typematrix_row;
}
