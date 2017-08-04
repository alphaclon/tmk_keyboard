/*
Copyright 2017 Moritz Wenk <MoritzWenk@web.de>

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

#ifndef EECONFIG_STATUSLED_H
#define EECONFIG_STATUSLED_H

#ifdef STATUS_LED_PWM_ENABLE
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EECONFIG_STATUSLED_MAGIC_NUMBER (uint16_t)0xFEAF
#define EECONFIG_STATUSLED_DEFAULT_BRIGHTNESS 250

#define EECONFIG_STATUSLED_MAGIC (uint16_t *)7
#define EECONFIG_STATUSLED_SCROLLLOCK (uint8_t*)9
#define EECONFIG_STATUSLED_CAPSLOCK (uint8_t*)10
#define EECONFIG_STATUSLED_NUMLOCK (uint8_t*)11

bool eeconfig_statusled_brightness_is_enabled(void);
void eeconfig_statusled_brightness_init(void);

uint8_t eeconfig_read_scrolllock_led_brightness(void);
void eeconfig_write_scrolllock_led_brightness(uint8_t brightness);

uint8_t eeconfig_read_capslock_led_brightness(void);
void eeconfig_write_capslock_led_brightness(uint8_t brightness);

uint8_t eeconfig_read_numlock_led_brightness(void);
void eeconfig_write_numlock_led_brightness(uint8_t brightness);

#ifdef __cplusplus
}
#endif

#endif
#endif
