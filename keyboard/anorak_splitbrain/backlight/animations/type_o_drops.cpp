
#include "type_o_drops.h"
#include "../../splitbrain.h"
#include "../control.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"

#define GAMMA_STEPS 8
extern uint8_t gamma_correction_table[GAMMA_STEPS];

#define RADIUS_COUNT 5

static uint8_t *pressed_keys = 0;
static uint8_t animation_frame = 1;

void type_o_drops_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    type_o_drops_animation_loop();
}

void type_o_drops_animation_start()
{
    animation_frame = 1;
    pressed_keys = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));
    animation_prepare(animation_frame);
}

void type_o_drops_animation_stop()
{
    animation_postpare(animation_frame);
    free(pressed_keys);
}

void type_o_drops_animation_loop()
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

                for (uint8_t r = 0; r < pressed_keys[row * MATRIX_ROWS + col]; ++r)
                {
                	issi.drawCircle(led_col, led_row, r, gamma_correction_table[r]);
                }

                pressed_keys[row * MATRIX_ROWS + col]--;
            }
        }
    }

    issi.blitToFrame(animation_frame);
}
