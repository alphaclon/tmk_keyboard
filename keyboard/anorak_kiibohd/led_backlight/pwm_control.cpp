
#include "pwm_control.h"
#include "led_masks.h"

extern "C" {
#ifdef USE_BUFFERED_TWI
#include "../twi/i2c.h"
#else
#include "../i2cmaster/i2cmaster.h"
#endif
#include "debug.h"
}

uint8_t LedMask[ISSI_LED_MASK_SIZE] = { 0 };
uint8_t LedPWMPageBuffer[ISSI_TOTAL_CHANNELS] = { 0 };

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
	memcpy_P(LedMask, LedMaskFull, ISSI_LED_MASK_SIZE);
	issi.setLEDEnableMask(LedMask);
}

void IS31FL3731_PWM_control(tLedPWMControlCommand *control)
{
	// Configure based upon the given mode
	// TODO Perhaps do gamma adjustment?
	switch (control->mode)
	{
	case LedControlMode_brightness_decrease:
		LedPWMPageBuffer[control->index] -= control->amount;
		//issi.writeRegister8(0, control->index, LedPWMPageBuffer[control->index]);
		break;

	case LedControlMode_brightness_increase:
		LedPWMPageBuffer[control->index] += control->amount;
		//issi.writeRegister8(0, control->index, LedPWMPageBuffer[control->index]);
		break;

	case LedControlMode_brightness_set:
		LedPWMPageBuffer[control->index] = control->amount;
		//issi.writeRegister8(0, control->index, LedPWMPageBuffer[control->index]);
		break;

	case LedControlMode_brightness_decrease_all:
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] -= control->amount;
		}
		dprintf("decrease_all %d\n", LedPWMPageBuffer[0]);
		issi.setLEDPWM(LedPWMPageBuffer);
		break;

	case LedControlMode_brightness_increase_all:
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] += control->amount;
		}
		dprintf("increase_all %d\n", LedPWMPageBuffer[0]);
		issi.setLEDPWM(LedPWMPageBuffer);
		break;

	case LedControlMode_brightness_decrease_mask:
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] += control->amount;
		}
		issi.setLEDPWM(LedPWMPageBuffer);
		break;

	case LedControlMode_brightness_increase_mask:
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] += control->amount;
		}
		issi.setLEDPWM(LedPWMPageBuffer);
		break;

	case LedControlMode_brightness_set_all:
		//control->amount /= 6;
		dprintf("set_all %d\n", control->amount);
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] = control->amount;
		}
		issi.setLEDPWM(LedPWMPageBuffer);
		break;

	case LedControlMode_enable_mask:
		for (uint8_t i = 0; i < ISSI_LED_MASK_SIZE; i++)
		{
			LedMask[i] |= control->mask[i];
		}
		issi.setLEDEnableMask(LedMask);
		break;

	case LedControlMode_disable_mask:
		for (uint8_t i = 0; i < ISSI_LED_MASK_SIZE; i++)
		{
			LedMask[i] &= ~(control->mask[i]);
		}
		issi.setLEDEnableMask(LedMask);
		break;

	case LedControlMode_xor_mask:
		for (uint8_t i = 0; i < ISSI_LED_MASK_SIZE; i++)
		{
			LedMask[i] ^= control->mask[i];
		}
		issi.setLEDEnableMask(LedMask);
		break;
	}

}
