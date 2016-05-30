#ifndef _PWM_CONTROL_H_
#define _PWM_CONTROL_H_

#include "IS31FL3731/Adafruit_IS31FL3731.h"

extern uint8_t const LedMaskFull[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskWASD[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskCtrl[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskLogo[ISSI_LED_MASK_SIZE];
extern uint8_t const LedMaskJump[ISSI_LED_MASK_SIZE];

// Basic LED Control Capability
typedef enum _tLedPWMControlMode
{
    // Single LED Modes
    LedControlMode_brightness_decrease,
    LedControlMode_brightness_increase,
    LedControlMode_brightness_set,
    // Set all LEDs (index argument not required)
    LedControlMode_brightness_decrease_all,
    LedControlMode_brightness_increase_all,
    LedControlMode_brightness_decrease_mask,
    LedControlMode_brightness_increase_mask,
    LedControlMode_brightness_set_all,
	// Region support
	LedControlMode_enable_mask,
	LedControlMode_disable_mask,
	LedControlMode_xor_mask,

} tLedPWMControlMode;

typedef struct _tLedPWMControlCommand
{
	tLedPWMControlMode mode;
    uint8_t amount;
    uint16_t index;
    uint8_t const *mask;
} tLedPWMControlCommand;

void IS31FL3731_init();
void IS31FL3731_enable();
void IS31FL3731_PWM_control(tLedPWMControlCommand *control);

#endif
