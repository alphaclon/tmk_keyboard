#define SLEEP_LED_ENABLE

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "control.h"
#include "led.h"
#include "sleep_led.h"

#ifdef SLEEP_LED_ENABLE

uint8_t breathing = 0;

void sleep_led_init(void)
{
	issi.setBreathConfig(7, 7, 7);
}

void sleep_led_enable(void)
{
	breathing = 1;
	issi.setBreathMode(1);
}

void sleep_led_disable(void)
{
	breathing = 0;
	issi.setBreathMode(0);
}

void sleep_led_toggle(void)
{
	if (breathing)
		sleep_led_disable();
	else
		sleep_led_enable();
}

#endif
