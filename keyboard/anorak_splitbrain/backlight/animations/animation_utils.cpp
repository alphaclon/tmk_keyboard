
#include "animation_utils.h"
#include "../../splitbrain.h"
#include "../../utils.h"
#include "../control.h"
#include "../key_led_map.h"
#include "../led_masks.h"
#include "config.h"
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

animation_interface animation;
static uint8_t *key_pressed_count = 0;

void get_full_led_mask(uint8_t mask[ISSI_LED_MASK_SIZE])
{
    if (is_left_side_of_keyboard())
    {
        memcpy_P(mask, LedMaskFull_Left, ISSI_LED_MASK_SIZE);
    }
    else
    {
        memcpy_P(mask, LedMaskFull_Right, ISSI_LED_MASK_SIZE);
    }
}

void animation_prepare(uint8_t animation_frame)
{
    uint8_t mask[ISSI_LED_MASK_SIZE];

    dprintf("ani: save, %d\n", freeRam());

    key_pressed_count = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));

    get_full_led_mask(mask);

    issi.clear();
    issi.setFrame(animation_frame);
    issi.enableLeds(mask, animation_frame);
    issi.blitToFrame(animation_frame);
    issi.displayFrame(animation_frame);

    dprintf("ani: ram:%d\n", freeRam());
}

void animation_postpare(uint8_t animation_frame)
{
	free(key_pressed_count);

    issi.setFrame(0);
    issi.displayFrame(0);

    dprintf("ram: %d\n", freeRam());
}

void animation_default_animation_start()
{
    animation_prepare(1);
}

void animation_default_animation_stop()
{
    animation_postpare(1);
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
