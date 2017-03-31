#include "eeconfig_backlight.h"
#include <avr/eeprom.h>
#include <stdbool.h>
#include <stdint.h>

void eeconfig_backlight_enable(void)
{
#ifdef BACKLIGHT_ENABLE
    eeprom_write_word(EECONFIG_BACKLIGHT_MAGIC, EECONFIG_BACKLIGHT_MAGIC_NUMBER);
#endif
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
	eeprom_update_byte(EECONFIG_BACKLIGHT_SECTORS, 0xFF);
    for (uint8_t i = 0; i < EECONFIG_BACKLIGHT_SECTOR_PWM_SIZE; i++)
    	eeprom_update_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + i, EECONFIG_BACKLIGHT_DEFAULT_BRIGHTNESS);
    eeprom_update_byte(EECONFIG_BACKLIGHT_PWM_ACTIVE_MAP, 0xFF);
#endif
}

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight_sectors_state(void)
{
    return eeprom_read_byte(EECONFIG_BACKLIGHT_SECTORS);
}

void eeconfig_write_backlight_sectors_state(uint8_t val)
{
    //eeprom_write_byte(EECONFIG_BACKLIGHT_SECTORS, val);
    eeprom_update_byte(EECONFIG_BACKLIGHT_SECTORS, val);
}

void eeconfig_read_backlight_sector_brightness(uint8_t sector, uint8_t *red, uint8_t *green, uint8_t *blue)
{
    *red = eeprom_read_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3));
    *green = eeprom_read_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 1);
    *blue = eeprom_read_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 2);
}

void eeconfig_write_backlight_sector_brightness(uint8_t sector, uint8_t red, uint8_t green, uint8_t blue)
{
    //eeprom_write_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3), red);
    //eeprom_write_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 1, green);
    //eeprom_write_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 2, blue);

    eeprom_update_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3), red);
    eeprom_update_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 1, green);
    eeprom_update_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 2, blue);
}

uint8_t eeconfig_read_backlight_pwm_active_map()
{
    return eeprom_read_byte(EECONFIG_BACKLIGHT_PWM_ACTIVE_MAP);
}

void eeconfig_write_backlight_pwm_active_map(uint8_t map)
{
    //eeprom_write_byte(EECONFIG_BACKLIGHT_PWM_ACTIVE_MAP, map);
    eeprom_update_byte(EECONFIG_BACKLIGHT_PWM_ACTIVE_MAP, map);
}

void eeconfig_read_backlight_pwm_map(uint8_t map, uint8_t *buffer, bool read_lower)
{
	uint16_t offset = (map*EECONFIG_BACKLIGHT_PWM_MAP_SIZE);
	if (read_lower)
		offset += EECONFIG_BACKLIGHT_PWM_MAP_SIZE_HALF
	eeprom_read_block(buffer, EECONFIG_BACKLIGHT_PWM_MAP + offset, EECONFIG_BACKLIGHT_PWM_MAP_SIZE_HALF);
}

void eeconfig_write_backlight_pwm_map(uint8_t map, uint8_t *buffer, bool write_lower)
{
	uint16_t offset = (map*EECONFIG_BACKLIGHT_PWM_MAP_SIZE);
	if (write_lower)
		offset += EECONFIG_BACKLIGHT_PWM_MAP_SIZE_HALF
	eeprom_write_block(buffer, EECONFIG_BACKLIGHT_PWM_MAP + offset, EECONFIG_BACKLIGHT_PWM_MAP_SIZE_HALF);
}

#endif
