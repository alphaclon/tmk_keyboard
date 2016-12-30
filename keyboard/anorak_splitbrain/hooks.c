
#include "backlight/backlight_kiibohd.h"
#include "debug.h"
#include "infodisplay.h"
#include "splitbrain.h"
#include <avr/interrupt.h>

void hook_early_init(void)
{
    splitbrain_config_init();
    matrixcpu_init();
    backlight_setup();
}

void hook_late_init(void)
{
    backlight_setup_finish();
}
