/*
Copyright 2014 Moritz Wenk <MoritzWenk@web.de>


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/interrupt.h>
#include "pwm_control.h"

extern "C" {
#include "debug.h"
#include "backlight.h"
#include "backlight_kiibohd.h"
}

extern "C" {

void backlight_setup(void)
{
	IS31FL3731_init();
}

void backlight_internal_enable(void)
{
	IS31FL3731_enable();
}

void backlight_set(uint8_t level)
{
	dprintf("backlight_set %d\n", level);

	tLedPWMControlCommand control;
	control.mode = LedControlMode_brightness_set_all;

	switch (level)
	{
	case 0:
		control.amount = 0;
		break;
	case 1:
		control.amount = 63;
		break;
	case 2:
		control.amount = 127;
		break;
	case 3:
		control.amount = 190;
		break;
	case 4:
		control.amount = 255;
		break;
	default:
		control.amount = 255;
		break;
	}

	IS31FL3731_PWM_control(&control);
}

void backlight_pwm_increase(uint8_t level)
{
	dprintf("backlight_increase\n", level);

	tLedPWMControlCommand control;
	control.mode = LedControlMode_brightness_increase_all;
	control.amount = level;
	IS31FL3731_PWM_control(&control);
}

void backlight_pwm_decrease(uint8_t level)
{
	dprintf("backlight_decrease\n", level);

	tLedPWMControlCommand control;
	control.mode = LedControlMode_brightness_decrease_all;
	control.amount = level;
	IS31FL3731_PWM_control(&control);
}

void backlight_set_region(uint8_t region)
{
	dprintf("backlight_set_region %d\n", region);

	tLedPWMControlCommand control;
	control.mode = LedControlMode_xor_mask;

	switch (region)
	{
	case BACKLIGHT_REGION_ALL:
		memcpy_P(control.maks, LedMaskFull, ISSI_LED_MASK_SIZE);
		//control.mask = LedMaskFull;
		break;
	case BACKLIGHT_REGION_WASD:
		memcpy_P(control.maks, LedMaskWASD, ISSI_LED_MASK_SIZE);
		//control.mask = LedMaskWASD;
		break;
	case BACKLIGHT_REGION_JUMP:
		memcpy_P(control.maks, LedMaskJump, ISSI_LED_MASK_SIZE);
		//control.mask = LedMaskJump;
		break;
	case BACKLIGHT_REGION_CONTROLS:
		memcpy_P(control.maks, LedMaskCtrl, ISSI_LED_MASK_SIZE);
		//control.mask = LedMaskCtrl;
		break;
	case BACKLIGHT_REGION_LOGO:
	case BACKLIGHT_REGION_CASE:
		memcpy_P(control.maks, LedMaskLogo, ISSI_LED_MASK_SIZE);
		//control.mask = LedMaskLogo;
		break;
	}

	IS31FL3731_PWM_control(&control);
}
}
