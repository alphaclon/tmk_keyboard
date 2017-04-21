
#ifndef KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_TYPE_O_CIRCLES_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_TYPE_O_CIRCLES_H_

#include "matrix.h"

void set_animation_type_o_circles(void);

void type_o_circles_animation_start(void);
void type_o_circles_animation_loop(void);
void type_o_circles_animation_stop(void);
void type_o_circles_typematrix_row(uint8_t row_number, matrix_row_t row);

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_TYPE_O_CIRCLES_H_ */
