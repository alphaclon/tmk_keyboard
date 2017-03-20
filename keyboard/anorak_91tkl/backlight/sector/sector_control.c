
#define BACKLIGHT_ENABLE

#include "sector_control.h"
#include "../issi/is31fl3733_rgb_91tkl.h"
#include "config.h"
#include "eeconfig.h"
#include "eeconfig_backlight.h"
#include "sector_led_masks.h"

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

uint8_t sector_mask = 0;
uint8_t sector_brightness[8][3] = {0};
uint8_t selected_sector = 0xff;

void sector_dump_mask(void)
{
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

void sector_nand_mask(enum keyboard_sector sector)
{
    uint8_t mask[IS31FL3733_LED_ENABLE_SIZE];

    memcpy_P(mask, LedMasksTop[sector], IS31FL3733_LED_ENABLE_SIZE);
    IS31FL3733_ApplyMask_NAnd(issi.device1->device, mask);

    memcpy_P(mask, LedMasksBottom[sector], IS31FL3733_LED_ENABLE_SIZE);
    IS31FL3733_ApplyMask_NAnd(issi.device2->device, mask);

    sector_dump_mask();
}

void sector_or_mask(enum keyboard_sector sector)
{
    uint8_t mask[IS31FL3733_LED_ENABLE_SIZE];

    memcpy_P(mask, LedMasksTop[sector], IS31FL3733_LED_ENABLE_SIZE);
    IS31FL3733_ApplyMask_Or(issi.device1->device, mask);

    memcpy_P(mask, LedMasksBottom[sector], IS31FL3733_LED_ENABLE_SIZE);
    IS31FL3733_ApplyMask_Or(issi.device2->device, mask);

    sector_dump_mask();
}

void sector_select(enum keyboard_sector sector)
{
    selected_sector = sector;
    dprintf("sector_set [%u] on=%u\n", selected_sector, sector_is_enabled(sector));
}

bool sector_is_enabled(enum keyboard_sector sector)
{
    return (sector_mask & SECTOR_BV(sector));
}

void sector_set_off(enum keyboard_sector sector)
{
    dprintf("sector_set_off %u: off\n", sector);

    if (sector_is_enabled(sector))
        sector_nand_mask(sector);

    sector_mask &= ~SECTOR_BV(sector);
    IS31FL3733_RGB_91TKL_Update_LedEnableStates(&issi);
}

void sector_set_on(enum keyboard_sector sector)
{
	dprintf("sector_set_on %u: on\n", sector);

    if (!sector_is_enabled(sector))
        sector_or_mask(sector);

    sector_mask |= SECTOR_BV(sector);
    IS31FL3733_RGB_91TKL_Update_LedEnableStates(&issi);
}

void sector_toggle(enum keyboard_sector sector)
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
    uint8_t mask[IS31FL3733_LED_ENABLE_SIZE];

    memcpy_P(mask, LedMaskFull_Top, IS31FL3733_LED_ENABLE_SIZE);
    IS31FL3733_ApplyMask_NAnd(issi.device1->device, mask);

    memcpy_P(mask, LedMaskFull_Bottom, IS31FL3733_LED_ENABLE_SIZE);
    IS31FL3733_ApplyMask_NAnd(issi.device2->device, mask);

    selected_sector = 0;

    IS31FL3733_RGB_91TKL_Update_LedEnableStates(&issi);
}

void sector_set_brightness(enum keyboard_sector sector, enum IS31FL3733_RGB_Color color, uint8_t brightness)
{
    IS31FL3733_RGB_91TKL_SetBrightnessForMasked(&issi, color, brightness);
    sector_brightness[sector][color] = brightness;
    IS31FL3733_RGB_91TKL_Update_LedPwmValues(&issi);
}

void sector_increase_brightness(enum keyboard_sector sector, enum IS31FL3733_RGB_Color color)
{
    uint8_t brightness = sector_brightness[sector][color];

    if (brightness >= BRIGHTNESS_STEPS)
        brightness = BRIGHTNESS_STEPS - 1;

    if (brightness < (BRIGHTNESS_STEPS - 1))
        brightness++;

    sector_set_brightness(sector, color, brightness);
}

void sector_increase_brightness_selected(enum IS31FL3733_RGB_Color color)
{
    sector_increase_brightness(selected_sector, color);
}

void sector_decrease_brightness(enum keyboard_sector sector, enum IS31FL3733_RGB_Color color)
{
    uint8_t brightness = sector_brightness[sector][color];

    if (brightness > 1)
        brightness--;

    sector_set_brightness(sector, color, brightness);
}

void sector_decrease_brightness_selected(enum IS31FL3733_RGB_Color color)
{
    sector_decrease_brightness(selected_sector, color);
}

void sector_save_state()
{
#ifdef BACKLIGHT_ENABLE
    dprintf("save\n");

    eeconfig_write_backlight_state(sector_mask);

    for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
        for (uint8_t color = 0; color < 3; color++)
            eeconfig_write_backlight_sector_brightness(sector, color, sector_brightness[sector][color]);
#endif
}

void sector_load_state()
{
#ifdef BACKLIGHT_ENABLE
    sector_mask = eeconfig_read_backlight_state();
    dprintf("sector_load_state: sector_mask: 0x%X\n", sector_mask);

    for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
        for (uint8_t color = 0; color < 3; color++)
            sector_brightness[sector][color] = eeconfig_read_backlight_sector_brightness(sector, color);
#endif
}

void sector_restore_state(void)
{
    dprintf("sector_restore_state\n");

    sector_set_all_off();
    sector_load_state();

    for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
    {
        if (sector_is_enabled(sector))
        {
            sector_or_mask(sector);
            sector_mask |= SECTOR_BV(sector);
        }

        for (uint8_t color = 0; color < 3; color++)
        {
            uint8_t brightness = sector_brightness[sector][color];
            IS31FL3733_RGB_91TKL_SetBrightnessForMasked(&issi, color, brightness);
            sector_brightness[sector][color] = brightness;
        }
    }

    IS31FL3733_RGB_91TKL_Update_LedEnableStates(&issi);
    IS31FL3733_RGB_91TKL_Update_LedPwmValues(&issi);
}

void sector_control_init(void)
{
	sector_mask = 0;
	memset(sector_brightness, 0, sizeof(sector_brightness));
}

void sector_dump_state(void)
{
    printf("sector_dump_state\n");
    printf("sector_mask: 0x%X\n", sector_mask);
    for (uint8_t sector = 0; sector < SECTOR_MAX; sector++)
    {
        printf("sector [%u]: %u", sector, sector_is_enabled(sector));
        for (uint8_t color = 0; color < 3; color++)
            printf(" %u", sector_brightness[sector][color]);
        printf("\n");
    }
}
