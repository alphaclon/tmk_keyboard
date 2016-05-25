#ifndef _TWI_H_
#define _TWI_H_

#include <inttypes.h>

#define TWI_OK              0
#define TWI_ERR_BUSY        1
#define TWI_ERR_BUFFER_LEN  2

#define TWI_CMD_LCD_SET_BACKLIGHT  	0x01
#define TWI_CMD_LCD_WRITE_RAW_DATA	0x02
#define TWI_CMD_LCD_CLEAR_SCREEN 		0x03
#define TWI_CMD_LCD_WRITE_TEXT 			0x04
#define TWI_CMD_ENCODER_CHANGE  		0x20


// init the twi
// piSlaveaddress:  own I2C address
// piBR:            baudrate prescaler (register TWBR)
void twi_init (uint8_t piSlaveAddress, uint8_t piBR);

// checks for a received message and copies the message into the supplied buffer
// ppiBuffer:   pointer to local databuffer
// piBufLen:    max len of databuffer
uint8_t twi_getMsg (uint8_t *ppiBuffer, uint8_t piBufLen);

// starts an async write request to slave with address piSlaveAddr.
// piSlaveAddr: I2C address of slave
// ppiData:     pointer to databytes
// piDataLen:   bufferlen in bytes
uint8_t twi_WriteBytesAsync (uint8_t piSlaveAddr, uint8_t *ppiData, uint8_t piDataLen);

// starts an async read request for slave with address piSlaveAddr and tries to receive piDataLen databytes
uint8_t twi_ReadBytesAsync (uint8_t piSlaveAddr, uint8_t piDataLen);

// checks for IDLE state
// returns "0" when IDLE, "1" else
uint8_t twi_busy (void);

#endif
