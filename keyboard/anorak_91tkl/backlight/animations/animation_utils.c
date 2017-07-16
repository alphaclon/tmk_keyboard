
#include "animation_utils.h"
#include "config.h"
#include "../sector/sector_control.h"
#include "../key_led_map.h"
#include "../../utils.h"
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

animation_interface animation;

static uint8_t *upper_leds;
static uint8_t *upper_pwm;
static uint8_t *lower_leds;
static uint8_t *lower_pwm;

static uint8_t *key_pressed_count = 0;
static bool hardware_was_enabled = false;

void animation_prepare(bool set_all_to_black)
{
    dprintf("ani: save, %d\n", freeRam());

    hardware_was_enabled = is31fl3733_91tkl_is_hardware_enabled(&issi);

    upper_leds = (uint8_t *)malloc(IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
    upper_pwm = (uint8_t *)malloc(IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

    lower_leds = (uint8_t *)malloc(IS31FL3733_LED_ENABLE_SIZE * sizeof(uint8_t));
    lower_pwm = (uint8_t *)malloc(IS31FL3733_LED_PWM_SIZE * sizeof(uint8_t));

    key_pressed_count = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));

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

    is31fl3733_91tkl_hardware_shutdown(&issi, false);

    dprintf("ani: ram:%d\n", freeRam());
}

void animation_postpare()
{
    dprintf("ani: restore, %d\n", freeRam());

    if (!hardware_was_enabled)
        is31fl3733_91tkl_hardware_shutdown(&issi, true);

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

    free(key_pressed_count);

    dprintf("ram: %d\n", freeRam());
}

void animation_default_animation_start()
{
    animation_prepare(false);
}

void animation_default_animation_start_clear()
{
    animation_prepare(true);
}

void animation_default_animation_stop()
{
    animation_postpare();
}

uint8_t key_was_pressed(uint8_t key_row, uint8_t key_col)
{
    return key_pressed_count[key_row * MATRIX_COLS + key_col];
}

void animation_default_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
    {
        if ((row & ((matrix_row_t)1 << key_col)))
            key_pressed_count[row_number * MATRIX_COLS + key_col]++;
    }
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
    RGB color_rgb = hsv_to_rgb(color);
    draw_keymatrix_rgb_pixel(device_91tkl, key_row, key_col, color_rgb);

#if 0
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
#endif
}

void draw_direct_keymatrix_rgb_pixel(IS31FL3733_91TKL *device_91tkl, int16_t key_row, int16_t key_col, RGB color)
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
        // is31fl3733_rgb_set_pwm(device, col, row, color);
        is31fl3733_rgb_direct_set_pwm(device, col, row, color);
    }
}

void draw_direct_keymatrix_hsv_pixel(IS31FL3733_91TKL *device_91tkl, int16_t key_row, int16_t key_col, HSV color)
{
    RGB color_rgb = hsv_to_rgb(color);
    draw_direct_keymatrix_rgb_pixel(device_91tkl, key_row, key_col, color_rgb);
}
