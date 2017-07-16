
#include "type_o_circles.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "utils.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define RADIUS_COUNT (MATRIX_COLS)
static uint8_t *pressed_keys = 0;

void draw_circle_outline(IS31FL3733_91TKL *device, int16_t x0, int16_t y0, int16_t r, RGB color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    draw_keymatrix_rgb_pixel(device, x0, y0 + r, color);
    draw_keymatrix_rgb_pixel(device, x0, y0 - r, color);
    draw_keymatrix_rgb_pixel(device, x0 + r, y0, color);
    draw_keymatrix_rgb_pixel(device, x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        draw_keymatrix_rgb_pixel(device, x0 + x, y0 + y, color);
        draw_keymatrix_rgb_pixel(device, x0 - x, y0 + y, color);
        draw_keymatrix_rgb_pixel(device, x0 + x, y0 - y, color);
        draw_keymatrix_rgb_pixel(device, x0 - x, y0 - y, color);
        draw_keymatrix_rgb_pixel(device, x0 + y, y0 + x, color);
        draw_keymatrix_rgb_pixel(device, x0 - y, y0 + x, color);
        draw_keymatrix_rgb_pixel(device, x0 + y, y0 - x, color);
        draw_keymatrix_rgb_pixel(device, x0 - y, y0 - x, color);
    }
}

void type_o_circles_animation_start(void)
{
    animation_prepare(true);
    pressed_keys = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));

    dprintf("ram: %d\n", freeRam());
}

void type_o_circles_animation_stop(void)
{
    animation_postpare();
    free(pressed_keys);
}

void type_o_circles_animation_loop(void)
{
    is31fl3733_fill(issi.upper->device, 0);
    is31fl3733_fill(issi.lower->device, 0);

    uint8_t d = animation.hsv.v / RADIUS_COUNT;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            if (pressed_keys[key_row * MATRIX_COLS + key_col] > 0)
            {
                uint8_t keyr = pressed_keys[key_row * MATRIX_COLS + key_col];
                HSV hsv = {.h = animation.hsv.h, .s = animation.hsv.s, .v = animation.hsv.v};
                // HSV hsv = {animation.hsv.h, animation.hsv.s, animation.hsv.v};
                uint8_t endr = RADIUS_COUNT - keyr;

                for (uint8_t r = endr; r > 0; r--)
                {
                    RGB rgb = hsv_to_rgb(hsv);
                    draw_circle_outline(&issi, key_row, key_col, r, rgb);
                    hsv.v -= d;
                }

                uint8_t sat = (uint8_t)((animation.hsv2.s / RADIUS_COUNT) * endr);
                HSV hsv2 = { .h = animation.hsv2.h, .s = sat, .v = animation.hsv2.v};
                // HSV hsv2 = {animation.hsv2.h, sat, animation.hsv2.v};
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv2);

                pressed_keys[key_row * MATRIX_COLS + key_col]--;
            }
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void type_o_circles_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
    {
        if (pressed_keys[row_number * MATRIX_COLS + key_col] == 0 &&
            (row & ((matrix_row_t)1 << key_col)) /*matrix_is_on(key_row, key_col)*/)
            pressed_keys[row_number * MATRIX_COLS + key_col] = RADIUS_COUNT - 1;
    }
}

void set_animation_type_o_circles()
{
    dprintf("type_o_circles\n");

    animation.delay_in_ms = FPS_TO_DELAY(20);
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_circles_animation_start;
    animation.animationStop = &type_o_circles_animation_stop;
    animation.animationLoop = &type_o_circles_animation_loop;
    animation.animation_typematrix_row = &type_o_circles_typematrix_row;
}
