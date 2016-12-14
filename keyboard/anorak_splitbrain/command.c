/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <stdbool.h>
#include "wait.h"
#include "keycode.h"
#include "host.h"
#include "keymap.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "timer.h"
#include "keyboard.h"
#include "bootloader.h"
#include "action_layer.h"
#include "action_util.h"
#include "eeconfig.h"
#include "sleep_led.h"
#include "led.h"
#include "command.h"
#include "backlight.h"

#ifdef MOUSEKEY_ENABLE
#include "mousekey.h"
#endif

#ifdef PROTOCOL_PJRC
#   include "usb_keyboard.h"
#   ifdef EXTRAKEY_ENABLE
#       include "usb_extra.h"
#   endif
#endif

#ifdef PROTOCOL_VUSB
#   include "usbdrv.h"
#endif


/***********************************************************
 * Command common
 ***********************************************************/

static void command_common_splitbrain_help(void)
{
    print("\n\t- Magic -\n"
          "d:	debug\n"
          "x:	debug matrix\n"
          "k:	debug keyboard\n"
          "m:	debug mouse\n"
          "v:	version\n"
          "s:	status\n"
          "c:	console mode\n"
          "0-4:	layer0-4(F10-F4)\n"
          "Paus:	bootloader\n"

#ifdef KEYBOARD_LOCK_ENABLE
          "Caps:	Lock\n"
#endif

#ifdef BOOTMAGIC_ENABLE
          "e:	eeprom\n"
#endif

#ifdef NKRO_ENABLE
          "n:	NKRO\n"
#endif

#ifdef SLEEP_LED_ENABLE
          "z:	sleep LED test\n"
#endif
    );
}

