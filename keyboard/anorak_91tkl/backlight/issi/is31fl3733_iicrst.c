
#include "is31fl3733_iicrst.h"
#include <avr/io.h>
#include <util/delay.h>

#ifdef DEBUG_ISSI
#include "debug.h"
#else
#include "nodebug.h"
#endif

void iic_reset_enable_upper(bool enabled)
{
    dprintf("iic_reset_enable_upper: %u\n", enabled);

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

void iic_reset_enable_lower(bool enabled)
{
    dprintf("iic_reset_enable_lower: %u\n", enabled);

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

void iic_reset_upper()
{
	iic_reset_enable_upper(true);
	_delay_ms(1);
	iic_reset_enable_upper(false);
}

void iic_reset_lower()
{
	iic_reset_enable_lower(true);
	_delay_ms(1);
	iic_reset_enable_lower(false);
}
