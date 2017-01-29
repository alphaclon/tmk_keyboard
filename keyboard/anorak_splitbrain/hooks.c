
#include "backlight/backlight_kiibohd.h"
#include "debug.h"
#include "matrixdisplay/infodisplay.h"
#include "nfo_led.h"
#include "splitbrain.h"
#include "twi/twi_config.h"
#include "uart/uart.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

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
    splitbrain_init();
}

void hook_late_init(void)
{
    matrixcpu_init();
    backlight_setup();
    backlight_setup_finish();

    while (!uart_tx_buffer_empty())
        _delay_ms(50);
}
