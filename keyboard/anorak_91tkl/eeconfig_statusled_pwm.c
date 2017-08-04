#include "eeconfig_statusled_pwm.h"

#ifdef STATUS_LED_PWM_ENABLE

#include <avr/eeprom.h>
#include <stdbool.h>
#include <stdint.h>

void eeconfig_statusled_enable(void)
{
    eeprom_update_word(EECONFIG_STATUSLED_MAGIC, EECONFIG_STATUSLED_MAGIC_NUMBER);
}

void eeconfig_statusled_disable(void)
{
    eeprom_update_word(EECONFIG_STATUSLED_MAGIC, 0xFFFF);
}

bool eeconfig_statusled_brightness_is_enabled(void)
{
    return (eeprom_read_word(EECONFIG_STATUSLED_MAGIC) == EECONFIG_STATUSLED_MAGIC_NUMBER);
}

void eeconfig_statusled_brightness_init(void)
{
	eeconfig_statusled_enable();

	eeconfig_write_scrolllock_led_brightness(EECONFIG_STATUSLED_DEFAULT_BRIGHTNESS);
	eeconfig_write_capslock_led_brightness(EECONFIG_STATUSLED_DEFAULT_BRIGHTNESS);
	eeconfig_write_numlock_led_brightness(EECONFIG_STATUSLED_DEFAULT_BRIGHTNESS);
}

uint8_t eeconfig_read_scrolllock_led_brightness(void)
{
    return eeprom_read_byte(EECONFIG_STATUSLED_SCROLLLOCK);
}

void eeconfig_write_scrolllock_led_brightness(uint8_t val)
{
    eeprom_update_byte(EECONFIG_STATUSLED_SCROLLLOCK, val);
}

uint8_t eeconfig_read_capslock_led_brightness(void)
{
    return eeprom_read_byte(EECONFIG_STATUSLED_CAPSLOCK);
}

void eeconfig_write_capslock_led_brightness(uint8_t val)
{
    eeprom_update_byte(EECONFIG_STATUSLED_CAPSLOCK, val);
}

uint8_t eeconfig_read_numlock_led_brightness(void)
{
    return eeprom_read_byte(EECONFIG_STATUSLED_NUMLOCK);
}

void eeconfig_write_numlock_led_brightness(uint8_t val)
{
    eeprom_update_byte(EECONFIG_STATUSLED_NUMLOCK, val);
}

#endif
