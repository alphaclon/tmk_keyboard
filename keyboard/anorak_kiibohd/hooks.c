
#include "led_backlight/backlight_kiibohd.h"

void hook_early_init(void)
{
	backlight_setup();
}

void hook_late_init(void)
{
	backlight_setup_finish();
}
