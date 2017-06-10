#ifndef _IS31FL3733_TWI_H_
#define _IS31FL3733_TWI_H_

#include <inttypes.h>
#include <stdbool.h>

uint8_t i2c_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
uint8_t i2c_write_no_errorhandling_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
uint8_t i2c_queued_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
uint8_t i2c_read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
uint8_t i2c_read_no_errorhandling_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
uint8_t i2c_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_write_no_errorhandling_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_queued_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_read_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data);
uint8_t i2c_read_no_errorhandling_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data);


uint8_t i2c_dummy_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
uint8_t i2c_dummy_read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
uint8_t i2c_dummy_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_dummy_read_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data);

bool i2c_detect(unsigned char slave_address);

#endif /* _IS31FL3733_TWI_H_ */
