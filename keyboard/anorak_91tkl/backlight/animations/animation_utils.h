#ifndef KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_

#include "matrix.h"
#include <inttypes.h>

struct _animation_interface
{
    uint8_t brightness;
    uint16_t delay_in_ms;
    uint32_t duration_in_ms;

    void (*animationStart)(void);
    void (*animationStop)(void);
    void (*animationLoop)(void);
    void (*animation_typematrix_row)(uint8_t row_number, matrix_row_t row);

    uint16_t loop_timer;
    uint32_t duration_timer;
};

typedef struct _animation_interface animation_interface;

extern animation_interface animation;

void animation_prepare(uint8_t animation_frame);
void animation_postpare(uint8_t animation_frame);

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_ANIMATIONS_ANIMATION_UTILS_H_ */
