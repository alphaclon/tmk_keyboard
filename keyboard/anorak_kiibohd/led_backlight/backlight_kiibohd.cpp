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

#include "backlight_kiibohd.h"
#include "pwm_control.h"

extern "C" {
#include "debug.h"
#include "backlight.h"
}

extern "C" {

void backlight_setup()
{
	dprintf("backlight_setup\n");
	IS31FL3731_init();
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

void backlight_set_region(uint8_t region)
{
	dprintf("backlight_set_region %d\n", region);

	tLedPWMControlCommand control;
	control.mode = LedControlMode_enable_mask;

	switch (region)
	{
	case BACKLIGHT_REGION_ALL:
		control.mask = LedEnableMask;
		break;
	case BACKLIGHT_REGION_WASD:
		control.mask = LedEnableMask;
		break;
	case BACKLIGHT_REGION_JUMP:
		control.mask = LedJumpMask;
		break;
	case BACKLIGHT_REGION_CONTROLS:
		control.mask = LedEnableMask;
		break;
	case BACKLIGHT_REGION_LOGO:
	case BACKLIGHT_REGION_CASE:
		control.mask = LedLogoMask;
		break;
	}

	IS31FL3731_PWM_control(&control);
}
}
