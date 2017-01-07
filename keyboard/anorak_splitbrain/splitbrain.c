/*
Copyright 2016 Moritz Wenk <MoritzWenk@web.de>


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Datenformat für Austausch zwischen linker und rechter Hälft
 *
 * <start> <length> <command> <... payload ...> <crc8> <stop>
 *
 * commands:
 *
 * 0x01 init
 *
 * 0x02 row
 *      send row status from the other side
 * 		payload length: 5 bytes
 * 		byte 1: <row number>
 *      byte 2..5: <row>
 *
 * 0x03 ping
 * 	    payload length: 1 byte
 * 	    byte 1: 0: slave -> not connected to usb, 1: master -> connected to usb
 *
 *
 * 0x04 sync
 *
 */

#include "splitbrain.h"
#include "LUFA/Drivers/USB/USB.h"
#include "action.h"
#include "backlight.h"
#include "backlight/backlight_kiibohd.h"
#include "backlight/eeconfig_backlight.h"
#include "config.h"
#include "crc8.h"
#include "eeconfig.h"
#include "matrix.h"
#include "nfo_led.h"
#include "timer.h"
#include "uart/uart.h"
#include <avr/io.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>

#ifdef DEBUG_SPLITBRAIN
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define BAUD 115200 // 9600 14400 19200 38400 57600 115200

/*
 #define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
 #define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
 #define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)      // Fehler in Promille, 1000 = kein Fehler.

 #if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
 #warning Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
 #endif
 */

#define LOW_BYTE(x) (x & 0xff)         // 16Bit 	--> 8Bit
#define HIGH_BYTE(x) ((x >> 8) & 0xff) // 16Bit 	--> 8Bit

#define PROTOCOL_VERSION 2

//#define DATAGRAM_START 0x41
//#define DATAGRAM_STOP 0x45

#define DATAGRAM_START 0x02
#define DATAGRAM_STOP 0x03

#define DATAGRAM_CMD_CONNECT 0x49
#define DATAGRAM_CMD_CONNECT_ACK 0x48
#define DATAGRAM_CMD_PING 0x50
#define DATAGRAM_CMD_ROW 0x52
#define DATAGRAM_CMD_ROW_ACK 0x51
#define DATAGRAM_CMD_SYNC 0x53
#define DATAGRAM_CMD_SLEEP 0x54
#define DATAGRAM_CMD_CMD 0x60

#define MAX_MSG_LENGTH 16

#define INIT_TIMEOUT 250
#define PING_TIMEOUT 500
#define CONNECTION_TIMEOUT (PING_TIMEOUT * 2 + (PING_TIMEOUT / 2))
#define ROW_ACK_TIMEOUT 10

#define MAX_ROW_RESEND 3

#ifdef DEBUG_SPLITBRAIN
#undef INIT_TIMEOUT
#define INIT_TIMEOUT 5000
#undef PING_TIMEOUT
#define PING_TIMEOUT 10000
#endif

bool _is_left_side_of_keyboard = false;
bool _is_right_side_of_keyboard = false;
bool _is_connected_to_other_side = false;
bool _waiting_for_row_ack = false;
bool _is_other_side_connected_to_usb = false;
bool _is_other_side_sleeping = false;

uint16_t last_receive_ts = 0;
uint16_t last_send_ts = 0;
uint16_t last_init_send_ts = 0;
uint16_t last_row_send_ts = 0;

uint8_t last_row_number = 0;
matrix_row_t last_row = 0;
uint16_t row_resend_counter = 0;
uint8_t row_resend_count = 0;

matrix_row_t other_sides_rows[MATRIX_ROWS];

uint8_t recv_buffer[MAX_MSG_LENGTH];
uint8_t send_buffer[MAX_MSG_LENGTH];

enum recvStatus
{
    recvStatusIdle = 0,
    recvStatusFoundStart = 1,
    recvStatusRecvPayload = 2,
    recvStatusFindStop = 3
};

enum recvStatus recv_status = recvStatusIdle;

struct _row_datagram
{
    uint8_t row_number;
    matrix_row_t row;
};

typedef struct _row_datagram row_datagram;

void splitbrain_get_my_side(void);
void send_connect_request_to_other_side(void);
void send_connect_ack_to_other_side(void);
bool is_connected_to_usb(void);
char is_connected_to_usb_as_char(void);
bool accept_connection_request(uint8_t usb, uint8_t side, uint8_t protocol_version);
char this_side_as_char(void);
void send_row_ack_to_other_side(bool ack);
void resend_row_to_other_side(void);
void reset_connection_on_timeout(void);
void reset_other_sides_rows(void);

