/*! \file i2c.h \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.h'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.03
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef I2C_H
#define I2C_H

#include <inttypes.h>
#include <util/twi.h>

// include project-specific configuration
#include "i2cconf.h"

// defines and constants
#define TWCR_CMD_MASK 0x0F
#define TWSR_STATUS_MASK 0xF8

// return values
#define I2C_OK 0x00
#define I2C_ERROR_NODEV 0x01

// types
typedef enum {
    I2C_IDLE = 0,
    I2C_BUSY = 1,
    I2C_MASTER_TX = 2,
    I2C_MASTER_RX = 3,
    I2C_SLAVE_TX = 4,
    I2C_SLAVE_RX = 5
} eI2cStateType;

// functions

//! Initialize I2C (TWI) interface
void i2cInit(void);

//! Set the I2C transaction bitrate (in KHz)
void i2cSetBitrate(uint16_t bitrateKHz);

// I2C setup and configurations commands
//! Set the local (AVR processor's) I2C device address
void i2cSetLocalSlaveDeviceAddr(uint8_t deviceAddr, uint8_t genCallEn);

//! Set the user function which handles receiving (incoming) data as a slave
void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(uint8_t receiveDataLength, uint8_t const *recieveData));
//! Set the user function which handles transmitting (outgoing) data as a slave
void i2cSetSlaveTransmitHandler(uint8_t (*i2cSlaveTx_func)(uint8_t transmitDataLengthMax, uint8_t const *transmitData));

//! Get current I2c bus status from TWSR
uint8_t i2cGetStatus(void);

// returns 1 if receive is from a General Call.
uint8_t i2cGetGenCall(void);
// high-level I2C transaction commands

//! Get the current high-level state of the I2C interface
eI2cStateType i2cGetState(void);

#endif
