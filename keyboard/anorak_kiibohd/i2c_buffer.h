#ifndef _I2C_BUFFER_H_
#define _I2C_BUFFER_H_

typedef struct I2C_Buffer
{
    uint16_t head;
    uint16_t tail;
    uint8_t sequencePos;
    uint16_t size;
    uint8_t *buffer;
} I2C_Buffer;

uint8_t I2C_TxBufferPop();
void I2C_BufferPush(uint8_t byte, I2C_Buffer *buffer);
uint16_t I2C_BufferLen(I2C_Buffer *buffer);
uint8_t I2C_Send(uint8_t *data, uint8_t sendLen, uint8_t recvLen);

#endif /* _I2C_BUFFER_H_ */
