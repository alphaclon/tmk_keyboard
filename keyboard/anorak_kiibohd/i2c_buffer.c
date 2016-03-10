/* Copyright (C) 2014-2016 by Jacob Alexander
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "i2c_buffer.h"

#define I2C_TxBufferLength 300
#define I2C_RxBufferLength 8

// Before sending the sequence, I2C_TxBuffer_CurLen is assigned and as each byte is sent, it is decremented
// Once I2C_TxBuffer_CurLen reaches zero, a STOP on the I2C bus is sent
volatile uint8_t I2C_TxBufferPtr[ I2C_TxBufferLength ];
volatile uint8_t I2C_RxBufferPtr[ I2C_TxBufferLength ];

volatile I2C_Buffer I2C_TxBuffer = { 0, 0, 0, I2C_TxBufferLength, (uint8_t*)I2C_TxBufferPtr };
volatile I2C_Buffer I2C_RxBuffer = { 0, 0, 0, I2C_RxBufferLength, (uint8_t*)I2C_RxBufferPtr };

#define NL "\n"
#define printHex(arg) printf_P(PSTR("0x%x"), arg)

// ----- Interrupt Functions -----

void i2c0_isr()
{
    cli(); // Disable Interrupts

    uint8_t status = I2C0_S; // Read I2C Bus status

    // Master Mode Transmit
    if ( I2C0_C1 & I2C_C1_TX )
    {
        // Check current use of the I2C bus
        // Currently sending data
        if ( I2C_TxBuffer.sequencePos > 0 )
        {
            // Make sure slave sent an ACK
            if ( status & I2C_S_RXAK )
            {
                // NACK Detected, disable interrupt
                erro_print("I2C NAK detected...");
                I2C0_C1 = I2C_C1_IICEN;

                // Abort Tx Buffer
                I2C_TxBuffer.head = 0;
                I2C_TxBuffer.tail = 0;
                I2C_TxBuffer.sequencePos = 0;
            }
            else
            {
                // Transmit byte
                I2C0_D = I2C_TxBufferPop();
            }
        }
        // Receiving data
        else if ( I2C_RxBuffer.sequencePos > 0 )
        {
            // Master Receive, addr sent
            if ( status & I2C_S_ARBL )
            {
                // Arbitration Lost
                erro_print("Arbitration lost...");
                // TODO Abort Rx

                I2C0_C1 = I2C_C1_IICEN;
                I2C0_S = I2C_S_ARBL | I2C_S_IICIF; // Clear ARBL flag and interrupt
            }
            if ( status & I2C_S_RXAK )
            {
                // Slave Address NACK Detected, disable interrupt
                erro_print("Slave Address I2C NAK detected...");
                // TODO Abort Rx

                I2C0_C1 = I2C_C1_IICEN;
            }
            else
            {
                dbug_msg("Attempting to read byte - ");
                printHex( I2C_RxBuffer.sequencePos );
                print( NL );
                I2C0_C1 = I2C_RxBuffer.sequencePos == 1
                    ? I2C_C1_IICEN | I2C_C1_IICIE | I2C_C1_MST | I2C_C1_TXAK // Single byte read
                    : I2C_C1_IICEN | I2C_C1_IICIE | I2C_C1_MST; // Multi-byte read
            }
        }
        else
        {
            /*
            dbug_msg("STOP - ");
            printHex( I2C_BufferLen( (I2C_Buffer*)&I2C_TxBuffer ) );
            print(NL);
            */

            // Delay around STOP to make sure it actually happens...
            delayMicroseconds( 1 );
            I2C0_C1 = I2C_C1_IICEN; // Send STOP
            delayMicroseconds( 7 );

            // If there is another sequence, start sending
            if ( I2C_BufferLen( (I2C_Buffer*)&I2C_TxBuffer ) < I2C_TxBuffer.size )
            {
                // Clear status flags
                I2C0_S = I2C_S_IICIF | I2C_S_ARBL;

                // Wait...till the master dies
                while ( I2C0_S & I2C_S_BUSY );

                // Enable I2C interrupt
                I2C0_C1 = I2C_C1_IICEN | I2C_C1_IICIE | I2C_C1_MST | I2C_C1_TX;

                // Transmit byte
                I2C0_D = I2C_TxBufferPop();
            }
        }
    }
    // Master Mode Receive
    else
    {
        // XXX Do we need to handle 2nd last byte?
        //I2C0_C1 = I2C_C1_IICEN | I2C_C1_IICIE | I2C_C1_MST | I2C_C1_TXAK; // No STOP, Rx, NAK on recv

        // Last byte
        if ( I2C_TxBuffer.sequencePos <= 1 )
        {
            // Change to Tx mode
            I2C0_C1 = I2C_C1_IICEN | I2C_C1_MST | I2C_C1_TX;

            // Grab last byte
            I2C_BufferPush( I2C0_D, (I2C_Buffer*)&I2C_RxBuffer );

            delayMicroseconds( 1 ); // Should be enough time before issuing the stop
            I2C0_C1 = I2C_C1_IICEN; // Send STOP
        }
        else
        {
            // Retrieve data
            I2C_BufferPush( I2C0_D, (I2C_Buffer*)&I2C_RxBuffer );
        }
    }

    I2C0_S = I2C_S_IICIF; // Clear interrupt

    sei(); // Re-enable Interrupts
}

