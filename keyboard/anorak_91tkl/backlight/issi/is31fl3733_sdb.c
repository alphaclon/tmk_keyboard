
#include "is31fl3733_sdb.h"
#include <avr/io.h>

#ifdef DEBUG_ISSI
#include "debug.h"
#else
#include "nodebug.h"
#endif

void sdb_hardware_enable_upper(bool enabled)
{
    dprintf("sdb_hardware_enable_upper: %u\n", enabled);

    if (enabled)
    {
        // set SDB pin to LOW (PD6)
        DDRD |= (1 << 6);
        PORTD &= ~(1 << 6);
    }
    else
    {
        // set SDB pin to HIGH (PD6)
        DDRD |= (1 << 6);
        PORTD |= (1 << 6);
    }
}

void sdb_hardware_enable_lower(bool enabled)
{
    dprintf("sdb_hardware_enable_lower: %u\n", enabled);

    if (enabled)
    {
        // set SDB pin to LOW (PD7)
        DDRD |= (1 << 7);
        PORTD &= ~(1 << 7);
    }
    else
    {
        // set SDB pin to HIGH (PE4)
        DDRD |= (1 << 7);
        PORTD |= (1 << 7);
    }
}
