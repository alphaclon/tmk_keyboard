#ifndef _BACKLIGHT_KIIBOHD_
#define _BACKLIGHT_KIIBOHD_

enum backlight_region
{
    BACKLIGHT_REGION_ALL = 0x01,
    BACKLIGHT_REGION_WASD = 0x02,
    BACKLIGHT_REGION_JUMP = 0x04,
    BACKLIGHT_REGION_CONTROLS = 0x08,
    BACKLIGHT_REGION_LOGO = 0x10,
    BACKLIGHT_REGION_CASE = 0x20,
    BACKLIGHT_REGION_OTHER = 0x40,
};

#define BACKLIGHT_MAX_REGIONS 7

void backlight_initialize_regions(void);

void backlight_set_brightness(uint8_t brightness);
void backlight_set_brightness_for_region(uint8_t region, uint8_t brightness);

void backlight_increase_brightness(uint8_t delta_brightness);
void backlight_decrease_brightness(uint8_t delta_brightness);

void backlight_toggle_region(uint8_t region);
void backlight_disable_region(uint8_t region);
void backlight_enable_region(uint8_t region);

void backlight_select_region(uint8_t region);
void backlight_toggle_selected_region();

void backlight_increase_brightness_selected_region(uint8_t delta_brightness);
void backlight_decrease_brightness_selected_region(uint8_t delta_brightness);

void backlight_save_region_states();
void backlight_load_region_states();

#endif
