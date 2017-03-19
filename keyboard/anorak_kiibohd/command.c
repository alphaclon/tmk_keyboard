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

#include <stdbool.h>
#include <stdint.h>

#include "command.h"
#include "action_layer.h"
#include "action_util.h"
#include "backlight.h"
#include "bootloader.h"
#include "debug.h"
#include "eeconfig.h"
#include "host.h"
#include "keyboard.h"
#include "keycode.h"
#include "keymap.h"
#include "led.h"
#include "print.h"
#include "sleep_led.h"
#include "timer.h"
#include "util.h"
#include "wait.h"

#ifdef MOUSEKEY_ENABLE
#include "mousekey.h"
#endif

#ifdef PROTOCOL_PJRC
#include "usb_keyboard.h"
#ifdef EXTRAKEY_ENABLE
#include "usb_extra.h"
#endif
#endif

#ifdef PROTOCOL_VUSB
#include "usbdrv.h"
#endif

/***********************************************************
 * Command common
 ***********************************************************/

bool command_extra(uint8_t code)
{
    switch (code)
    {
    case KC_PAUSE:
    case KC_ESC:
        clear_keyboard();
        print("\n\nbootloader... ");
        wait_ms(1000);
        bootloader_jump(); // not return
        break;
    }
    return false;
}
