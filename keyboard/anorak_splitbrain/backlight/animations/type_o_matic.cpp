
#include "type_o_matic.h"
#include "../control.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include "matrix.h"

// const uint8_t PROGMEM type_o_matic_gamma_lot[] = {1, 2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 60, 60, 40, 30, 20, 15, 10,
// 8, 6, 4, 3, 2, 1};
static uint8_t animation_frame = 1;

void type_o_matic_animation_start()
{
    animation_frame = 1;

    animation_prepare(animation_frame);
}

void type_o_matic_animation_stop()
{
    animation_postpare(animation_frame);
}

void type_o_matic_animation_loop()
{
    uint8_t led_row;
    uint8_t led_col;

    for (uint8_t row = 0; row < MATRIX_ROWS; ++row)
    {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col)
        {
            uint8_t color;
        	getLedPosByMatrixKey(row, col, &led_row, &led_col);

            if (matrix_is_on(row, col))
            {
                issi.drawPixel(led_row, led_col, animation.brightness);
            }

            else
            {
            	color = issi.getPixel(led_row, led_col);

            	if (color >= 5)
            		color -= 5;

                issi.drawPixel(led_row, led_col, color);
            }
        }
    }

    issi.blitToFrame(animation_frame);
}
