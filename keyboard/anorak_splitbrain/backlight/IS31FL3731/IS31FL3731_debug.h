#ifndef KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_IS31FL3731_IS31FL3731_DEBUG_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_IS31FL3731_IS31FL3731_DEBUG_H_

#if defined(DEBUG_ISSI) || defined(DEBUG_BACKLIGHT)
#include "debug.h"
#else
#include "nodebug.h"
#endif

#ifdef DEBUG_ISSI
#define LS_(arg) dprintf(arg "\r\n")
#define LV_(s, args...) dprintf(s "\r\n", args)
#else
#define LS_(arg)
#define LV_(s, args...)
#endif

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_IS31FL3731_IS31FL3731_DEBUG_H_ */
