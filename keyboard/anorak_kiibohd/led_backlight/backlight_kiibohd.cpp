/*
Copyright 2014 Moritz Wenk <MoritzWenk@web.de>


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

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "pwm_control.h"
#include "led_masks.h"

extern "C" {
#include "config.h"
#include "debug.h"
#include "eeconfig.h"
#include "backlight.h"
#include "backlight_kiibohd.h"
}

#define MINIMAL_ALLOWED_BRIGHTNES 1

extern "C" {

uint8_t regions = 0;
uint8_t region_brightness[8] = {0};
uint8_t current_region = BACKLIGHT_REGION_ALL;

void set_region_mask_for_control(uint8_t region, uint8_t mask[ISSI_LED_MASK_SIZE])
{
    switch (region)
    {
    case BACKLIGHT_REGION_ALL:
        memcpy_P(mask, LedMaskFull, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_WASD:
        memcpy_P(mask, LedMaskWASD, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_JUMP:
        memcpy_P(mask, LedMaskJump, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_CONTROLS:
        memcpy_P(mask, LedMaskCtrl, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_LOGO:
        memcpy_P(mask, LedMaskLogo, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_CASE:
        memcpy_P(mask, LedMaskCase, ISSI_LED_MASK_SIZE);
        break;
    case BACKLIGHT_REGION_OTHER:
        memcpy_P(mask, LedMaskOthr, ISSI_LED_MASK_SIZE);
        break;
    default:
        memcpy_P(mask, LedMaskFull, ISSI_LED_MASK_SIZE);
        break;
    }
}

uint8_t get_array_position_for_region(uint8_t region)
{
    uint8_t pos = 0;

    switch (region)
    {
    case BACKLIGHT_REGION_ALL:
        pos = 0;
        break;
    case BACKLIGHT_REGION_WASD:
        pos = 1;
        break;
    case BACKLIGHT_REGION_JUMP:
        pos = 2;
        break;
    case BACKLIGHT_REGION_CONTROLS:
        pos = 3;
        break;
    case BACKLIGHT_REGION_LOGO:
        pos = 4;
        break;
    case BACKLIGHT_REGION_CASE:
        pos = 5;
        break;
    case BACKLIGHT_REGION_OTHER:
        pos = 6;
        break;
    default:
        break;
    }

    return pos;
}

void set_brightness_for_region(uint8_t region, uint8_t brightness, tLedPWMControlMode mode)
{
    tLedPWMControlCommand control;
    control.mode = mode;
    control.amount = brightness;
    set_region_mask_for_control(region, control.mask);
    IS31FL3731_PWM_control(&control);
}

void set_and_save_brightness_for_region(uint8_t region, uint8_t pos, uint8_t brightness)
{
    set_brightness_for_region(current_region, brightness, LedControlMode_brightness_set_by_mask);
    region_brightness[pos] = brightness;
    //eeconfig_write_backlight_region_brightness(pos, brightness);
}

void set_brightness_for_all_regions(uint8_t brightness, tLedPWMControlMode mode)
{
    tLedPWMControlCommand control;
    control.mode = mode;
    control.amount = brightness;
    IS31FL3731_PWM_control(&control);
}

void backlight_increase_brightness_for_region(uint8_t region, uint8_t delta_brightness)
{
    dprintf("backlight_increase_brightness_for_region %d\n", delta_brightness);

    if ( !(regions & region) )
        return;

    uint8_t pos = get_array_position_for_region(region);
    uint8_t brightness = region_brightness[pos];

    brightness += delta_brightness;

    if (brightness < delta_brightness)
        brightness = 255;

    set_and_save_brightness_for_region(region, pos, brightness);
}

void backlight_increase_brightness_selected_region(uint8_t delta_brightness)
{
    backlight_increase_brightness_for_region(current_region, delta_brightness);
}

void backlight_decrease_brightness_for_region(uint8_t region, uint8_t delta_brightness)
{
    dprintf("backlight_decrease_brightness_for_region %d\n", delta_brightness);

    if ( !(regions & region) )
        return;

    uint8_t pos = get_array_position_for_region(region);
    uint8_t brightness = region_brightness[pos];

    if (delta_brightness >= brightness)
        brightness = MINIMAL_ALLOWED_BRIGHTNES;
    else
        brightness -= delta_brightness;

    set_and_save_brightness_for_region(region, pos, brightness);
}

void backlight_decrease_brightness_selected_region(uint8_t delta_brightness)
{
    backlight_decrease_brightness_for_region(current_region, delta_brightness);
}

void backlight_set_brightness_for_region(uint8_t region, uint8_t brightness)
{
    dprintf("backlight_brightness_set_by_mask %d\n", brightness);
    uint8_t pos = get_array_position_for_region(current_region);
    set_and_save_brightness_for_region(current_region, pos, brightness);
}

void backlight_brightness_set_all(uint8_t brightness)
{
    dprintf("backlight_brightness_set_all %d\n", brightness);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);
        backlight_set_brightness_for_region(region, brightness);
    }
}

void backlight_increase_brightness(uint8_t delta_brightness)
{
    dprintf("backlight_increase_brightness %d\n", delta_brightness);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);
        backlight_increase_brightness_for_region(region, delta_brightness);
    }
}

void backlight_decrease_brightness(uint8_t delta_brightness)
{
    dprintf("backlight_brightness_decrease %d\n", delta_brightness);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);
        backlight_decrease_brightness_for_region(region, delta_brightness);
    }
}

void set_region_mode(uint8_t region, tLedRegionControlMode mode)
{
    dprintf("set_region_mode %d %d\n", region, mode);

    tLedRegionControlCommand control;
    control.mode = mode;
    set_region_mask_for_control(region, control.mask);
    IS31FL3731_region_control(&control);
}

void backlight_select_region(uint8_t region)
{
    current_region = region;
}

void backlight_toggle_region(uint8_t region)
{
    current_region = region;

    if (regions & region)
        regions &= ~region;
    else
        regions |= region;

    set_region_mode(region, LedControlMode_xor_mask);
    //eeconfig_write_backlight_regions(regions);
}

void backlight_toggle_selected_region()
{
    backlight_toggle_region(current_region);
}

void backlight_disable_region(uint8_t region)
{
    regions &= ~region;
    current_region = region;

    set_region_mode(region, LedControlMode_disable_mask);
    //eeconfig_write_backlight_regions(regions);
}

void backlight_enable_region(uint8_t region)
{
    regions |= region;
    current_region = region;

    set_region_mode(region, LedControlMode_enable_mask);
    //eeconfig_write_backlight_regions(regions);
}

void backlight_save_region_states()
{
    eeconfig_write_backlight_regions(regions);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        eeconfig_write_backlight_region_brightness(pos, region_brightness[pos]);
    }
}

void backlight_load_region_states()
{
    regions = eeconfig_read_backlight_regions();

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t brightness = eeconfig_read_backlight_region_brightness(pos);

        if (brightness < MINIMAL_ALLOWED_BRIGHTNES)
            brightness = MINIMAL_ALLOWED_BRIGHTNES;

        region_brightness[pos] = brightness;
    }
}

void backlight_set_regions_from_saved_state(void)
{
    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);

        if (regions & region)
            set_region_mode(region, LedControlMode_enable_mask);

        uint8_t brightness = region_brightness[pos];
        set_brightness_for_region(region, brightness, LedControlMode_brightness_set_by_mask);
    }
}

void backlight_setup(void)
{
    regions = 0xff;
    current_region = 0;

    backlight_config_t backlight_config;
    backlight_config.raw = eeconfig_read_backlight();

    if (backlight_config.level == 0)
    {
        backlight_config.level = BACKLIGHT_LEVELS - 1;
        eeconfig_write_backlight(backlight_config.raw);
    }

    IS31FL3731_init();
}

void backlight_set(uint8_t level)
{
    dprintf("backlight_set %d\n", level);

    if (level == 0)
    {
        set_region_mode(BACKLIGHT_REGION_ALL, LedControlMode_disable_mask);
    }
    else
    {
        backlight_set_regions_from_saved_state();
    }
}

void backlight_initialize_regions(void)
{
    IS31FL3731_enable();
    set_region_mode(BACKLIGHT_REGION_ALL, LedControlMode_disable_mask);

    backlight_load_region_states();
    backlight_set_regions_from_saved_state();
}

} /* C */
