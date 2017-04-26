#ifndef _IS31FL3733_TWI_H_
#define _IS31FL3733_TWI_H_

#include <inttypes.h>

/// Pointer to I2C write data to register function.
uint8_t i2c_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
/// Pointer to I2C write data to register function.
uint8_t i2c_queued_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
/// Pointer to I2C read data from register function.
uint8_t i2c_read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
/// Pointer to I2C write byte to register function.
uint8_t i2c_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
/// Pointer to I2C write byte to register function.
uint8_t i2c_queued_write_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
/// Pointer to I2C read byte from register function.
uint8_t i2c_read_reg8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data);

#endif /* _IS31FL3733_TWI_H_ */