static bool command_common(uint8_t code)
{
#ifdef KEYBOARD_LOCK_ENABLE
    static host_driver_t *host_driver = 0;
#endif
#ifdef SLEEP_LED_ENABLE
    static bool sleep_led_test = false;
#endif
    switch (code) {
#ifdef SLEEP_LED_ENABLE
        case KC_Z:
            // test breathing sleep LED
            print("Sleep LED test\n");
            if (sleep_led_test) {
                sleep_led_disable();
                led_set(host_keyboard_leds());
            } else {
                sleep_led_enable();
            }
            sleep_led_test = !sleep_led_test;
            break;
#endif
#ifdef BOOTMAGIC_ENABLE
        case KC_E:
            print("eeconfig:\n");
            print_eeconfig();
            break;
#endif
#ifdef KEYBOARD_LOCK_ENABLE
        case KC_CAPSLOCK:
            if (host_get_driver()) {
                host_driver = host_get_driver();
                clear_keyboard();
                host_set_driver(0);
                print("Locked.\n");
            } else {
                host_set_driver(host_driver);
                print("Unlocked.\n");
            }
            break;
#endif
        case KC_H:
        case KC_G:
        case KC_SLASH: /* ? */
            command_common_splitbrain_help();
            break;
        case KC_C:
            debug_matrix   = false;
            debug_keyboard = false;
            debug_mouse    = false;
            debug_enable   = false;
            command_console_help();
            print("C> ");
            command_state = CONSOLE;
            break;
        case KC_PAUSE:
            clear_keyboard();
            print("\n\nbootloader... ");
            wait_ms(1000);
            bootloader_jump(); // not return
            break;
        case KC_D:
            if (debug_enable) {
                print("\ndebug: off\n");
                debug_matrix   = false;
                debug_keyboard = false;
                debug_mouse    = false;
                debug_enable   = false;
            } else {
                print("\ndebug: on\n");
                debug_enable   = true;
            }
            break;
        case KC_X: // debug matrix toggle
            debug_matrix = !debug_matrix;
            if (debug_matrix) {
                print("\nmatrix: on\n");
                debug_enable = true;
            } else {
                print("\nmatrix: off\n");
            }
            break;
        case KC_K: // debug keyboard toggle
            debug_keyboard = !debug_keyboard;
            if (debug_keyboard) {
                print("\nkeyboard: on\n");
                debug_enable = true;
            } else {
                print("\nkeyboard: off\n");
            }
            break;
        case KC_M: // debug mouse toggle
            debug_mouse = !debug_mouse;
            if (debug_mouse) {
                print("\nmouse: on\n");
                debug_enable = true;
            } else {
                print("\nmouse: off\n");
            }
            break;
        case KC_V: // print version & information
            print("\n\t- Version -\n");
            print("DESC: " STR(DESCRIPTION) "\n");
            print("VID: " STR(VENDOR_ID) "(" STR(MANUFACTURER) ") "
                  "PID: " STR(PRODUCT_ID) "(" STR(PRODUCT) ") "
                  "VER: " STR(DEVICE_VER) "\n");
            print("BUILD: " STR(VERSION) " (" __TIME__ " " __DATE__ ")\n");
            /* build options */
            print("OPTIONS:"
#ifdef PROTOCOL_PJRC
            " PJRC"
#endif
#ifdef PROTOCOL_LUFA
            " LUFA"
#endif
#ifdef PROTOCOL_VUSB
            " VUSB"
#endif
#ifdef PROTOCOL_CHIBIOS
            " CHIBIOS"
#endif
#ifdef BOOTMAGIC_ENABLE
            " BOOTMAGIC"
#endif
#ifdef MOUSEKEY_ENABLE
            " MOUSEKEY"
#endif
#ifdef EXTRAKEY_ENABLE
            " EXTRAKEY"
#endif
#ifdef CONSOLE_ENABLE
            " CONSOLE"
#endif
#ifdef COMMAND_ENABLE
            " COMMAND"
#endif
#ifdef NKRO_ENABLE
            " NKRO"
#endif
#ifdef KEYMAP_SECTION_ENABLE
            " KEYMAP_SECTION"
#endif
            " " STR(BOOTLOADER_SIZE) "\n");

            print("GCC: " STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
#if defined(__AVR__)
                  " AVR-LIBC: " __AVR_LIBC_VERSION_STRING__
                  " AVR_ARCH: avr" STR(__AVR_ARCH__) "\n");
#elif defined(__arm__)
            // TODO
            );
#endif
            break;
        case KC_S:
            print("\n\t- Status -\n");
            print_val_hex8(host_keyboard_leds());
            print_val_hex8(keyboard_protocol);
            print_val_hex8(keyboard_idle);
#ifdef NKRO_ENABLE
            print_val_hex8(keyboard_nkro);
#endif
            print_val_hex32(timer_read32());

#ifdef PROTOCOL_PJRC
            print_val_hex8(UDCON);
            print_val_hex8(UDIEN);
            print_val_hex8(UDINT);
            print_val_hex8(usb_keyboard_leds);
            print_val_hex8(usb_keyboard_idle_count);
#endif

#ifdef PROTOCOL_PJRC
#   if USB_COUNT_SOF
            print_val_hex8(usbSofCount);
#   endif
#endif
            break;
#ifdef NKRO_ENABLE
        case KC_N:
            clear_keyboard(); //Prevents stuck keys.
            keyboard_nkro = !keyboard_nkro;
            if (keyboard_nkro) {
                print("NKRO: on\n");
            } else {
                print("NKRO: off\n");
            }
            break;
#endif
        case KC_ESC:
        case KC_GRV:
        case KC_0:
        case KC_F10:
            switch_default_layer(0);
            break;
        case KC_1 ... KC_9:
            switch_default_layer((code - KC_1) + 1);
            break;
        case KC_F1 ... KC_F9:
            switch_default_layer((code - KC_F1) + 1);
            break;
        default:
            print("?");
            return false;
    }
    return true;
}

bool command_extra(uint8_t code)
{
	return command_common_splitbrain(code);
}

bool command_console_extra(uint8_t code)
{
    (void)code;
    return false;
}
