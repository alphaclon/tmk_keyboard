#include "eeconfig_backlight.h"
#include <avr/eeprom.h>
#include <stdbool.h>
#include <stdint.h>

void eeconfig_backlight_init(void)
{
#ifdef BACKLIGHT_ENABLE
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGIONS, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + 1, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + 2, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + 3, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + 4, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + 5, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + 6, 0);
    eeprom_write_byte(EECONFIG_BACKLIGHT_REGION_PWM + 7, 0);
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