void splitbrain_init()
{
    _is_left_side_of_keyboard = false;
    _is_right_side_of_keyboard = false;
    _is_connected_to_other_side = false;
    _waiting_for_row_ack = false;
    _is_other_side_connected_to_usb = false;
    _is_other_side_sleeping = false;

    last_receive_ts = 0;
    last_send_ts = 0;
    last_init_send_ts = 0;
    last_row_send_ts = 0;

    last_row_number = 0;
    last_row = 0;
    row_resend_counter = 0;
    row_resend_count = 0;

    reset_other_sides_rows();
    last_init_send_ts = timer_read() - INIT_TIMEOUT;
    uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
    splitbrain_get_my_side();
    recv_status = recvStatusIdle;
}

void splitbrain_get_my_side()
{
    // Port PE0 is HIGH for right side and LOW for left side

    DDRE &= ~(1 << 0);
    PORTE |= (1 << 0);

    _delay_us(50);

    _is_left_side_of_keyboard = ((PINE & (1 << 0)) == 0);
    _is_right_side_of_keyboard = !_is_left_side_of_keyboard;
}

bool is_left_side_of_keyboard()
{
    return _is_left_side_of_keyboard;
}

bool is_right_side_of_keyboard()
{
    return _is_right_side_of_keyboard;
}

bool has_usb(void)
{
    USBCON |= (1 << OTGPADE); // enables VBUS pad
    _delay_us(5);
    return (USBSTA & (1 << VBUS)); // checks state of VBUS
}

bool is_connected_to_usb()
{
    return (USB_DeviceState == DEVICE_STATE_Configured);
}

bool is_other_side_connected_to_usb(void)
{
    return _is_other_side_connected_to_usb;
}

bool is_other_side_sleeping(void)
{
    return _is_other_side_sleeping;
}

char is_connected_to_usb_as_char()
{
    return (is_connected_to_usb() ? 'C' : 'D');
}

char this_side_as_char()
{
    return (_is_left_side_of_keyboard ? 'L' : 'R');
}

bool accept_connection_request(uint8_t usb, uint8_t other_side, uint8_t protocol_version)
{
    char this_side = this_side_as_char();

    dprintf("con side: %c, other: %c\r\n", this_side, other_side);
    dprintf("con  usb: %c, other: %c\r\n", is_connected_to_usb_as_char(), usb);
    dprintf("con prot: %d, other: %d\r\n", PROTOCOL_VERSION, protocol_version);

    bool side_matches = (other_side != this_side);
    bool connect_matches = (usb != is_connected_to_usb_as_char());
    bool protocol_matches = (PROTOCOL_VERSION == protocol_version);

    return (side_matches && connect_matches && protocol_matches);
}

void accept_sync_request(uint8_t const *buffer, uint8_t length)
{
#ifdef BACKLIGHT_ENABLE
    uint8_t backlight = buffer[0];
    if (backlight != eeconfig_read_backlight())
    {
        dprintf("sync: fix backlight %u %u\r\n", backlight, eeconfig_read_backlight());
        eeconfig_write_backlight(backlight);
    }

    uint8_t regions = buffer[1];
    if (regions != eeconfig_read_backlight_regions())
    {
        dprintf("sync: fix regions %u %u\r\n", regions, eeconfig_read_backlight_regions());
        eeconfig_write_backlight_regions(regions);
    }

    for (uint8_t i = 0; i < BACKLIGHT_MAX_REGIONS; i++)
    {
        uint8_t region_brightness = buffer[i + 2];
        if (region_brightness != eeconfig_read_backlight_region_brightness(i))
        {
            dprintf("sync: fix brightness %u %u\r\n", region_brightness, eeconfig_read_backlight_region_brightness(i));
            eeconfig_write_backlight_region_brightness(i, region_brightness);
        }
    }
#endif
}

void reset_other_sides_rows()
{
    for (uint8_t r = 0; r < MATRIX_ROWS; ++r)
        other_sides_rows[r] = 0;
}

matrix_row_t get_other_sides_row(uint8_t row_number)
{
    return other_sides_rows[row_number];
}

uint8_t get_datagram_cmd(uint8_t const *buffer)
{
    return buffer[2];
}

