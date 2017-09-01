
#include "type_o_circles.h"
#include "../../splitbrain.h"
#include "../control.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define RADIUS_COUNT 15

static uint8_t *pressed_keys = 0;
static uint8_t animation_frame = 1;


void type_o_circles_animation_start(void)
{
    animation_frame = 1;
    pressed_keys = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));
    animation_prepare(animation_frame);
}

void type_o_circles_animation_stop(void)
{
    animation_postpare(animation_frame);
    free(pressed_keys);
}

void type_o_circles_animation_loop(void)
{
    bool is_left_side;
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
                getLedPosByMatrixKey(row, col, &led_row, &led_col, &is_left_side);

                if (is_left_side != is_left_side_of_keyboard())
                    continue;

                uint8_t r = RADIUS_COUNT - pressed_keys[row * MATRIX_ROWS + col];
                issi.drawCircle(led_col, led_row, r, 128);
                pressed_keys[row * MATRIX_ROWS + col]--;
            }
        }
    }

    issi.blitToFrame(animation_frame);
}

void type_o_circles_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    type_o_circles_animation_loop();
}

void set_animation_type_o_circles()
{
    dprintf("type_o_circles\r\n");

    animation.brightness = 255;
    animation.delay_in_ms = 250;
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_circles_animation_start;
    animation.animationStop = &type_o_circles_animation_stop;
    animation.animationLoop = &type_o_circles_animation_loop;
    animation.animation_typematrix_row = &type_o_circles_typematrix_row;
}
