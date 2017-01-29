
#include "control.h"
#include "../twi/twi_config.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#ifdef DEBUG_BACKLIGHT
#include "debug.h"
#else
#include "nodebug.h"
#endif

IS31FL3731Buffered issi;

void IS31FL3731_init()
{
    issi.begin();

#if TWILIB == AVR315 && defined(DEBUG_BACKLIGHT)
    issi.dumpConfiguration();
    issi.dumpLeds(0);

    dprintf("I2C: wait\r\n");
    while (TWI_Transceiver_Busy())
    {
        _delay_ms(1);
    }
    dprintf("I2C: wait done\r\n");
#endif
}