void interpret_command(uint8_t const *buffer, uint8_t length)
{
    uint8_t cmd = get_datagram_cmd(buffer);
    // dprintf("cmd: %x %c, l:%u\r\n", cmd, cmd, length);

    if (cmd == DATAGRAM_CMD_CONNECT)
    {
        dprintf("recv connect\r\n");

        if (accept_connection_request(buffer[3], buffer[4], buffer[5]))
        {
            _is_connected_to_other_side = true;
            send_connect_ack_to_other_side();
            dprintf("connect success!\r\n");
        }
        else
        {
            dprintf("connect failed!\r\n");
        }
    }
    else if (cmd == DATAGRAM_CMD_CONNECT_ACK)
    {
        uint16_t rtt = timer_elapsed(last_init_send_ts);

        _is_connected_to_other_side = true;
        _is_other_side_connected_to_usb = false;

        send_sync_to_other_side();

        backlight_load_region_states();
        backlight_init();

        dprintf("connect ACKed! rtt %u\r\n", rtt);
    }
    else if (cmd == DATAGRAM_CMD_ROW)
    {
        row_datagram const *rd = (row_datagram const *)(buffer + 3);
        dprintf("recv row %u\r\n", rd->row_number);
        if (rd->row_number < MATRIX_ROWS)
            other_sides_rows[rd->row_number] = rd->row;
        send_row_ack_to_other_side(rd->row_number < MATRIX_ROWS);
    }
    else if (cmd == DATAGRAM_CMD_ROW_ACK)
    {
        uint16_t rtt = timer_elapsed(last_row_send_ts);
        _waiting_for_row_ack = false;
        uint8_t ack = buffer[3];

        dprintf("row ACK: %c, rtt %u\r\n", ack, rtt);

        if (ack != 'A')
        {
            resend_row_to_other_side();
        }
    }
    else if (cmd == DATAGRAM_CMD_PING)
    {
        // dprintf("recv ping %u\r\n", timer_read());
    }
    else if (cmd == DATAGRAM_CMD_SYNC)
    {
        dprintf("recv sync\r\n");
        accept_sync_request(buffer + 3, length - 3);
        _is_other_side_connected_to_usb = true;
    }
    else if (cmd == DATAGRAM_CMD_SLEEP)
    {
        bool sleep = buffer[3];
        dprintf("recv sleep %u\r\n", sleep);

        if (sleep)
        {
            _is_other_side_sleeping = true;
            // backlight_sleep_led_enable();
            // USB_DeviceState = DEVICE_STATE_Suspended;
        }
        else
        {
            _is_other_side_sleeping = false;
            // backlight_sleep_led_enable();
            // USB_DeviceState = DEVICE_STATE_Unattached;
        }
    }
    else if (cmd == DATAGRAM_CMD_CMD)
    {
        dprintf("recv cmd\r\n");

        char *cmd = (char *)(buffer + 3);
        cmd[length] = '\0';
        dprintf("[%s]\r\n", cmd);

        // TODO: do something with this command
    }

    last_receive_ts = timer_read();
}

void dump_buffer(uint8_t const *buffer, uint8_t length)
{
    dprintf("[");
    for (uint8_t i = 0; i < length; ++i)
        dprintf("%02X ", buffer[i]);
    dprintf("]\r\n");
}

bool check_crc(uint8_t const *buffer, uint8_t length)
{
    uint8_t crc = crc8_calc(buffer, 0x2D, length - 1);

    if (crc != buffer[length - 1])
    {
        dprintf("crc error: [%X] [%X]\r\n", crc, buffer[length - 1]);
        dprintf("msg: ");
        dump_buffer(buffer, length);
        return false;
    }

    return true;
}

bool might_be_a_datagram(uint8_t buffer_pos, uint8_t payload_length)
{
    // minimal length of a datagram is <start><cmd><crc><stop> = 4 bytes
    return (buffer_pos >= payload_length);
}

bool datagram_is_valid(uint8_t buffer_pos, uint8_t expected_length)
{
    return (buffer_pos == expected_length && check_crc(recv_buffer, buffer_pos));
}

