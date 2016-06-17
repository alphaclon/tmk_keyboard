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
#include <avr/pgmspace.h>
#include "pwm_control.h"
#include "led_masks.h"

extern "C" {
#include "config.h"
#include "debug.h"
#include "eeconfig.h"
#include "backlight.h"
#include "backlight_kiibohd.h"
}

extern "C" {

uint8_t regions = 0xff;

void backlight_setup(void)
{
	regions = eeconfig_read_backlight_regions();

    IS31FL3731_init();
}

void backlight_internal_enable(void)
{
    IS31FL3731_enable();

    backlight_unset_region(BACKLIGHT_REGION_ALL);

    if (regions & BACKLIGHT_REGION_CASE)
        backlight_set_region(BACKLIGHT_REGION_CASE);
    if (regions & BACKLIGHT_REGION_CONTROLS)
        backlight_set_region(BACKLIGHT_REGION_CONTROLS);
    if (regions & BACKLIGHT_REGION_JUMP)
        backlight_set_region(BACKLIGHT_REGION_JUMP);
    if (regions & BACKLIGHT_REGION_LOGO)
        backlight_set_region(BACKLIGHT_REGION_LOGO);
    if (regions & BACKLIGHT_REGION_OTHER)
        backlight_set_region(BACKLIGHT_REGION_OTHER);
    if (regions & BACKLIGHT_REGION_WASD)
        backlight_set_region(BACKLIGHT_REGION_WASD);
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

void backlight_region(uint8_t region, tLedPWMControlMode mode)
{
    dprintf("backlight_set_region %d\n", region);

    tLedPWMControlCommand control;
    control.mode = mode;

    switch (region)
    {
    case BACKLIGHT_REGION_ALL:
        memcpy_P(control.mask, LedMaskFull, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_WASD:
        memcpy_P(control.mask, LedMaskWASD, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_JUMP:
        memcpy_P(control.mask, LedMaskJump, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_CONTROLS:
        memcpy_P(control.mask, LedMaskCtrl, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_OTHER:
    	memcpy_P(control.mask, LedMaskOthr, ISSI_LED_MASK_SIZE);
		break;
    case BACKLIGHT_REGION_LOGO:
    case BACKLIGHT_REGION_CASE:
        memcpy_P(control.mask, LedMaskLogo, ISSI_LED_MASK_SIZE);
        break;
    }

    IS31FL3731_PWM_control(&control);
}

void backlight_toggle_region(uint8_t region)
{
	if (regions & region)
		regions &= ~region;
	else
		regions |= region;

    backlight_region(region, LedControlMode_xor_mask);
}

void backlight_unset_region(uint8_t region)
{
	regions &= ~region;
	backlight_region(region, LedControlMode_disable_mask);
}

void backlight_set_region(uint8_t region)
{
	regions |= region;
	backlight_region(region, LedControlMode_enable_mask);
}
}
