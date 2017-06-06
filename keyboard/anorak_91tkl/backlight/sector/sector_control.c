
#include "sector_control.h"
#include "../issi/is31fl3733_91tkl.h"
#include "../../utils.h"
#include "config.h"
#include "eeconfig.h"
#include "../eeconfig_backlight.h"
#include "sector_led_masks.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>

#ifdef DEBUG_BACKLIGHT
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define HSV_COLOR_STEP 8

typedef union PACKED {
    struct
    {
        uint8_t h;
        uint8_t s;
        uint8_t v;
    };
    uint8_t lvl[3];
} Levels;

#if BACKLIGHT_LEVELS == 0
#error Invalid BACKLIGHT_LEVELS value. Must be greater zero!
#endif

uint8_t sector_mask = 0;
Levels sector_levels[SECTOR_MAX];
uint8_t selected_sector = 0xff;
uint8_t custom_pwm_map = 0xff;
bool has_custom_pwm_map = false;
uint8_t mask[IS31FL3733_LED_ENABLE_SIZE];

void sector_disable_leds_by_mask(KeyboardSector sector)
{
    dprintf("sector_disable_leds_by_mask %u:\n", sector);

    memcpy_P(mask, LedMasksTop[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_disable_leds_by_mask(issi.upper->device, mask);

    memcpy_P(mask, LedMasksBottom[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_disable_leds_by_mask(issi.lower->device, mask);
}

void sector_enable_leds_by_mask(KeyboardSector sector)
{
    dprintf("sector_enable_leds_by_mask %u:\n", sector);

    memcpy_P(mask, LedMasksTop[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_enable_leds_by_mask(issi.upper->device, mask);

    memcpy_P(mask, LedMasksBottom[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_enable_leds_by_mask(issi.lower->device, mask);
}

void sector_enable_all_leds(void)
{
    dprintf("sector_enable_all_leds\n");

    memcpy_P(mask, LedMaskFull_Top, IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_enable_leds_by_mask(issi.upper->device, mask);

    memcpy_P(mask, LedMaskFull_Bottom, IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_enable_leds_by_mask(issi.lower->device, mask);
}

void sector_set_mask(KeyboardSector sector)
{
    dprintf("sector_set_mask %u:\n", sector);

    memcpy_P(mask, LedMasksTop[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_set_mask(issi.upper->device, mask);

    memcpy_P(mask, LedMasksBottom[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_set_mask(issi.lower->device, mask);
}

void sector_nand_mask(KeyboardSector sector)
{
    dprintf("sector_nand_mask %u:\n", sector);

    memcpy_P(mask, LedMasksTop[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_nand_mask(issi.upper->device, mask);

    memcpy_P(mask, LedMasksBottom[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_nand_mask(issi.lower->device, mask);
}

void sector_or_mask(KeyboardSector sector)
{
    dprintf("sector_or_mask %u:\n", sector);

    memcpy_P(mask, LedMasksTop[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_or_mask(issi.upper->device, mask);

    memcpy_P(mask, LedMasksBottom[sector], IS31FL3733_LED_ENABLE_SIZE);
    is31fl3733_or_mask(issi.lower->device, mask);
}

void sector_clear_mask(void)
{
    dprintf("sector_clear_mask\n");
    is31fl3733_clear_mask(issi.upper->device);
    is31fl3733_clear_mask(issi.lower->device);
}

bool sector_is_enabled(KeyboardSector sector)
{
    return (sector_mask & SECTOR_BV(sector));
}

void sector_select(KeyboardSector sector)
{
    selected_sector = sector;
    dprintf("sector_select [%u] on=%u\n", selected_sector, sector_is_enabled(sector));
}

void sector_set_off(KeyboardSector sector)
{
    dprintf("sector_set_off %u: off\n", sector);
    sector_disable_leds_by_mask(sector);
    sector_mask &= ~SECTOR_BV(sector);
    is31fl3733_91tkl_update_led_enable(&issi);
}

void sector_set_on(KeyboardSector sector)
{
    dprintf("sector_set_on %u: on\n", sector);
    sector_enable_leds_by_mask(sector);
    sector_mask |= SECTOR_BV(sector);
    is31fl3733_91tkl_update_led_enable(&issi);
}

void sector_toggle(KeyboardSector sector)
{
    if (sector_is_enabled(sector))
    {
        sector_set_off(sector);
    }
    else
    {
        sector_set_on(sector);
    }
}

void sector_set_selected(bool on)
{
	if (on)
		sector_set_on(selected_sector);
	else
		sector_set_off(selected_sector);
}

void sector_toggle_selected(void)
{
    sector_toggle(selected_sector);
}

void sector_set_selected_on(void)
{
    sector_set_on(selected_sector);
}

void sector_set_selected_off(void)
{
    sector_set_off(selected_sector);
}

void sector_set_all_off(void)
{
    selected_sector = 0;

    is31fl3733_led_disable_all(issi.upper->device);
    is31fl3733_led_disable_all(issi.lower->device);

    is31fl3733_clear_mask(issi.upper->device);
    is31fl3733_clear_mask(issi.lower->device);

    is31fl3733_91tkl_update_led_enable(&issi);
}

void sector_selected_set_hsv_color(HSV color)
{
    dprintf("sector_set_hsv_color_by_levels: %u, %X%X%X\n", selected_sector, color.h, color.s, color.v);

    sector_set_mask(selected_sector);
    is31fl3733_91tkl_fill_hsv_masked(&issi, color);
}

void sector_set_hsv_color(KeyboardSector sector, HSV color)
{
    dprintf("sector_set_hsv_color_by_levels: %u, %X%X%X\n", sector, color.h, color.s, color.v);

    sector_set_mask(sector);
    is31fl3733_91tkl_fill_hsv_masked(&issi, color);
}

void sector_set_hsv_color_by_levels(KeyboardSector sector, Levels *levels)
{
    HSV color;
    color.h = levels->h;
    color.s = levels->s;
    color.v = levels->v;

    sector_set_hsv_color(sector, color);
}

void sector_increase_hsv_by_name(KeyboardSector sector, HSVColorName color_name)
{
    dprintf("sector_increase_sector_hsv_by_name: %u %u\n", sector, color_name);
    sector_levels[sector].lvl[color_name] = increment(sector_levels[sector].lvl[color_name], HSV_COLOR_STEP, 0, 255);
    sector_set_hsv_color_by_levels(sector, &sector_levels[selected_sector]);
}

void sector_selected_increase_hsv_color(HSVColorName color)
{
    sector_increase_hsv_by_name(selected_sector, color);
    is31fl3733_91tkl_update_led_pwm(&issi);
}

void sector_all_increase_hsv_color(HSVColorName color)
{
    for (uint8_t sector = 0; sector < SECTOR_MAX; ++sector)
        sector_increase_hsv_by_name(sector, color);
    is31fl3733_91tkl_update_led_pwm(&issi);
}

void sector_decrease_sector_hsv_by_name(KeyboardSector sector, HSVColorName color_name)
{
    dprintf("sector_decrease_sector_hsv_by_name: %u %u\n", sector, color_name);
    sector_levels[sector].lvl[color_name] = decrement(sector_levels[sector].lvl[color_name], HSV_COLOR_STEP, 0, 255);
    sector_set_hsv_color_by_levels(sector, &sector_levels[sector]);
}

void sector_selected_decrease_hsv_color(HSVColorName color)
{
    sector_decrease_sector_hsv_by_name(selected_sector, color);
    is31fl3733_91tkl_update_led_pwm(&issi);
}

void sector_all_decrease_hsv_color(HSVColorName color)
{
    for (uint8_t sector = 0; sector < SECTOR_MAX; ++sector)
        sector_decrease_sector_hsv_by_name(sector, color);
    is31fl3733_91tkl_update_led_pwm(&issi);
}

void sector_save_state()
{
#ifdef BACKLIGHT_ENABLE
    dprintf("sector_save_state\n");

    eeconfig_write_backlight_sectors_state(sector_mask);

    for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
    {
        eeconfig_write_backlight_sector_values(sector, sector_levels[sector].h, sector_levels[sector].s,
                                               sector_levels[sector].v);
    }

    eeconfig_write_backlight_pwm_active_map(custom_pwm_map);
#endif
}

void sector_load_custom_pwm_map(void)
{
#ifdef BACKLIGHT_ENABLE
    dprintf("sector_load_custom_pwm_map: %u\n", custom_pwm_map);

    uint8_t *buffer;
    buffer = is31fl3733_pwm_buffer(issi.upper->device);
    eeconfig_read_backlight_pwm_map(custom_pwm_map, buffer, false);
    buffer = is31fl3733_pwm_buffer(issi.lower->device);
    eeconfig_read_backlight_pwm_map(custom_pwm_map, buffer, true);

    sector_enable_all_leds();
#endif
}

void sector_save_custom_pwm_map(void)
{
#ifdef BACKLIGHT_ENABLE
    dprintf("sector_save_custom_pwm_map: %u\n", custom_pwm_map);

    uint8_t *buffer;
    buffer = is31fl3733_pwm_buffer(issi.upper->device);
    eeconfig_write_backlight_pwm_map(custom_pwm_map, buffer, false);
    buffer = is31fl3733_pwm_buffer(issi.lower->device);
    eeconfig_write_backlight_pwm_map(custom_pwm_map, buffer, true);

    eeconfig_write_backlight_pwm_active_map(custom_pwm_map);
#endif
}

void sector_load_state()
{
#ifdef BACKLIGHT_ENABLE
    sector_mask = eeconfig_read_backlight_sectors_state();
    dprintf("sector_load_state: sector_mask: 0x%X\n", sector_mask);

    for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
    {
        eeconfig_read_backlight_sector_values(sector, &sector_levels[sector].h, &sector_levels[sector].s,
                                              &sector_levels[sector].v);
    }

    custom_pwm_map = eeconfig_read_backlight_pwm_active_map();

    if (custom_pwm_map >= EECONFIG_BACKLIGHT_PWM_MAP_COUNT)
        custom_pwm_map = 0xff;

    has_custom_pwm_map = (custom_pwm_map < EECONFIG_BACKLIGHT_PWM_MAP_COUNT);
#endif
}

void sector_restore_sector(uint8_t sector)
{
    if (sector_is_enabled(sector))
    {
        sector_enable_leds_by_mask(sector);
        sector_set_hsv_color_by_levels(sector, &sector_levels[sector]);
    }
}

void sector_restore_state(void)
{
    dprintf("sector_restore_state\n");

    sector_set_all_off();
    sector_load_state();

    if (has_custom_pwm_map)
    {
        sector_load_custom_pwm_map();
    }
    else
    {
        for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
            sector_restore_sector(sector);
    }

    if (sector_mask || has_custom_pwm_map)
    {
        is31fl3733_91tkl_update_led_pwm(&issi);
        is31fl3733_91tkl_update_led_enable(&issi);
    }
}

void sector_set_sector_mode()
{
    sector_set_custom_map(0xff);
}

void sector_set_custom_map(uint8_t custom_map)
{
    custom_pwm_map = custom_map;
    has_custom_pwm_map = (custom_pwm_map < EECONFIG_BACKLIGHT_PWM_MAP_COUNT);
    sector_restore_state();
}

uint8_t sector_get_custom_map(void)
{
	return custom_pwm_map;
}

void sector_next_custom_map()
{
    custom_pwm_map = increment(custom_pwm_map, 1, 0, EECONFIG_BACKLIGHT_PWM_MAP_COUNT);
    if (custom_pwm_map >= EECONFIG_BACKLIGHT_PWM_MAP_COUNT)
        custom_pwm_map = 0xff;

    sector_set_custom_map(custom_pwm_map);
}

void sector_control_init(void)
{
    sector_mask = 0;
    has_custom_pwm_map = false;
    memset(sector_levels, 0, sizeof(sector_levels));
    sector_set_all_off();
}

void sector_dump_state(void)
{
#ifdef DEBUG_BACKLIGHT
    dprintf("sector_dump_state\n");
    dprintf("sector_mask: 0x%X\n", sector_mask);
    dprintf("custom_pwm_map: %u\n", custom_pwm_map);
    for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
    {
        dprintf("sector [%u]: e:%u,", sector, sector_is_enabled(sector));
        for (uint8_t color = 0; color < 3; color++)
            dprintf(" %u", sector_levels[sector].lvl[color]);
        dprintf("\n");
    }
#endif
}

void sector_dump_mask(uint8_t *mask)
{
#if defined(DEBUG_BACKLIGHT_EXTENDED) && defined(DEBUG_BACKLIGHT)
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
