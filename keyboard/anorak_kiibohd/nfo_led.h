#pragma once

/*
 * PD5 -> grüne LED, active low
 * PC7 -> gelbe LED, active high
 *
 * #define LED_CONFIG      (DDRD |= (1<<6))
 * #define LED_ON          (PORTD &= ~(1<<6))
 * #define LED_OFF         (PORTD |= (1<<6))
 *
 *
 */

#ifndef NO_DEBUG_LEDS
#define LedInfo1_Init()  do { DDRD |= (1<<5); PORTD |= (1<<5); } while (0)
#define LedInfo1_On()    do { PORTD &= ~(1<<5); } while (0)
#define LedInfo1_Off()   do { PORTD |= (1<<5); } while (0)
#define LedInfo1_Toggle()   do { PIND |= (1<<5); } while (0)

#define LedInfo2_Init()  do { DDRC |= (1<<7); PORTC &= ~(1<<7);} while (0)
#define LedInfo2_On()    do { PORTC |= (1<<7);} while (0)
#define LedInfo2_Off()   do { PORTC &= ~(1<<7); } while (0)
#define LED_YELLOW_TGL()   do { PIND |= (1<<7); } while (0)
#else
#define LedInfo1_Init()  do { } while (0)
#define LedInfo1_On()    do { } while (0)
#define LedInfo1_Off()   do { } while (0)
#define LedInfo1_Toggle()   do { } while (0)

#define LedInfo2_Init()  do { } while (0)
#define LedInfo2_On()    do { } while (0)
#define LedInfo2_Off()   do { } while (0)
#define LED_YELLOW_TGL()   do { } while (0)
#endif
