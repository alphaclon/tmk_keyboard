
#include "backlight/backlight_kiibohd.h"
#include "debug.h"
#include "infodisplay.h"
#include "splitbrain.h"

void hook_early_init(void)
{
    dprintf("hook_early_init\r\n");
    splitbrain_config_init();
    matrixcpu_init();
    backlight_setup();
}

void hook_late_init(void)
{
    dprintf("hook_late_init\r\n");
    backlight_setup_finish();

    dprintf("splitbrain: %c\r\n", this_side_as_char());
}
