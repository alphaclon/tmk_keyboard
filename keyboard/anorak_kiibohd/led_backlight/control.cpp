
#include "control.h"
#include "twi_config.h"
#include <avr/interrupt.h>

IS31FL3731Buffered issi;

void IS31FL3731_init()
{
    issi.begin();
}
