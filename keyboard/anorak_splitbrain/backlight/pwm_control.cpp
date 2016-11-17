
#include "pwm_control.h"
#include "led_control.h"
#include "led_masks.h"

#define DEBUG_BACKLIGHT
#ifdef DEBUG_BACKLIGHT
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define R_EXT 18
#define POWER_FACTOR 64.7
#define I_LED_MAX (POWER_FACTOR / R_EXT)
#define I_LED(pwm) ((I_LED_MAX * pwm) / 256)

uint8_t current_pwm_bank = 0;
uint8_t LedPWMPageBuffer[ISSI_USED_CHANNELS] = {0};
uint16_t maximum_total_pwm = ISSI_TOTAL_CHANNELS * 255;
double maximum_total_I_led = 100;

void IS31FL3731_set_power_target(uint16_t value)
{
    maximum_total_pwm = value;
}

void IS31FL3731_set_power_target_I_max(double value)
{
    maximum_total_I_led = value;
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

    //dprintf("%u:%u\n", mask_byte, mask_bit);

    if (mask_byte >= ISSI_LED_MASK_SIZE)
            return 0;

    //dprintf("b: %u, s: %u\n", control->mask[mask_byte], (control->mask[mask_byte] & (1 << mask_bit)) ? 1 : 0);

    return ((control->mask[mask_byte] & (1 << mask_bit)));
}

uint8_t channel_enabled_masked(uint16_t channel)
{
    uint8_t mask_byte = channel / 8;
    uint8_t mask_bit = channel % 8;

    //dprintf("%u:%u\n", mask_byte, mask_bit);

    if (mask_byte >= ISSI_LED_MASK_SIZE)
            return 0;

    //dprintf("b: %u, s: %u\n", LedMask[mask_byte], (LedMask[mask_byte] & (1 << mask_bit)) ? 1 : 0);

    return ((LedMask[mask_byte] & (1 << mask_bit)));
}

double get_I_led_summed_up()
{
	uint8_t count = 0;
    double sum = 0;

    for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
    {
    	if (LedPWMPageBuffer[channel])
    	{
    		count++;
    		sum += LedPWMPageBuffer[channel] * I_LED_MAX;
    	}
    }

    sum /= (256*count);

    return sum;
}

void fix_max_I_led()
{
    double I_leds = get_pwm_summed_up();

    dprintf("fixing %.2f\n", I_leds);

    while (I_leds > maximum_total_I_led)
    {
        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (LedPWMPageBuffer[channel] > 5)
                LedPWMPageBuffer[channel] -= 5;
        }

    	I_leds = get_pwm_summed_up();
    }

    dprintf("fixed to %.2f\n", I_leds);
}

uint16_t get_pwm_summed_up()
{
    uint16_t sum = 0; // Maximum is 255 * ISSI_TOTAL_CHANNELS

    for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        sum += LedPWMPageBuffer[channel];

    return sum;
}

void fix_max_pwm()
{
    if (maximum_total_pwm >= ISSI_TOTAL_CHANNELS * 255)
        return;

    uint16_t sum = get_pwm_summed_up();

    dprintf("fixing %u\n", sum);

    while (sum > maximum_total_pwm)
    {
        sum = 0;

        for (uint8_t channel = 0; channel < ISSI_USED_CHANNELS; channel++)
        {
            if (LedPWMPageBuffer[channel] > 5)
                LedPWMPageBuffer[channel] -= 5;
            sum += LedPWMPageBuffer[channel];
        }
    }

    dprintf("fixed %u\n", sum);
}

void IS31FL3731_PWM_control(tLedPWMControlCommand *control)
{
    dprintf("IS_PWM_control %d: %d\n", control->mode, control->amount);

    //select_next_bank();

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

    //fix_max_pwm();
    fix_max_I_led();

    issi.setLedsBrightness(LedPWMPageBuffer, current_pwm_bank);
    //issi.displayFrame(current_pwm_bank);

#ifdef DEBUG_BACKLIGHT
    issi.dumpBrightness(current_led_bank);
#endif
}
