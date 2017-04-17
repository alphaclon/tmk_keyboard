
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
        // set SDB pin to LOW (PE4)
        DDRE |= (1 << 4);
        PORTE &= ~(1 << 4);
    }
    else
    {
        // set SDB pin to HIGH (PE4)
        DDRE |= (1 << 4);
        PORTE |= (1 << 4);
    }
}

void sdb_hardware_enable_lower(bool enabled)
{
    dprintf("sdb_hardware_enable_lower: %u\n", enabled);

    if (enabled)
    {
        // set SDB pin to LOW (PE4)
        DDRE |= (1 << 4);
        PORTE &= ~(1 << 4);
    }
    else
    {
        // set SDB pin to HIGH (PE4)
        DDRE |= (1 << 4);
        PORTE |= (1 << 4);
    }
}
