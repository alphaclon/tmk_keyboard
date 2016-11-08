/*
 Copyright 2012 Jun Wako <wakojun@gmail.com>
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
 * scan matrix
 */
#include "matrix.h"
#include "debug.h"
#include "nfo_led.h"
#include "print.h"
#include "util.h"
#include "timer.h"
#include "debug.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include "backlight/backlight_kiibohd.h"
#include "splitbrain.h"

#ifndef DEBOUNCE_TIME
#define DEBOUNCE_TIME 5
#endif

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];
static uint16_t debouncing_times[MATRIX_ROWS];
static bool debouncing[MATRIX_ROWS] = { false };

static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);

inline uint8_t matrix_rows(void)
{
	return MATRIX_ROWS;
}

inline uint8_t matrix_cols(void)
{
	return MATRIX_COLS;
}

void matrix_setup(void)
{
	// You need to set JTD bit of MCUCR yourself to use PF4-7 as GPIO. Those
	// pins are configured to serve JTAG function by default.
	// MCUs like ATMegaU or AT90USB* are affeteced with this.
	// JTAG disable for PORT F. write JTD bit twice within four cycles.
	MCUCR |= (1 << JTD);
	MCUCR |= (1 << JTD);

	splitbrain_config_init();

    LED_GREEN_INIT();
    LED_YELLOW_INIT();

    LED_YELLOW_ON();
    LED_GREEN_ON();
}

void matrix_init(void)
{
	// initialize row and col
	unselect_rows();
	init_cols();

	// initialize matrix state: all keys off
	for (uint8_t i = 0; i < MATRIX_ROWS; i++)
	{
		matrix[i] = 0;
		matrix_debouncing[i] = 0;
		debouncing_times[i] = 0;
		debouncing[i] = false;
	}

    LED_GREEN_OFF();
    LED_YELLOW_OFF();
}

uint8_t matrix_scan(void)
{
	for (uint8_t i = 0; i < MATRIX_ROWS; i++)
	{
		select_row(i);
		_delay_us(30);  // without this wait read unstable value.
		matrix_row_t cols = read_cols();

#ifndef NO_DEBUG_LEDS
		if (cols)
			LED_YELLOW_ON();
		else
			LED_YELLOW_OFF();
#endif

		if (matrix_debouncing[i] != cols)
		{
			matrix_debouncing[i] = cols;
			debouncing_times[i] = timer_read();
			debouncing[i] = true;
		}

		unselect_rows();
	}

	for (int i = 0; i < MATRIX_ROWS; i++)
	{
		if (debouncing[i])
		{
			LED_GREEN_ON();

			if (timer_elapsed(debouncing_times[i]) > DEBOUNCE_TIME)
			{
				matrix[i] = matrix_debouncing[i];
				debouncing[i] = false;

				send_row_to_other_side(i, matrix[i]);
			}
		}
		else
		{
			LED_GREEN_OFF();
		}
	}

	receive_data_from_other_side();
	send_ping_to_other_side();
	validate_communication_to_other_side();

	return 1;
}

bool matrix_is_modified(void)
{
	// NOTE: no longer used
	for (int i = 0; i < MATRIX_ROWS; i++)
		if (debouncing[i])
			return false;
	return true;
}

inline bool matrix_has_ghost(void)
{
	return false;
}

inline bool matrix_is_on(uint8_t row, uint8_t col)
{
	return ((matrix[row] | get_other_sides_row(row)) & ((matrix_row_t) 1 << col));
}

inline matrix_row_t matrix_get_row(uint8_t row)
{
	return (matrix[row] | get_other_sides_row(row));
}

void matrix_print(void)
{
	print("\nr/c 0123456789012345678\n");
	for (uint8_t row = 0; row < MATRIX_ROWS; row++)
	{
		phex(row);
		print(": ");
		pbin_reverse(matrix_get_row(row));
		print("\n");
	}
}

uint8_t matrix_key_count(void)
{
	uint8_t count = 0;
	for (uint8_t i = 0; i < MATRIX_ROWS; i++)
	{
		count += bitpop32(matrix_get_row(i));
	}
	return count;
}

static void init_cols(void)
{
	// Column pin configuration
	// Total available: 23
	// We use 18 columns
	// Input with pull-up (DDR:0, PORT:1)
	//
	// Columns 1..8 are on Port C 0..7
	// Columns 9..16 are on Port F 0..7
	// Column 17: PA 6
	// Column 18: PA 7
	// Column 19,20,21: PD 4,5,6
	// Column 22: PB 0
	// Column 23: PB 4

	DDRC &= ~0xFF;
	PORTC |= 0xFF;

	DDRF &= ~0xFF;
	PORTF |= 0xFF;

	DDRA &= ~(1 << 7 | 1 << 6);
	PORTA |= (1 << 7 | 1 << 6);

	/*
	DDRD &= ~(1 << 6 | 1 << 5 | 1 << 4);
	PORTD |= (1 << 6 | 1 << 5 | 1 << 4);

	DDRB &= ~(1 << 4 | 1 << 0);
	PORTB |= (1 << 4 | 1 << 0);
	*/
}

static matrix_row_t read_cols(void)
{
	return (PINC |
		   (PINF << 8) |
		   (PINA & (1<<7) ? 0 : ((matrix_row_t)1<<16)) |
		   (PINA & (1<<6) ? 0 : ((matrix_row_t)1<<17)));
}

static void unselect_rows(void)
{
	// ROWs are on Port A, Bin 0..5
	// Hi-Z (DDR:0, PORT:0) to unselect

	DDRA &= ~(0x3F);
	PORTA &= ~(0x3F);
}

static void select_row(uint8_t row)
{
	// Output low (DDR:1, PORT:0) to select
	// ROWs are on Port A, Bin 0..5

	// xprintf("row %d ", row);

	DDRA |= (1 << row);
	PORTA &= ~(1 << row);
}
