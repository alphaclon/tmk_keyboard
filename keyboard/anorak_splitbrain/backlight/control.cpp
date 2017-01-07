
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
#if TWILIB == AVR315

    TWI_Master_Initialise();

#elif TWILIB == BUFFTW

    i2cInit();
    i2cSetBitrate(400);

#else

    i2c_init();

#endif

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
