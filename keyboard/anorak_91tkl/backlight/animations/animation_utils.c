
#include "animation_utils.h"
#include "config.h"
#include "../sector/sector_control.h"
#include "../key_led_map.h"
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

animation_interface animation;

static uint8_t* upper_leds;
static uint8_t* upper_pwm;
static uint8_t* lower_leds;
static uint8_t* lower_pwm;

void animation_prepare(bool set_all_to_black)
{
	upper_leds = (uint8_t*)malloc(IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
	upper_pwm = (uint8_t*)malloc(IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

	lower_leds = (uint8_t*)malloc(IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
	lower_pwm = (uint8_t*)malloc(IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

	memcpy(upper_leds, is31fl3733_led_buffer(issi.upper->device), IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
	memcpy(upper_pwm, is31fl3733_pwm_buffer(issi.upper->device), IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

	memcpy(lower_leds, is31fl3733_led_buffer(issi.lower->device), IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
	memcpy(lower_pwm, is31fl3733_pwm_buffer(issi.lower->device), IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

	sector_enable_all_leds();

	if (set_all_to_black)
	{
		is31fl3733_fill(issi.upper->device, 0);
		is31fl3733_fill(issi.lower->device, 0);
	}

	is31fl3733_91tkl_update_led_pwm(&issi);
	is31fl3733_91tkl_update_led_enable(&issi);

	dprintf("go\n");
}

void animation_postpare()
{
	memcpy(is31fl3733_led_buffer(issi.upper->device), upper_leds, IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
	memcpy(is31fl3733_pwm_buffer(issi.upper->device), upper_pwm, IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

	memcpy(is31fl3733_led_buffer(issi.lower->device), lower_leds, IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
	memcpy(is31fl3733_pwm_buffer(issi.lower->device), lower_pwm, IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

	is31fl3733_91tkl_update_led_pwm(&issi);
	is31fl3733_91tkl_update_led_enable(&issi);

	free(upper_leds);
	free(upper_pwm);

	free(lower_leds);
	free(lower_pwm);
}

void draw_keymatrix_rgb_pixel(IS31FL3733_91TKL *device_91tkl, int16_t key_row, int16_t key_col, RGB color)
{
    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    IS31FL3733_RGB *device;

    if (key_col < 0 || key_col >= MATRIX_COLS)
        return;
    if (key_row < 0 || key_row >= MATRIX_ROWS)
        return;

    if (getLedPosByMatrixKey(key_row, key_col, &device_number, &row, &col))
    {
    	device = DEVICE_BY_NUMBER(issi, device_number);
    	is31fl3733_rgb_set_pwm(device, col, row, color);
    }
}

void draw_keymatrix_hsv_pixel(IS31FL3733_91TKL *device_91tkl, int16_t key_row, int16_t key_col, HSV color)
{
    uint8_t row;
    uint8_t col;
    uint8_t device_number;
    IS31FL3733_RGB *device;

    if (key_col < 0 || key_col >= MATRIX_COLS)
        return;
    if (key_row < 0 || key_row >= MATRIX_ROWS)
        return;

    if (getLedPosByMatrixKey(key_row, key_col, &device_number, &row, &col))
    {
    	device = DEVICE_BY_NUMBER(issi, device_number);
    	is31fl3733_hsv_set_pwm(device, col, row, color);
    }
}
