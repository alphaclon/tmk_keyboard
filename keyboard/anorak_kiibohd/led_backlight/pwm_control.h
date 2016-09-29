#ifndef _PWM_CONTROL_H_
#define _PWM_CONTROL_H_

#include "IS31FL3731/Adafruit_IS31FL3731.h"

// Basic LED PWM Control Capability
typedef enum _tLedPWMControlMode
{
    // Single LED Modes
    LedControlMode_brightness_set_led,
    LedControlMode_brightness_decrease_led,
    LedControlMode_brightness_increase_led,
    // Set all LEDs / masked LEDs
    LedControlMode_brightness_set_all,
    LedControlMode_brightness_set_all_masked,
    LedControlMode_brightness_set_by_mask,
    // Increase/Decrease all LEDs / masked LEDs
    LedControlMode_brightness_decrease_all,
    LedControlMode_brightness_increase_all,
    LedControlMode_brightness_decrease_all_masked,
    LedControlMode_brightness_increase_all_masked,
    LedControlMode_brightness_decrease_by_mask,
    LedControlMode_brightness_increase_by_mask,
} tLedPWMControlMode;

typedef struct _tLedPWMControlCommand
{
    tLedPWMControlMode mode;
    uint8_t amount;
    uint16_t index;
    uint8_t mask[ISSI_LED_MASK_SIZE];
} tLedPWMControlCommand;

// Basic LED Region Control Capability
typedef enum _tLedRegionControlMode
{
    // Region support
    LedControlMode_enable_mask,
    LedControlMode_disable_mask,
    LedControlMode_xor_mask,
} tLedRegionControlMode;

typedef struct _tLedRegionControlCommand
{
    tLedRegionControlMode mode;
    uint8_t mask[ISSI_LED_MASK_SIZE];
} tLedRegionControlCommand;

void IS31FL3731_init();
void IS31FL3731_enable();
void IS31FL3731_PWM_control(tLedPWMControlCommand *control);
void IS31FL3731_region_control(tLedRegionControlCommand *control);

void IS31FL3731_set_maximum_power_consumption(uint16_t value);

#endif
