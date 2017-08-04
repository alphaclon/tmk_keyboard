#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "led.h"
#include "sleep_led.h"

/* Software PWM
 *  ______           ______           __
 * |  ON  |___OFF___|  ON  |___OFF___|   ....
 * |<-------------->|<-------------->|<- ....
 *     PWM period       PWM period
 *
 * 256              interrupts/period[resolution]
 * 64               periods/second[frequency]
 * 256*64           interrupts/second
 * F_CPU/(256*64)   clocks/interrupt
 */
#define SLEEP_LED_TIMER_TOP F_CPU/(256*64)

void sleep_led_init(void)  __attribute__ ((weak));
void sleep_led_init(void)
{
#ifdef SLEEP_LED_TIMER_1
    /* Timer1 setup */
    /* CTC mode */
    TCCR1B |= _BV(WGM12);
    /* Clock selelct: clk/1 */
    TCCR1B |= _BV(CS10);
    /* Set TOP value */
    uint8_t sreg = SREG;
    cli();
    OCR1AH = (SLEEP_LED_TIMER_TOP>>8)&0xff;
    OCR1AL = SLEEP_LED_TIMER_TOP&0xff;
    SREG = sreg;
#else
    /* Timer3 setup */
    /* CTC mode */
    TCCR3B |= _BV(WGM32);
    /* Clock selelct: clk/1 */
    TCCR3B |= _BV(CS30);
    /* Set TOP value */
    uint8_t sreg = SREG;
    cli();
    OCR3AH = (SLEEP_LED_TIMER_TOP>>8)&0xff;
    OCR3AL = SLEEP_LED_TIMER_TOP&0xff;
    SREG = sreg;
#endif
}

void sleep_led_enable(void)  __attribute__ ((weak));
void sleep_led_enable(void)
{
#ifdef SLEEP_LED_TIMER_1
    /* Enable Compare Match Interrupt */
    TIMSK1 |= _BV(OCIE1A);
#else
    /* Enable Compare Match Interrupt */
    TIMSK3 |= _BV(OCIE3A);
#endif
}

void sleep_led_disable(void)  __attribute__ ((weak));
void sleep_led_disable(void)
{
#ifdef SLEEP_LED_TIMER_1
    /* Disable Compare Match Interrupt */
    TIMSK1 &= ~_BV(OCIE1A);
#else
    /* Disable Compare Match Interrupt */
    TIMSK3 &= ~_BV(OCIE3A);
#endif
}

__attribute__ ((weak))
void sleep_led_on(void)
{
    led_set(1<<USB_LED_CAPS_LOCK);
}

__attribute__ ((weak))
void sleep_led_off(void)
{
    led_set(0);
}


#ifdef SLEEP_LED_USE_COMMON

/* Breathing Sleep LED brightness (PWM On period) table
 * (64[steps] * 4[duration]) / 64[PWM periods/s] = 4 second breath cycle
 *
 * http://www.wolframalpha.com/input/?i=%28sin%28+x%2F64*pi%29**8+*+255%2C+x%3D0+to+63
 * (0..63).each {|x| p ((sin(x/64.0*PI)**8)*255).to_i }
 */
static const uint8_t breathing_table[64] PROGMEM = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 6, 10,
15, 23, 32, 44, 58, 74, 93, 113, 135, 157, 179, 199, 218, 233, 245, 252,
255, 252, 245, 233, 218, 199, 179, 157, 135, 113, 93, 74, 58, 44, 32, 23,
15, 10, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#ifdef SLEEP_LED_TIMER_1
ISR(TIMER1_COMPA_vect)
#else
ISR(TIMER3_COMPA_vect)
#endif
{
    /* Software PWM
     * timer:1111 1111 1111 1111
     *       \_____/\/ \_______/____  count(0-255)
     *          \    \______________  duration of step(4)
     *           \__________________  index of step table(0-63)
     */
    static union {
        uint16_t row;
        struct {
            uint8_t count:8;
            uint8_t duration:2;
            uint8_t index:6;
        } pwm;
    } timer = { .row = 0 };

    timer.row++;

    // LED on
    if (timer.pwm.count == 0) {
        sleep_led_on();
    }
    // LED off
    if (timer.pwm.count == pgm_read_byte(&breathing_table[timer.pwm.index])) {
        sleep_led_off();
    }
}

#endif


