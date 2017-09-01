#ifndef KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_

#include "matrix.h"
#include <inttypes.h>

#define FPS_TO_DELAY(fps) (1000/fps)

struct _animation_interface
{
    uint8_t brightness;
    uint16_t delay_in_ms;
    uint32_t duration_in_ms;

    uint16_t loop_timer;
    uint32_t duration_timer;

    bool is_running;
    bool is_suspended;

    void (*animationStart)(void);
    void (*animationStop)(void);
    void (*animationLoop)(void);
    void (*animation_typematrix_row)(uint8_t row_number, matrix_row_t row);
};

typedef struct _animation_interface animation_interface;

extern animation_interface animation;

void animation_prepare(uint8_t animation_frame);
void animation_postpare(uint8_t animation_frame);

void animation_default_animation_start(void);
void animation_default_animation_stop(void);

void animation_default_typematrix_row(uint8_t row_number, matrix_row_t row);

uint8_t key_was_pressed(uint8_t key_row, uint8_t key_col);

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_ */
