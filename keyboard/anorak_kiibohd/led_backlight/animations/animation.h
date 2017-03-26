#ifndef KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_H_

#include "matrix.h"
#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ANIMATION_TYPE_O_MATIC 0
#define ANIMATION_TYPE_O_CIRCLES 1
#define ANIMATION_BREATHING 2
#define ANIMATION_SWEEP 3

#ifdef ANIMATION_SWEEP
#define ANIMATIONS_COUNT 4
#else
#define ANIMATIONS_COUNT 3
#endif

void set_animation_sweep(void);
void set_animation_type_o_matic(void);
void set_animation_breathing(void);
void set_animation_type_o_circles(void);

void animation_test(void);
void animation_toggle(void);
void start_animation(void);
void stop_animation(void);
void set_animation(uint8_t animation_number);
void start_next_animation(void);
void start_previous_animation(void);
void increase_animation_speed(void);
void decrease_animation_speed(void);
bool animation_is_running(void);

void animate(void);
void animation_typematrix_row(uint8_t row_number, matrix_row_t row);


#ifdef __cplusplus
}
#endif

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_H_ */
