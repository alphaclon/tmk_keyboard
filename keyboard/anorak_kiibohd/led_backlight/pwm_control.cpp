
#include "pwm_control.h"

extern "C" {
#ifdef USE_BUFFERED_TWI
#include "../twi/i2c.h"
#else
#include "../i2cmaster/i2cmaster.h"
#endif
#include "debug.h"
}

uint8_t LedPWMPageBuffer[ISSI_TOTAL_CHANNELS] = { 0 };

uint8_t LedMask[ISSI_LED_MASK_SIZE] =
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

/* Row 1: ESC,1,2,3,4,5 00111111 LOGO 00001111 */
/* Row 2:  FN,Q,W,E,R,T 00111111  N/A 00000000 */
/* Row 3:  FN,A,S,D,F,G 00111111  N/A 00000000 */
/* Row 4:  FN,Y,X,C,V,B 00111111  N/A 00000000 */
/* Row 5:  FN,F,F,F,F,F 00111111  N/A 00000000 */
/* Row 5:         SPACE 00000001  N/A 00000000 */

uint8_t const LedMaskFull[ISSI_LED_MASK_SIZE] =
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

/* Row 1: ESC,1,2,3,4,5 00000000 LOGO 00000000 */
/* Row 2:  FN,Q,W,E,R,T 00001000  N/A 00000000 */
/* Row 3:  FN,A,S,D,F,G 00011100  N/A 00000000 */
/* Row 4:  FN,Y,X,C,V,B 00000000  N/A 00000000 */
/* Row 5:  FN,F,F,F,F,F 00000000  N/A 00000000 */
/* Row 5:         SPACE 00000000  N/A 00000000 */

uint8_t const LedMaskWASD[ISSI_LED_MASK_SIZE] =
{
	0x00, 0x00, /* C1-1 -> C1-16 */
	0x08, 0x00, /* C2-1 -> C2-16 */
	0x1C, 0x00, /* C3-1 -> C3-16 */
	0x00, 0x00, /* C4-1 -> C4-16 */
	0x00, 0x00, /* C5-1 -> C5-16 */
	0x00, 0x00, /* C6-1 -> C6-16 */
	0x00, 0x00, /* C7-1 -> C7-16 */
	0x00, 0x00, /* C8-1 -> C8-16 */
	0x00, 0x00, /* C9-1 -> C9-16 */
};

/* Row 1: ESC,1,2,3,4,5 00100000 LOGO 00001111 */
/* Row 2:  FN,Q,W,E,R,T 00100000  N/A 00000000 */
/* Row 3:  FN,A,S,D,F,G 00100000  N/A 00000000 */
/* Row 4:  FN,Y,X,C,V,B 00100000  N/A 00000000 */
/* Row 5:  FN,F,F,F,F,F 00111100  N/A 00000000 */
/* Row 5:         SPACE 00000000  N/A 00000000 */

uint8_t const LedMaskCtrl[ISSI_LED_MASK_SIZE] =
{
    0x20, 0x00, /* C1-1 -> C1-16 */
    0x20, 0x00, /* C2-1 -> C2-16 */
    0x20, 0x00, /* C3-1 -> C3-16 */
    0x20, 0x00, /* C4-1 -> C4-16 */
    0x3C, 0x00, /* C5-1 -> C5-16 */
	0x00, 0x00, /* C6-1 -> C6-16 */
    0x00, 0x00, /* C7-1 -> C7-16 */
    0x00, 0x00, /* C8-1 -> C8-16 */
    0x00, 0x00, /* C9-1 -> C9-16 */
};

/* Row 1: ESC,1,2,3,4,5 00000000 LOGO 00001111 */
/* Row 2:  FN,Q,W,E,R,T 00000000  N/A 00000000 */
/* Row 3:  FN,A,S,D,F,G 00000000  N/A 00000000 */
/* Row 4:  FN,Y,X,C,V,B 00000000  N/A 00000000 */
/* Row 5:  FN,F,F,F,F,F 00000000  N/A 00000000 */
/* Row 5:         SPACE 00000000  N/A 00000000 */

uint8_t const LedMaskLogo[ISSI_LED_MASK_SIZE] =
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

/* Row 1: ESC,1,2,3,4,5 00000000 LOGO 00000000 */
/* Row 2:  FN,Q,W,E,R,T 00000000  N/A 00000000 */
/* Row 3:  FN,A,S,D,F,G 00000000  N/A 00000000 */
/* Row 4:  FN,Y,X,C,V,B 00000000  N/A 00000000 */
/* Row 5:  FN,F,F,F,F,F 00000011  N/A 00000000 */
/* Row 5:         SPACE 00000001  N/A 00000000 */


uint8_t const LedMaskJump[ISSI_LED_MASK_SIZE] =
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
	issi.setLEDEnableMask(LedMaskFull);
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
