
#include "led_backlight/twi_config.h"
#include "led_backlight/backlight_kiibohd.h"

void i2c_init(void)
{
#if TWILIB == AVR315

    TWI_Master_Initialise();

#elif TWILIB == BUFFTW

    i2cInit();
    i2cSetBitrate(400);

#else

    i2c_init();

#endif
}

void hook_early_init(void)
{
    i2c_init();
}

void hook_late_init(void)
{
    backlight_setup();
    backlight_setup_finish();
}
