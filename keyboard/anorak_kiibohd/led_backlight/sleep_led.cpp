#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "control.h"
#include "debug.h"

extern "C"
{
#include "led.h"
#include "sleep_led.h"
}

uint8_t breathing = 0;

void backlight_sleep_led_init(void)
{
	breathing = 0;
}

void backlight_sleep_led_enable(void)
{
	//dprintf("sl_en\n");
	breathing = 1;
	issi.setAutoFramePlayConfig(0,1,40);
	issi.setAutoFramePlayMode(0);
	issi.setBreathConfig(7, 7, 2);
	issi.setBreathMode(1);
}

void backlight_sleep_led_disable(void)
{
	//dprintf("sl_di\n");
	breathing = 0;
	issi.setPictureMode();
	issi.setBreathMode(0);
	issi.displayFrame(0);
}

void backlight_sleep_led_toggle(void)
{
	if (breathing)
		sleep_led_disable();
	else
		sleep_led_enable();
}

#ifdef SLEEP_LED_ENABLE
extern "C"
{
void sleep_led_init(void)
{
	backlight_sleep_led_init();
}

void sleep_led_enable(void)
{
	backlight_sleep_led_enable();
}

void sleep_led_disable(void)
{
	backlight_sleep_led_disable();
}

void sleep_led_toggle(void)
{
	backlight_sleep_led_toggle();
}
}
#endif