void receive_data_from_other_side()
{
    static uint8_t buffer_pos = 0;
    static uint8_t expected_length = 0;
    uint8_t ucData;
    unsigned int rd;

    do
    {
        rd = uart_getc();

        /*
         * lower byte:  received byte from ring buffer
         * higher byte: last receive error
         */

        if (HIGH_BYTE(rd) == 0)
        {
            LedInfo2_On();
            ucData = LOW_BYTE(rd);
            // dprintf("recv: [%02X] s:%u\r\n", ucData, recv_status);

            if (ucData == DATAGRAM_START && recv_status == recvStatusIdle)
            {
                // dprintf("recv: start\r\n");

                buffer_pos = 0;

                recv_buffer[buffer_pos] = ucData;
                buffer_pos++;

                recv_status = recvStatusFoundStart;
            }

            else if (recv_status == recvStatusFoundStart)
            {
                // start + len + cmd + payload + crc
                expected_length = ucData + 4;

                // dprintf("recv: expected len %u\r\n", expected_length);

                recv_buffer[buffer_pos] = ucData;
                buffer_pos++;

                recv_status = recvStatusRecvPayload;

                if (expected_length >= MAX_MSG_LENGTH)
                {
                    // bail out
                    buffer_pos = 0;
                    recv_status = recvStatusIdle;
                    dprintf("recv: payload too long\r\n");
                }
            }

            else if (recv_status == recvStatusRecvPayload && buffer_pos < expected_length)
            {
                // dprintf("recv: bpos %u\r\n", buffer_pos);

                recv_buffer[buffer_pos] = ucData;
                buffer_pos++;

                if (buffer_pos >= expected_length)
                    recv_status = recvStatusFindStop;
            }

            else if (recv_status == recvStatusFindStop && ucData == DATAGRAM_STOP)
            {
                // dprintf("recv: stop %u\r\n", buffer_pos);

                if (datagram_is_valid(buffer_pos, expected_length))
                {
                    // dprintf("recv: ");
                    // dump_buffer(recv_buffer, buffer_pos);
                    interpret_command(recv_buffer, buffer_pos);
                }
                else
                {
                    if (get_datagram_cmd(recv_buffer) == DATAGRAM_CMD_ROW)
                    {
                        send_row_ack_to_other_side(false);
                    }
                }

                buffer_pos = 0;
                recv_status = recvStatusIdle;
            }

            if ((recv_status == recvStatusRecvPayload || recv_status == recvStatusFindStop) &&
                (buffer_pos > expected_length || buffer_pos >= MAX_MSG_LENGTH))
            {
                buffer_pos = 0;
                expected_length = 0;
                recv_status = recvStatusIdle;
                dprintf("recv: msg invalid: ");
                dump_buffer(recv_buffer, buffer_pos);
            }

            LedInfo2_Off();
        }
        else if ((rd & UART_NO_DATA) != UART_NO_DATA)
        {
            dprintf("uart error: 0x%X\r\n", HIGH_BYTE(rd));
        }
    } while (HIGH_BYTE(rd) == 0);
}

void uart_send(uint8_t const *data, uint8_t length)
{
    // dprintf("send ");
    // dump_buffer(data, length);

    for (uint8_t i = 0; i < length; ++i)
        uart_putc(*data++);
}

uint8_t fill_message_header(uint8_t command, uint8_t length)
{
    send_buffer[0] = DATAGRAM_START;
    send_buffer[1] = length;
    send_buffer[2] = command;
    return 3;
}

uint8_t fill_message_footer(uint8_t pos)
{
    send_buffer[pos] = crc8_calc(send_buffer, 0x2D, pos);
    send_buffer[pos + 1] = DATAGRAM_STOP;
    send_buffer[pos + 2] = 0;
    return pos + 2;
}

void send_message_to_other_side(uint8_t length)
{
    LedInfo1_On();
    uart_send(send_buffer, length);
    last_send_ts = timer_read();
    LedInfo2_Off();
}

void send_connect_request_to_other_side()
{
    // dprintf("send init\r\n");
    uint8_t pos = fill_message_header(DATAGRAM_CMD_CONNECT, 3);
    send_buffer[pos++] = is_connected_to_usb_as_char();
    send_buffer[pos++] = this_side_as_char();
    send_buffer[pos++] = PROTOCOL_VERSION;
    pos = fill_message_footer(pos);
    send_message_to_other_side(pos);
    last_init_send_ts = timer_read();
}

void send_connect_ack_to_other_side()
{
    dprintf("send init ack\r\n");
    uint8_t pos = fill_message_header(DATAGRAM_CMD_CONNECT_ACK, 0);
    pos = fill_message_footer(pos);
    send_message_to_other_side(pos);
    last_init_send_ts = timer_read();
}

void send_ping_to_other_side()
{
    if (!_is_connected_to_other_side)
        return;

    // dprintf("send ping\r\n");
    uint8_t pos = fill_message_header(DATAGRAM_CMD_PING, 0);
    // send_buffer[pos++] = is_connected_to_usb() ? 'C' : 'D';
    // send_buffer[pos++] = is_right_side_of_keyboard() ? 'R' : 'L';
    pos = fill_message_footer(pos);
    send_message_to_other_side(pos);
}

