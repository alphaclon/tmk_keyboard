
#include "pwm_control.h"

extern "C" {
#include "../twi/i2c.h"
//#include "../i2cmaster/i2cmaster.h"
#include "debug.h"
}

uint8_t LedPWMPageBuffer[ISSI_TOTAL_CHANNELS] = { 0 };

uint8_t LedEnableMask[ISSI_LED_MASK_SIZE] =
{
    0x3F, 0x0F, /* C1-1 -> C1-16 */
    0x3F, 0x00, /* C2-1 -> C2-16 */
    0x3F, 0x00, /* C3-1 -> C3-16 */
    0x3F, 0x00, /* C4-1 -> C4-16 */
    0x3F, 0x00, /* C5-1 -> C5-16 */
	0x01, 0x00, /* C6-1 -> C6-16 */
    0x00, 0x00, /* C7-1 -> C7-16 */
    0x00, 0x00, /* C8-1 -> C8-16 */
    0x00, 0x00, /* C9-1 -> C9-16 */
};

uint8_t LedLogoMask[ISSI_LED_MASK_SIZE] =
{
	0x00, 0x0F, /* C1-1 -> C1-16 */
	0x00, 0x00, /* C2-1 -> C2-16 */
	0x00, 0x00, /* C3-1 -> C3-16 */
	0x00, 0x00, /* C4-1 -> C4-16 */
	0x00, 0x00, /* C5-1 -> C5-16 */
	0x00, 0x00, /* C6-1 -> C6-16 */
    0x00, 0x00, /* C7-1 -> C7-16 */
    0x00, 0x00, /* C8-1 -> C8-16 */
    0x00, 0x00, /* C9-1 -> C9-16 */
};

uint8_t LedJumpMask[ISSI_LED_MASK_SIZE] =
{
	0x00, 0x00, /* C1-1 -> C1-16 */
	0x00, 0x00, /* C2-1 -> C2-16 */
	0x00, 0x00, /* C3-1 -> C3-16 */
	0x00, 0x00, /* C4-1 -> C4-16 */
    0x03, 0x00, /* C5-1 -> C5-16 */
	0x01, 0x00, /* C6-1 -> C6-16 */
    0x00, 0x00, /* C7-1 -> C7-16 */
    0x00, 0x00, /* C8-1 -> C8-16 */
    0x00, 0x00, /* C9-1 -> C9-16 */
};

Adafruit_IS31FL3731 issi;

void IS31FL3731_init()
{
	i2cInit();
	i2cSetBitrate(400);

	//i2c_init();

	issi.begin();
	issi.setLEDEnableMask(LedEnableMask);
}

void IS31FL3731_PWM_control(tLedPWMControlCommand *control)
{
	// Configure based upon the given mode
	// TODO Perhaps do gamma adjustment?
	switch (control->mode)
	{
	case LedControlMode_brightness_decrease:
		LedPWMPageBuffer[control->index] -= control->amount;
		break;

	case LedControlMode_brightness_increase:
		LedPWMPageBuffer[control->index] += control->amount;
		break;

	case LedControlMode_brightness_set:
		LedPWMPageBuffer[control->index] = control->amount;
		break;

	case LedControlMode_brightness_decrease_all:
		dprintf("LedControlMode_brightness_decrease_all %d\n", control->amount);
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] -= control->amount;
			issi.setLEDPWM(channel, LedPWMPageBuffer[channel]);
		}
		break;

	case LedControlMode_brightness_increase_all:
		dprintf("LedControlMode_brightness_increase_all %d\n", control->amount);
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] += control->amount;
			issi.setLEDPWM(channel, LedPWMPageBuffer[channel]);
		}
		break;

	case LedControlMode_brightness_decrease_mask:
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] += control->amount;
		}
		break;

	case LedControlMode_brightness_increase_mask:
		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] += control->amount;
		}
		break;

	case LedControlMode_brightness_set_all:
		dprintf("LedControlMode_brightness_set_all %d\n", control->amount);

		for (uint8_t channel = 0; channel < ISSI_TOTAL_CHANNELS; channel++)
		{
			LedPWMPageBuffer[channel] = control->amount;
		}

		issi.setSoftwareShutdown(1);
		issi.setLEDPWM(LedPWMPageBuffer);
		issi.setSoftwareShutdown(0);

		break;

	case LedControlMode_enable_mask:
		break;

	case LedControlMode_disable_mask:
		break;

	}

}
