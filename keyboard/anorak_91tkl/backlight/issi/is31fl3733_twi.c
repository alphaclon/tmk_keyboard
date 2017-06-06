
#include "is31fl3733_twi.h"
#include "../../twi/twi_config.h"
#include "debug.h"
#include <util/delay.h>
#include <stdbool.h>

uint8_t i2c_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{
    TWI_write_data_to_register(i2c_addr, reg_addr, buffer, count);

    unsigned char state = TWI_NO_STATE;
    state = TWI_Get_State_Info();
	if (state != TWI_NO_STATE)
	{
		xprintf("i2c_write_reg: write byte failed: 0x%X\n", state);
	}

    return count;
}

uint8_t i2c_queued_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{

	queued_twi_write_data_to_register(i2c_addr, reg_addr, buffer, count);
    return count;
}

uint8_t i2c_read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{
    uint8_t retry_count = 3;
    unsigned char state = TWI_NO_STATE;

    do
    {
		TWI_write_byte(i2c_addr, reg_addr);

		state = TWI_Get_State_Info();
		if (state != TWI_NO_STATE)
		{
			xprintf("i2c_read_reg: write byte failed: 0x%X\n", state);
			retry_count--;
			_delay_ms(1);
		}

    } while (state != TWI_NO_STATE && retry_count > 0);

    if (state != TWI_NO_STATE)
    	return 0;

    TWI_read_data(i2c_addr, count);
    bool lastTransOK = TWI_get_data_from_transceiver(buffer, count);

    if (!lastTransOK)
    {
    	xprintf("i2c_read_reg: get data failed! 0x%X\r\n", TWI_Get_State_Info());
        TWI_Master_Initialise();
        return 0;
    }

    return count;
}

uint8_t i2c_read_no_errorhandling_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{

    TWI_write_byte(i2c_addr, reg_addr);
    TWI_read_data(i2c_addr, count);
    TWI_get_data_from_transceiver(buffer, count);
    return count;
}

uint8_t i2c_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
    TWI_write_byte_to_register(i2c_addr, reg_addr, data);

    unsigned char state = TWI_NO_STATE;
    state = TWI_Get_State_Info();
	if (state != TWI_NO_STATE)
	{
		xprintf("i2c_write_reg8: write byte failed: 0x%X\n", state);
	}

    return 1;
}

uint8_t i2c_queued_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
	queued_twi_write_data_to_register(i2c_addr, reg_addr, &data, 1);
    return 1;
}

uint8_t i2c_read_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data)
{
    uint8_t retry_count = 3;
    unsigned char state = TWI_NO_STATE;

    do
    {
		TWI_write_byte(i2c_addr, reg_addr);

		state = TWI_Get_State_Info();
		if (state != TWI_NO_STATE)
		{
			xprintf("i2c_read_reg8: write byte failed: 0x%X\n", state);
			retry_count--;
			_delay_ms(1);
		}

    } while (state != TWI_NO_STATE && retry_count > 0);

    if (state != TWI_NO_STATE)
    	return 0;

    TWI_read_data(i2c_addr, 1);
    bool lastTransOK = TWI_get_data_from_transceiver(data, 1);

    if (!lastTransOK)
    {
    	xprintf("i2c_read_reg8: get data failed! 0x%X\r\n", TWI_Get_State_Info());
        return 0;
    }

    return 1;
}

uint8_t i2c_read_no_errorhandling_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data)
{
    TWI_write_byte(i2c_addr, reg_addr);
    TWI_read_data(i2c_addr, 1);
    TWI_get_data_from_transceiver(data, 1);
    return 1;
}

uint8_t i2c_dummy_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{
	return 0;
}

uint8_t i2c_dummy_read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{
	return 0;
}

uint8_t i2c_dummy_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data)
{
	return 0;
}

uint8_t i2c_dummy_read_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data)
{
	return 0;
}

