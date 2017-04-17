
#include "animation_utils.h"
#include "config.h"
#include <avr/pgmspace.h>

animation_interface animation;

void animation_prepare(uint8_t animation_frame)
{
    //uint8_t led_row;
    //uint8_t led_col;

    /* TODO: save old state
    uint8_t mask[ISSI_LED_MASK_SIZE];
    get_full_led_mask(mask);

    issi.clear();
    issi.setFrame(animation_frame);
    issi.enableLeds(mask, animation_frame);
    issi.blitToFrame(animation_frame);
    issi.displayFrame(animation_frame);
    */
}

void animation_postpare(uint8_t animation_frame)
{
	/* TODO: restore old state
    issi.setFrame(0);
    issi.displayFrame(0);
    */
}
