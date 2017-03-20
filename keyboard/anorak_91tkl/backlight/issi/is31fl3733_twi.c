/*
 * is31fl3733_twi.c
 *
 *  Created on: 02.03.2017
 *      Author: wenkm
 */

#include "is31fl3733_twi.h"
#include "../../twi/twi_config.h"
#include <stdbool.h>

#if defined(DEBUG_I2C)
#include "debug.h"
#else
#include "nodebug.h"
#endif

uint8_t i2c_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{
    TWI_write_data_to_register(i2c_addr, reg_addr, buffer, count);
    return count;
}

uint8_t i2c_read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{
    TWI_write_byte(i2c_addr, reg_addr);
    TWI_read_data(i2c_addr, count);
    bool lastTransOK = TWI_get_data_from_transceiver(buffer, count);

    if (!lastTransOK)
    {
        dprintf("transmission failed! 0x%X\r\n", TWI_Get_State_Info());
        TWI_Master_Initialise();
        return 0;
    }

    return count;
}

uint8_t i2c_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
#if TWILIB == AVR315

    TWI_write_byte_to_register(i2c_addr, reg_addr, data);
    return 1;

#elif TWILIB == BUFFTW

    uint8_t retval = i2cMasterSendCommandNI(i2c_addr, reg_addr, 1, &data);

    if (retval != I2C_OK)
    {
        LV_("writeRegister8 i2c error: 0x%X", retval);
    }

#else

    i2c_start_wait(i2c_addr + I2C_WRITE);
    i2c_write(reg);
    i2c_write(data);
    i2c_stop();

#endif
}

uint8_t i2c_read_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data)
{
#if TWILIB == AVR315

    TWI_write_byte(i2c_addr, reg_addr);
    TWI_read_data(i2c_addr, 1);

    bool lastTransOK = TWI_get_data_from_transceiver(&data, 1);

    if (!lastTransOK)
    {
        dprintf("transmission failed! reg:%u\r\n", reg_addr);
        return 0;
    }

    return 1;

#elif TWILIB == BUFFTW

    uint8_t retval = i2cMasterSendCommandNI(i2c_addr, reg_addr, 0, 0);

    if (retval != I2C_OK)
    {
        LV_("readRegister8 send i2c error: 0x%X", retval);
    }

    retval = i2cMasterReceiveNI(i2c_addr, 1, &data);

    if (retval != I2C_OK)
    {
        LV_("readRegister8 recv i2c error: 0x%X", retval);
    }

#else

    i2c_start_wait(i2c_addr + I2C_WRITE);
    i2c_write(reg);
    i2c_rep_start(i2c_addr + I2C_READ);
    data = i2c_readNak();
    i2c_stop();

#endif
}
