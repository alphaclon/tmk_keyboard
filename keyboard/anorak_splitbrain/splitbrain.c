/*
 * Datenformat für Austausch zwischen linker und rechter Hälft
 *
 * <start> <command> <... payload ...> <crc8> <stop>
 *
 * commands:
 *
 * 0x01 row status from the other side
 * 		payload length: 5 bytes
 * 		byte 1: <row number>
 *      byte 2..5: <row>
 *
 * 0x02 ping
 * 	    payload length: 1 byte
 * 	    byte 1: 0: slave -> not connected to usb, 1: master -> connected to usb
 *
 *
 */

#include <util/delay.h>
#include <avr/io.h>
#include "uart/uart.h"
#include "config.h"
#include "debug.h"
#include "matrix.h"
#include "timer.h"
#include "crc8.h"
#include "splitbrain.h"


#define BAUD 115200 // 9600 14400 19200 38400 57600 115200

/*
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)      // Fehler in Promille, 1000 = kein Fehler.

#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#warning Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
#endif
*/

#define LOW_BYTE(x)        	(x & 0xff)					// 16Bit 	--> 8Bit
#define HIGH_BYTE(x)       	((x >> 8) & 0xff)			// 16Bit 	--> 8Bit

#define DATAGRAM_START 0xAA
#define DATAGRAM_STOP  0xDD
#define DATAGRAM_CMD_ROW  0x01
#define DATAGRAM_CMD_PING 0x02

#define MAX_MSG_LENGTH 16

#define PING_TIME 1000

bool _is_left_side_of_keyboard = false;
bool _is_right_side_of_keyboard = false;
matrix_row_t other_sides_rows[MATRIX_ROWS] = { 0 };
uint16_t last_receive_ts = 0;
uint16_t last_send_ts = 0;
uint8_t recv_buffer[MAX_MSG_LENGTH];
uint8_t send_buffer[MAX_MSG_LENGTH];

enum recvStatus
{
	recvStatusIdle = 0,
	recvStatusFoundStart = 1
};

enum recvStatus recv_status = recvStatusIdle;

struct _row_datagram
{
	uint8_t row_number;
	matrix_row_t row;
};

typedef struct _row_datagram row_datagram;

void splitbrain_config_init()
{
	DDRE &= ~(1 << 0);
	PORTE |= (1 << 0);

	_delay_us(50);

	_is_left_side_of_keyboard = (PINE & (1 << 0)) == 0;
	_is_right_side_of_keyboard = !_is_left_side_of_keyboard;

	dprintf("left side: %d\r\n", _is_left_side_of_keyboard);

	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
}

bool is_left_side_of_keyboard()
{
	return _is_left_side_of_keyboard;
}

bool is_right_side_of_keyboard()
{
	return _is_right_side_of_keyboard;
}

matrix_row_t get_other_sides_row(uint8_t row_number)
{
	return other_sides_rows[row_number];
}

void interpret_command(uint8_t const *buffer, uint8_t length)
{
	if (buffer[1] == DATAGRAM_CMD_ROW)
	{
		row_datagram const *rd = (row_datagram const *)(buffer+2);
		if (rd->row_number < MATRIX_ROWS)
		{
			other_sides_rows[rd->row_number] = rd->row;
		}
	}
	else if (buffer[1] == DATAGRAM_CMD_PING)
	{
		dprintf("recv ping\r\n");
	}

	last_receive_ts = timer_read();
}

bool check_crc(uint8_t const *buffer, uint8_t length)
{
	uint8_t crc = crc8_calc(buffer, 0x2D, length - 2);

	if (crc != buffer[length - 1])
	{
		dprintf("crc error: %x %x\r\n", buffer[length - 1], crc);
		return false;
	}

	return true;
}

void dump_buffer(uint8_t const *buffer, uint8_t length)
{
	for (uint8_t i = 0; i < length; ++i)
	{
		dprintf("%02X ");
	}

	dprintf("\r\n");
}

void receive_data_from_other_side()
{
	static uint8_t receive_buffer[8];
	static uint8_t buffer_pos = 0;
	uint8_t ucData;
	unsigned int rd;

	do
	{
		rd = uart_getc();

		// wenn Zeichen empfangen
		if (HIGH_BYTE(rd) == 0)
		{
			ucData = LOW_BYTE(rd);

			if (ucData == DATAGRAM_START && recv_status == recvStatusIdle)
			{
				buffer_pos = 0;
				recv_status = recvStatusFoundStart;
			}

			if (recv_status == recvStatusFoundStart)
			{
				receive_buffer[buffer_pos] = ucData;
				buffer_pos++;

				if (buffer_pos > MAX_MSG_LENGTH)
				{
					// bail out
					buffer_pos = 0;
					recv_status = recvStatusIdle;

					dprintf("uart error: msg too long\r\n");

					continue;
				}
			}

			if (recv_status == recvStatusFoundStart && ucData == DATAGRAM_STOP)
			{
				// minimal length of a datagram is <start><cmd><crc><stop> = 4 bytes
				if (buffer_pos >= 4)
				{
					dump_buffer(receive_buffer, buffer_pos);
					if (check_crc(receive_buffer, buffer_pos))
					{
						interpret_command(receive_buffer, buffer_pos);
						buffer_pos = 0;
						recv_status = recvStatusIdle;
					}
				}
			}
		}
		else if (HIGH_BYTE(rd) != UART_NO_DATA)
		{
			dprintf("uart error: 0x%X\r\n", HIGH_BYTE(rd));
		}

	} while (HIGH_BYTE(rd) == 0);
}

void uart_send(const uint8_t *s)
{
	while (*s)
	  uart_putc(*s++);
}

uint8_t fill_message_header(uint8_t command)
{
	send_buffer[0] = DATAGRAM_START;
	send_buffer[1] = command;
	return 2;
}

void fill_message_footer(uint8_t pos)
{
	send_buffer[pos] = crc8_calc(send_buffer, 0x2D, pos);
	send_buffer[pos+1] = DATAGRAM_STOP;
	send_buffer[pos+2] = 0;
}

void send_message_to_other_side(void)
{
	uart_send(send_buffer);
	last_send_ts = timer_read();
}

void send_row_to_other_side(uint8_t row_number, matrix_row_t row)
{
	dprintf("send row %u\r\n", row_number);

	uint8_t pos = fill_message_header(DATAGRAM_CMD_ROW);
	row_datagram *rd = (row_datagram *)(send_buffer+pos);

	rd->row_number = row_number;
	rd->row = row;

	fill_message_footer(pos + 5);
	send_message_to_other_side();
}

void send_ping_to_other_side()
{
	dprintf("send ping\r\n");
	uint8_t pos = fill_message_header(DATAGRAM_CMD_PING);
	send_buffer[pos] = is_right_side_of_keyboard();
	fill_message_footer(pos + 1);
	send_message_to_other_side();
}

void validate_communication_to_other_side()
{
	if (timer_elapsed(last_receive_ts) > PING_TIME)
	{
		dprintf("timeout!\r\n");

		for (uint8_t r = 0; r < MATRIX_ROWS; ++r)
			other_sides_rows[r] = 0;
	}

	if (timer_elapsed(last_send_ts) > PING_TIME)
	{
		send_ping_to_other_side();
	}
}
