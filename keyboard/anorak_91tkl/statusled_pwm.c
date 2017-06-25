
#include "statusled_pwm.h"

#ifdef STATUS_LED_PWM_ENABLED

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
	// http://www.societyofrobots.com/member_tutorials/book/export/html/228
	// https://sites.google.com/site/qeewiki/books/avr-guide/pwm-on-the-atmega328
	//
	// PWM wave frequency is f= (16000000/prescaler)/(2*TOP)
	// prescaler 8, 8bit: (16000000/8)/(2*256) = 3906Hz
	// CS11 = 8
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
	if (value < 255 && value > 0)
	{
		OCR1A = value;
	}
}

void set_scrolllock_led_brightness(uint8_t value)
{
	if (value == 255)
		scrolllock_led_pwm = 1;
	else if (value == 0)
		scrolllock_led_pwm = 254;
	else
		scrolllock_led_pwm = 255 - value;

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
		//OCR1A = pwm_value_scroll_lock_led;
		TCCR1A |= (1 << COM1A1);
		DDRB |= (1 << PB5);
	}
	else
	{
		// PWM channel B off!
		TCCR1A &= ~(1 << COM1A1);
		DDRB &= ~(1 << PB5);
	}
}

void set_capslock_led_pwm_value(uint8_t value)
{
	if (value < 255 && value > 0)
	{
		OCR1B = value;
	}
}

void set_capslock_led_brightness(uint8_t value)
{
	if (value == 255)
		capslock_led_pwm = 1;
	else if (value == 0)
		capslock_led_pwm = 254;
	else
		capslock_led_pwm = 255 - value;

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
		//OCR1B = pwm_value_caps_lock_led;
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

