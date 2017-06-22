/*
 * Copyright (c) 2010 by Stefan Riepenhausen <rhn@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef HAVE_PWM_GENERAL_H
#define HAVE_PWM_GENERAL_H

#include <stdbool.h>
#include <inttypes.h>

#define STATUS_LED_PWM_ENABLED

void pwm_init(void);

void pwm_caps_lock_led(uint8_t value);
void pwm_scroll_lock_led(uint8_t value);

void pwm_caps_lock_led_enabled(bool enabled);
void pwm_scroll_lock_led_enabled(bool enabled);

uint8_t get_pwm_scroll_lock_led(void);
uint8_t get_pwm_caps_lock_led(void);

#endif  /* HAVE_PWM_GENERAL_H */
