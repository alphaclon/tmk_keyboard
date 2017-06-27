
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
static int8_t fb_x_dir;
static int16_t fb_y;
static int8_t fb_y_dir;
static HSV hsv;
static RGB rgb;

void flying_ball_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	if (!row)
		return;

	hsv.h = rand() % 255;

    fb_x_dir = rand() % 2 ? 1 : - 1;
    fb_y_dir = rand() % 2 ? 1 : - 1;
}

void flying_ball_animation_start(void)
{
	animation_prepare(true);

	hsv.h = animation.hsv.h;
	hsv.s = animation.hsv.s;
	hsv.v = animation.hsv.v;

    fb_x = rand() % MATRIX_COLS;
    fb_y = rand() % MATRIX_ROWS;

    fb_x_dir = rand() % 2 ? 1 : - 1;
    fb_y_dir = rand() % 2 ? 1 : - 1;

    //dprintf("x:%d/%d y:%d/%d  ", fb_x, fb_x_dir, fb_y, fb_y_dir);

	uint8_t row;
	uint8_t col;
	uint8_t device_number;
    IS31FL3733_RGB *device;

    if (getLedPosByMatrixKey(fb_y, fb_x, &device_number, &row, &col))
	{
    	device = DEVICE_BY_NUMBER(issi, device_number);
    	rgb = is31fl3733_rgb_get_pwm(device, col, row);
	}
}

void flying_ball_animation_loop(void)
{
	uint8_t row;
	uint8_t col;
	uint8_t device_number;
    IS31FL3733_RGB *device;

    draw_direct_keymatrix_rgb_pixel(&issi, fb_y, fb_x, rgb);

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

    //dprintf("x:%d/%d y:%d/%d  ", fb_x, fb_x_dir, fb_y, fb_y_dir);

    if (getLedPosByMatrixKey(fb_y, fb_x, &device_number, &row, &col))
    {
		device = DEVICE_BY_NUMBER(issi, device_number);
		rgb = is31fl3733_rgb_get_pwm(device, col, row);

		draw_direct_keymatrix_hsv_pixel(&issi, fb_y, fb_x, hsv);
    }

	//is31fl3733_91tkl_update_led_pwm(&issi);
}

void set_animation_flying_ball()
{
	dprintf("flying_ball\n");

    animation.delay_in_ms = FPS_TO_DELAY(5);    // 50ms = 20 fps
    animation.duration_in_ms = 0;

    animation.animationStart = &flying_ball_animation_start;
    animation.animationStop = &animation_default_animation_stop;
    animation.animationLoop = &flying_ball_animation_loop;
    animation.animation_typematrix_row = &flying_ball_typematrix_row;
}
