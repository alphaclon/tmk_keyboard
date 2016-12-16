#pragma once

#include "config.h"

/** Info-LEDs
 *
 * PE6 -> grüne LED, active low
 * PB7 -> gelbe LED, active low
 *
 */

#ifndef NO_DEBUG_LEDS
#define LedInfo1_Init()   DDRE |= (1<<6); PORTE |= (1<<6)
#define LedInfo1_On()     PORTE &= ~(1<<6)
#define LedInfo1_Off()    PORTE |= (1<<6)
#define LedInfo1_Toggle() PORTE ^= (1<<7)

#define LedInfo2_Init()   DDRB |= (1<<7); PORTB |= (1<<7)
#define LedInfo2_On()     PORTB &= ~(1<<7)
#define LedInfo2_Off()    PORTB |= (1 << 7)
#define LedInfo2_Toggle() PORTB ^= (1 << 7)
#else
#define LedInfo1_Init()     do { } while (0)
#define LedInfo1_On()       do { } while (0)
#define LedInfo1_Off()      do { } while (0)
#define LedInfo1_Toggle()   do { } while (0)

#define LedInfo2_Init()     do { } while (0)
#define LedInfo2_On()       do { } while (0)
#define LedInfo2_Off()      do { } while (0)
#define LED_YELLOW_TGL()    do { } while (0)
#endif
