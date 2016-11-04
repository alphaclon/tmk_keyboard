
#include "control.h"

extern "C" {
#if TWILIB == AVR315
#include "../avr315/TWI_Master.h"
#elif TWILIB == BUFFTW
#include "../twi/twi_master.h"
#else
#include "../i2cmaster/i2cmaster.h"
#endif
#include "debug.h"
}

IS31FL3731Buffered issi;

void IS31FL3731_init()
{
#if TWILIB == AVR315

	TWI_Master_Initialise();

#elif TWILIB == BUFFTW

    i2cInit();
    i2cSetBitrate(400);

#else

    i2c_init();

#endif

    issi.begin();
}

void testInterruptDrivenI2C()
{
    uint8_t cmd[ISSI_USED_CHANNELS + 1];
    cmd[0] = 0x24;
    memset(&cmd[1], 128, ISSI_USED_CHANNELS);
    i2cMasterSend((ISSI_ADDR_DEFAULT << 1), ISSI_USED_CHANNELS + 1, cmd);
}

