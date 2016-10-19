
#include "led_control.h"
#include "led_masks.h"

extern "C" {
#ifdef USE_BUFFERED_TWI
#include "twi/twi_master.h"
#else
#include "i2cmaster/i2cmaster.h"
#endif
#include "debug.h"
}

uint8_t currentBank = 0;
uint8_t LedMask[ISSI_LED_MASK_SIZE] = {0};

void IS31FL3731_region_control(tLedRegionControlCommand *control)
{
    //dprintf("IS_region_control: %d\n", control->mode);

    //select_next_bank();

    switch (control->mode)
    {
    case LedControlMode_enable_mask:
        for (uint8_t i = 0; i < ISSI_LED_MASK_SIZE; i++)
        {
            LedMask[i] |= control->mask[i];
        }
        break;

    case LedControlMode_disable_mask:
        for (uint8_t i = 0; i < ISSI_LED_MASK_SIZE; i++)
        {
            LedMask[i] &= ~(control->mask[i]);
        }
        break;

    case LedControlMode_xor_mask:
        for (uint8_t i = 0; i < ISSI_LED_MASK_SIZE; i++)
        {
            LedMask[i] ^= control->mask[i];
        }
        break;
    }

    issi.setLEDEnableMask(LedMask, currentBank);
    //issi.displayFrame(currentBank);
}

