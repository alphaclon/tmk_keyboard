/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *  \brief Board specific LED driver header for the PJRC Teensy 1.x/2.x boards.
 *  \copydetails Group_LEDs_91TKL
 *
 *  \note This file should not be included directly. It is automatically included as needed by the LEDs driver
 *        dispatch header located in LUFA/Drivers/Board/LEDs.h.
 */

/** \ingroup Group_LEDs
 *  \defgroup Group_LEDs_91TKL2 91TKL2
 *  \brief Board specific LED driver header for the PJRC Teensy 2 boards.
 *
 *  See \ref Group_LEDs_91TKL for more details.
 */

/** \ingroup Group_LEDs
 *  \defgroup Group_LEDs_91TKL 91TKL
 *  \brief Board specific LED driver header for the PJRC Teensy 1.x/2.x boards.
 *
 *  \note For version 2 Teensy boards, compile with <code>BOARD = 91TKL2</code>.
 *
 *  Board specific LED driver header for the PJRC Teensy boards (http://www.pjrc.com/teensy/index.html).
 *
 *  <b>91TKL</b>:
 *  <table>
 *    <tr><th>Name</th><th>Color</th><th>Info</th><th>Active Level</th><th>Port Pin</th></tr>
 *    <tr><td>LEDS_LED1</td><td>Green</td><td>General Indicator</td><td>High</td><td>PORTD.6</td></tr>
 *  </table>
 *
 *  <b>91TKL2</b>:
 *  <table>
 *    <tr><th>Name</th><th>Color</th><th>Info</th><th>Active Level</th><th>Port Pin</th></tr>
 *    <tr><td>LEDS_LED1</td><td>Green</td><td>General Indicator</td><td>Low</td><td>PORTD.6</td></tr>
 *  </table>
 *
 *  @{
 */

#ifndef __LEDS_91TKL_H__
#define __LEDS_91TKL_H__

	/* Includes: */
		#include "../../../../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_LEDS_H)
			#error Do not include this file directly. Include LUFA/Drivers/Board/LEDS.h instead.
		#endif

	/* Public Interface - May be used in end-application: */

			/*
			 * PB5, PB6: CapsLock + ScrollLock
			 * PE6, PE7: green + yellow info LED
			 * all LEDS are active low
			 *
			 * LED1: PB5, PE6
			 * LED2: PB6, PE7
			 */

		/* Macros: */
			/** LED mask for the first LED on the board. */
			#define LEDS_LED1        1
			#define LEDS_LED2        2

			/** LED mask for all the LEDs on the board. */
			#define LEDS_ALL_LEDS    LEDS_LED1 | LEDS_LED2

			/** LED mask for none of the board LEDs. */
			#define LEDS_NO_LEDS     0

		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void LEDs_Init(void)
			{
				DDRB  |= (1 << 5) | (1 << 6);
				DDRE  |= (1 << 6) | (1 << 7);

				PORTB |=  (1 << 5) | (1 << 6);
				PORTE |=  (1 << 6) | (1 << 7);
			}

			static inline void LEDs_Disable(void)
			{
				PORTB |=  (1 << 6) | (1 << 6);
				PORTE |=  (1 << 6) | (1 << 7);
			}

			static inline void LEDs_TurnOnLEDs(const uint8_t LEDMask)
			{
				if (LEDMask & LEDS_LED1)
				{
					PORTB &=  ~(1 << 5);
					PORTE &=  ~(1 << 6);
				}
				if (LEDMask & LEDS_LED2)
				{
					PORTB &=  ~(1 << 6);
					PORTE &=  ~(1 << 7);
				}
			}

			static inline void LEDs_TurnOffLEDs(const uint8_t LEDMask)
			{
				if (LEDMask & LEDS_LED1)
				{
					PORTB |=  (1 << 5);
					PORTE |=  (1 << 6);
				}
				if (LEDMask & LEDS_LED2)
				{
					PORTB |=  (1 << 6);
					PORTE |=  (1 << 7);
				}
			}

			static inline void LEDs_SetAllLEDs(const uint8_t LEDMask)
			{
				if (LEDMask & LEDS_LED1)
				{
					PORTB &=  ~(1 << 5);
					PORTE &=  ~(1 << 6);
				}
				else
				{
					PORTB |=  (1 << 5);
					PORTE |=  (1 << 6);
				}

				if (LEDMask & LEDS_LED2)
				{
					PORTB &=  ~(1 << 6);
					PORTE &=  ~(1 << 7);
				}
				else
				{
					PORTB |=  (1 << 6);
					PORTE |=  (1 << 7);
				}
			}

			static inline void LEDs_ChangeLEDs(const uint8_t LEDMask,
			                                   const uint8_t ActiveMask)
			{

			}

			static inline void LEDs_ToggleLEDs(const uint8_t LEDMask)
			{
				if (LEDMask & LEDS_LED1)
				{
					PORTB ^=  (1 << 5);
					PORTE ^=  (1 << 6);
				}
				if (LEDMask & LEDS_LED2)
				{
					PORTB ^=  (1 << 6);
					PORTE ^=  (1 << 7);
				}
			}

			static inline uint8_t LEDs_GetLEDs(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t LEDs_GetLEDs(void)
			{
				return 0;
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

