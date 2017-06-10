#ifndef KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_
#define KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_

#include "matrix.h"
#include "../issi/is31fl3733_91tkl.h"
#include <inttypes.h>
#include <stdbool.h>

#define FPS_TO_DELAY(fps) (1000/fps)
#define DEVICE_BY_NUMBER(issi, device_number) (device_number ? issi.upper : issi.lower)

struct _animation_interface
{
    RGB rgb;
    HSV hsv;
    HSV hsv2;

    uint16_t delay_in_ms;
    uint32_t duration_in_ms;

    uint16_t loop_timer;
    uint32_t duration_timer;

    bool is_running;

    void (*animationStart)(void);
    void (*animationStop)(void);
    void (*animationLoop)(void);
    void (*animation_typematrix_row)(uint8_t row_number, matrix_row_t row);
};

typedef struct _animation_interface animation_interface;

extern animation_interface animation;

void animation_prepare(bool set_all_to_black);
void animation_postpare(void);

void animation_default_animation_start(void);
void animation_default_animation_start_clear(void);
void animation_default_animation_stop(void);

void animation_default_typematrix_row(uint8_t row_number, matrix_row_t row);

uint8_t key_was_pressed(uint8_t key_row, uint8_t key_col);

void draw_keymatrix_rgb_pixel(IS31FL3733_91TKL *device_91tkl, int16_t row, int16_t col, RGB color);
void draw_keymatrix_hsv_pixel(IS31FL3733_91TKL *device_91tkl, int16_t row, int16_t col, HSV color);

void draw_direct_keymatrix_rgb_pixel(IS31FL3733_91TKL *device_91tkl, int16_t row, int16_t col, RGB color);
void draw_direct_keymatrix_hsv_pixel(IS31FL3733_91TKL *device_91tkl, int16_t row, int16_t col, HSV color);

#endif
