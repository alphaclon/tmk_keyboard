#ifndef KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_FLYING_BALL_H_
#define KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_FLYING_BALL_H_

#include "matrix.h"

void set_animation_flying_ball(void);

void flying_ball_animation_start(void);
void flying_ball_animation_loop(void);
void flying_ball_animation_stop(void);
void flying_ball_typematrix_row(uint8_t row_number, matrix_row_t row);

#endif /* KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_FLYING_BALL_H_ */
