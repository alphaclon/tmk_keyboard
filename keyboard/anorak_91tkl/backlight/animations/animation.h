#ifndef KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_H_

#include "matrix.h"
#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum animation_names_t
{
    animation_sweep = 0,
    animation_type_o_matic,
    animation_type_o_raindrops,
    animation_type_o_circles,
    animation_raindrops,
    animation_jellybean_raindrops,
    animation_cycle_all,
    animation_cycle_up_down,
    animation_cycle_left_right,
    animation_breathing,
    animation_LAST
};

typedef enum animation_names_t animation_names;

enum animation_hsv_names_t
{
	animation_hsv_1,
	animation_hsv_2
};

typedef enum animation_hsv_names_t animation_hsv_names;

void initialize_animation(void);
void animation_save_state(void);

void start_animation(void);
void stop_animation(void);
void toggle_animation(void);
void set_animation(animation_names animation_by_name);

void animation_next(void);
void animation_previous(void);
void animation_set_speed(uint16_t delay_in_ms);
void animation_increase_speed(void);
void animation_decrease_speed(void);
bool animation_is_running(void);

animation_names animation_current(void);

void animation_decrease_hsv_color(animation_hsv_names hsv_name, HSVColorName color_name);
void animation_increase_hsv_color(animation_hsv_names hsv_name, HSVColorName color_name);

void animate(void);
void animation_typematrix_row(uint8_t row_number, matrix_row_t row);

#ifdef __cplusplus
}
#endif

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_H_ */
