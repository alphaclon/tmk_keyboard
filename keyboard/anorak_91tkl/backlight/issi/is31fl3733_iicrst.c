
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
    dprintf("iicrst (upper): %u\n", enabled);

    if (enabled)
    {
        // set IICRST pin to HIGH (PD4)
        DDRD |= (1 << 4);
        PORTD |= (1 << 4);
    }
    else
    {
        // set IICRST pin to LOW (PD4)
        DDRD |= (1 << 4);
        PORTD &= ~(1 << 4);
    }
}

void iic_reset_enable_lower(bool enabled)
{
    dprintf("iicrst (lower): %u\n", enabled);

    if (enabled)
    {
        // set IICRST pin to HIGH (PD5)
        DDRD |= (1 << 5);
        PORTD |= (1 << 5);
    }
    else
    {
        // set IICRST pin to LOW (PD5)
        DDRD |= (1 << 5);
        PORTD &= ~(1 << 5);
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
