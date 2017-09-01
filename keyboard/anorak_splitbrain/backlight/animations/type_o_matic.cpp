
#include "type_o_matic.h"
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

static uint8_t animation_frame = 1;

void type_o_matic_animation_start(void)
{
    animation_frame = 1;

    animation_prepare(animation_frame);
}

void type_o_matic_animation_stop(void)
{
    animation_postpare(animation_frame);
}

void type_o_matic_animation_loop(void)
{
    uint8_t led_row;
    uint8_t led_col;
    bool is_left_side;

    for (uint8_t row = 0; row < MATRIX_ROWS; ++row)
    {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col)
        {
            uint8_t color;
            getLedPosByMatrixKey(row, col, &led_row, &led_col, &is_left_side);

            if (is_left_side != is_left_side_of_keyboard())
                continue;

            if (matrix_is_on(row, col))
            {
                issi.drawPixel(led_col, led_row, animation.brightness);
            }
            else
            {
                color = issi.getPixel(led_col, led_row);

                if (color >= 5)
                    color -= 5;
                else
                    color = 0;

                issi.drawPixel(led_col, led_row, color);
            }
        }
    }

    issi.blitToFrame(animation_frame);
}

void type_o_matic_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    type_o_matic_animation_loop();
}

void set_animation_type_o_matic()
{
    dprintf("type_o_matic\n");

    animation.brightness = 255;
    animation.delay_in_ms = FPS_TO_DELAY(4);
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_matic_animation_start;
    animation.animationStop = &type_o_matic_animation_stop;
    animation.animationLoop = &type_o_matic_animation_loop;
    animation.animation_typematrix_row = &type_o_matic_typematrix_row;
}
