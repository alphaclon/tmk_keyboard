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
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "led_backlight/backlight_kiibohd.h"
#include "util.h"
#include "matrix.h"
#include "config.h"
#include "print.h"
#include "debug.h"


#ifndef DEBOUNCE
#   define DEBOUNCE	5
#endif
static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);


/*
 * PD5 -> grüne LED, active low
 * PC7 -> gelbe LED, active high
 *
 * #define LED_CONFIG      (DDRD |= (1<<6))
 * #define LED_ON          (PORTD &= ~(1<<6))
 * #define LED_OFF         (PORTD |= (1<<6))
 *
 *
 */

#ifndef NO_DEBUG_LEDS
#define LED_GREEN_INIT()  do { DDRD |= (1<<5); PORTD |= (1<<5); } while (0)
#define LED_GREEN_ON()    do { PORTD &= ~(1<<5); } while (0)
#define LED_GREEN_OFF()   do { PORTD |= (1<<5); } while (0)
#define LED_GREEN_TGL()   do { PIND |= (1<<5); } while (0)

#define LED_YELLOW_INIT()  do { DDRC |= (1<<7); PORTC &= ~(1<<7);} while (0)
#define LED_YELLOW_ON()    do { PORTC |= (1<<7);} while (0)
#define LED_YELLOW_OFF()   do { PORTC &= ~(1<<7); } while (0)
#define LED_YELLOW_TGL()   do { PIND |= (1<<7); } while (0)
#else
#define LED_GREEN_INIT()  do { } while (0)
#define LED_GREEN_ON()    do { } while (0)
#define LED_GREEN_OFF()   do { } while (0)
#define LED_GREEN_TGL()   do { } while (0)

#define LED_YELLOW_INIT()  do { } while (0)
#define LED_YELLOW_ON()    do { } while (0)
#define LED_YELLOW_OFF()   do { } while (0)
#define LED_YELLOW_TGL()   do { } while (0)
#endif

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_setup(void)
{
    // You need to set JTD bit of MCUCR yourself to use PF4-7 as GPIO. Those
    // pins are configured to serve JTAG function by default.
    // MCUs like ATMegaU or AT90USB* are affeteced with this.
    // JTAG disable for PORT F. write JTD bit twice within four cycles.
    MCUCR |= (1<<JTD);
    MCUCR |= (1<<JTD);

    LED_GREEN_INIT();
    LED_YELLOW_INIT();

    LED_YELLOW_ON();
    LED_GREEN_ON();
}

void matrix_init(void)
{
    backlight_initialize_regions();

    // initialize row and col
    unselect_rows();
    init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
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

        if (cols) LED_YELLOW_ON();
        else LED_YELLOW_OFF();

        if (matrix_debouncing[i] != cols)
        {
            matrix_debouncing[i] = cols;
            if (debouncing)
            {
                debug("bounce!: ");
                debug_hex(debouncing);debug("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect_rows();
    }

    if (debouncing)
    {
        LED_GREEN_ON();
        if (--debouncing)
        {
            _delay_ms(1);
        }
        else
        {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++)
            {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }
    else
    {
        LED_GREEN_OFF();
    }

    return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing) return false;
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789012345678\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

static void  init_cols(void)
{
    // Column pin configuration
    // Input with pull-up (DDR:0, PORT:1)
    // COL 1: PB5  5
    // COL 2: PB4  4
    // COL 3: PE6  6
    // COL 4: PD7  7
    // COL 5: PC6  6
    // COL 6: PD4  4

    DDRB &= ~(1<<5 | 1 << 4);
    PORTB |= (1<<5 | 1 << 4);

    DDRE &= ~(1<<6);
    PORTE |= (1<<6);

    DDRD &= ~(1<<7 | 1 << 4 | 1<<3);
    PORTD |= (1<<7 | 1 << 4 | 1<<3);

    DDRC &= ~(1<<6);
    PORTC |= (1<<6);
}

static matrix_row_t read_cols(void)
{
    return ( ((PINB&(1<<5)) ? 0 : (1<<0)) |
             ((PINB&(1<<4)) ? 0 : (1<<1)) |
             ((PINE&(1<<6)) ? 0 : (1<<2)) |
             ((PIND&(1<<7)) ? 0 : (1<<3)) |
             ((PINC&(1<<6)) ? 0 : (1<<4)) |
             ((PIND&(1<<4)) ? 0 : (1<<5)) |
             ((PIND&(1<<3)) ? 0 : (1<<6)) );
}

static void unselect_rows(void)
{
    // Hi-Z (DDR:0, PORT:0) to unselect

    DDRF &= ~(1<<7 | 1<<6);
    PORTF &= ~(1<<7 | 1<<6);

    DDRB &= ~(1<<7 | 1<<6);
    PORTB &= ~(1<<7 | 1<<6);

    DDRD &= ~(1<<6);
    PORTD &= ~(1<<6);
}

static void select_row(uint8_t row)
{
    // Output low (DDR:1, PORT:0) to select
    // ROW 1: PF7
    // ROW 2: PF6
    // ROW 3: PB6
    // ROW 4: PB7
    // ROW 5: PD6

    //xprintf("row %d ", row);

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
