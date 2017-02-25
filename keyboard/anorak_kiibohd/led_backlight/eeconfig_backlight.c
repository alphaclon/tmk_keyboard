#include "eeconfig_backlight.h"
#include <avr/eeprom.h>
#include <stdbool.h>
#include <stdint.h>

void eeconfig_backlight_enable(void)
{
    eeprom_write_word(EECONFIG_BACKLIGHT_MAGIC, EECONFIG_BACKLIGHT_MAGIC_NUMBER);
}

void eeconfig_backlight_disable(void)
{
    eeprom_write_word(EECONFIG_BACKLIGHT_MAGIC, 0xFFFF);
}

bool eeconfig_backlight_is_enabled(void)
{
    return (eeprom_read_word(EECONFIG_BACKLIGHT_MAGIC) == EECONFIG_BACKLIGHT_MAGIC_NUMBER);
}

void eeconfig_backlight_init(void)
{
#ifdef BACKLIGHT_ENABLE
	eeconfig_backlight_enable();
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGIONS, 0xFF);
    for (uint8_t i = 0; i < 8; i++)
    	eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + i, EECONFIG_BACKLIGHT_DEFAULT_BRIGHTNESS);
#endif
}

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight_regions(void)
{
    return eeprom_read_byte(EECONFIG_BACKLIGHT_REGIONS);
}

void eeconfig_write_backlight_regions(uint8_t val)
{
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGIONS, val);
}

uint8_t eeconfig_read_backlight_region_brightness(uint8_t region)
{
    return eeprom_read_byte(EECONFIG_BACKLIGHT_REGION_PWM + region);
}

void eeconfig_write_backlight_region_brightness(uint8_t region, uint8_t brightness)
{
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + region, brightness);
}
#endif
