/*! \file i2c.c \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Description : I2C (pronounced "eye-squared-see") is a two-wire bidirectional
//		network designed for easy transfer of information between a wide variety
//		of intelligent devices.  Many of the Atmel AVR series processors have
//		hardware support for transmitting and receiving using an I2C-type bus.
//		In addition to the AVRs, there are thousands of other parts made by
//		manufacturers like Philips, Maxim, National, TI, etc that use I2C as
//		their primary means of communication and control.  Common device types
//		are A/D & D/A converters, temp sensors, intelligent battery monitors,
//		MP3 decoder chips, EEPROM chips, multiplexing switches, etc.
//
//		I2C uses only two wires (SDA and SCL) to communicate bidirectionally
//		between devices.  I2C is a multidrop network, meaning that you can have
//		several devices on a single bus.  Because I2C uses a 7-bit number to
//		identify which device it wants to talk to, you cannot have more than
//		127 devices on a single bus.
//
//		I2C ordinarily requires two 4.7K pull-up resistors to power (one each on
//		SDA and SCL), but for small numbers of devices (maybe 1-4), it is enough
//		to activate the internal pull-up resistors in the AVR processor.  To do
//		this, set the port pins, which correspond to the I2C pins SDA/SCL, high.
//		For example, on the mega163, sbi(PORTC, 0); sbi(PORTC, 1);.
//
//		For complete information about I2C, see the Philips Semiconductor
//		website.  They created I2C and have the largest family of devices that
//		work with I2C.
//
// Note: Many manufacturers market I2C bus devices under a different or generic
//		bus name like "Two-Wire Interface".  This is because Philips still holds
//		"I2C" as a trademark.  For example, SMBus and SMBus devices are hardware
//		compatible and closely related to I2C.  They can be directly connected
//		to an I2C bus along with other I2C devices are are generally accessed in
//		the same way as I2C devices.  SMBus is often found on modern motherboards
//		for temp sensing and other low-level control tasks.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "legacy.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdbool.h>
#include <util/twi.h>
#include <string.h>

#include "debug.h"
#include "i2c.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

//#define I2C_DEBUG

// I2C state and address variables
static volatile eI2cStateType I2cState;
static uint8_t I2cDeviceAddrRW;
// send/transmit buffer (outgoing data)
static uint8_t I2cSendData[I2C_SEND_DATA_BUFFER_SIZE];
static uint8_t I2cSendDataIndex;
static uint8_t I2cSendDataLength;
// receive buffer (incoming data)
static uint8_t I2cReceiveData[I2C_RECEIVE_DATA_BUFFER_SIZE];
static uint8_t I2cReceiveDataIndex;
static uint8_t I2cReceiveDataLength;
uint8_t GenCal_Flag;

// functions
void i2cInit(void)
{
    GenCal_Flag = 1;
    // set i2c bit rate to 100KHz
    i2cSetBitrate(100);
    // enable TWI (two-wire interface)
    sbi(TWCR, TWEN);
    // set state
    I2cState = I2C_IDLE;
    // enable TWI interrupt and slave address ACK
    sbi(TWCR, TWIE);
    sbi(TWCR, TWEA);
    // outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
    // enable interrupts
    sei();
}

void i2cSetBitrate(uint16_t bitrateKHz)
{
    uint8_t bitrate_div;
    // set i2c bitrate
    // SCL freq = F_CPU/(16+2*TWBR))
#ifdef TWPS0
    // for processors with additional bitrate division (mega128)
    // SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
    // set TWPS to zero
    cbi(TWSR, TWPS0);
    cbi(TWSR, TWPS1);
#endif
    // calculate bitrate division
    //	original
    /*	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
            if(bitrate_div >= 16)
                    bitrate_div = (bitrate_div-16)/2;
            outb(TWBR, bitrate_div); */

    // modified by MBR 11/19/03
    //(CPU/1000/bitrate - 16)/2

    bitrate_div = (((F_CPU / 1000) / bitrateKHz) - 16) / 2;

    if (bitrate_div < 10)
        bitrate_div = 10;

    outb(TWBR, bitrate_div);
}

inline void i2cSendStart(void)
{
    // send start condition
    outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTA));
}

inline void i2cSendStop(void)
{
    // transmit stop condition
    // leave with TWEA on for slave receiving
    outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA) | BV(TWSTO));
}

inline void i2cWaitForComplete(void)
{
    // wait for i2c interface to complete operation
    while (!(inb(TWCR) & BV(TWINT)))
        ;
}

inline void i2cSendByte(uint8_t data)
{
    // save data to the TWDR
    outb(TWDR, data);
    // begin send
    outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT));
}

