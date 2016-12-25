/*
 * led_masks.h
 *
 *  Created on: 31.05.2016
 *      Author: wenkm
 */

#ifndef KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_LED_MASKS_H_
#define KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_LED_MASKS_H_

#include "IS31FL3731/IS31FL3731.h"

extern uint8_t const LedMaskFull_Left[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskOthr_Left[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskWASD_Left[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskCtrl_Left[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskCurs_Left[ISSI_LED_MASK_SIZE];

extern uint8_t const LedMaskFull_Right[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskOthr_Right[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskWASD_Right[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskCtrl_Right[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskCurs_Right[ISSI_LED_MASK_SIZE];

#endif /* KEYBOARD_ANORAK_KIIBOHD_LED_BACKLIGHT_LED_MASKS_H_ */
