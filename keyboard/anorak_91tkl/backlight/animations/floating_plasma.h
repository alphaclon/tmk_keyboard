
#ifndef KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_FLOATING_PLASMA_H_
#define KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_FLOATING_PLASMA_H_

#include "animation_options.h"
#include "animation_enable.h"
#include <inttypes.h>

#ifdef ANIMATION_ENABLE_FLOATING_PLASMA

void set_animation_floating_plasma(void);

void set_animation_option_floating_plasma(animation_options option);
animation_options get_animation_option_floating_plasma(void);
uint16_t get_animation_options_floating_plasma(void);

#endif
#endif /* KEYBOARD_ANORAK_91TKL_BACKLIGHT_ANIMATIONS_FLOATING_PLASMA_H_ */
