#ifndef _BACKLIGHT_KIIBOHD_
#define _BACKLIGHT_KIIBOHD_

#include <inttypes.h>

#define BACKLIGHT_WASD 0
#define BACKLIGHT_CONTROLS 1
#define BACKLIGHT_CURSOR 2
#define BACKLIGHT_OTHER 3
#define BACKLIGHT_ALL 4

#define BACKLIGHT_BV(arg) (1 << arg)


enum backlight_region
{
    backlight_region_WASD = BACKLIGHT_BV(BACKLIGHT_WASD),
    backlight_region_controls = BACKLIGHT_BV(BACKLIGHT_CONTROLS),
    backlight_region_cursor = BACKLIGHT_BV(BACKLIGHT_CURSOR),
    backlight_region_other = BACKLIGHT_BV(BACKLIGHT_OTHER),
	backlight_region_ALL = BACKLIGHT_BV(BACKLIGHT_ALL),
};

#define BACKLIGHT_MAX_REGIONS BACKLIGHT_ALL


#ifdef __cplusplus
extern "C" {
#endif

void backlight_setup(void);
void backlight_setup_finish(void);

void backlight_set_brightness(uint8_t brightness);
void backlight_set_brightness_for_region(uint8_t region, uint8_t brightness);
uint8_t backlight_get_brightness_for_region(uint8_t region);

void backlight_increase_brightness(void);
void backlight_decrease_brightness(void);

void backlight_toggle_region(uint8_t region);
void backlight_disable_region(uint8_t region);
void backlight_enable_region(uint8_t region);
bool backlight_is_region_enabled(uint8_t region);

void backlight_select_region(uint8_t region);
void backlight_toggle_selected_region(void);
void backlight_selected_region_on(void);
void backlight_selected_region_off(void);

void backlight_increase_brightness_selected_region(void);
void backlight_decrease_brightness_selected_region(void);

void backlight_save_region_states(void);
void backlight_load_region_states(void);

void backlight_enableShutdown(bool enabled);

void backlight_dump_issi_state(void);

#ifdef __cplusplus
}
#endif

#endif
