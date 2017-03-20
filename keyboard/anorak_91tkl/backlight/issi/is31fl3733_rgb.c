/*
 * issi31fl3733_rgb.c
 *
 *  Created on: 02.03.2017
 *      Author: wenkm
 */

#include "is31fl3733_rgb.h"

void IS31FL3733_RGB_Init(IS31FL3733_RGB *device, enum IS31FL3733_RGB_ColorOrder color_order)
{
    switch (color_order)
    {
    case RGB:
        device->red_offset = 0;
        device->green_offset = 1;
        device->blue_offset = 2;
        break;
    case RBG:
        device->red_offset = 0;
        device->green_offset = 2;
        device->blue_offset = 1;
        break;
    }

    IS31FL3733_Init(device->device);
}

void IS31FL3733_RGB_SetBrightnessForMasked(IS31FL3733_RGB *device, enum IS31FL3733_RGB_Color color, uint8_t brightness)
{
    uint8_t i;
    uint8_t mask_byte;
    uint8_t mask_bit;

    switch (color)
    {
    case red:
    	i = device->red_offset;
    	break;
    case green:
    	i = device->green_offset;
    	break;
    case blue:
    	i = device->blue_offset;
    	break;
    }

    // Set brightness level of all LED's.
    for (; i < IS31FL3733_LED_PWM_SIZE; i += 3)
    {
        uint8_t mask_byte = i / 8;
        uint8_t mask_bit = i % 8;

        if (device->device->leds_mask[mask_byte] & (1 << mask_bit))
        	device->device->pwm[i] = brightness;
    }
}
