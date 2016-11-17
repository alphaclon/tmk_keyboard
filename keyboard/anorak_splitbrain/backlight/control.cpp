
#include "control.h"
#include "debug.h"
#include <avr/interrupt.h>

extern "C" {
#if TWILIB == AVR315
#include "avr315/TWI_Master.h"
#elif TWILIB == BUFFTW
#include "twi/twi_master.h"
#else
#include "i2cmaster/i2cmaster.h"
#endif
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

    issi.dumpConfiguration();
    issi.dumpLeds(0);
    issi.dumpBrightness(0);
    issi.test();
    issi.dumpLeds(0);
    issi.dumpBrightness(0);
}

extern "C" {
#if TWILIB == AVR315
void testInterruptDrivenI2C()
{

}
#elif TWILIB == BUFFTW
void testInterruptDrivenI2C()
{
	dprintf("I2C: test\r\n");

	/*
	uint8_t cmd1[2] = {0xFD, 0};
	i2cMasterSend((ISSI_ADDR_DEFAULT << 1), 2, cmd1);

    uint8_t cmd2[ISSI_USED_CHANNELS + 1] = { 64 };
    cmd2[0] = 0x24;
    i2cMasterSend((ISSI_ADDR_DEFAULT << 1), ISSI_USED_CHANNELS + 1, cmd2);
    */

	/*
	uint8_t cmd[ISSI_USED_CHANNELS] = { 64 };

	TWI_Master_Initialise();
	TWI_Start_Transceiver_With_Data_1((ISSI_ADDR_DEFAULT << 1), 0xFD, 0);
	TWI_Start_Transceiver_With_Data_2((ISSI_ADDR_DEFAULT << 1), 0x24, cmd, ISSI_USED_CHANNELS);
	*/
}
#endif
}
