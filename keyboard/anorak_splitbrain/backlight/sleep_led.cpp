
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "../backlight/control.h"

extern "C" {
#include "../matrixdisplay/infodisplay.h"
#include "../splitbrain.h"
#include "led.h"
#include "sleep_led.h"
}

#ifdef DEBUG_SLEEP_LED
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define SLEEP_LED_BANK 6

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

    /*
    issi.enableLed(14, 1, SLEEP_LED_BANK);
    issi.setLedBrightness(14, 255, SLEEP_LED_BANK);
    issi.setAutoFramePlayConfig(0,1,40);
    issi.setAutoFramePlayMode(SLEEP_LED_BANK);
    issi.setBreathConfig(7, 7, 2);
    issi.setBreathMode(1);
    */

    //if (issi.is_initialized())
    //    issi.enableHardwareShutdown(true);

    /*
    if (mcpu_is_initialized())
    {
        mcpu_send_animation_stop();
        mcpu_send_sleep(1);
    }
    */

    //send_sleep_to_other_side(true);
}

void backlight_sleep_led_disable(void)
{
    dprintf("backlight_sleep_led_disable\n");
    breathing = 0;

    /*
    issi.setBreathMode(0);
    issi.setPictureMode();
    issi.displayFrame(0);
    */

    //if (issi.is_initialized())
    //    issi.enableHardwareShutdown(false);

    /*
    if (mcpu_is_initialized())
        mcpu_send_sleep(0);
	*/

    //send_sleep_to_other_side(false);

    //dprintf("backlight_sleep_led_disable\n");
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
