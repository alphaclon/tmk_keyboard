#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "IS31FL3731/IS31FL3731_buffered.h"

extern IS31FL3731Buffered issi;

void IS31FL3731_init(void);
void IS31FL3731_enableHardwareShutdown(bool enabled);

#endif
