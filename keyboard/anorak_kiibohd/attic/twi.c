#include "twi.h"
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TWI_BUF_LEN 132

#define TWI_MODE_IDLE 0
#define TWI_MODE_READ 1
#define TWI_MODE_WRITE 2


#define TWI_FLAG_MSG_RECEIVED 1

uint8_t maiDataBuf[TWI_BUF_LEN];
uint8_t miDataBufLen;
uint8_t miRemoteSlaveAddr;
volatile uint8_t miTWIMode;
volatile uint8_t miFlags;

uint8_t twi_getFlags(void)
{
	return (miFlags);
}

ISR(TWI_vect)
{	
	//putstring ("TWI_ISR \n");

	static uint8_t liDataIdx = 0;

	switch (TWSR & 0xF8)
	{
		case 0x00: // Error detected
			break;
			
		case 0xf8: // no action taken
			//sendchar('n');
			TWCR |= (1<<TWINT);
			break;
		
		case 0x08: // Start sent
		case 0x10: //Repeated Start sent
			// Send Address
			//sendchar('s');
			TWDR = (miRemoteSlaveAddr<<1) | (miTWIMode == TWI_MODE_READ ? 1 : 0);
			TWCR |= (1<<TWINT);
			break;
		
		// *** Master Transmitter Mode States ***
		
		case 0x18: // MT-Mode SLA+W has been sent, ACK received
		case 0x20: // MT-Mode SLA+W has been sent, NOT ACK received -> trotzdem senden
			// Erstes Datenbyte senden
			//putstring("m+w ");
			liDataIdx = 1;
			TWDR = maiDataBuf[0];
			TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
			break;
			
		case 0x28: // Data Byte has been sent, ACK received
		case 0x30: // Data Byte has been sent, NOT ACK received -> trotzdem weitersenden
			if (liDataIdx<miDataBufLen)
			{
				//sendchar('n');
				// Nächstes Datenbyte senden
				TWDR = maiDataBuf[liDataIdx++];
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
			}
			else
			{
				// Transfer completed -> send stop
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA) | (1<<TWSTO);
				miTWIMode = TWI_MODE_IDLE;
			}
			break;
			
		// *** Master Receiver Mode States ***
		
		case 0x40: // MR-Mode SLA+R has been sent, ACK received
			// auf Daten warten und mit ACK bestätigen
			liDataIdx = 0;
			//putstring("m+ra ");
			
			if (miDataBufLen == 1)
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN);
			else
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
			break;
			
		case 0x48: // MR-Mode SLA+R has been sent, NOT ACK received
			//putstring("m+wna ");
			TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWSTO) | (1<<TWEA);
			break;
		
		case 0x50: // Data Byte received, ACK returned 
			//putstring ("ra ");
		case 0x58: // Data Byte received, NOT ACK returned 
			//putstring ("rna ");
			// aktuelles Byte im Buffer sichern
			maiDataBuf[liDataIdx++] = TWDR;
			
			// auf Endebedingung prüfen
			if (miDataBufLen == liDataIdx)
			{
				// received complete message
				// send STOP
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWSTO) | (1<<TWEA);
				miFlags |= TWI_FLAG_MSG_RECEIVED;
				miTWIMode = TWI_MODE_IDLE;
				//sendchar('F');
			}
			else
			{
				// auf weitere Daten warten und mit ACK bestätigen, letztes Byte nicht ACKen
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | ((liDataIdx+1) == miDataBufLen ? 0 : (1<<TWEA));
			}
			break;
			
		// *** Slave Receiver Mode States ***
		
		case 0x60: // Own SLA+W has been received
			//sendchar('S');
			// init Databuffer and wait for data
			liDataIdx = 0;
			TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
			break;
			
		case 0x80: // Data received for slave address, ACK sent
		case 0x88: // Data received for slave address, NOT ACK sent
		case 0x90: // Data received for general call, ACK sent
		case 0x98: // Data received for general call, NOT ACK sent
			// read data byte into buffer
			maiDataBuf[liDataIdx++] = TWDR;
			if (liDataIdx == 1)
			{
				// erstes Byte enthält die Paketlänge
				miDataBufLen = maiDataBuf[0];
			}
			
			if (liDataIdx == miDataBufLen)
			{
				// received complete message
				miFlags |= TWI_FLAG_MSG_RECEIVED;
				miTWIMode = TWI_MODE_IDLE;
				// Enable Ack Bit wieder setzen
				TWCR |= (1<<TWEA);
			}
			else
			{
				// auf weitere Daten warten und mit ACK bestätigen, falls es sich nicht um das letzte Byte handelt
				TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | ((liDataIdx+1) == miDataBufLen ? 0 : (1<<TWEA));
			}

			break;
		
		default:
			// dont know what to do -> clear TWINT flag
			//TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
			TWCR |= (1<<TWINT);
			break;
		
	}
	
}


