/*
Copyright 2016 Moritz Wenk <MoritzWenk@web.de>

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

#include "backlight.h"
#include "../nfo_led.h"
#include "../splitbrain.h"
#include "../twi/twi_config.h"
#include "backlight_kiibohd.h"
#include "config.h"
#include "led_control.h"
#include "led_masks.h"
#include "pwm_control.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "eeconfig.h"
#include "eeconfig_backlight.h"
#ifdef __cplusplus
}
#endif

#ifdef DEBUG_BACKLIGHT
#include "../uart/uart.h"
#include "debug.h"
#else
#include "nodebug.h"
#endif

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define BRIGHTNESS_MAX_LEVEL 8

uint8_t regions = 0;
uint8_t region_brightness[8] = {0};
uint8_t current_region = backlight_region_ALL;

#ifdef DEBUG_BACKLIGHT_NAMES
const char REGION1[] PROGMEM = "WASD";
const char REGION2[] PROGMEM = "Controls";
const char REGION3[] PROGMEM = "Cursor";
const char REGION4[] PROGMEM = "Other";
const char REGION5[] PROGMEM = "ALL";

PGM_P region_names_table[] = {REGION1, REGION2, REGION3, REGION4, REGION5};
static char stringBuffer[64];
#endif

void set_region_mask_for_control(uint8_t region, uint8_t mask[ISSI_LED_MASK_SIZE])
{
    if (is_left_side_of_keyboard())
    {
        switch (region)
        {
        case backlight_region_WASD:
            memcpy_P(mask, LedMaskWASD_Left, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_controls:
            memcpy_P(mask, LedMaskCtrl_Left, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_cursor:
            memcpy_P(mask, LedMaskCurs_Left, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_other:
            memcpy_P(mask, LedMaskOthr_Left, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_ALL:
            memcpy_P(mask, LedMaskFull_Left, ISSI_LED_MASK_SIZE);
            break;
        default:
            memset(mask, 0, ISSI_TOTAL_LED_MASK_SIZE);
            dprintf("unknown region: %u\n", region);
            break;
        }
    }
    else
    {
        switch (region)
        {
        case backlight_region_WASD:
            memcpy_P(mask, LedMaskWASD_Right, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_controls:
            memcpy_P(mask, LedMaskCtrl_Right, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_cursor:
            memcpy_P(mask, LedMaskCurs_Right, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_other:
            memcpy_P(mask, LedMaskOthr_Right, ISSI_LED_MASK_SIZE);
            break;
        case backlight_region_ALL:
            memcpy_P(mask, LedMaskFull_Right, ISSI_LED_MASK_SIZE);
            break;
        default:
            memset(mask, 0, ISSI_TOTAL_LED_MASK_SIZE);
            dprintf("unknown region: %u\n", region);
            break;
        }
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
    case backlight_region_controls:
        pos = BACKLIGHT_CONTROLS;
        break;
    case backlight_region_cursor:
        pos = BACKLIGHT_CURSOR;
        break;
    case backlight_region_other:
        pos = BACKLIGHT_OTHER;
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
#ifdef DEBUG_BACKLIGHT_NAMES
    strcpy_P(stringBuffer, region_names_table[region]);
    dprintf("set_region_mode r:%d [%s] m:%d\n", region, stringBuffer, mode);
#else
    dprintf("set_region_mode r:%d m:%d\n", region, mode);
#endif
    tLedRegionControlCommand control;
    control.mode = mode;
    set_region_mask_for_control(region, control.mask);

    IS31FL3731_region_control(&control);
}

void set_brightness_for_region(uint8_t region, uint8_t brightness, tLedPWMControlMode mode)
{
    dprintf("set_brightness_for_region r:%u bri:%u\n", region, brightness);
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
    dprintf("backlight_increase_brightness_for_region %d\n", region);

    if ((regions & region) == 0)
    {
        // set_region_mode(region, LedControlMode_enable_mask);
        return;
    }

    uint8_t pos = get_index_for_region(region);
    uint8_t brightness = region_brightness[pos];

    if (brightness >= GAMMA_STEPS)
        brightness = GAMMA_STEPS - 1;

    if (brightness < (GAMMA_STEPS - 1))
        brightness++;

    dprintf("new brightness %d\n", brightness);

    set_and_save_brightness_for_region(region, pos, brightness);
}

void backlight_increase_brightness_selected_region()
{
    backlight_increase_brightness_for_region(current_region);
}

void backlight_decrease_brightness_for_region(uint8_t region)
{
    dprintf("backlight_decrease_brightness_for_region %d\n", region);

    if ((regions & region) == 0)
    {
        // set_region_mode(region, LedControlMode_enable_mask);
        return;
    }

    uint8_t pos = get_index_for_region(region);
    uint8_t brightness = region_brightness[pos];

    if (brightness > 0)
        brightness--;

    dprintf("new brightness %d\n", brightness);

    set_and_save_brightness_for_region(region, pos, brightness);
}

void backlight_decrease_brightness_selected_region()
{
    backlight_decrease_brightness_for_region(current_region);
}

void backlight_set_brightness_for_region(uint8_t region, uint8_t brightness)
{
    dprintf("bl_bri_set_by_mask %d\n", brightness);
    uint8_t pos = get_index_for_region(current_region);
    set_and_save_brightness_for_region(current_region, pos, brightness);
}

void backlight_brightness_set_all(uint8_t brightness)
{
    dprintf("bl_bri_set_all %d\n", brightness);

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

#ifdef DEBUG_BACKLIGHT_NAMES
    strcpy_P(stringBuffer, region_names_table[get_index_for_region(region)]);
    dprintf("backlight_select_region %u [%s] on=%u\n", region, stringBuffer, (regions & region) ? 1 : 0);
#else
    dprintf("backlight_select_region %u on=%u\n", region, (regions & region) ? 1 : 0);
#endif
}

void backlight_toggle_region(uint8_t region)
{
    current_region = region;

    if (regions & region)
    {
        regions &= ~region;
        set_region_mode(region, LedControlMode_disable_mask);

#ifdef DEBUG_BACKLIGHT_NAMES
    strcpy_P(stringBuffer, region_names_table[get_index_for_region(region)]);
    dprintf("toggle_region %u [%s]: off\n", region, stringBuffer);
#else
    dprintf("toggle_region %u: off\n", region);
#endif
    }
    else
    {
        regions |= region;
        set_region_mode(region, LedControlMode_enable_mask);

#ifdef DEBUG_BACKLIGHT_NAMES
    strcpy_P(stringBuffer, region_names_table[get_index_for_region(region)]);
    dprintf("toggle_region %u [%s]: on\n", region, stringBuffer);
#else
    dprintf("toggle_region %u: on\n", region);
#endif

    }
}

void backlight_selected_region_on(void)
{
    backlight_enable_region(current_region);
}

void backlight_selected_region_off(void)
{
    backlight_disable_region(current_region);
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

#ifdef DEBUG_BACKLIGHT_NAMES
    strcpy_P(stringBuffer, region_names_table[region]);
    dprintf("disable_region %u [%s]: off\n", region, stringBuffer);
#else
    dprintf("disable_region %u: off\n", region);
#endif
}

void backlight_enable_region(uint8_t region)
{
    regions |= region;
    current_region = region;
    set_region_mode(region, LedControlMode_enable_mask);

#ifdef DEBUG_BACKLIGHT_NAMES
    strcpy_P(stringBuffer, region_names_table[region]);
    dprintf("enable_region %u [%s]: on\n", region, stringBuffer);
#else
    dprintf("enable_region %u: on\n", region);
#endif
}

void backlight_save_region_states()
{
#ifdef BACKLIGHT_ENABLE
    dprintf("save\n");

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

    dprintf("saved region states: 0x%X\n", regions);

    for (uint8_t pos = 0; pos < BACKLIGHT_MAX_REGIONS; pos++)
    {
        uint8_t region = (1 << pos);

        dprintf("set saved region %u on=%u\n", region, (regions & region) ? 1 : 0);

        if (regions & region)
        {
            set_region_mode(region, LedControlMode_enable_mask);
        }

        uint8_t brightness = region_brightness[pos];
        set_and_save_brightness_for_region(region, pos, brightness);
    }
}

#ifdef __cplusplus
extern "C" {
#endif

// Defined in tmk_core/common/backlight.h
void backlight_set(uint8_t level)
{
    dprintf("backlight_set level:%d\n", level);

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
    dprintf("backlight_setup\r\n");

    if (!eeconfig_backlight_is_enabled())
    {
        eeconfig_backlight_init();
    }

    IS31FL3731_init();
    IS31FL3731_set_power_target_I_max(50);

    regions = 0;
    current_region = backlight_region_ALL;

#ifdef BACKLIGHT_ENABLE
    backlight_config_t backlight_config;
    backlight_config.raw = eeconfig_read_backlight();

    dprintf("backlight_setup on:%u, level:%u\r\n", backlight_config.enable, backlight_config.level);

    if (backlight_config.level == 0)
    {
        dprintf("fix level\r\n");
        backlight_config.level = BACKLIGHT_LEVELS - 1;
        eeconfig_write_backlight(backlight_config.raw);
    }

    backlight_load_region_states();
#endif

    dprintf("backlight_setup done\r\n");
}

void backlight_setup_finish()
{
    IS31FL3731_set_power_target_I_max(150);
    dprintf("backlight_setup_finish\r\n");
}

#ifdef __cplusplus
}
#endif

void backlight_dump_issi_state()
{
    issi.dumpConfiguration();
    issi.dumpLeds(0);
    issi.dumpBrightness(0);
}
