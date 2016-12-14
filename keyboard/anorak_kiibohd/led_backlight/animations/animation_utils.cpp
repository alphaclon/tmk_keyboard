
#include "animation_utils.h"
#include "../../splitbrain.h"
#include "../control.h"
#include "../key_led_map.h"
#include "config.h"
#include "led_masks.h"
#include <avr/pgmspace.h>

void get_full_led_mask(uint8_t mask[ISSI_LED_MASK_SIZE])
{
    memcpy_P(mask, LedMaskFull, ISSI_LED_MASK_SIZE);
}

/*
void get_full_led_mask(uint8_t region, uint8_t mask[ISSI_LED_MASK_SIZE])
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
*/

void animation_prepare(uint8_t animation_frame)
{
    uint8_t led_row;
    uint8_t led_col;
    uint8_t mask[ISSI_LED_MASK_SIZE];

    get_full_led_mask(mask);

    issi.clear();
    issi.setFrame(animation_frame);
    issi.enableLeds(mask, animation_frame);
    issi.blitToFrame(animation_frame);
    issi.displayFrame(animation_frame);
}

void animation_postpare(uint8_t animation_frame)
{
    issi.setFrame(0);
    issi.displayFrame(0);
}
