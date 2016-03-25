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

#include "debug.h"
#include "backlight.h"
#include "backlight_kiibohd.h"
#include "IS31FL3731.h"

#ifdef USE_ASYNC_I2C
#include "TWIlib.h"
#else
#include "i2cmaster/i2cmaster.h"
#endif

void backlight_setup()
{
	//TWIInit();
	i2c_init();

	IS31FL3731_test(ISSI_ADDR_DEFAULT);
	//IS31FL3731_init(ISSI_ADDR_DEFAULT);
}

void backlight_set(uint8_t level)
{
	LedControl control;
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

	IS31FL3731_control(&control);
}

void backlight_set_region(uint8_t region)
{
	dprintf("backlight_set_region 0x%x\n", region);

	switch (region)
	{
	case BACKLIGHT_REGION_ALL:
		break;
	default:
		break;
	}
}
