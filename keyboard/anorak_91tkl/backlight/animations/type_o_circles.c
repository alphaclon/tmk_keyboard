
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

#define RADIUS_COUNT 15
static uint8_t *pressed_keys = 0;

// Draw a circle outline
void draw_circle_outline(IS31FL3733_91TKL *device, int16_t key_row, int16_t key_col, int16_t r, RGB color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    draw_keymatrix_rgb_pixel(device, key_col, key_row + r, color);
    draw_keymatrix_rgb_pixel(device, key_col, key_row - r, color);
    draw_keymatrix_rgb_pixel(device, key_col + r, key_row, color);
    draw_keymatrix_rgb_pixel(device, key_col - r, key_row, color);

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

        draw_keymatrix_rgb_pixel(device, key_col + y, key_row + x, color);
        draw_keymatrix_rgb_pixel(device, key_col - y, key_row + x, color);
        draw_keymatrix_rgb_pixel(device, key_col + y, key_row - x, color);
        draw_keymatrix_rgb_pixel(device, key_col - y, key_row - x, color);
        draw_keymatrix_rgb_pixel(device, key_col + x, key_row + y, color);
        draw_keymatrix_rgb_pixel(device, key_col - x, key_row + y, color);
        draw_keymatrix_rgb_pixel(device, key_col + x, key_row - y, color);
        draw_keymatrix_rgb_pixel(device, key_col - x, key_row - y, color);
    }
}

void type_o_circles_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    type_o_circles_animation_loop();
}

void type_o_circles_animation_start()
{
	animation_prepare(true);
    pressed_keys = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));

    dprintf("ram: %d\n", freeRam());
}

void type_o_circles_animation_stop()
{
	animation_postpare();
    free(pressed_keys);
}

void type_o_circles_animation_loop()
{
    is31fl3733_fill(issi.upper->device, 0);
    is31fl3733_fill(issi.lower->device, 0);

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            if (pressed_keys[key_row * MATRIX_ROWS + key_col] == 0 && matrix_is_on(key_row, key_col))
                pressed_keys[key_row * MATRIX_ROWS + key_col] = RADIUS_COUNT;

            if (pressed_keys[key_row * MATRIX_ROWS + key_col] > 0)
            {
                uint8_t r = RADIUS_COUNT - pressed_keys[key_row * MATRIX_ROWS + key_col];

                // TODO: Farbe abhängig vom Radius setzen

                draw_circle_outline(&issi, key_row, key_col, r, animation.rgb);
                pressed_keys[key_row * MATRIX_ROWS + key_col]--;
            }
        }
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void set_animation_type_o_circles()
{
	dprintf("type_o_circles\n");

    animation.delay_in_ms = FPS_TO_DELAY(2);
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_circles_animation_start;
    animation.animationStop = &type_o_circles_animation_stop;
    animation.animationLoop = &type_o_circles_animation_loop;
    animation.animation_typematrix_row = &type_o_circles_typematrix_row;
}


