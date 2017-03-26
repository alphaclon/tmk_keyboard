#ifndef KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_TWI_CONFIG_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_TWI_CONFIG_H_

#define AVR315 1
#define AVR315_SYNC 2
#define AVR315_QUEUED 3
#define BUFFTW 4
#define I2CMAS 5

#define TWILIB AVR315_QUEUED

#define TWI_SEND_DATA_BUFFER_SIZE 144+2

#if TWILIB == AVR315 || TWILIB == AVR315_QUEUED
#include "avr315/TWI_Master.h"
#elif TWILIB == BUFFTW
#include "twi/twi_master.h"
#elif TWILIB == I2CMAS
#include "i2cmaster/i2cmaster.h"
#elif AVR315_SYNC
#include "avr315/TWI_Master.h"
#include "i2cmaster/i2cmaster.h"
#else
#error I2C interface implementation not defined!
#endif

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_BACKLIGHT_TWI_CONFIG_H_ */
