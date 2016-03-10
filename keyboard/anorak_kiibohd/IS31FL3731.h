/*
Copyright 2016 Moritz Wenk <MoritzWenk@web.de>

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

#ifndef _IS31FL3731_H_
#define _IS31Fl3731_H_

#include <inttypes.h>
#include "IS31FL3731Defaults.h"

/*
 * Modified the original code from Adafruit for the IS31FL3731
 * https://github.com/adafruit/Adafruit_IS31FL3731
 *
 */

#define ISSI_REG_CONFIG  0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME  0x01

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B    // helpfully called 'page nine'

// Basic LED Control Capability
typedef enum LedControlMode
{
    // Single LED Modes
    LedControlMode_brightness_decrease,
    LedControlMode_brightness_increase,
    LedControlMode_brightness_set,
    // Set all LEDs (index argument not required)
    LedControlMode_brightness_decrease_all,
    LedControlMode_brightness_increase_all,
    LedControlMode_brightness_set_all,
} LedControlMode;

typedef struct LedControl
{
    LedControlMode mode;
    uint8_t amount;
    uint16_t index;
} LedControl;

void IS31FL3731_init(uint8_t addr = ISSI_ADDR_DEFAULT);
void IS31FL3731_drawPixel(int16_t x, int16_t y, uint16_t color);
void IS31FL3731_setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank = 0);
void IS31FL3731_audioSync(bool sync);
void IS31FL3731_setFrame(uint8_t b);
void IS31FL3731_displayFrame(uint8_t frame);
void IS31FL3731_control(LedControl *control);

#endif
