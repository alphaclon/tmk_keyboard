
#if 0

#include "../backlight/issi/is31fl3733_rgb_91tkl.h"
#include "debug.h"
#include "sleep_led.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

uint8_t breathing = 0;

void backlight_sleep_led_init(void)
{
    dprintf("backlight_sleep_led_init\n");
    breathing = 0;
}

void backlight_sleep_led_on(void)
{
    dprintf("backlight_sleep_led_on\n");
}

void backlight_sleep_led_off(void)
{
    dprintf("backlight_sleep_led_off\n");
}

void backlight_sleep_led_enable(void)
{
    dprintf("backlight_sleep_led_enable\n");
    breathing = 1;

    if (IS31FL3731_RGB_91TKL_Initialized())
    {
    	IS31FL3733_Software_Shutdown(issi.device1->device, true);
    	IS31FL3733_Software_Shutdown(issi.device2->device, true);
    }
}

void backlight_sleep_led_disable(void)
{
    dprintf("backlight_sleep_led_disable\n");
    breathing = 0;

    if (IS31FL3731_RGB_91TKL_Initialized())
    {
    	IS31FL3733_Software_Shutdown(issi.device1->device, false);
    	IS31FL3733_Software_Shutdown(issi.device2->device, false);
    }
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


#endif
