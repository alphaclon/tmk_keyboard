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
    eeprom_update_byte(EECONFIG_BACKLIGHT_ANIMATION, 0);

    eeconfig_write_animation_current(0);
    eeconfig_write_animation_hsv_values(0, 300, 250, 192);
    eeconfig_write_animation_hsv_values(1, 130, 70, 194);
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

void eeconfig_read_backlight_sector_values(uint8_t sector, uint8_t *v1, uint8_t *v2, uint8_t *v3)
{
    *v1 = eeprom_read_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3));
    *v2 = eeprom_read_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 1);
    *v3 = eeprom_read_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 2);
}

void eeconfig_write_backlight_sector_brightness(uint8_t sector, uint8_t v1, uint8_t v2, uint8_t v3)
{
    //eeprom_write_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3), red);
    //eeprom_write_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 1, green);
    //eeprom_write_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 2, blue);

    eeprom_update_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3), v1);
    eeprom_update_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 1, v2);
    eeprom_update_byte(EECONFIG_BACKLIGHT_SECTOR_PWM + (sector * 3) + 2, v3);
}

uint8_t eeconfig_read_animation_current(void)
{
    return eeprom_read_byte(EECONFIG_BACKLIGHT_ANIMATION);
}

void eeconfig_write_animation_current(uint8_t current)
{
	eeprom_update_byte(EECONFIG_BACKLIGHT_ANIMATION, current);
}

void eeconfig_read_animation_hsv_values(uint8_t hsv, uint8_t *v1, uint8_t *v2, uint8_t *v3)
{
    *v1 = eeprom_read_byte(EECONFIG_BACKLIGHT_ANIMATION_HSV_1 + (hsv * 3));
    *v2 = eeprom_read_byte(EECONFIG_BACKLIGHT_ANIMATION_HSV_1 + (hsv * 3) + 1);
    *v3 = eeprom_read_byte(EECONFIG_BACKLIGHT_ANIMATION_HSV_1 + (hsv * 3) + 2);
}

void eeconfig_write_animation_hsv_values(uint8_t hsv, uint8_t *v1, uint8_t *v2, uint8_t *v3)
{
    eeprom_update_byte(EECONFIG_BACKLIGHT_ANIMATION_HSV_1 + (hsv * 3), v1);
    eeprom_update_byte(EECONFIG_BACKLIGHT_ANIMATION_HSV_1 + (hsv * 3) + 1, v2);
    eeprom_update_byte(EECONFIG_BACKLIGHT_ANIMATION_HSV_1 + (hsv * 3) + 2, v3);
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
