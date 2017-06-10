#ifndef _SECTOR_CONTROL_
#define _SECTOR_CONTROL_

#include "../issi/is31fl3733_rgb.h"
#include "sectors.h"
#include <inttypes.h>

#define SECTOR_CONTROL_LEVEL_MODE 0xff

void sector_control_init(void);

void sector_select(KeyboardSector sector);
bool sector_is_enabled(KeyboardSector sector);

void sector_toggle_selected(void);
void sector_set_selected_on(void);
void sector_set_selected_off(void);
void sector_set_selected(bool on);
void sector_set_all_off(void);

void sector_selected_set_hsv_color(HSV color);
void sector_set_hsv_color(KeyboardSector sector, HSV color);
HSV sector_get_hsv_color(KeyboardSector sector);

void sector_all_increase_hsv_color(HSVColorName color);
void sector_selected_increase_hsv_color(HSVColorName color);

void sector_all_decrease_hsv_color(HSVColorName color);
void sector_selected_decrease_hsv_color(HSVColorName color);

void sector_save_state(void);
void sector_load_state(void);
void sector_restore_state(void);

void sector_set_sector_mode(void);
void sector_set_custom_map(uint8_t custom_map);
uint8_t sector_get_custom_map(void);

void sector_next_custom_map(void);

void sector_save_custom_pwm_map(void);

void sector_dump_state(void);
void sector_dump_mask(uint8_t *mask);

void sector_enable_all_leds(void);

#endif
