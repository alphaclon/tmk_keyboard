#ifndef _SECTOR_CONTROL_
#define _SECTOR_CONTROL_

#include "../issi/is31fl3733_rbg.h"
#include "sectors.h"

#define BRIGHTNESS_STEPS BACKLIGHT_LEVELS

void sector_control_init(void);

void sector_select(enum keyboard_sector sector);

void sector_toggle_selected(void);
void sector_set_selected_on(void);
void sector_set_selected_off(void);
void sector_set_all_off(void);

void sector_increase_brightness_selected(enum IS31FL3733_RGB_Color color);
void sector_decrease_brightness_selected(enum IS31FL3733_RGB_Color color);

void sector_increase_brightness_all(enum IS31FL3733_RGB_Color color);
void sector_decrease_brightness_all(enum IS31FL3733_RGB_Color color);

void sector_save_state(void);
void sector_load_state(void);
void sector_restore_state(void);

void sector_dump_state(void);

#endif
