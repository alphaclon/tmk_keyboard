/*
 * led_masks.h
 *
 *  Created on: 31.05.2016
 *      Author: wenkm
 */

#ifndef KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_LED_MASKS_H_
#define KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_LED_MASKS_H_

#include "IS31FL3731/Adafruit_IS31FL3731.h"

extern uint8_t const LedMaskFull[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskOthr[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskWASD[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskCtrl[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskLogo[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskJump[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskCase[ISSI_LED_MASK_SIZE];

#endif /* KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_LED_MASKS_H_ */
