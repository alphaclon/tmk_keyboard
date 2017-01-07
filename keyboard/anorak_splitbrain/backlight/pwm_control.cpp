
#include "pwm_control.h"
#include "led_control.h"
#include "led_masks.h"

#ifdef DEBUG_BACKLIGHT
#include "debug.h"
#else
#include "nodebug.h"
#endif

/*
 * I_PWM = (I_LED / 256) * SUM(LED_PWM)
 */

#define R_EXT 18
#define POWER_FACTOR 64700
#define I_LED_MAX (POWER_FACTOR / R_EXT)

uint8_t current_pwm_bank = 0;
uint8_t LedPWMPageBuffer[ISSI_USED_CHANNELS] = {0};
uint32_t maximum_total_I_led = 100;

void IS31FL3731_set_power_target_I_max(double value)
{
    maximum_total_I_led = value;
}

uint32_t get_I_led_summed_up()
{
    uint32_t sum = 0;

    for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
    {
        if (LedPWMPageBuffer[channel])
        {
            sum += LedPWMPageBuffer[channel];
        }
    }

    sum *= I_LED_MAX;
    sum /= 256;
    sum /= 1000;

    return sum;
}

void fix_max_I_led()
{
    uint32_t I_leds = get_I_led_summed_up();

    while (I_leds > maximum_total_I_led)
    {
        dprintf("fixing %lu\r\n", I_leds);

        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (LedPWMPageBuffer[channel] > 5)
                LedPWMPageBuffer[channel] -= 5;
        }

        I_leds = get_I_led_summed_up();
    }

    dprintf("fixed to %lu\r\n", I_leds);
}

void select_next_bank()
{
    ++current_pwm_bank;
    if (current_pwm_bank >= ISSI_TOTAL_FRAMES)
        current_pwm_bank = 0;
}

uint8_t channel_enabled_by_mask(uint16_t channel, tLedPWMControlCommand *control)
{
    uint8_t mask_byte = channel / 8;
    uint8_t mask_bit = channel % 8;

    // dprintf("%u:%u\n", mask_byte, mask_bit);

    if (mask_byte >= ISSI_LED_MASK_SIZE)
        return 0;

    // dprintf("b: %u, s: %u\n", control->mask[mask_byte], (control->mask[mask_byte] & (1 << mask_bit)) ? 1 : 0);

    return ((control->mask[mask_byte] & (1 << mask_bit)));
}

uint8_t channel_enabled_masked(uint16_t channel)
{
    uint8_t mask_byte = channel / 8;
    uint8_t mask_bit = channel % 8;

    // dprintf("%u:%u\n", mask_byte, mask_bit);

    if (mask_byte >= ISSI_LED_MASK_SIZE)
        return 0;

    // dprintf("b: %u, s: %u\n", LedMask[mask_byte], (LedMask[mask_byte] & (1 << mask_bit)) ? 1 : 0);

    return ((LedMask[mask_byte] & (1 << mask_bit)));
}

void IS31FL3731_PWM_control(tLedPWMControlCommand *control)
{
    dprintf("IS_PWM_control c:%d, a:%d\n", control->mode, control->amount);

    // select_next_bank();

    switch (control->mode)
    {
    case LedControlMode_brightness_decrease_led:
        LedPWMPageBuffer[control->index] -= control->amount;
        // issi.setLEDPWM(control->index, LedPWMPageBuffer[control->index], currentBank);
        break;

    case LedControlMode_brightness_increase_led:
        LedPWMPageBuffer[control->index] += control->amount;
        // issi.setLEDPWM(control->index, LedPWMPageBuffer[control->index], currentBank);
        break;

    case LedControlMode_brightness_set_led:
        LedPWMPageBuffer[control->index] = control->amount;
        // issi.setLEDPWM(control->index, LedPWMPageBuffer[control->index], currentBank);
        break;

    case LedControlMode_brightness_set_all:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            LedPWMPageBuffer[channel] = control->amount;
        }
        break;

    case LedControlMode_brightness_set_by_mask:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (channel_enabled_by_mask(channel, control))
                LedPWMPageBuffer[channel] = control->amount;
        }
        break;

    case LedControlMode_brightness_set_all_masked:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (channel_enabled_masked(channel))
                LedPWMPageBuffer[channel] = control->amount;
        }
        break;

    case LedControlMode_brightness_decrease_all:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            LedPWMPageBuffer[channel] -= control->amount;
        }
        break;

    case LedControlMode_brightness_increase_all:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_decrease_all_masked:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (channel_enabled_masked(channel))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_increase_all_masked:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (channel_enabled_masked(channel))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_decrease_by_mask:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (channel_enabled_by_mask(channel, control))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_increase_by_mask:
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (channel_enabled_by_mask(channel, control))
                LedPWMPageBuffer[channel] += control->amount;
        }
        break;
    }

    // fix_max_I_led();

    uint32_t I_led = get_I_led_summed_up();
    dprintf("I_led: %lu\r\n", I_led);

#ifdef DEBUG_BACKLIGHT_EXTENDED
    dprintf("led mask:\r\n");
    for (uint8_t r = 0; r < ISSI_USED_ROWS; ++r)
    {
        dprintf("%02u: ", r);
        for (uint8_t c = 0; c < 2; ++c)
        {
            dprintf("%02X ", control->mask[r * 2 + c]);
        }
        dprintf("\r\n");
    }
    dprintf("\r\n");
    dprintf("pwm:\r\n");
    for (uint8_t i = 0; i < ISSI_USED_ROWS; i++)
    {
    	dprintf("%02u: ", i);
        for (uint8_t j = 0; j < 16; j++)
        {
            dprintf("%03u ", LedPWMPageBuffer[i * 16 + j]);
        }

        dprintf("\n");
    }
    dprintf("\r\n");
#endif

    if (issi.is_initialized())
    {
        issi.setLedsBrightness(LedPWMPageBuffer, current_pwm_bank);
        // issi.displayFrame(current_pwm_bank);
    }
}
