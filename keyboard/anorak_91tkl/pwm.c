/*
 * Copyright (c) 2009 by Christian Dietrich <stettberger@dokucode.de>
 * Copyright (c) 2010 by Stefan Riepenhausen <rhn@gmx.net>
 * 20030116 - 1.0 - Created- LHM
 * Copyright (c) 2003, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "pwm.h"

static uint8_t pwm_value_scroll_lock_led = 240;
static uint8_t pwm_value_caps_lock_led = 240;

#define FRAME_TIME 20

void pwm_init()
{
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

	OCR1A = pwm_value_scroll_lock_led;
	OCR1B = pwm_value_caps_lock_led;

	/*
	TCCR1A = (1 << COM1A1) | (1 << COM1B1); // set none-inverting mode

	TCCR1A = (1 << WGM10);  // Mode 5: Fast PWM, 8bit
	TCCR1B = (1 << WGM12);

	TCCR1B = (1 << CS11);   // prescaler 8
	*/

	TCCR1A = (1 << COM1A1) | (1 << COM1B1); // set none-inverting mode
	TCCR1A |= (1 << WGM10);  // Mode 5: Fast PWM, 8bit

	TCCR1B = (1 << WGM12);
	TCCR1B |= (1 << CS11);   // prescaler 8

	//enable global interrupts
	sei();

	DDRB |= (1 << PB5) | (1 << PB6);
}

void pwm_scroll_lock_led_value(uint8_t value)
{
	if (value < 255 && value > 0)
	{
		OCR1A = value;
	}
}

void pwm_scroll_lock_led(uint8_t value)
{
	pwm_value_scroll_lock_led = value;
	pwm_scroll_lock_led_value(value);
}

uint8_t get_pwm_scroll_lock_led()
{
	return pwm_value_scroll_lock_led;
}

void pwm_scroll_lock_led_enabled(bool enabled)
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

void pwm_caps_lock_led_value(uint8_t value)
{
	if (value < 255 && value > 0)
	{
		OCR1B = value;
	}
}

void pwm_caps_lock_led(uint8_t value)
{
	pwm_value_caps_lock_led = value;
	pwm_caps_lock_led_value(value);
}

uint8_t get_pwm_caps_lock_led()
{
	return pwm_value_caps_lock_led;
}

void pwm_caps_lock_led_enabled(bool enabled)
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
