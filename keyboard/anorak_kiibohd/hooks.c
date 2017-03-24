
#include "keyboard.h"
#include "matrix.h"
#include "led.h"
#include "action.h"
#ifdef SLEEP_LED_ENABLE
#include "sleep_led.h"
#endif
#include "uart/uart.h"
#include "led_backlight/animations/animation.h"
#include "led_backlight/twi_config.h"
#include "led_backlight/backlight_kiibohd.h"

#include "led_backlight/avr315/twi_transmit_queue.h"


#define BAUD 115200 // 9600 14400 19200 38400 57600 115200

void twi_init(void)
{
#if TWILIB == AVR315

    TWI_Master_Initialise();

#elif TWILIB == BUFFTW

    i2cInit();
    i2cSetBitrate(400);

#else

    i2c_init();

#endif
}

void hook_early_init(void)
{
#ifdef DEBUG_LUFA_UART
	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
#endif
    twi_init();
}

void hook_late_init(void)
{
    backlight_setup();
    backlight_setup_finish();

    tx_queue_test();
}

extern uint8_t keyboard_led_stats;
static uint8_t _led_stats = 0;

void hook_usb_suspend_entry(void)
{
    // Turn LED off to save power
    // Set 0 with putting aside status before suspend and restore
    // it after wakeup, then LED is updated at keyboard_task() in main loop
    _led_stats = keyboard_led_stats;
    keyboard_led_stats = 0;
    led_set(keyboard_led_stats);

    matrix_clear();
    clear_keyboard();
#ifdef SLEEP_LED_ENABLE
    sleep_led_enable();
#endif
}

void hook_usb_wakeup(void)
{
    //suspend_wakeup_init();

    // clear keyboard state
    matrix_clear();
    clear_keyboard();
#ifdef BACKLIGHT_ENABLE
    //backlight_init();
#endif

#ifdef SLEEP_LED_ENABLE
    sleep_led_disable();
#endif

#ifdef BACKLIGHT_ENABLE
    stop_animation();
#endif

    // Restore LED status
    // BIOS/grub won't recognize/enumerate if led_set() takes long(around 40ms?)
    // Converters fall into the case and miss wakeup event(timeout to reply?) in the end.
    // led_set(host_keyboard_leds());
    // Instead, restore stats and update at keyboard_task() in main loop
    keyboard_led_stats = _led_stats;
}