inline uint8_t I2C_BufferCopy( uint8_t *data, uint8_t sendLen, uint8_t recvLen, I2C_Buffer *buffer )
{
    uint8_t reTurn = 0;

    // If sendLen is greater than buffer fail right away
    if ( sendLen > buffer->size )
        return 0;

    // Calculate new tail to determine if buffer has enough space
    // The first element specifies the expected number of bytes from the slave (+1)
    // The second element in the new buffer is the length of the buffer sequence (+1)
    uint16_t newTail = buffer->tail + sendLen + 2;
    if ( newTail >= buffer->size )
        newTail -= buffer->size;

    if ( I2C_BufferLen( buffer ) < sendLen + 2 )
        return 0;

/*
    print("|");
    printHex( sendLen + 2 );
    print("|");
    printHex( *tail );
    print("@");
    printHex( newTail );
    print("@");
*/

    // If buffer is clean, return 1, otherwise 2
    reTurn = buffer->head == buffer->tail ? 1 : 2;

    // Add to buffer, already know there is enough room (simplifies adding logic)
    uint8_t bufferHeaderPos = 0;
    for ( uint16_t c = 0; c < sendLen; c++ )
    {
        // Add data to buffer
        switch ( bufferHeaderPos )
        {
        case 0:
            buffer->buffer[ buffer->tail ] = recvLen;
            bufferHeaderPos++;
            c--;
            break;

        case 1:
            buffer->buffer[ buffer->tail ] = sendLen;
            bufferHeaderPos++;
            c--;
            break;

        default:
            buffer->buffer[ buffer->tail ] = data[ c ];
            break;
        }

        // Check for wrap-around case
        if ( buffer->tail + 1 >= buffer->size )
        {
            buffer->tail = 0;
        }
        // Normal case
        else
        {
            buffer->tail++;
        }
    }

    return reTurn;
}


inline uint16_t I2C_BufferLen( I2C_Buffer *buffer )
{
    // Tail >= Head
    if ( buffer->tail >= buffer->head )
        return buffer->head + buffer->size - buffer->tail;

    // Head > Tail
    return buffer->head - buffer->tail;
}


void I2C_BufferPush( uint8_t byte, I2C_Buffer *buffer )
{
    printf("DATA: ");
    printHex( byte );

    // Make sure buffer isn't full
    if ( buffer->tail + 1 == buffer->head || ( buffer->head > buffer->tail && buffer->tail + 1 - buffer->size == buffer->head ) )
    {
        printf("I2C_BufferPush failed, buffer full: ");
        printHex( byte );
        printf( NL );
        return;
    }

    // Check for wrap-around case
    if ( buffer->tail + 1 >= buffer->size )
    {
        buffer->tail = 0;
    }
    // Normal case
    else
    {
        buffer->tail++;
    }

    // Add byte to buffer
    buffer->buffer[ buffer->tail ] = byte;
}


uint8_t I2C_TxBufferPop()
{
    // Return 0xFF if no buffer left (do not rely on this)
    if ( I2C_BufferLen( (I2C_Buffer*)&I2C_TxBuffer ) >= I2C_TxBuffer.size )
    {
        printf("No buffer to pop an entry from... ");
        printHex( I2C_TxBuffer.head );
        printf(" ");
        printHex( I2C_TxBuffer.tail );
        printf(" ");
        printHex( I2C_TxBuffer.sequencePos );
        printf(NL);
        return 0xFF;
    }

    // If there is currently no sequence being sent, the first entry in the RingBuffer is the length
    if ( I2C_TxBuffer.sequencePos == 0 )
    {
        I2C_TxBuffer.sequencePos = 0xFF; // So this doesn't become an infinite loop
        I2C_RxBuffer.sequencePos = I2C_TxBufferPop();
        I2C_TxBuffer.sequencePos = I2C_TxBufferPop();
    }

    uint8_t data = I2C_TxBuffer.buffer[ I2C_TxBuffer.head ];

    // Prune head
    I2C_TxBuffer.head++;

    // Wrap-around case
    if ( I2C_TxBuffer.head >= I2C_TxBuffer.size )
        I2C_TxBuffer.head = 0;

    // Decrement buffer sequence (until next stop will be sent)
    I2C_TxBuffer.sequencePos--;

    /*
    dbug_msg("Popping: ");
    printHex( data );
    print(" ");
    printHex( I2C_TxBuffer.head );
    print(" ");
    printHex( I2C_TxBuffer.tail );
    print(" ");
    printHex( I2C_TxBuffer.sequencePos );
    print(NL);
    */
    return data;
}


uint8_t I2C_Send( uint8_t *data, uint8_t sendLen, uint8_t recvLen )
{
    // Check head and tail pointers
    // If full, return 0
    // If empty, start up I2C Master Tx
    // If buffer is non-empty and non-full, just append to the buffer
    switch ( I2C_BufferCopy( data, sendLen, recvLen, (I2C_Buffer*)&I2C_TxBuffer ) )
    {
    // Not enough buffer space...
    case 0:
        /*
        printf("Not enough Tx buffer space... ");
        printHex( I2C_TxBuffer.head );
        printf(":");
        printHex( I2C_TxBuffer.tail );
        printf("+");
        printHex( sendLen );
        printf("|");
        printHex( I2C_TxBuffer.size );
        printf( NL );
        */
        return 0;

    // Empty buffer, initialize I2C
    case 1:

        // Depending on what type of transfer, the first byte is configured for R or W
        I2C_TxBufferPop();

        return 1;
    }

    // Dirty buffer, I2C already initialized
    return 2;
}

