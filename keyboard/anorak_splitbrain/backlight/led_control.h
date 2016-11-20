#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include "control.h"

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

extern uint8_t LedMask[ISSI_LED_MASK_SIZE];

void IS31FL3731_region_control(tLedRegionControlCommand *control);

#endif
