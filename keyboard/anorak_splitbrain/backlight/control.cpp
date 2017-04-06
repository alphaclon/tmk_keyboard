
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
}

void IS31FL3731_enableHardwareShutdown(bool enabled)
{
    issi.enableHardwareShutdown(enabled);
}
