
#include "type_o_matic.h"
#include "../control.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"

static uint8_t animation_frame = 1;

void type_o_matic_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	type_o_matic_animation_loop();
}

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
                issi.drawPixel(led_col, led_row, animation.brightness);
            }

            else
            {
            	color = issi.getPixel(led_row, led_col);

            	if (color >= 5)
            		color -= 5;

                issi.drawPixel(led_col, led_row, color);
            }
        }
    }

    issi.blitToFrame(animation_frame);
}
