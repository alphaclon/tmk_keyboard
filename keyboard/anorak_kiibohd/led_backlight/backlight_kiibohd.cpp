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
#include <util/delay.h>
#include "pwm_control.h"
#include "led_control.h"
#include "led_masks.h"
#include "twi_config.h"

extern "C" {
#include "config.h"
#include "eeconfig.h"
#include "eeconfig_backlight.h"
#include "backlight.h"
#include "backlight_kiibohd.h"
#include "../nfo_led.h"
}

#ifdef DEBUG_BACKLIGHT
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define BRIGHTNESS_MAX_LEVEL 8

extern "C" {

uint8_t regions = 0;
uint8_t region_brightness[8] = {0};
uint8_t current_region = backlight_region_ALL;

void set_region_mask_for_control(uint8_t region, uint8_t mask[ISSI_LED_MASK_SIZE])
{
    switch (region)
    {
    case backlight_region_WASD:
        memcpy_P(mask, LedMaskWASD, ISSI_LED_MASK_SIZE);
        break;
    case backlight_region_jump:
        memcpy_P(mask, LedMaskJump, ISSI_LED_MASK_SIZE);
        break;
    case backlight_region_controls:
        memcpy_P(mask, LedMaskCtrl, ISSI_LED_MASK_SIZE);
        break;
    case backlight_region_logo:
        memcpy_P(mask, LedMaskLogo, ISSI_LED_MASK_SIZE);
        break;
    case backlight_region_other:
        memcpy_P(mask, LedMaskOthr, ISSI_LED_MASK_SIZE);
        break;
    case backlight_region_case:
        memcpy_P(mask, LedMaskCase, ISSI_LED_MASK_SIZE);
        break;
    case backlight_region_ALL:
        memcpy_P(mask, LedMaskFull, ISSI_LED_MASK_SIZE);
        break;
    default:
        memset(mask, 0, ISSI_TOTAL_LED_MASK_SIZE);
        //dprintf("unknown region %d\n", region);
        break;
    }

#ifdef DEBUG_BACKLIGHT_EXTENDED
    dprintf("selected mask: %u\r\n", region);
    for (uint8_t r = 0; r < ISSI_USED_ROWS; ++r)
    {
        dprintf("%02u: ", r);
        for (uint8_t c = 0; c < 2; ++c)
        {
            dprintf("%02X ", mask[r * 2 + c]);
        }
        dprintf("\r\n");
    }
    dprintf("\r\n");
#endif
}

uint8_t get_index_for_region(uint8_t region)
{
    uint8_t pos = 0;

    switch (region)
    {
    case backlight_region_WASD:
        pos = BACKLIGHT_WASD;
        break;
    case backlight_region_jump:
        pos = BACKLIGHT_JUMP;
        break;
    case backlight_region_controls:
        pos = BACKLIGHT_CONTROLS;
        break;
    case backlight_region_logo:
        pos = BACKLIGHT_LOGO;
        break;
    case backlight_region_other:
        pos = BACKLIGHT_OTHER;
        break;
    case backlight_region_case:
        pos = BACKLIGHT_CASE;
        break;
    case backlight_region_ALL:
        pos = BACKLIGHT_ALL;
        break;
    default:
        break;
    }

    return pos;
}

void set_region_mode(uint8_t region, tLedRegionControlMode mode)
{
    //dprintf("set_region_mode r:%d m:%d\n", region, mode);
    tLedRegionControlCommand control;
    control.mode = mode;
    set_region_mask_for_control(region, control.mask);

    IS31FL3731_region_control(&control);
}

void set_brightness_for_region(uint8_t region, uint8_t brightness, tLedPWMControlMode mode)
{
    dprintf("set bri %d\n", brightness);
    tLedPWMControlCommand control;
    control.mode = mode;
    control.amount = gamma_correction_table[brightness];
    set_region_mask_for_control(region, control.mask);
    IS31FL3731_PWM_control(&control);
}

void set_and_save_brightness_for_region(uint8_t region, uint8_t pos, uint8_t brightness)
{
    if (brightness >= GAMMA_STEPS)
        brightness = GAMMA_STEPS - 1;

    set_brightness_for_region(region, brightness, LedControlMode_brightness_set_by_mask);
    region_brightness[pos] = brightness;
}

void set_brightness_for_all_regions(uint8_t brightness, tLedPWMControlMode mode)
{
    tLedPWMControlCommand control;
    control.mode = mode;
    control.amount = brightness;
    IS31FL3731_PWM_control(&control);
}

void backlight_increase_brightness_for_region(uint8_t region)
{
    //dprintf("bl_inc_bri_for_region %d\n", region);

    if ((regions & region) == 0)
    {
        //set_region_mode(region, LedControlMode_enable_mask);
        return;
    }

    uint8_t pos = get_index_for_region(region);
    uint8_t brightness = region_brightness[pos];

    if (brightness >= GAMMA_STEPS)
        brightness = GAMMA_STEPS - 1;

    if (brightness < (GAMMA_STEPS - 1))
        brightness++;

    //dprintf("bri %d\n", brightness);

    set_and_save_brightness_for_region(region, pos, brightness);
}

void backlight_increase_brightness_selected_region()
{
    backlight_increase_brightness_for_region(current_region);
}

void backlight_decrease_brightness_for_region(uint8_t region)
{
    //dprintf("bl_dec_bri_for_region %d\n", region);

    if ((regions & region) == 0)
    {
        //set_region_mode(region, LedControlMode_enable_mask);
        return;
    }

    uint8_t pos = get_index_for_region(region);
    uint8_t brightness = region_brightness[pos];

    if (brightness > 0)
        brightness--;

    dprintf("bri %d\n", brightness);

    set_and_save_brightness_for_region(region, pos, brightness);
}

void backlight_decrease_brightness_selected_region()
{
    backlight_decrease_brightness_for_region(current_region);
}

void backlight_set_brightness_for_region(uint8_t region, uint8_t brightness)
{
    dprintf("backlight_set_brightness_for_region %d\n", brightness);
    uint8_t pos = get_index_for_region(current_region);
    set_and_save_brightness_for_region(current_region, pos, brightness);
}

void backlight_brightness_set_all(uint8_t brightness)
{
    //dprintf("bl_bri_set_all %d\n", brightness);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);
        backlight_set_brightness_for_region(region, brightness);
    }
}

