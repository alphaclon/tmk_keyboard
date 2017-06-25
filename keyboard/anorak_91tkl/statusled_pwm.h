
#ifndef HAVE_PWM_GENERAL_H
#define HAVE_PWM_GENERAL_H

#include "eeconfig_statusled_pwm.h"

#ifdef STATUS_LED_PWM_ENABLED

#include <stdbool.h>
#include <inttypes.h>

void statusled_pwm_init(void);
void save_status_led_values(void);

void set_capslock_led_brightness(uint8_t value);
void set_scrolllock_led_brightness(uint8_t value);

void set_capslock_led_enabled(bool enabled);
void set_scrolllock_led_enabled(bool enabled);

uint8_t get_scrolllock_led_brightness(void);
uint8_t get_capslock_led_brightness(void);

#endif

#endif  /* HAVE_PWM_GENERAL_H */
