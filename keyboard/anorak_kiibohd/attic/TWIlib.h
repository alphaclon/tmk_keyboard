/*
 * TWIlib.h
 *
 * Created: 6/01/2014 10:38:42 PM
 *  Author: Chris Herring
 *  http://www.chrisherring.net/all/tutorial-interrupt-driven-twi-interface-for-avr-part1/
 */


#ifndef TWILIB_H_
#define TWILIB_H_

// Function declarations
uint8_t TWITransmitData(void *const TXdata, uint8_t dataLen, uint8_t repStart);
void TWIInit(void);
uint8_t TWIReadData(uint8_t TWIaddr, uint8_t bytesToRead, uint8_t repStart);
uint8_t TWIReceiveBufferAt(uint8_t pos);
uint8_t isTWIReady(void);

#endif // TWICOMMS_H_
