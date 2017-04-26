/**
 * \file
 *
 * \brief Application to generate sample driver to AVRs TWI module
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel micro controller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "TWI_Master.h"
#include "../../nfo_led.h"
#include "twi_transmit_queue.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#if defined(DEBUG_I2C)
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define MAX_MTX_NACK_COUNT 3

static unsigned char TWI_buf[TWI_BUFFER_SIZE]; // Transceiver buffer
static unsigned char *TWI_buf_ptr;             // Transceiver buffer pointer
static unsigned char TWI_data_length;          // Number of bytes to be transmitted.
static unsigned char TWI_state = TWI_NO_STATE; // State byte. Default set to TWI_NO_STATE.

union TWI_statusReg TWI_statusReg = {0}; // TWI_statusReg is defined in TWI_Master.h

static tx_queue_data_t *tail = 0;
static tx_queue_data_t *head = 0;

static volatile uint8_t mtx_adr_nack_total = 0;
static volatile uint8_t mtx_adr_nack_count = 0;
static volatile uint8_t mtx_adr_nack_lost = 0;

static volatile uint8_t mtx_data_nack_total = 0;
static volatile uint8_t mtx_data_nack_count = 0;
static volatile uint8_t mtx_data_nack_lost = 0;

/****************************************************************************
Call this function to set up the TWI master to its initial standby state.
Remember to enable interrupts from the main application after initializing the TWI.
****************************************************************************/
void TWI_Master_Initialise(void)
{
    TWBR = TWI_TWBR; // Set bit rate register (Baud rate). Defined in header file. Driver presumes prescaler to be 00.
    TWDR = 0xFF;     // Default content = SDA released.
    TWCR = (1 << TWEN) |                               // Enable TWI-interface and release TWI pins.
           (0 << TWIE) | (0 << TWINT) |                // Disable Interrupt.
           (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // No Signal requests.
           (0 << TWWC);                                //
}

/****************************************************************************
Call this function to test if the TWI_ISR is busy transmitting.
****************************************************************************/
unsigned char TWI_Transceiver_Busy(void)
{
    return (TWCR & (1 << TWIE)); // If TWI Interrupt is enabled then the Transceiver is busy
}

/****************************************************************************
Call this function to fetch the state information of the previous operation. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation. If there was an error, then the function
will return the TWI State code.
****************************************************************************/
unsigned char TWI_Get_State_Info(void)
{
    while (TWI_Transceiver_Busy())
        ;               // Wait until TWI has completed the transmission.
    return (TWI_state); // Return error state.
}

/*************************************************************************
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
unsigned char TWI_detect(unsigned char slave_address)
{
    uint8_t twst;

    if ((slave_address & (TRUE << TWI_READ_BIT))) // If it is a read operation, then do nothing
        return 1;

    // send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // wait until transmission completed
    while (!(TWCR & (1 << TWINT)))
        ;

    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ((twst != TW_START) && (twst != TW_REP_START))
        return 1;

    // send device address
    TWDR = slave_address;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wail until transmission completed and ACK/NACK has been received
    while (!(TWCR & (1 << TWINT)))
        ;

    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK))
        return 1;

    return 0;

} /* i2c_start */

/****************************************************************************
Call this function to send a prepared message. The first byte must contain the slave address and the
read/write bit. Consecutive bytes contain the data to be sent, or empty locations for data to be read
from the slave. Also include how many bytes that should be sent/read including the address byte.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_write_byte(unsigned char slave_address, unsigned char data_byte)
{
    if ((slave_address & (TRUE << TWI_READ_BIT))) // If it is a read operation, then do nothing
        return;

    dprintf("TWI_write_byte\n");

    if (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    dprintf("TWI_write_byte 0x%u\n", data_byte);

    TWI_data_length = 2;        // Number of data to transmit.
    TWI_buf[0] = slave_address; // Store slave address with R/W setting.
    TWI_buf[1] = data_byte;

    TWI_buf_ptr = TWI_buf;

    TWI_statusReg.all = 0;
    TWI_state = TWI_NO_STATE;
    TWCR = (1 << TWEN) |                               // TWI Interface enabled.
           (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
           (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
           (0 << TWWC);
}

/****************************************************************************
Call this function to send a prepared message. The first byte must contain the slave address and the
read/write bit. Consecutive bytes contain the data to be sent, or empty locations for data to be read
from the slave. Also include how many bytes that should be sent/read including the address byte.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_write_byte_to_register(unsigned char slave_address, unsigned char register_address, unsigned char data_byte)
{
    if ((slave_address & (TRUE << TWI_READ_BIT))) // If it is a read operation, then do nothing
        return;

    dprintf("TWI_write_byte_to_register %u %u\n", register_address, data_byte);

    while (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    TWI_data_length = 3;        // Number of data to transmit.
    TWI_buf[0] = slave_address; // Store slave address with R/W setting.
    TWI_buf[1] = register_address;
    TWI_buf[2] = data_byte;

    TWI_buf_ptr = TWI_buf;

    TWI_statusReg.all = 0;
    TWI_state = TWI_NO_STATE;
    TWCR = (1 << TWEN) |                               // TWI Interface enabled.
           (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
           (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
           (0 << TWWC);
}

/****************************************************************************
Call this function to send a prepared message. The first byte must contain the slave address and the
read/write bit. Consecutive bytes contain the data to be sent, or empty locations for data to be read
from the slave. Also include how many bytes that should be sent/read including the address byte.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_write_data_to_register(unsigned char slave_address, unsigned char register_address, const unsigned char *data,
                                unsigned char data_length)
{
    if ((slave_address & (TRUE << TWI_READ_BIT))) // If it is a read operation, then do nothing
        return;

    dprintf("TWI_write_data_to_register l:%u\n", data_length);

    while (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    TWI_data_length = data_length + 2; // Number of data to transmit.
    TWI_buf[0] = slave_address;        // Store slave address with R/W setting.
    TWI_buf[1] = register_address;
    for (unsigned char temp = 0; temp < data_length; temp++)
        TWI_buf[temp + 2] = data[temp];

    TWI_buf_ptr = TWI_buf;

    TWI_statusReg.all = 0;
    TWI_state = TWI_NO_STATE;
    TWCR = (1 << TWEN) |                               // TWI Interface enabled.
           (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
           (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
           (0 << TWWC);
}

/****************************************************************************
Call this function to send a prepared message. The first byte must contain the slave address and the
read/write bit. Consecutive bytes contain the data to be sent, or empty locations for data to be read
from the slave. Also include how many bytes that should be sent/read including the address byte.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Transceiver_With_Data(const unsigned char *data, unsigned char data_length)
{
    dprintf("TWI_Start_Transceiver_With_Data l:%u\n", data_length);

    while (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    TWI_data_length = data_length; // Number of data to transmit.
    TWI_buf[0] = data[0];          // Store slave address with R/W setting.

    if (!(data[0] & (TRUE << TWI_READ_BIT))) // If it is a write operation, then also copy data.
    {
        for (unsigned char temp = 1; temp < data_length; temp++)
            TWI_buf[temp] = data[temp];
    }

    TWI_buf_ptr = TWI_buf;

    TWI_statusReg.all = 0;
    TWI_state = TWI_NO_STATE;
    TWCR = (1 << TWEN) |                               // TWI Interface enabled.
           (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
           (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
           (0 << TWWC);                                //
}

/****************************************************************************
Call this function to send a prepared message. The first byte must contain the slave address and the
read/write bit. Consecutive bytes contain the data to be sent, or empty locations for data to be read
from the slave. Also include how many bytes that should be sent/read including the address byte.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_read_data(unsigned char slave_address, unsigned char data_length)
{
    dprintf("TWI_read_data l:%u\n", data_length);

    while (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    TWI_data_length = data_length + 1;                   // Number of data to transmit.
    TWI_buf[0] = slave_address | (TRUE << TWI_READ_BIT); // Store slave address with R/W setting.

    TWI_buf_ptr = TWI_buf;

    TWI_statusReg.all = 0;
    TWI_state = TWI_NO_STATE;
    TWCR = (1 << TWEN) |                               // TWI Interface enabled.
           (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
           (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
           (0 << TWWC);
}

/****************************************************************************
Call this function to resend the last message. The driver will reuse the data previously put in the transceiver buffers.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Transceiver(void)
{
    dprintf("TWI_Start_Transceiver\n");

    while (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    TWI_buf_ptr = TWI_buf;

    TWI_statusReg.all = 0;
    TWI_state = TWI_NO_STATE;
    TWCR = (1 << TWEN) |                               // TWI Interface enabled.
           (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
           (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
           (0 << TWWC);                                //
}

/****************************************************************************
Call this function to read out the requested data from the TWI transceiver buffer. I.e. first call
TWI_Start_Transceiver to send a request for data to the slave. Then Run this function to collect the
data when they have arrived. Include a pointer to where to place the data and the number of bytes
requested (including the address field) in the function call. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation, before reading out the data and returning.
If there was an error in the previous transmission the function will return the TWI error code.
****************************************************************************/
unsigned char TWI_get_data_from_transceiver(unsigned char *msg, unsigned char msgSize)
{
    while (!tx_queue_is_empty())
        ; // Wait until tx queue is empty

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    dprintf("TWI_get_data_from_transceiver l:%u, ok:%u\n", msgSize, TWI_statusReg.lastTransOK);

    if (TWI_statusReg.lastTransOK) // Last transmission competed successfully.
    {
        for (unsigned char i = 0; i < msgSize; i++) // Copy data from Transceiver buffer.
        {
            msg[i] = TWI_buf[i + 1];
        }
    }

    dprintf("TWI_get_data_from_transceiver done\n");

    return (TWI_statusReg.lastTransOK);
}

// * -------------------------------------------------------------------------------------------------
// * -------------------------------------------------------------------------------------------------
// * -------------------------------------------------------------------------------------------------
// * ------------------------------------------- Queued TX -------------------------------------------
// * -------------------------------------------------------------------------------------------------
// * -------------------------------------------------------------------------------------------------
// * -------------------------------------------------------------------------------------------------

void queued_twi_start_transceiver(void)
{
    dprintf("qtst_start\n");

    if (tx_queue_is_empty())
        return;

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    if (!tx_queue_front(&head))
    {
        // dprintf("no queue front\n");
        // tx_queue_print_status();
        return;
    }

    TWI_buf_ptr = head->data;
    TWI_data_length = head->data_length;

    dprintf("qtst_start l:%u\n", TWI_data_length);

    TWI_statusReg.all = 0;
    TWI_state = TWI_NO_STATE;
    TWCR = (1 << TWEN) |                               // TWI Interface enabled.
           (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
           (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
           (0 << TWWC);                                //
}

void queued_twi_write_byte(unsigned char slave_address, unsigned char data_byte)
{
    // dprintf("queued_twi_write_byte 0x%u\n", data_byte);
    dprintf("qtwb 0x%u\n", data_byte);

    if ((slave_address & (TRUE << TWI_READ_BIT))) // If it is a read operation, then do nothing
        return;

    while (tx_queue_is_full())
        ; // Wait until there is a free buffer in the queue

    tx_queue_get_empty_tail(&tail);

    tail->data_length = 2; // Number of data to transmit.

    tail->data[0] = slave_address; // Store slave address with R/W setting.
    tail->data[1] = data_byte;

    bool queue_was_empty = tx_queue_is_empty();

    tx_queue_push_tail();

    if (queue_was_empty || tx_queue_size() == 1)
        queued_twi_start_transceiver();
}

void queued_twi_write_byte_to_register(unsigned char slave_address, unsigned char register_address,
                                       unsigned char data_byte)
{
    // dprintf("queued_twi_write_byte_to_register %u %u\n", register_address, data_byte);
    dprintf("qtwbtr %u %u\n", register_address, data_byte);

    if ((slave_address & (TRUE << TWI_READ_BIT))) // If it is a read operation, then do nothing
        return;

    while (tx_queue_is_full())
        ; // Wait until there is a free buffer in the queue

    tx_queue_get_empty_tail(&tail);

    tail->data_length = 3; // Number of data to transmit.

    tail->data[0] = slave_address; // Store slave address with R/W setting.
    tail->data[1] = register_address;
    tail->data[2] = data_byte;

    bool queue_was_empty = tx_queue_is_empty();

    // dprintf("qtwbtr qe:%u\n", queue_was_empty);

    tx_queue_push_tail();

    if (queue_was_empty || tx_queue_size() == 1)
        queued_twi_start_transceiver();
}

void queued_twi_write_data_to_register(unsigned char slave_address, unsigned char register_address,
                                       const unsigned char *data, unsigned char data_length)
{
    // dprintf("queued_twi_write_data_to_register l:%u\n", data_length);
    dprintf("qtwdtr l:%u\n", data_length);

    if ((slave_address & (TRUE << TWI_READ_BIT))) // If it is a read operation, then do nothing
        return;

#ifndef DEBUG_TX_QUEUE
    while (tx_queue_is_full())
        ; // Wait until there is a free buffer in the queue
#else
    if (tx_queue_is_full())
    {
        print("qtwdtr full, wait\n");
        while (tx_queue_is_full())
            ; // Wait until there is a free buffer in the queue
    }
#endif

    if (!tx_queue_get_empty_tail(&tail))
    {
        print("qtwdtr ! tail\n");
        return;
    }

    tail->data_length = data_length + 2; // Number of data to transmit.

    tail->data[0] = slave_address; // Store slave address with R/W setting.
    tail->data[1] = register_address;

    for (unsigned char pos = 0; pos < data_length; pos++)
        tail->data[pos + 2] = data[pos];

    // tx_queue_print_status();

    bool queue_was_empty = tx_queue_is_empty();
    // dprintf("qtwdtr l:%u, qe:%u\n", data_length, queue_was_empty);

    if (!tx_queue_push_tail())
    {
        print("qtwdtr ! push tail\n");
        return;
    }

    // tx_queue_print_status();
    // if (tx_queue_size() > 1)
    //	dprintf("qtwdtr ql:%u\n", tx_queue_size());

    if (queue_was_empty || tx_queue_size() == 1)
    {
        // dprintf("qtwdtr start, ql:%u\n", tx_queue_size());
        queued_twi_start_transceiver();
    }
}

void queued_twi_stats(void)
{
    xprintf("qs %u\n", tx_queue_size());
    xprintf("state 0x%X\n", TWI_state);
    xprintf("adr: %u %u\n", mtx_adr_nack_total, mtx_adr_nack_lost);
    xprintf("data: %u %u\n", mtx_data_nack_total, mtx_data_nack_lost);
}

// ********** Interrupt Handlers ********** //
/****************************************************************************
This function is the Interrupt Service Routine (ISR), and called when the TWI interrupt is triggered;
that is whenever a TWI event has occurred. This function should not be called directly from the main
application.
****************************************************************************/
ISR(TWI_vect)
{
    static unsigned char TWI_bufPtr;
    static tx_queue_data_t *head = 0;

    switch (TWSR)
    {

    /*
     *
     * Master Transmit Mode (TX)
     *
     */

    case TWI_START:     // START has been transmitted
    case TWI_REP_START: // Repeated START has been transmitted
        TWI_bufPtr = 0; // Set buffer pointer to the TWI Address location
    // NO BREAK
    case TWI_MTX_ADR_ACK:  // SLA+W has been transmitted and ACK received
    case TWI_MTX_DATA_ACK: // Data byte has been transmitted and ACK received

        if (TWI_bufPtr < TWI_data_length)
        {
            // LedInfo1_On();

            TWDR = TWI_buf_ptr[TWI_bufPtr++];
            TWCR = (1 << TWEN) |                               // TWI Interface enabled
                   (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag to send byte
                   (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | //
                   (0 << TWWC);                                //

            // LedInfo1_Off();
        }
        else // Send STOP after last byte
        {
            LedInfo2_On();

            TWCR = (1 << TWEN) |                               // TWI Interface enabled
                   (0 << TWIE) | (1 << TWINT) |                // Disable TWI Interrupt and clear the flag
                   (0 << TWEA) | (0 << TWSTA) | (1 << TWSTO) | // Initiate a STOP condition.
                   (0 << TWWC);                                //

            TWI_statusReg.lastTransOK = TRUE; // Set status bits to completed successfully.

            tx_queue_pop();

            if (tx_queue_front(&head))
            {
                //_delay_us(4);
                //_delay_ms(1);

                TWI_buf_ptr = head->data;
                TWI_data_length = head->data_length;

                TWI_statusReg.all = 0;
                TWI_state = TWI_NO_STATE;
                TWCR = (1 << TWEN) |                               // TWI Interface enabled.
                       (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
                       (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
                       (0 << TWWC);                                //
            }

            LedInfo2_Off();
        }
        break;

    case TWI_MTX_ADR_NACK: // SLA+W has been transmitted and NACK received

        mtx_adr_nack_total++;
        mtx_adr_nack_count++;

        if (mtx_data_nack_count <= MAX_MTX_NACK_COUNT && tx_queue_front(&head))
        {
            _delay_us(4);

            TWI_buf_ptr = head->data;
            TWI_data_length = head->data_length;

            TWI_statusReg.all = 0;
            TWI_state = TWI_NO_STATE;
            TWCR = (1 << TWEN) |                               // TWI Interface enabled.
                   (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
                   (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
                   (0 << TWWC);
        }
        else
        {
            mtx_adr_nack_count = 0;
            mtx_adr_nack_lost++;

            tx_queue_pop();

            if (tx_queue_front(&head))
            {
                _delay_us(4);

                TWI_buf_ptr = head->data;
                TWI_data_length = head->data_length;

                TWI_statusReg.all = 0;
                TWI_state = TWI_NO_STATE;
                TWCR = (1 << TWEN) |                               // TWI Interface enabled.
                       (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
                       (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
                       (0 << TWWC);
            }
            else
            {
                TWI_state = TWSR;                   // Store TWSR and automatically sets clears noErrors bit.
                                                    // Reset TWI Interface
                TWCR = (1 << TWEN) |                // Enable TWI-interface and release TWI pins
                       (0 << TWIE) | (0 << TWINT) | // Disable Interrupt
                       (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // No Signal requests
                       (0 << TWWC);
            }
        }
        break;

    case TWI_MTX_DATA_NACK: // Data byte has been transmitted and NACK received

        mtx_data_nack_total++;
        mtx_data_nack_count++;

        if (mtx_data_nack_count <= MAX_MTX_NACK_COUNT && tx_queue_front(&head))
        {
            _delay_us(4);

            TWI_buf_ptr = head->data;
            TWI_data_length = head->data_length;

            TWI_statusReg.all = 0;
            TWI_state = TWI_NO_STATE;
            TWCR = (1 << TWEN) |                               // TWI Interface enabled.
                   (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
                   (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
                   (0 << TWWC);
        }
        else
        {
            mtx_data_nack_count = 0;
            mtx_data_nack_lost++;

            tx_queue_pop();

            if (tx_queue_front(&head))
            {
                _delay_us(4);

                TWI_buf_ptr = head->data;
                TWI_data_length = head->data_length;

                TWI_statusReg.all = 0;
                TWI_state = TWI_NO_STATE;
                TWCR = (1 << TWEN) |                               // TWI Interface enabled.
                       (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag.
                       (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
                       (0 << TWWC);
            }
            else
            {
                TWI_state = TWSR;                   // Store TWSR and automatically sets clears noErrors bit.
                                                    // Reset TWI Interface
                TWCR = (1 << TWEN) |                // Enable TWI-interface and release TWI pins
                       (0 << TWIE) | (0 << TWINT) | // Disable Interrupt
                       (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // No Signal requests
                       (0 << TWWC);
            }
        }
        break;

    /*
     *
     * Master Receive Mode (RX)
     *
     */

    case TWI_MRX_DATA_ACK: // Data byte has been received and ACK transmitted
        TWI_buf[TWI_bufPtr++] = TWDR;
    // NO BREAK
    case TWI_MRX_ADR_ACK:                       // SLA+R has been transmitted and ACK received
        if (TWI_bufPtr < (TWI_data_length - 1)) // Detect the last byte to NACK it.
        {
            TWCR = (1 << TWEN) |                // TWI Interface enabled
                   (1 << TWIE) | (1 << TWINT) | // Enable TWI Interrupt and clear the flag to read next byte
                   (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // Send ACK after reception
                   (0 << TWWC);                                //
        }
        else // Send NACK after next reception
        {
            TWCR = (1 << TWEN) |                // TWI Interface enabled
                   (1 << TWIE) | (1 << TWINT) | // Enable TWI Interrupt and clear the flag to read next byte
                   (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // Send NACK after reception
                   (0 << TWWC);                                //
        }
        break;
    case TWI_MRX_DATA_NACK: // Data byte has been received and NACK transmitted
        TWI_buf[TWI_bufPtr] = TWDR;
        TWI_statusReg.lastTransOK = TRUE;                  // Set status bits to completed successfully.
        TWCR = (1 << TWEN) |                               // TWI Interface enabled
               (0 << TWIE) | (1 << TWINT) |                // Disable TWI Interrupt and clear the flag
               (0 << TWEA) | (0 << TWSTA) | (1 << TWSTO) | // Initiate a STOP condition.
               (0 << TWWC);                                //
        break;
    case TWI_ARB_LOST:                                     // Arbitration lost
        TWCR = (1 << TWEN) |                               // TWI Interface enabled
               (1 << TWIE) | (1 << TWINT) |                // Enable TWI Interrupt and clear the flag
               (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a (RE)START condition.
               (0 << TWWC);                                //
        break;

    case TWI_MRX_ADR_NACK: // SLA+R has been transmitted and NACK received
    case TWI_BUS_ERROR:    // Bus error due to an illegal START or STOP condition
    default:
        TWI_state = TWSR;                                  // Store TWSR and automatically sets clears noErrors bit.
                                                           // Reset TWI Interface
        TWCR = (1 << TWEN) |                               // Enable TWI-interface and release TWI pins
               (0 << TWIE) | (0 << TWINT) |                // Disable Interrupt
               (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // No Signal requests
               (0 << TWWC);                                //
    }
}
