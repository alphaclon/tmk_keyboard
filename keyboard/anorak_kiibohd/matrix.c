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
#include "config.h"
#include "debug.h"
#include "led_backlight/animations/animation.h"
#include "led_backlight/backlight_kiibohd.h"
#include "nfo_led.h"
#include "print.h"
#include "timer.h"
#include "util.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

#ifndef DEBOUNCE_TIME
#define DEBOUNCE_TIME 5
#endif

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];
static uint16_t debouncing_times[MATRIX_ROWS];
static bool debouncing[MATRIX_ROWS] = {false};

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

    LedInfo1_Init();
    LedInfo2_Init();

    LedInfo2_On();
    LedInfo1_On();
}

void matrix_init(void)
{
    // initialize row and col
    matrix_clear();

    LedInfo1_Off();
    LedInfo2_Off();
}

void matrix_clear(void)
{
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
}

uint8_t matrix_scan(void)
{
    for (uint8_t row = 0; row < MATRIX_ROWS; row++)
    {
        select_row(row);
        _delay_us(15); // without this wait read unstable value.
        matrix_row_t cols = read_cols();

        if (cols)
            LedInfo2_On();
        else
            LedInfo2_Off();

        if (matrix_debouncing[row] != cols)
        {
            // dprintf("bounce %u\r\n", row);

            matrix_debouncing[row] = cols;
            debouncing_times[row] = timer_read();
            debouncing[row] = true;
        }

        unselect_rows();
    }

#if 0
    if (debouncing)
    {
        LedInfo1_On();

    	if (timer_elapsed(debouncing_time) > DEBOUNCE_TIME)
    	{
			for (int i = 0; i < MATRIX_ROWS; i++)
			{
				matrix[i] = matrix_debouncing[i];
			}

			debouncing = false;
    	}
    }
    else
    {
        LedInfo1_Off();
    }
#endif

    for (uint8_t row = 0; row < MATRIX_ROWS; row++)
    {
        if (debouncing[row])
        {
            LedInfo1_On();

            if (timer_elapsed(debouncing_times[row]) > DEBOUNCE_TIME)
            {
                // dprintf("bounced %u\r\n", row);

                matrix[row] = matrix_debouncing[row];
                debouncing[row] = false;

                animation_typematrix_row(row, matrix[row]);
            }
        }
        else
        {
            LedInfo1_Off();
        }
    }

#ifdef BACKLIGHT_ENABLE
    animate();
#endif

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
    return (matrix[row] & ((matrix_row_t)1 << col));
}

inline matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456\n");
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
        count += bitpop16(matrix[i]);
    }
    return count;
}

static void init_cols(void)
{
    // Column pin configuration
    // Input with pull-up (DDR:0, PORT:1)
    // COL 1: PB5  5
    // COL 2: PB4  4
    // COL 3: PE6  6
    // COL 4: PD7  7
    // COL 5: PC6  6
    // COL 6: PD4  4
    // COL 7: PD3  3

    DDRB &= ~(1 << 5 | 1 << 4);
    PORTB |= (1 << 5 | 1 << 4);

    DDRE &= ~(1 << 6);
    PORTE |= (1 << 6);

#ifndef DEBUG_LUFA_UART
    DDRD &= ~(1 << 7 | 1 << 4 | 1 << 3);
    PORTD |= (1 << 7 | 1 << 4 | 1 << 3);
#else
    DDRD &= ~(1 << 7 | 1 << 4);
    PORTD |= (1 << 7 | 1 << 4);
#endif

    DDRC &= ~(1 << 6);
    PORTC |= (1 << 6);
}

static matrix_row_t read_cols(void)
{
#ifndef DEBUG_LUFA_UART
    return (((PINB & (1 << 5)) ? 0 : (1 << 0)) | ((PINB & (1 << 4)) ? 0 : (1 << 1)) |
            ((PINE & (1 << 6)) ? 0 : (1 << 2)) | ((PIND & (1 << 7)) ? 0 : (1 << 3)) |
            ((PINC & (1 << 6)) ? 0 : (1 << 4)) | ((PIND & (1 << 4)) ? 0 : (1 << 5)) |
            ((PIND & (1 << 3)) ? 0 : (1 << 6)));
#else
    return (((PINB & (1 << 5)) ? 0 : (1 << 0)) | ((PINB & (1 << 4)) ? 0 : (1 << 1)) |
            ((PINE & (1 << 6)) ? 0 : (1 << 2)) | ((PIND & (1 << 7)) ? 0 : (1 << 3)) |
            ((PINC & (1 << 6)) ? 0 : (1 << 4)) | ((PIND & (1 << 4)) ? 0 : (1 << 5)));

#endif
}

static void unselect_rows(void)
{
    // Hi-Z (DDR:0, PORT:0) to unselect

    DDRF &= ~(1 << 7 | 1 << 6);
    PORTF &= ~(1 << 7 | 1 << 6);

    DDRB &= ~(1 << 7 | 1 << 6);
    PORTB &= ~(1 << 7 | 1 << 6);

    DDRD &= ~(1 << 6);
    PORTD &= ~(1 << 6);
}

static void select_row(uint8_t row)
{
    // Output low (DDR:1, PORT:0) to select
    // ROW 1: PF7
    // ROW 2: PF6
    // ROW 3: PB6
    // ROW 4: PB7
    // ROW 5: PD6

    // xprintf("row %d ", row);

    switch (row)
    {
    case 0:
        DDRF |= (1 << 7);
        PORTF &= ~(1 << 7);
        break;
    case 1:
        DDRF |= (1 << 6);
        PORTF &= ~(1 << 6);
        break;
    case 2:
        DDRB |= (1 << 6);
        PORTB &= ~(1 << 6);
        break;
    case 3:
        DDRB |= (1 << 7);
        PORTB &= ~(1 << 7);
        break;
    case 4:
        DDRD |= (1 << 6);
        PORTD &= ~(1 << 6);
        break;
    }
}