void fTWI_SendStart(void)
{
		
	// send START
	TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWSTA) | (1<<TWEA) | (1<<TWEN);
	
}

uint8_t twi_busy(void)
{
	return (miTWIMode == TWI_MODE_IDLE ? 0:1);
}

// 7bit piSlaveAddress wird noch ein bit nach rechts geshiftet (001b - 111b oder 1dec - 7dec)
// 400 kHz = piBR = 1
// piBR für schlechte Leitung + LM75 = 50
void twi_init(uint8_t piSlaveAddress, uint8_t piBR)
{

	// Enable Pullups
	PORTC |= (1<<PC0) | (1<<PC1);
	
	// 400 kHz: TWBR = 1
	TWBR = piBR ; //5;
	TWSR &= ~((1<<TWPS1) | (1<<TWPS0));
	//TWSR |= (0<<TWPS1) | (0<<TWPS0);
	
	// Set Slave Address
	TWAR = (piSlaveAddress<<1);

	TWCR = (1<<TWIE) | (1<<TWEN) | (1<<TWEA);
		
	miTWIMode = TWI_MODE_IDLE;
	
}

uint8_t twi_WriteBytesAsync (uint8_t piSlaveAddr, uint8_t *ppiData, uint8_t piDataLen)
{
	
	// Auf Idle testen
	if (miTWIMode != TWI_MODE_IDLE)
		return (1);
	
	// Auf Bufferlänge prüfen
	if (piDataLen>TWI_BUF_LEN)
	{
		// Buffer zu klein -> Fehler
		return (2);
	}
			
	// Daten kopieren
	uint8_t liIdx;
	
	for (liIdx=0; liIdx<piDataLen; liIdx++)
	{
		maiDataBuf[liIdx] = ppiData[liIdx];
	}

	miRemoteSlaveAddr = piSlaveAddr;
	miDataBufLen = piDataLen;
	miTWIMode = TWI_MODE_WRITE;

	fTWI_SendStart();	
	
	return (0);
	
}

uint8_t twi_ReadBytesAsync (uint8_t piSlaveAddr, uint8_t piDataLen)
{
	
	// Auf Idle testen
	if (miTWIMode != TWI_MODE_IDLE)
		return (1);

	// Auf Bufferlänge prüfen
	//if (piDataLen>TWI_BUF_LEN)
	//{
	  // Buffer zu klein -> Fehler
	//	return (2);
	//}
	
	miRemoteSlaveAddr = piSlaveAddr;
	miTWIMode = TWI_MODE_READ;
	miDataBufLen = piDataLen;

	fTWI_SendStart();	
		
	return (0);
	
}

uint8_t twi_getMsg(uint8_t *ppiBuffer, uint8_t piBufLen)
{
	if (miFlags & TWI_FLAG_MSG_RECEIVED)
	{
		miFlags &= ~TWI_FLAG_MSG_RECEIVED;
		// msg available -> copy data to buffer
		uint8_t liIdx;
		
		for (liIdx = 0; liIdx < miDataBufLen; liIdx++)
		{
			ppiBuffer[liIdx] = maiDataBuf[liIdx];
		}
	
		return (miDataBufLen);
	}
	else
	{
		return (0);
	}
}