inline void i2cReceiveByte(uint8_t ackFlag)
{
    // begin receive over i2c
    if (ackFlag)
    {
        // ackFlag = true: ACK the recevied data
        outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));
    }
    else
    {
        // ackFlag = false: NACK the recevied data
        outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT));
    }
}

inline uint8_t i2cGetReceivedByte(void)
{
    // retieve received data byte from i2c TWDR
    return (inb(TWDR));
}

inline uint8_t i2cGetStatus(void)
{
    // retieve current i2c status from i2c TWSR
    return (inb(TWSR));
}

void i2cMasterSend(uint8_t deviceAddr, uint8_t length, uint8_t const *data)
{
#ifdef I2C_DEBUG
    dprintf("I2C: send1\r\n");
#endif

    // wait for interface to be ready
    while (I2cState)
        ;

    // set state
    I2cState = I2C_MASTER_TX;
    // save data
    I2cDeviceAddrRW = (deviceAddr & 0xFE); // RW cleared: write operation
    //memcpy(I2cSendData, data, length);
    for (uint8_t i = 0; i < length; i++)
        I2cSendData[i] = *data++;
    I2cSendDataIndex = 0;
    I2cSendDataLength = length;
    // send start condition
    i2cSendStart();
}

void i2cMasterReceive(uint8_t deviceAddr, uint8_t length, uint8_t *data)
{
    // wait for interface to be ready
    while (I2cState)
        ;
    // set state
    I2cState = I2C_MASTER_RX;
    // save data
    I2cDeviceAddrRW = (deviceAddr | 0x01); // RW set: read operation
    I2cReceiveDataIndex = 0;
    I2cReceiveDataLength = length;
    // send start condition
    i2cSendStart();
    // wait for data
    while (I2cState)
        ;
    // return data
    //memcpy(data, I2cReceiveData, length);
    for (uint8_t i = 0; i < length; i++)
        *data++ = I2cReceiveData[i];
}

uint8_t i2cMasterSendNI(uint8_t deviceAddr, uint8_t length, uint8_t const *data)
{
    uint8_t retval = I2C_OK;

    // disable TWI interrupt
    cbi(TWCR, TWIE);

    // send start condition
    i2cSendStart();
    i2cWaitForComplete();

    // send device address with write
    i2cSendByte(deviceAddr & 0xFE);
    i2cWaitForComplete();

    // check if device is present and live
    if (inb(TWSR) == TW_MT_SLA_ACK)
    {
        // send data
        while (length)
        {
            i2cSendByte(*data++);
            i2cWaitForComplete();
            length--;
        }
    }
    else
    {
        // device did not ACK it's address,
        // data will not be transferred
        // return error
        retval = I2C_ERROR_NODEV;
    }

    // transmit stop condition
    // leave with TWEA on for slave receiving
    i2cSendStop();
    while (!(inb(TWCR) & BV(TWSTO)))
        ;

    // enable TWI interrupt
    sbi(TWCR, TWIE);

    return retval;
}

uint8_t i2cMasterSendCommandNI(uint8_t deviceAddr, uint8_t cmd, uint8_t length, uint8_t const *data)
{
    uint8_t retval = I2C_OK;

    // disable TWI interrupt
    cbi(TWCR, TWIE);

    // send start condition
    i2cSendStart();
    i2cWaitForComplete();

    // send device address with write
    i2cSendByte(deviceAddr & 0xFE);
    i2cWaitForComplete();

    // check if device is present and live
    if (inb(TWSR) == TW_MT_SLA_ACK)
    {
        // send command with write
        i2cSendByte(cmd);
        i2cWaitForComplete();

        // send data
        while (length)
        {
            i2cSendByte(*data++);
            i2cWaitForComplete();
            length--;
        }
    }
    else
    {
        // device did not ACK it's address,
        // data will not be transferred
        // return error
        retval = I2C_ERROR_NODEV;
    }

    // transmit stop condition
    // leave with TWEA on for slave receiving
    i2cSendStop();
    while (!(inb(TWCR) & BV(TWSTO)))
        ;

    // enable TWI interrupt
    sbi(TWCR, TWIE);

    return retval;
}

