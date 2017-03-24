#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "../nfo_led.h"
#include "control.h"
#include "debug.h"

extern "C"
{
#include "led.h"
#include "sleep_led.h"
}

#define SLEEP_LED_BANK 6

uint8_t breathing = 0;

void backlight_sleep_led_init(void)
{
	breathing = 0;
}

void backlight_sleep_led_on(void)
{
    //dprintf("backlight_sleep_led_on\n");
}

void backlight_sleep_led_off(void)
{
    //dprintf("backlight_sleep_led_off\n");
}

void backlight_sleep_led_enable(void)
{
	//dprintf("backlight_sleep_led_enable\n");
	breathing = 1;
	LedInfo1_Toggle();

	/*
	// breathing for current configuration
	issi.setAutoFramePlayConfig(0,1,40);
	issi.setAutoFramePlayMode(0);
	issi.setBreathConfig(7, 7, 2);
	issi.setBreathMode(true);

	// breathing for one LED
	issi.enableLed(14, 1, SLEEP_LED_BANK);
	issi.setLedBrightness(14, 255, SLEEP_LED_BANK);
	issi.setAutoFramePlayConfig(0,1,40);
	issi.setAutoFramePlayMode(SLEEP_LED_BANK);
	issi.setBreathConfig(7, 7, 2);
	issi.setBreathMode(true);
	*/

	issi.enableHardwareShutdown(true);

	/*
#if TWILIB == AVR315_SYNC || TWILIB == BUFFTW || TWILIB == I2CMAS
	if (issi.is_initialized())
		issi.enableSoftwareShutdown(true);
#endif
    */
}

void backlight_sleep_led_disable(void)
{
	//dprintf("backlight_sleep_led_disable\n");
	breathing = 0;

	/*
	issi.setBreathMode(false);
	issi.setPictureMode();
	issi.displayFrame(0);
	*/

	issi.enableHardwareShutdown(false);

	/*
#if TWILIB == AVR315_SYNC || TWILIB == BUFFTW || TWILIB == I2CMAS
	if (issi.is_initialized())
		issi.enableSoftwareShutdown(false);
#endif
	*/
}

void backlight_sleep_led_toggle(void)
{
	if (breathing)
		sleep_led_disable();
	else
		sleep_led_enable();
}

#ifdef SLEEP_LED_ENABLE
#ifdef __cplusplus
extern "C" {
#endif
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

void sleep_led_on(void)
{
    backlight_sleep_led_on();
}

void sleep_led_off(void)
{
    backlight_sleep_led_off();
}
#ifdef __cplusplus
}
#endif
#endif
