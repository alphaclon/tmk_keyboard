#pragma once

#include "config.h"

/*
 * PD5 -> INFO LED, active low
 */

#ifndef NO_DEBUG_LEDS
#define LED_NFO_INIT()                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        DDRD |= (1 << 5);                                                                                              \
        PORTD |= (1 << 5);                                                                                             \
    } while (0)
#define LED_NFO_ON()                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        PORTD &= ~(1 << 5);                                                                                            \
    } while (0)
#define LED_NFO_OFF()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        PORTD |= (1 << 5);                                                                                             \
    } while (0)
#define LED_NFO_TGL()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        PIND |= (1 << 5);                                                                                              \
    } while (0)
#else
#define LED_NFO_INIT()                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define LED_NFO_ON()                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define LED_NFO_OFF()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define LED_NFO_TGL()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif


