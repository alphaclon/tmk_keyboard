
#ifndef KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_COLOR_CYCLE_UP_DOWN_H_
#define KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_COLOR_CYCLE_UP_DOWN_H_

#include "matrix.h"

void set_animation_color_cycle_up_down(void);

void color_cycle_up_down_animation_start(void);
void color_cycle_up_down_animation_loop(void);
void color_cycle_up_down_animation_stop(void);
void color_cycle_up_down_typematrix_row(uint8_t row_number, matrix_row_t row);

#endif /* KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_COLOR_CYCLE_LEFT_RIGHT_H_ */
