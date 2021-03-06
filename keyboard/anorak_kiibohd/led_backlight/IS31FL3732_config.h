#ifndef _ADAFRUIT_IS31FL3731_CONFIG_H_
#define _ADAFRUIT_IS31FL3731_CONFIG_H_

#include "twi_config.h"

#define ISSI_ADDR_DEFAULT 0x74

#define ISSI_USED_ROWS 6
#define ISSI_USED_CHANNELS (ISSI_USED_ROWS * ISSI_TOTAL_COLUMS)
#define ISSI_LED_MASK_SIZE (ISSI_USED_ROWS * 2)

#endif
