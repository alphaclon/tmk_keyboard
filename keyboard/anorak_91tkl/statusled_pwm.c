
#include "statusled_pwm.h"

#ifdef STATUS_LED_PWM_ENABLE

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>

static uint8_t scrolllock_led_pwm = EECONFIG_STATUSLED_DEFAULT_BRIGHTNESS;
static uint8_t capslock_led_pwm = EECONFIG_STATUSLED_DEFAULT_BRIGHTNESS;

void statusled_pwm_init()
{
	if (!eeconfig_statusled_brightness_is_enabled())
		eeconfig_statusled_brightness_init();

	scrolllock_led_pwm = eeconfig_read_scrolllock_led_brightness();
	capslock_led_pwm = eeconfig_read_capslock_led_brightness();

	OCR1A = 0;
	OCR1B = 0;

	cli();

	// Tutorial:
	// https://sites.google.com/site/qeewiki/books/avr-guide/pwm-on-the-atmega328
	//
	// Fast PWM mode with variable TOP, 8bit
	//
	// COM1A1 = 1, COM1A0 = 0: None-inverted mode (HIGH at bottom, LOW on Match)
	//
	// PWM_fequency = clock_speed / [Prescaller_value * (1 + TOP_Value) ]
	//
	// CS11 = 1: prescaler 8
	// Fast PWM, 8bit: WGM12 | WGM10

	OCR1A = scrolllock_led_pwm;
	OCR1B = capslock_led_pwm;

	TCCR1A = (1 << COM1A1) | (1 << COM1B1); // set none-inverting mode

	TCCR1A |= (1 << WGM10);  // Mode 5: Fast PWM, 8bit
	TCCR1B = (1 << WGM12);

	TCCR1B |= (1 << CS11);   // prescaler 8

	//enable global interrupts
	sei();

	DDRB |= (1 << PB5) | (1 << PB6);
}

void save_status_led_values()
{
	eeconfig_write_scrolllock_led_brightness(scrolllock_led_pwm);
	eeconfig_write_capslock_led_brightness(capslock_led_pwm);
}

void set_scroll_lock_led_pwm_value(uint8_t value)
{
	OCR1A = value;
}

uint8_t rescale_pwm_value(uint8_t value)
{
	uint8_t led_pwm;

    if (value == 255)
        led_pwm = 1;
    else if (value == 0)
        led_pwm = 254;
    else
        led_pwm = 255 - value;

    return led_pwm;
}

void set_scrolllock_led_brightness(uint8_t value)
{
	scrolllock_led_pwm = rescale_pwm_value(value);
    set_scroll_lock_led_pwm_value(scrolllock_led_pwm);
}

uint8_t get_scrolllock_led_brightness()
{
	return 255 - scrolllock_led_pwm;
}

void set_scrolllock_led_enabled(bool enabled)
{
	if (enabled)
	{
		TCCR1A |= (1 << COM1A1);
		DDRB |= (1 << PB5);
	}
	else
	{
		// PWM channel A off!
		TCCR1A &= ~(1 << COM1A1);
		DDRB &= ~(1 << PB5);
	}
}

void set_capslock_led_pwm_value(uint8_t value)
{
	OCR1B = value;
}

void set_capslock_led_brightness(uint8_t value)
{
	capslock_led_pwm = rescale_pwm_value(value);
	set_capslock_led_pwm_value(capslock_led_pwm);
}

uint8_t get_capslock_led_brightness()
{
	return 255 - capslock_led_pwm;
}

void set_capslock_led_enabled(bool enabled)
{
	if (enabled)
	{
		TCCR1A |= (1 << COM1B1);
		DDRB |= (1 << PB6);
	}
	else
	{
		// PWM channel B off!
		TCCR1A &= ~(1 << COM1B1);
		DDRB &= ~(1 << PB6);
	}
}

#endif

