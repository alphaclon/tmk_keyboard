
#ifndef SLEEP_LED_USE_COMMON

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "led.h"
#include "sleep_led.h"
#include "statusled_pwm.h"
#include <math.h>

#ifdef DEBUG
#include "debug.h"
#else
#include "nodebug.h"
#endif

#ifdef NOAVR
#define ISR(arg) void arg()
#endif

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
#define SLEEP_LED_TIMER_TOP F_CPU / (256 * 64)

/* Breathing Sleep LED brightness (PWM On period) table
 * (64[steps] * 4[duration]) / 64[PWM periods/s] = 4 second breath cycle
 *
 * http://www.wolframalpha.com/input/?i=%28sin%28+x%2F64*pi%29**8+*+255%2C+x%3D0+to+63
 * (0..63).each {|x| p ((sin(x/64.0*PI)**8)*255).to_i }
 */
static const uint8_t breathing_table[64] PROGMEM = {
    0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   2,   4,   6,   10,  15,  23,  32,  44,  58, 74,
    93, 113, 135, 157, 179, 199, 218, 233, 245, 252, 255, 252, 245, 233, 218, 199, 179, 157, 135, 113, 93, 74,
    58, 44,  32,  23,  15,  10,  6,   4,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0};


static uint8_t scaled_breathing_table[64];

void sleep_led_init(void)
{
	double max_brightness = get_capslock_led_brightness();
	for (uint8_t i = 0; i < 64; i++)
	{
		double brightness = pgm_read_byte(&breathing_table[i]);
		brightness /= max_brightness;
		scaled_breathing_table[i] = 255 - (uint8_t)round(brightness);

		/*
		uint8_t brightness = pgm_read_byte(&breathing_table[i]);
		brightness /= max_brightness;
		scaled_breathing_table[i] = 255 - brightness;
		*/
	}

    /* Timer3 setup */
    /* CTC mode */
    TCCR3B |= _BV(WGM32);
    /* Clock selelct: clk/1 */
    TCCR3B |= _BV(CS30);
    /* Set TOP value */
    uint8_t sreg = SREG;
    cli();
    OCR3AH = (SLEEP_LED_TIMER_TOP >> 8) & 0xff;
    OCR3AL = SLEEP_LED_TIMER_TOP & 0xff;
    SREG = sreg;
}

void sleep_led_enable(void)
{
	set_capslock_led_enabled(true);
	set_capslock_led_pwm_value(255);

    /* Enable Compare Match Interrupt */
    TIMSK3 |= _BV(OCIE3A);
}

void sleep_led_disable(void)
{
    /* Disable Compare Match Interrupt */
    TIMSK3 &= ~_BV(OCIE3A);

	set_capslock_led_enabled(false);
	set_capslock_led_brightness(get_capslock_led_brightness());
}

ISR(TIMER3_COMPA_vect)
{
    /* Software PWM
     * timer:1111 1111 1111 1111
     *       \_____/\/ \_______/____  count(0-255)
     *          \    \______________  duration of step(4)
     *           \__________________  index of step table(0-63)
     */
    static union {
        uint16_t row;
        struct
        {
            uint8_t count : 8;
            uint8_t duration : 2;
            uint8_t index : 6;
        } pwm;
    } timer = {.row = 0};

    timer.row++;

    if (timer.pwm.count == 0)
    {
    	set_capslock_led_pwm_value(scaled_breathing_table[timer.pwm.index]);
    }
}

#endif