uint8_t i2cMasterReceiveNI(uint8_t deviceAddr, uint8_t length, uint8_t *data)
{
    uint8_t retval = I2C_OK;

    // disable TWI interrupt
    cbi(TWCR, TWIE);

    // send start condition
    i2cSendStart();
    i2cWaitForComplete();

    // send device address with read
    i2cSendByte(deviceAddr | 0x01);
    i2cWaitForComplete();

    // check if device is present and live
    if (inb(TWSR) == TW_MR_SLA_ACK)
    {
        // accept receive data and ack it
        while (length > 1)
        {
            i2cReceiveByte(true);
            i2cWaitForComplete();
            *data++ = i2cGetReceivedByte();
            // decrement length
            length--;
        }

        // accept receive data and nack it (last-byte signal)
        i2cReceiveByte(false);
        i2cWaitForComplete();
        *data++ = i2cGetReceivedByte();
    }
    else
    {
        // device did not ACK it's address,
        // data will not be transferred
        // return error
        retval = I2C_ERROR_NODEV;
    }

    // transmit stop condition
    // leave with TWEA on for slave receiving
    i2cSendStop();

    // enable TWI interrupt
    sbi(TWCR, TWIE);

    return retval;
}

//! I2C (TWI) interrupt service routine
ISR(TWI_vect)
{
    // read status bits
    uint8_t status = inb(TWSR) & TWSR_STATUS_MASK;

    switch (status)
    {
    // Master General
    case TW_START:     // 0x08: Sent start condition
    case TW_REP_START: // 0x10: Sent repeated start condition
#ifdef I2C_DEBUG
        dprintf("I2C: M->START\r\n");
#endif
        // send device address
        i2cSendByte(I2cDeviceAddrRW);
        break;

    // Master Transmitter & Receiver status codes
    case TW_MT_SLA_ACK:  // 0x18: Slave address acknowledged
    case TW_MT_DATA_ACK: // 0x28: Data acknowledged
#ifdef I2C_DEBUG
        dprintf("I2C: MT->SLA_ACK or DATA_ACK\r\n");
#endif
        if (I2cSendDataIndex < I2cSendDataLength)
        {
            // send data
            i2cSendByte(I2cSendData[I2cSendDataIndex++]);
        }
        else
        {
            // transmit stop condition, enable SLA ACK
            i2cSendStop();
            // set state
            I2cState = I2C_IDLE;
        }
        break;
    case TW_MR_DATA_NACK: // 0x58: Data received, NACK reply issued
#ifdef I2C_DEBUG
        dprintf("I2C: MR->DATA_NACK\r\n");
#endif
        // store final received data byte
        I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
    // continue to transmit STOP condition
    case TW_MR_SLA_NACK:  // 0x48: Slave address not acknowledged
    case TW_MT_SLA_NACK:  // 0x20: Slave address not acknowledged
    case TW_MT_DATA_NACK: // 0x30: Data not acknowledged
#ifdef I2C_DEBUG
        dprintf("I2C: MTR->SLA_NACK or MT->DATA_NACK\r\n");
#endif
        // transmit stop condition, enable SLA ACK
        i2cSendStop();
        // set state
        I2cState = I2C_IDLE;
        break;
    case TW_MT_ARB_LOST: // 0x38: Bus arbitration lost
                         // case TW_MR_ARB_LOST:				// 0x38: Bus arbitration lost
#ifdef I2C_DEBUG
        dprintf("I2C: MT->ARB_LOST\r\n");
#endif
        // release bus
        outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT));
        // set state
        I2cState = I2C_IDLE;
        // release bus and transmit start when bus is free
        // outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
        break;
    case TW_MR_DATA_ACK: // 0x50: Data acknowledged
#ifdef I2C_DEBUG
        dprintf("I2C: MR->DATA_ACK\r\n");
#endif
        // store received data byte
        I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
    // fall-through to see if more bytes will be received
    case TW_MR_SLA_ACK: // 0x40: Slave address acknowledged
#ifdef I2C_DEBUG
        dprintf("I2C: MR->SLA_ACK\r\n");
#endif
        if (I2cReceiveDataIndex < (I2cReceiveDataLength - 1))
            // data byte will be received, reply with ACK (more bytes in transfer)
            i2cReceiveByte(true);
        else
            // data byte will be received, reply with NACK (final byte in transfer)
            i2cReceiveByte(false);
        break;

    // Misc
    case TW_NO_INFO: // 0xF8: No relevant state information
// do nothing
#ifdef I2C_DEBUG
        dprintf("I2C: NO_INFO\r\n");
#endif
        break;
    case TW_BUS_ERROR: // 0x00: Bus error due to illegal start or stop condition
#ifdef I2C_DEBUG
        dprintf("I2C: BUS_ERROR\r\n");
#endif
        // reset internal hardware and release bus
        outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTO) | BV(TWEA));
        // set state
        I2cState = I2C_IDLE;
        break;
    }
}

eI2cStateType i2cGetState(void)
{
    return I2cState;
}

uint8_t i2cGetGenCall(void)
{
    return GenCal_Flag;
}
