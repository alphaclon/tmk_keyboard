#pragma once

#include "config.h"

/*
 * PE6 -> grüne LED, active low
 * PB7 -> gelbe LED, active low
 *
 *
 */

#ifndef NO_DEBUG_LEDS
#define LED_GREEN_INIT()   DDRE |= (1<<6); PORTE |= (1<<6)
#define LED_GREEN_ON()     PORTE &= ~(1<<6)
#define LED_GREEN_OFF()    PORTE |= (1<<6)
#define LED_GREEN_TGL()    PORTE ^= (1<<7)

#define LED_YELLOW_INIT()  DDRB |= (1<<7); PORTB |= (1<<7)
#define LED_YELLOW_ON()    PORTB &= ~(1<<7)
#define LED_YELLOW_OFF()   PORTB |= (1<<7)
#define LED_YELLOW_TGL()   PORTB ^= (1<<7)
#else
#define LED_GREEN_INIT()  do { } while (0)
#define LED_GREEN_ON()    do { } while (0)
#define LED_GREEN_OFF()   do { } while (0)
#define LED_GREEN_TGL()   do { } while (0)

#define LED_YELLOW_INIT()  do { } while (0)
#define LED_YELLOW_ON()    do { } while (0)
#define LED_YELLOW_OFF()   do { } while (0)
#define LED_YELLOW_TGL()   do { } while (0)
#endif
