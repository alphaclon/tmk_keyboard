#ifndef KEYBOARD_SECTOR_LED_MASKS_H_
#define KEYBOARD_SECTOR_LED_MASKS_H_

#include "../issi/is31fl3733.h"
#include <avr/pgmspace.h>

extern uint8_t const LedMaskFull_Top[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskOthr_Top[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskWASD_Top[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskCtrl_Top[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskCurs_Top[IS31FL3733_LED_ENABLE_SIZE];

extern uint8_t const LedMaskFull_Bottom[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskOthr_Bottom[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskWASD_Bottom[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskCtrl_Bottom[IS31FL3733_LED_ENABLE_SIZE];
extern uint8_t const LedMaskCurs_Bottom[IS31FL3733_LED_ENABLE_SIZE];

extern PGM_P LedMasksTop[];
extern PGM_P LedMasksBottom[];

#endif /* KEYBOARD_ANORAK_SECTOR_LED_MASKS_H_ */
