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

#define I2C_DEBUG

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

// function pointer to i2c receive routine
//! I2cSlaveReceive is called when this processor
// is addressed as a slave for writing
static void (*i2cSlaveReceive)(uint8_t receiveDataLength, uint8_t const *recieveData);

//! I2cSlaveTransmit is called when this processor
// is addressed as a slave for reading
static uint8_t (*i2cSlaveTransmit)(uint8_t transmitDataLengthMax, uint8_t const *transmitData);

// functions
void i2cInit(void)
{
    GenCal_Flag = 1;
    // clear SlaveReceive and SlaveTransmit handler to null
    i2cSlaveReceive = 0;
    i2cSlaveTransmit = 0;
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

void i2cSetLocalSlaveDeviceAddr(uint8_t deviceAddr, uint8_t genCallEn)
{
    // set local device address (used in slave mode only)
    outb(TWAR, ((deviceAddr & 0xFE) | (genCallEn ? 1 : 0)));
}

void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(uint8_t receiveDataLength, uint8_t const *recieveData))
{
    i2cSlaveReceive = i2cSlaveRx_func;
}

void i2cSetSlaveTransmitHandler(uint8_t (*i2cSlaveTx_func)(uint8_t transmitDataLengthMax, uint8_t const *transmitData))
{
    i2cSlaveTransmit = i2cSlaveTx_func;
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

//! I2C (TWI) interrupt service routine
ISR(TWI_vect)
{
    // read status bits
    uint8_t status = inb(TWSR) & TWSR_STATUS_MASK;

    switch (status)
    {
    // Slave Receiver status codes
    case TW_SR_SLA_ACK:            // 0x60: own SLA+W has been received, ACK has been returned
    case TW_SR_ARB_LOST_SLA_ACK:   // 0x68: own SLA+W has been received, ACK has been returned
    case TW_SR_GCALL_ACK:          // 0x70:     GCA+W has been received, ACK has been returned
    case TW_SR_ARB_LOST_GCALL_ACK: // 0x78:     GCA+W has been received, ACK has been returned
#ifdef I2C_DEBUG
        dprintf("I2C: SR->SLA_ACK\r\n");
#endif
        // we are being addressed as slave for writing (data will be received from master)
        // set state
        I2cState = I2C_SLAVE_RX;
        // prepare buffer
        I2cReceiveDataIndex = 0;
        // receive data byte and return ACK
        outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));
        break;
    case TW_SR_DATA_ACK:
        GenCal_Flag = 0;       // 0x80: data byte has been received, ACK has been returned
    case TW_SR_GCALL_DATA_ACK: // 0x90: data byte has been received, ACK has been returned
#ifdef I2C_DEBUG
        dprintf("I2C: SR->DATA_ACK\r\n");
#endif
        // get previously received data byte
        I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
        // check receive buffer status
        if (I2cReceiveDataIndex < I2C_RECEIVE_DATA_BUFFER_SIZE)
        {
            // receive data byte and return ACK
            i2cReceiveByte(true);
            // outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
        }
        else
        {
            // receive data byte and return NACK
            i2cReceiveByte(false);
            // outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
        }
        break;
    case TW_SR_DATA_NACK:       // 0x88: data byte has been received, NACK has been returned
    case TW_SR_GCALL_DATA_NACK: // 0x98: data byte has been received, NACK has been returned
#ifdef I2C_DEBUG
        dprintf("I2C: SR->DATA_NACK\r\n");
#endif
        // receive data byte and return NACK
        GenCal_Flag = 0;
        i2cReceiveByte(false);
        // outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
        break;
    case TW_SR_STOP: // 0xA0: STOP or REPEATED START has been received while addressed as slave
#ifdef I2C_DEBUG
        dprintf("I2C: SR->SR_STOP\r\n");
#endif
        // switch to SR mode with SLA ACK
        outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));
        // i2c receive is complete, call i2cSlaveReceive
        if (i2cSlaveReceive)
            i2cSlaveReceive(I2cReceiveDataIndex, I2cReceiveData);
        // set state
        I2cState = I2C_IDLE;
        GenCal_Flag = 1;
        break;

    // Slave Transmitter
    case TW_ST_SLA_ACK:          // 0xA8: own SLA+R has been received, ACK has been returned
    case TW_ST_ARB_LOST_SLA_ACK: // 0xB0:     GCA+R has been received, ACK has been returned
#ifdef I2C_DEBUG
        dprintf("I2C: ST->SLA_ACK\r\n");
#endif
        // we are being addressed as slave for reading (data must be transmitted back to master)
        // set state
        I2cState = I2C_SLAVE_TX;
        // request data from application
        if (i2cSlaveTransmit)
            I2cSendDataLength = i2cSlaveTransmit(I2C_SEND_DATA_BUFFER_SIZE, I2cSendData);
        // reset data index
        I2cSendDataIndex = 0;
    // fall-through to transmit first data byte
    case TW_ST_DATA_ACK: // 0xB8: data byte has been transmitted, ACK has been received
#ifdef I2C_DEBUG
        dprintf("I2C: ST->DATA_ACK\r\n");
#endif
        // transmit data byte
        outb(TWDR, I2cSendData[I2cSendDataIndex++]);
        if (I2cSendDataIndex < I2cSendDataLength)
            // expect ACK to data byte
            outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));
        else
            // expect NACK to data byte
            outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT));
        break;
    case TW_ST_DATA_NACK: // 0xC0: data byte has been transmitted, NACK has been received
    case TW_ST_LAST_DATA: // 0xC8:
#ifdef I2C_DEBUG
        dprintf("I2C: ST->DATA_NACK or LAST_DATA\r\n");
#endif
        // all done
        // switch to open slave
        outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));
        // set state
        I2cState = I2C_IDLE;
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
