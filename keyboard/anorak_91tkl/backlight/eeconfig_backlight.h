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

#ifdef __cplusplus
extern "C" {
#endif

#define EECONFIG_BACKLIGHT_MAGIC_NUMBER (uint16_t)0xAFFE
#define EECONFIG_BACKLIGHT_DEFAULT_BRIGHTNESS 3

/* eeprom parameteter address */
/* size for sectors is: 8*3 = 24 bytes
 * size for pwm maps: 192*2 = 384 bytes
 *   5 pwm maps: 1920 bytes
 * eeprom size: 2048
 *      size left: 2048-1920 = 128
 *
 */

#define EECONFIG_BACKLIGHT_MAGIC (uint16_t *)7

#define EECONFIG_BACKLIGHT_SECTORS (uint8_t *)9
#define EECONFIG_BACKLIGHT_SECTOR_PWM (uint8_t *)10

#define EECONFIG_BACKLIGHT_SECTOR_PWM_SIZE (8*3)

#define EECONFIG_BACKLIGHT_PWM_ACTIVE_MAP (uint8_t *)127
#define EECONFIG_BACKLIGHT_PWM_MAP (uint8_t *)128

#define EECONFIG_BACKLIGHT_PWM_MAP_COUNT 5
#define EECONFIG_BACKLIGHT_PWM_MAP_SIZE_HALF 192
#define EECONFIG_BACKLIGHT_PWM_MAP_SIZE (EECONFIG_BACKLIGHT_PWM_MAP_SIZE_HALF*2)

bool eeconfig_backlight_is_enabled(void);
void eeconfig_backlight_init(void);

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight_sectors_state(void);
void eeconfig_write_backlight_sectors_state(uint8_t mask);

void eeconfig_read_backlight_sector_brightness(uint8_t sector, uint8_t *red, uint8_t *green, uint8_t *blue);
void eeconfig_write_backlight_sector_brightness(uint8_t sector, uint8_t red, uint8_t green, uint8_t blue);

uint8_t eeconfig_read_backlight_pwm_active_map();
void eeconfig_write_backlight_pwm_active_map(uint8_t map);

void eeconfig_read_backlight_pwm_map(uint8_t map, uint8_t *buffer, bool read_lower);
void eeconfig_write_backlight_pwm_map(uint8_t map, uint8_t *buffer, bool write_lower);
#endif

#ifdef __cplusplus
}
#endif

#endif
