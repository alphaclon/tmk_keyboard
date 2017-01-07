
#include "backlight/backlight_kiibohd.h"
#include "debug.h"
#include "matrixdisplay/infodisplay.h"
#include "splitbrain.h"
#include <avr/interrupt.h>

#include "nfo_led.h"
#include "uart/uart.h"
#include <stdio.h>
#include <util/delay.h>

void hook_early_init(void)
{
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
