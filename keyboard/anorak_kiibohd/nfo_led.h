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
#define LED_GREEN_INIT()  do { DDRD |= (1<<5); PORTD |= (1<<5); } while (0)
#define LED_GREEN_ON()    do { PORTD &= ~(1<<5); } while (0)
#define LED_GREEN_OFF()   do { PORTD |= (1<<5); } while (0)
#define LED_GREEN_TGL()   do { PIND |= (1<<5); } while (0)

#define LED_YELLOW_INIT()  do { DDRC |= (1<<7); PORTC &= ~(1<<7);} while (0)
#define LED_YELLOW_ON()    do { PORTC |= (1<<7);} while (0)
#define LED_YELLOW_OFF()   do { PORTC &= ~(1<<7); } while (0)
#define LED_YELLOW_TGL()   do { PIND |= (1<<7); } while (0)
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
