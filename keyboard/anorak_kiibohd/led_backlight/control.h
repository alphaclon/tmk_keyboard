#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "IS31FL3731/Adafruit_IS31FL3731.h"

extern Adafruit_IS31FL3731 issi;

void IS31FL3731_init(void);



void testInterruptDrivenI2C(void);

#endif
