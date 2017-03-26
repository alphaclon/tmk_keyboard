
#include "keyboard.h"
#include "matrix.h"
#include "led.h"
#include "action.h"
#ifdef SLEEP_LED_ENABLE
#include "sleep_led.h"
#endif
#include "led_backlight/animations/animation.h"
#include "led_backlight/twi_config.h"
#include "led_backlight/backlight_kiibohd.h"

#if defined(LUFA_DEBUG_UART) || defined(DEBUG_ISSI_PERFORMANCE) || defined(DEBUG_OUTPUT_ENABLE)
#include "uart/uart.h"
#include "debug.h"
#include "utils.h"
#include "led_backlight/avr315/twi_transmit_queue.h"
#else
#include "nodebug.h"
#endif

#define BAUD 115200 // 9600 14400 19200 38400 57600 115200

void twi_init(void)
{
#if TWILIB == AVR315 || TWILIB == AVR315_SYNC || TWILIB ==AVR315_QUEUED

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
#ifdef LUFA_DEBUG_UART
	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
#endif
    twi_init();
}

void hook_late_init(void)
{
#ifdef DEBUG_OUTPUT_ENABLE
    debug_config.enable = 1;
    debug_config.matrix = 0;
    debug_config.keyboard = 0;
#endif

	dprintf("late_init\n");

    backlight_setup();
    backlight_setup_finish();
}

#ifdef DEBUG_LATE_TEST
void hook_late_test(void)
{
#ifdef DEBUG_OUTPUT_ENABLE
    debug_config.enable = 1;
    debug_config.matrix = 0;
    debug_config.keyboard = 0;
#endif

	dprintf("late_test\n");
	dprintf("free ram: %d\n", freeRam());

    backlight_enable_region(backlight_region_logo);
    backlight_set_brightness_for_region(backlight_region_logo, 7);

	backlight_test();

    animation_test();
}
#endif

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

#ifdef BACKLIGHT_ENABLE
    stop_animation();
#endif

    backlight_enableShutdown(true);

#ifdef SLEEP_LED_ENABLE
    sleep_led_enable();
#endif
}

void hook_usb_wakeup(void)
{
    //This replaces the call of suspend_wakeup_init()
	// suspend_wakeup_init();

    // clear keyboard state
    matrix_clear();
    clear_keyboard();
#ifdef BACKLIGHT_ENABLE
    //backlight_init(); /! do not call this! I2C IRQ will destroy USB communication!
#endif

#ifdef SLEEP_LED_ENABLE
    sleep_led_disable();
#endif

    backlight_enableShutdown(false);

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
