
#include "sweep.h"
#include "config.h"
#include "matrix.h"
#include "../issi/is31fl3733_91tkl.h"
#include "animation_utils.h"
#include <avr/pgmspace.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define SWEEP_ROWS (MATRIX_ROWS*3)

// The lookup table to make the brightness changes be more visible
uint8_t const sweep[] PROGMEM = {1, 3, 6, 10, 20, 30, 40, 50, 60, 70, 80, 100, 100, 80, 70, 60, 50, 40, 30, 20, 10, 6, 3, 1};
static uint8_t incr = 0;

void sweep_animation_loop()
{
    // animate over all the pixels, and set the brightness from the sweep table

	incr++;
	if (incr >= 24)
		incr = 0;

	for (uint8_t x = 0; x < MATRIX_COLS; x++)
	{
		for (uint8_t y = 0; y < SWEEP_ROWS; y++)
		{
			is31fl3733_set_pwm(issi.upper->device, x, y, pgm_read_byte(&sweep[(x + y + incr) % 24]));
			is31fl3733_set_pwm(issi.lower->device, x, y, pgm_read_byte(&sweep[(x + y + incr) % 24]));
		}
	}

	is31fl3733_91tkl_update_led_pwm(&issi);
}

void set_animation_sweep()
{
	dprintf("sweep\n");

    animation.delay_in_ms = FPS_TO_DELAY(20);
    animation.duration_in_ms = 0;

    animation.animationStart = &animation_default_animation_start_clear;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &sweep_animation_loop;
    animation.animation_typematrix_row = 0;
}
