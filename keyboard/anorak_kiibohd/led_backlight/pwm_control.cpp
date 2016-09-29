
#include "pwm_control.h"

extern "C" {
#ifdef USE_BUFFERED_TWI
#include "../twi/i2c.h"
#else
#include "../i2cmaster/i2cmaster.h"
#endif
#include "debug.h"
}

uint8_t currentBank = 0;
uint8_t LedMask[ISSI_LED_MASK_SIZE] = {0};
uint8_t LedPWMPageBuffer[ISSI_TOTAL_CHANNELS] = {0};
uint16_t maximum_total_pwm = ISSI_TOTAL_CHANNELS * 255;

Adafruit_IS31FL3731 issi;

void IS31FL3731_init()
{
#ifdef USE_BUFFERED_TWI
    i2cInit();
    i2cSetBitrate(400);
#else
    i2c_init();
#endif

    issi.begin();
}

void IS31FL3731_enable()
{
    //memcpy_P(LedMask, LedMaskFull, ISSI_LED_MASK_SIZE);
    //issi.setLEDEnableMaskForAllBanks(LedMask);
}

void IS31FL3731_set_maximum_power_consumption(uint16_t value)
{
    maximum_total_pwm = value;
}

void select_next_bank()
{
    ++currentBank;
    if (currentBank >= ISSI_TOTAL_FRAMES)
        currentBank = 0;
}

uint8_t channel_enabled_by_mask(uint16_t channel, tLedPWMControlCommand *control)
{
    uint8_t mask_byte = channel / 8;
    uint8_t mask_bit = channel % 8;

    if ((mask_byte < ISSI_LED_MASK_SIZE) && (control->mask[i] & (1 << mask_bit)))
        return 1;
    return 0;
}

uint8_t channel_enabled_masked(uint16_t channel)
{
    uint8_t mask_byte = channel / 8;
    uint8_t mask_bit = channel % 8;

    if ((mask_byte < ISSI_LED_MASK_SIZE) && (LedMask[i] & (1 << mask_bit)))
        return 1;
    return 0;
}

uint16_t get_pwm_summed_up()
{
    uint16_t sum = 0; // Maximum is 255 * ISSI_TOTAL_CHANNELS

    for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        sum += LedPWMPageBuffer[channel];

    return sum;
}

void fix_max_pwm()
{
    if (maximum_total_pwm >= ISSI_TOTAL_CHANNELS * 255)
        return;

    uint16_t sum = get_pwm_summed_up();

    while (sum > maximum_total_pwm)
    {
        sum = 0;

        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            if (LedPWMPageBuffer[channel] > 5)
                LedPWMPageBuffer[channel] -= 5;
            sum += LedPWMPageBuffer[channel];
        }
    }
}

void IS31FL3731_PWM_control(tLedPWMControlCommand *control)
{
    dprintf("IS31FL3731_PWM_control %d: %d\n", control->mode, control->amount);

    select_next_bank();

    // Configure based upon the given mode
    // TODO Perhaps do gamma adjustment?

    switch (control->mode)
    {
    case LedControlMode_brightness_decrease_led:
        LedPWMPageBuffer[control->index] -= control->amount;
        //issi.setLEDPWM(control->index, LedPWMPageBuffer[control->index], currentBank);
        break;

    case LedControlMode_brightness_increase_led:
        LedPWMPageBuffer[control->index] += control->amount;
        //issi.setLEDPWM(control->index, LedPWMPageBuffer[control->index], currentBank);
        break;

    case LedControlMode_brightness_set_led:
        LedPWMPageBuffer[control->index] = control->amount;
        //issi.setLEDPWM(control->index, LedPWMPageBuffer[control->index], currentBank);
        break;

    case LedControlMode_brightness_set_all:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            LedPWMPageBuffer[channel] = control->amount;
        }
        break;

    case LedControlMode_brightness_set_by_mask:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            if (channel_enabled_by_mask(channel, control))
                LedPWMPageBuffer[channel] = control->amount;
        }
        break;

    case LedControlMode_brightness_set_all_masked:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            if (channel_enabled_masked(channel))
                LedPWMPageBuffer[channel] = control->amount;
        }
        break;

    case LedControlMode_brightness_decrease_all:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            LedPWMPageBuffer[channel] -= control->amount;
        }
        break;

    case LedControlMode_brightness_increase_all:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_decrease_all_masked:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            if (channel_enabled_masked(channel))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_increase_all_masked:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            if (channel_enabled_masked(channel))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_decrease_by_mask:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            if (channel_enabled_by_mask(channel, control))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_increase_by_mask:
        for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
        {
            if (channel_enabled_by_mask(channel, control))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;
    }

    fix_max_pwm();

    issi.setLEDPWM(LedPWMPageBuffer, currentBank);
    issi.displayFrame(currentBank);
}

void IS31FL3731_region_control(tLedRegionControlCommand *control)
{
    dprintf("IS31FL3731_region_control: %d\n", control->mode);

    select_next_bank();

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
    issi.displayFrame(currentBank);
}