void backlight_increase_brightness()
{
    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);
        backlight_increase_brightness_for_region(region);
    }
}

void backlight_decrease_brightness()
{
    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);
        backlight_decrease_brightness_for_region(region);
    }
}

void backlight_select_region(uint8_t region)
{
    current_region = region;
    //dprintf("bl_sel_region %u on=%u\n", region, (regions & region) ? 1 : 0);
}

void backlight_toggle_region(uint8_t region)
{
    dprintf("bl_tog_region %d\n", region);

    current_region = region;

    if (regions & region)
    {
        regions &= ~region;
        set_region_mode(region, LedControlMode_disable_mask);
        //dprintf("region %d: off\n", region);
    }
    else
    {
        regions |= region;
        set_region_mode(region, LedControlMode_enable_mask);
        //dprintf("region %d: on\n", region);
    }
}

void backlight_toggle_selected_region()
{
    backlight_toggle_region(current_region);
}

void backlight_disable_region(uint8_t region)
{
    regions &= ~region;
    current_region = region;
    dprintf("region %u: off\n", region);
    set_region_mode(region, LedControlMode_disable_mask);
}

void backlight_enable_region(uint8_t region)
{
    regions |= region;
    current_region = region;
    dprintf("region %u: on\n", region);
    set_region_mode(region, LedControlMode_enable_mask);
}

void backlight_save_region_states()
{
#ifdef BACKLIGHT_ENABLE
    //dprintf("save\n");

    eeconfig_write_backlight_regions(regions);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        eeconfig_write_backlight_region_brightness(pos, region_brightness[pos]);
    }
#endif
}

void backlight_load_region_states()
{
#ifdef BACKLIGHT_ENABLE
    regions = eeconfig_read_backlight_regions();

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t brightness = eeconfig_read_backlight_region_brightness(pos);
        region_brightness[pos] = brightness;
    }
#endif
}

void backlight_set_regions_from_saved_state(void)
{
    set_region_mode(backlight_region_ALL, LedControlMode_disable_mask);

    dprintf("regions %u\n", regions);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);

        dprintf("region %u on=%u\n", region, (regions & region) ? 1 : 0);

        if (regions & region)
        {
            set_region_mode(region, LedControlMode_enable_mask);

            uint8_t brightness = region_brightness[pos];
            set_and_save_brightness_for_region(region, pos, brightness);
        }
    }
}

// Defined in tmk_core/common/backlight.h
void backlight_set(uint8_t level)
{
    dprintf("bl_set %d\n", level);

    LedInfo2_Toggle();

    if (level == 0)
    {
        set_region_mode(backlight_region_ALL, LedControlMode_disable_mask);
    }
    else
    {
        backlight_set_regions_from_saved_state();
    }
}

void backlight_setup()
{
	dprintf("bl_setup\n");

    IS31FL3731_init();
    IS31FL3731_set_power_target_I_max(500);

    regions = 0;
    current_region = backlight_region_ALL;

#ifdef BACKLIGHT_ENABLE
    dprintf("bl_setup\n");

    if (!eeconfig_backlight_is_enabled())
    {
        eeconfig_backlight_init();
    }

    backlight_config_t backlight_config;
    backlight_config.raw = eeconfig_read_backlight();

    if (backlight_config.level == 0)
    {
        backlight_config.level = BACKLIGHT_LEVELS - 1;
        eeconfig_write_backlight(backlight_config.raw);
    }

    backlight_load_region_states();
#endif
}

void backlight_setup_finish()
{
}

void backlight_test(void)
{
	issi.dumpConfiguration();
	issi.dumpLeds(0);
	issi.dumpBrightness(0);
}

} /* C */
