#include "uart/uart.h"
#include <avr/io.h>
#include "config.h"
#include "matrix.h"
#include "splitbrain.h"

#define BAUD 38400 // 9600 14400 19200 38400 57600 115200

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.

#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
#endif

#define LOW_BYTE(x)        	(x & 0xff)					// 16Bit 	--> 8Bit
#define HIGH_BYTE(x)       	((x >> 8) & 0xff)			// 16Bit 	--> 8Bit

bool _is_left_side_of_keyboard = false;
matrix_row_t rows[MATRIX_ROWS] = { 0 };

void splitbrain_config_init()
{
	DDRE &= ~(1 << 0);
	PORTE |= (1 << 0);

	_is_left_side_of_keyboard = (PINE & (1 << 0));

	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
}

bool is_left_side_of_keyboard()
{
	return _is_left_side_of_keyboard;
}

void receive_data_from_other_side()
{
	static uint8_t row_buffer[8];
	uint8_t buffer_pos = 0;
	uint8_t ucData;
	unsigned int rd;

	do
	{
		rd = uart_getc();

		// wenn Zeichen empfangen
		if (HIGH_BYTE(rd) == 0)
		{
			ucData = LOW_BYTE(rd);

			row_buffer[buffer_pos] = ucData;
			buffer_pos++;
		}

	} while (HIGH_BYTE(rd) == 0);
}
