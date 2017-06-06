
#include "flying_ball.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static int16_t fb_x;
static uint8_t fb_x_dir;
static int16_t fb_y;
static uint8_t fb_y_dir;
static uint8_t offset;
static HSV hsv;
static RGB rgb;

void set_animation_flying_ball()
{
	dprintf("flying_ball\n");

    animation.delay_in_ms = FPS_TO_DELAY(20);    // 50ms = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &flying_ball_animation_start;
    animation.animationStop = &flying_ball_animation_stop;
    animation.animationLoop = &flying_ball_animation_loop;
    animation.animation_typematrix_row = &flying_ball_typematrix_row;
}

void flying_ball_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	hsv.h = rand() % 255;
}

void flying_ball_animation_start()
{
	offset = 0;

	hsv.h = animation.hsv.h;
	hsv.s = animation.hsv.s;
	hsv.v = animation.hsv.v;

    fb_x = rand() % MATRIX_COLS;
    fb_y = rand() % MATRIX_ROWS;

    fb_x_dir = rand() % 2;
    fb_y_dir = rand() % 2;

    animation_prepare(false);
}

void flying_ball_animation_stop()
{
    animation_postpare();
}

void flying_ball_animation_loop()
{
	uint8_t row;
	uint8_t col;
	uint8_t device_number;
    IS31FL3733_RGB *device;

    draw_rgb_pixel(&issi, fb_x, fb_y, rgb);

    fb_x += fb_x_dir;
    fb_y += fb_y_dir;

    if (fb_x >= MATRIX_COLS)
	{
    	fb_x_dir = -1;
    	fb_x = MATRIX_COLS - 1;
	}
    else if (fb_x < 0)
    {
    	fb_x_dir = 1;
    	fb_x = 0;
    }

    if (fb_y >= MATRIX_ROWS)
	{
    	fb_y_dir = -1;
    	fb_y = MATRIX_ROWS - 1;
	}
    else if (fb_y < 0)
    {
    	fb_y_dir = 1;
    	fb_y = 0;
    }

    if (getLedPosByMatrixKey(fb_y, fb_x, &device_number, &row, &col))
    {
		device = DEVICE_BY_NUMBER(issi, device_number);
		rgb = is31fl3733_rgb_get_pwm(device, row, col);

		draw_hsv_pixel(&issi, fb_x, fb_y, hsv);
    }

    is31fl3733_91tkl_update_led_pwm(&issi);

    offset++;
}