void send_sleep_to_other_side(bool sleep)
{
    if (!_is_connected_to_other_side || !is_connected_to_usb())
        return;

    dprintf("send sleep %u\r\n", sleep);
    uint8_t pos = fill_message_header(DATAGRAM_CMD_SLEEP, 1);
    send_buffer[pos++] = sleep;
    pos = fill_message_footer(pos);
    send_message_to_other_side(pos);
    last_init_send_ts = timer_read();
}

void send_sync_to_other_side()
{
    if (!_is_connected_to_other_side)
        return;

    dprintf("send sync\r\n");

#ifdef BACKLIGHT_ENABLE
    uint8_t pos = fill_message_header(DATAGRAM_CMD_SYNC, 1 + 1 + BACKLIGHT_MAX_REGIONS);
    dprintf("sync: bl:%u\r\n", eeconfig_read_backlight());
    send_buffer[pos++] = eeconfig_read_backlight();
    send_buffer[pos++] = eeconfig_read_backlight_regions();
    dprintf("sync: rg:%u\r\n", eeconfig_read_backlight_regions());
    for (uint8_t i = 0; i < BACKLIGHT_MAX_REGIONS; i++)
    {
        dprintf("sync: rg%u:%u\r\n", i, eeconfig_read_backlight_region_brightness(i));
        send_buffer[pos++] = eeconfig_read_backlight_region_brightness(i);
    }
#else
    uint8_t pos = fill_message_header(DATAGRAM_CMD_SYNC, 0);
#endif
    pos = fill_message_footer(pos);
    send_message_to_other_side(pos);
}

void send_row_to_other_side(uint8_t row_number, matrix_row_t row)
{
    if (!_is_connected_to_other_side)
        return;

    dprintf("send row %u\r\n", row_number);

    last_row_number = row_number;
    last_row = row;

    uint8_t pos = fill_message_header(DATAGRAM_CMD_ROW, sizeof(row_datagram));
    row_datagram *rd = (row_datagram *)(send_buffer + pos);

    rd->row_number = row_number;
    rd->row = row;

    row_resend_count = MAX_ROW_RESEND;
    _waiting_for_row_ack = true;
    last_row_send_ts = timer_read();

    pos = fill_message_footer(pos + sizeof(row_datagram));
    send_message_to_other_side(pos);
}

void send_row_ack_to_other_side(bool ack)
{
    dprintf("send row ack %c\r\n", (ack ? 'A' : 'N'));
    uint8_t pos = fill_message_header(DATAGRAM_CMD_ROW_ACK, 1);
    send_buffer[pos++] = ack ? 'A' : 'N';
    pos = fill_message_footer(pos);
    send_message_to_other_side(pos);
    last_init_send_ts = timer_read();
}

void send_command_to_other_side(char const *cmd)
{
    if (!_is_connected_to_other_side)
        return;

    dprintf("send cmd\r\n");
    uint8_t len = strlen(cmd);
    uint8_t pos = fill_message_header(DATAGRAM_CMD_ROW_ACK, len);
    strncpy((char *)(send_buffer + pos), cmd, len);
    pos = fill_message_footer(pos + len);
    send_message_to_other_side(pos);
    last_init_send_ts = timer_read();
}

void resend_row_to_other_side()
{
    if (row_resend_count)
    {
        row_resend_count--;
    }
    else
    {
        return;
    }

    row_resend_counter++;
    dprintf("resend row! %u\r\n", row_resend_counter);
    send_row_to_other_side(last_row_number, last_row);
}

void reset_connection_on_timeout()
{
    dprintf("connection broken!\r\n");
    _is_connected_to_other_side = false;
    _is_other_side_connected_to_usb = false;
    _waiting_for_row_ack = false;
    reset_other_sides_rows();
    last_receive_ts = timer_read();
}

void communication_watchdog()
{
    if (_is_connected_to_other_side)
    {
        if (timer_elapsed(last_receive_ts) > CONNECTION_TIMEOUT)
        {
            reset_connection_on_timeout();
        }

        if (timer_elapsed(last_send_ts) > PING_TIMEOUT)
        {
            send_ping_to_other_side();
        }

        if (_waiting_for_row_ack && row_resend_count && timer_elapsed(last_row_send_ts) > ROW_ACK_TIMEOUT)
        {
            resend_row_to_other_side();
        }
    }
    else
    {
        if (is_connected_to_usb() && timer_elapsed(last_init_send_ts) > INIT_TIMEOUT)
        {
            send_connect_request_to_other_side();
        }
    }
}

void splitbrain_communication_task()
{
    receive_data_from_other_side();
    communication_watchdog();
}
