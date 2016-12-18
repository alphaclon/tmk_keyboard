
#include "type_o_circles.h"
#include "../control.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include "matrix.h"

#define RADIUS_COUNT 15

static uint8_t *pressed_keys = 0;
static uint8_t animation_frame = 1;

void type_o_circles_animation_start()
{
    animation_frame = 1;
    pressed_keys = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));
    animation_prepare(animation_frame);
}

void type_o_circles_animation_stop()
{
    animation_postpare(animation_frame);
    free(pressed_keys);
}

void type_o_circles_animation_loop()
{
    uint8_t led_row;
    uint8_t led_col;

    issi.clear();

    for (uint8_t row = 0; row < MATRIX_ROWS; ++row)
    {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col)
        {
            if (pressed_keys[row * MATRIX_ROWS + col] == 0 && matrix_is_on(row, col))
                pressed_keys[row * MATRIX_ROWS + col] = RADIUS_COUNT;

            if (pressed_keys[row * MATRIX_ROWS + col] > 0)
            {
                uint8_t r = RADIUS_COUNT - pressed_keys[row * MATRIX_ROWS + col];
                getLedPosByMatrixKey(row, col, &led_row, &led_col);
                issi.drawCircle(led_row, led_col, r, 128);
                pressed_keys[row * MATRIX_ROWS + col]--;
            }
        }
    }

    issi.blitToFrame(animation_frame);
}
