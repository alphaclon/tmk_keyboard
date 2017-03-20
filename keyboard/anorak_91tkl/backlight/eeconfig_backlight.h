/*
Copyright 2013 Jun Wako <wakojun@gmail.com>
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

#ifndef EECONFIG_BACKLIGHT_H
#define EECONFIG_BACKLIGHT_H

#include <stdbool.h>
#include <stdint.h>

#define EECONFIG_BACKLIGHT_MAGIC_NUMBER (uint16_t)0xAFFE
#define EECONFIG_BACKLIGHT_DEFAULT_BRIGHTNESS 3
/* eeprom parameteter address */
#define EECONFIG_BACKLIGHT_MAGIC (uint16_t *)7
#define EECONFIG_BACKLIGHT_REGIONS (uint8_t *)9
#define EECONFIG_BACKLIGHT_REGION_PWM (uint8_t *)10

#ifdef __cplusplus
extern "C" {
#endif

bool eeconfig_backlight_is_enabled(void);
void eeconfig_backlight_init(void);

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight_state(void);
void eeconfig_write_backlight_state(uint8_t mask);

uint8_t eeconfig_read_backlight_sector_brightness(uint8_t sector, uint8_t color);
void eeconfig_write_backlight_sector_brightness(uint8_t sector, uint8_t color, uint8_t brightness);
#endif

#ifdef __cplusplus
}
#endif

#endif
