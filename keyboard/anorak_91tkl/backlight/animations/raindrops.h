
#ifndef KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_RAINDROPS_H_
#define KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_RAINDROPS_H_

#include "matrix.h"

void set_animation_raindrops(void);

void raindrops_animation_start(void);
void raindrops_animation_loop(void);
void raindrops_animation_stop(void);
void raindrops_typematrix_row(uint8_t row_number, matrix_row_t row);

#endif /* KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_RAINDROPS_H_ */
