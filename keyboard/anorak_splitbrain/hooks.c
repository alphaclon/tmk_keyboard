
#include "action.h"
#include "keyboard.h"
#include "led.h"
#include "matrix.h"
#ifdef SLEEP_LED_ENABLE
#include "sleep_led.h"
#endif
#include "backlight/animations/animation.h"
#include "backlight/backlight_kiibohd.h"
#include "matrixdisplay/infodisplay.h"
#include "nfo_led.h"
#include "splitbrain.h"
#include "twi/twi_config.h"
#include <util/delay.h>

#ifdef DEBUG_HOOKS
#include "debug.h"
#else
#include "nodebug.h"
#endif

void twi_init(void)
{
#if TWILIB == AVR315 || TWILIB == AVR315_SYNC || TWILIB == AVR315_QUEUED

    TWI_Master_Initialise();

#elif TWILIB == BUFFTW

    i2cInit();
    i2cSetBitrate(400);

#else

    twi_init();

#endif
}

void hook_early_init(void)
{
    twi_init();
    splitbrain_init();
}

void hook_late_init(void)
{
#ifdef DEBUG_OUTPUT_ENABLE
    debug_config.enable = 1;
    debug_config.matrix = 0;
    debug_config.keyboard = 0;
    debug_config.mouse = 0;
#endif

    splitbrain_post_usb_connect_init();

	splitbrain_communication_task();
	mcpu_init();

	splitbrain_communication_task();

#ifdef BACKLIGHT_ENABLE
    backlight_setup();
    backlight_setup_finish();
#endif

    splitbrain_communication_task();
}

void hook_late_start(void)
{
	dprintf("hook_late_start\n");

	splitbrain_communication_task();

#ifdef DEBUG_LATE_TEST
    dprintf("late_test\n");
    dprintf("free ram: %d\n", freeRam());

    backlight_enable_region(backlight_region_logo);
    backlight_set_brightness_for_region(backlight_region_logo, 7);

    backlight_dump_issi_state();

    animation_test();
#endif

    _delay_us(100);
    mcpu_send_animation_stop();
    _delay_us(100);
    mcpu_send_scroll_text(PSTR("Anorak splitbrain"), MATRIX_ANIMATION_DIRECTION_LEFT, 15);

    dprintf("hook_late_start\n");
}

extern uint8_t keyboard_led_stats;
static uint8_t _led_stats = 0;

void hook_usb_suspend_entry(void)
{
	//dprintf("huse\n");

    // Turn LED off to save power
    // Set 0 with putting aside status before suspend and restore
    // it after wakeup, then LED is updated at keyboard_task() in main loop
    _led_stats = keyboard_led_stats;
    keyboard_led_stats = 0;
    led_set(keyboard_led_stats);

    matrix_clear();
    clear_keyboard();

    send_sleep_to_other_side(true);
    mcpu_hardware_shutdown(true);

#ifdef BACKLIGHT_ENABLE
    suspend_animation();
    backlight_enableShutdown(true);
#endif

#ifdef SLEEP_LED_ENABLE
    sleep_led_enable();
#endif
}

void hook_usb_wakeup(void)
{
	//dprintf("huwu\n");

	//
    // ---> This replaces the call of suspend_wakeup_init() <--
	//
    // suspend_wakeup_init();

    // clear keyboard state
    matrix_clear();
    clear_keyboard();
#ifdef BACKLIGHT_ENABLE
    //
    // --> do not call this! I2C IRQ will destroy USB communication! <--
    //
    // backlight_init();
    backlight_enableShutdown(false);
    resume_animation_in_idle_state();
#endif

#ifdef SLEEP_LED_ENABLE
    sleep_led_disable();
#endif

    mcpu_hardware_shutdown(false);
    send_sleep_to_other_side(false);

    // Restore LED status
    // BIOS/grub won't recognize/enumerate if led_set() takes long(around 40ms?)
    // Converters fall into the case and miss wakeup event(timeout to reply?) in the end.
    // led_set(host_keyboard_leds());
    // Instead, restore stats and update at keyboard_task() in main loop
    keyboard_led_stats = _led_stats;
}
