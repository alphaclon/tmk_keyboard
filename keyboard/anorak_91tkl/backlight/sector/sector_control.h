#ifndef _SECTOR_CONTROL_
#define _SECTOR_CONTROL_

#include "../issi/is31fl3733_rgb.h"
#include "sectors.h"

#define SECTOR_CONTROL_LEVEL_MODE 0xff

void sector_control_init(void);

void sector_select(keyboard_sector sector);

void sector_toggle_selected(void);
void sector_set_selected_on(void);
void sector_set_selected_off(void);
void sector_set_all_off(void);

void sector_selected_increase_color(RGBColorName color);
void sector_selected_decrease_color(RGBColorName color);

void sector_increase_color(RGBColorName color);
void sector_decrease_color(RGBColorName color);

void sector_selected_increase_hsv_color(HSVColorName color);
void sector_selected_decrease_hsv_color(HSVColorName color);

void sector_increase_hsv_color(HSVColorName color);
void sector_decrease_hsv_color(HSVColorName color);

void sector_save_state(void);
void sector_load_state(void);
void sector_restore_state(void);

void sector_control_set_mode(uint8_t custom_map);

void sector_dump_state(void);
void sector_dump_mask(uint8_t *mask);

#endif
