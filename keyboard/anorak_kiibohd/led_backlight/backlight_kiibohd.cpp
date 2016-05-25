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

#include "IS31FL3731/Adafruit_IS31FL3731.h"
#include "backlight_kiibohd.h"


extern "C" {
#include "debug.h"
#include "backlight.h"
#include "../twi/i2c.h"
}

uint8_t LedMaskAll[] =
{
    0x0, 0x0, /* C1-1 -> C1-16 */
    0x0, 0x0, /* C2-1 -> C2-16 */
    0x0, 0x0, /* C3-1 -> C3-16 */
    0x0, 0x0, /* C4-1 -> C4-16 */
    0x0, 0x0, /* C5-1 -> C5-16 */
    0x0, 0x0, /* C6-1 -> C6-16 */
    0x0, 0x0, /* C7-1 -> C7-16 */
    0x0, 0x0, /* C8-1 -> C8-16 */
    0x0, 0x0, /* C9-1 -> C9-16 */
};

Adafruit_IS31FL3731 issi;

extern "C" {

void backlight_setup()
{
	dprintf("backlight_setup\n");

	i2cInit();
	i2cSetBitrate(400);

	issi.begin();
	issi.setLEDPWM(1, 0xff, 0);

	/*

	issi.setRowEnableMask(0, 0x3F0F);
	issi.setRowEnableMask(1, 0x3F00);
	issi.setRowEnableMask(2, 0x3F00);
	issi.setRowEnableMask(3, 0x3F00);
	issi.setRowEnableMask(4, 0x3F00);
	issi.setRowEnableMask(5, 0x0001);

	*/

	//TWIInit();
	//i2c_init();

	//IS31FL3731_test(ISSI_ADDR_DEFAULT);
	//IS31FL3731_init(ISSI_ADDR_DEFAULT);
}

void backlight_set(uint8_t level)
{
	dprintf("backlight_set %d\n", level);

	/*
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
	*/
}

void backlight_set_region(uint8_t region)
{
	dprintf("backlight_set_region 0x%x\n", region);

	switch (region)
	{
	case BACKLIGHT_REGION_ALL:
		issi.displayFrame(0);
		break;
	default:
		break;
	}
}
}
