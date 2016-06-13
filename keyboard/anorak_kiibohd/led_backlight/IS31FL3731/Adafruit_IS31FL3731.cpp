
#include "Adafruit_IS31FL3731.h"
#include <util/delay.h>

extern "C" {
#ifdef USE_BUFFERED_TWI
#include "../../twi/i2c.h"
#else
#include "../../i2cmaster/i2cmaster.h"
#endif
}

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

/* Constructor */
Adafruit_IS31FL3731::Adafruit_IS31FL3731(uint8_t x, uint8_t y) : Adafruit_GFX(x, y)
{
    _i2caddr = 0;
    _frame = 0;
}

Adafruit_IS31FL3731::~Adafruit_IS31FL3731()
{
}

bool Adafruit_IS31FL3731::begin(uint8_t addr)
{
    _i2caddr = (addr << 1);
    _frame = 0;

    // shutdown
    setSoftwareShutdown(1);

    _delay_ms(1);

    // picture mode
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

    displayFrame(_frame);
    audioSync(false);

    for (uint8_t f = 0; f < ISSI_TOTAL_FRAMES; f++)
    {
        for (uint8_t i = 0; i <= 0x11; i++)
            writeRegister8(f, i, 0x0); // each 8 LEDs off
    }

    // all LEDs on & PWM
    for (uint8_t f = 0; f < ISSI_TOTAL_FRAMES; f++)
    {
        for (uint8_t i = 0; i < ISSI_TOTAL_CHANNELS; i++)
            setLEDPWM(i, 0, f); // set each led to the default PWM
    }

    // out of shutdown
    setSoftwareShutdown(0);

    return true;
}

void Adafruit_IS31FL3731::setSoftwareShutdown(uint8_t shutdown)
{
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, shutdown ? 0x00 : 0x01);
}

void Adafruit_IS31FL3731::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    // check rotation, move pixel around if necessary
    switch (getRotation())
    {
    case 1:
        _swap_int16_t(x, y);
        x = 16 - x - 1;
        break;
    case 2:
        x = 16 - x - 1;
        y = 9 - y - 1;
        break;
    case 3:
        _swap_int16_t(x, y);
        y = 9 - y - 1;
        break;
    }

    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;
    if (color > 255)
        color = 255; // PWM 8bit max

    setLEDPWM(x + y * 16, color, _frame);
}

void Adafruit_IS31FL3731::setFrame(uint8_t f)
{
    _frame = f;
}

void Adafruit_IS31FL3731::displayFrame(uint8_t f)
{
    if (f > 7)
        f = 0;
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, f);
}

void Adafruit_IS31FL3731::selectBank(uint8_t bank)
{
#ifdef USE_BUFFERED_TWI
    uint8_t cmd[2] = {ISSI_COMMANDREGISTER, bank};
    i2cMasterSendNI(_i2caddr, 2, cmd);
#else
    i2c_start_wait(_i2caddr + I2C_WRITE);
    i2c_write(ISSI_COMMANDREGISTER);
    i2c_write(bank);
    i2c_stop();
#endif

    /*
     Wire.beginTransmission(_i2caddr);
     Wire.write((byte)ISSI_COMMANDREGISTER);
     Wire.write(bank);
     Wire.endTransmission();
     */
}

void Adafruit_IS31FL3731::audioSync(bool sync)
{
    if (sync)
    {
        writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1);
    }
    else
    {
        writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
    }
}

void Adafruit_IS31FL3731::setLEDEnable(uint8_t lednum, uint8_t enable, uint8_t bank)
{
    if (lednum >= 144)
        return;

    uint8_t ledreg = lednum / 8;
    uint8_t leddata = readRegister8(bank, ledreg);

    if (enable)
    {
        leddata |= 1 << (lednum % 8);
    }
    else
    {
        leddata &= ~(1 << (lednum % 8));
    }

    writeRegister8(bank, ledreg, leddata);
}

void Adafruit_IS31FL3731::setLEDEnableMask(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE], uint8_t bank)
{
    //setSoftwareShutdown(1);
    selectBank(bank);

#ifdef USE_BUFFERED_TWI
    uint8_t cmd[1] = {0};
    i2cMasterSendNI(_i2caddr, 1, cmd);
    i2cMasterSendNI(_i2caddr, ISSI_LED_MASK_SIZE, ledEnableMask);
#else
    i2c_start_wait(_i2caddr + I2C_WRITE);
    i2c_write(0x0);
    for (uint8_t p = 0; p < ISSI_LED_MASK_SIZE; ++p)
        i2c_write(ledEnableMask[p]);
    i2c_stop();
#endif

    //setSoftwareShutdown(0);
}

void Adafruit_IS31FL3731::setLEDEnableMaskForAllBanks(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE])
{
	for (uint8_t b = 0; b < 8; ++b)
	{
		setLEDEnableMask(ledEnableMask, b);
	}
}

void Adafruit_IS31FL3731::setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank)
{
    if (lednum >= 144)
        return;

    writeRegister8(bank, 0x24 + lednum, pwm);
}

void Adafruit_IS31FL3731::setLEDPWM(uint8_t const pwm[ISSI_TOTAL_CHANNELS], uint8_t bank)
{
    //setSoftwareShutdown(1);
    selectBank(bank);

#ifdef USE_BUFFERED_TWI
    uint8_t cmd[1] = {0x24};
    i2cMasterSendNI(_i2caddr, 1, cmd);
    i2cMasterSendNI(_i2caddr, ISSI_USED_CHANNELS, pwm);
#else
    i2c_start_wait(_i2caddr + I2C_WRITE);
    i2c_write(0x24);
    for (uint8_t p = 0; p < ISSI_USED_CHANNELS; ++p)
        i2c_write(pwm[p]);
    i2c_stop();
#endif

    //setSoftwareShutdown(0);
}

/*************/

void Adafruit_IS31FL3731::writeRegister8(uint8_t b, uint8_t reg, uint8_t data)
{
    selectBank(b);

#ifdef USE_BUFFERED_TWI
    uint8_t cmd[2] = {reg, data};
    i2cMasterSendNI(_i2caddr, 2, cmd);
#else
    i2c_start_wait(_i2caddr + I2C_WRITE);
    i2c_write(reg);
    i2c_write(data);
    i2c_stop();
#endif

    /*
    Wire.beginTransmission(_i2caddr);
    Wire.write((byte) reg);
    Wire.write((byte) data);
    Wire.endTransmission();
    */
    // Serial.print("$"); Serial.print(reg, HEX);
    // Serial.print(" = 0x"); Serial.println(data, HEX);
}

void Adafruit_IS31FL3731::writeRegister16(uint8_t b, uint8_t reg, uint16_t data)
{
    selectBank(b);

#ifdef USE_BUFFERED_TWI
    uint8_t cmd[3] = {reg, data >> 8, data & 0xFF};
    i2cMasterSendNI(_i2caddr, 3, cmd);
#else
    i2c_start_wait(_i2caddr + I2C_WRITE);
    i2c_write(reg);
    i2c_write(data >> 8);
    i2c_write(data & 0xFF);
    i2c_stop();
#endif
}

uint8_t Adafruit_IS31FL3731::readRegister8(uint8_t bank, uint8_t reg)
{
    selectBank(bank);

    uint8_t data;

#ifdef USE_BUFFERED_TWI
    uint8_t cmd[1] = {reg};
    i2cMasterSendNI(_i2caddr, 1, cmd);
    i2cMasterReceiveNI(_i2caddr, 1, &data);
#else
    i2c_start_wait(_i2caddr + I2C_WRITE);
    i2c_write(reg);
    i2c_rep_start(_i2caddr + I2C_READ);
    data = i2c_readNak();
    i2c_stop();
#endif

    return data;

    /*

    Wire.beginTransmission(_i2caddr);
    Wire.write((byte) reg);
    Wire.endTransmission();

    Wire.beginTransmission(_i2caddr);
    Wire.requestFrom(_i2caddr, (byte) 1);
    x = Wire.read();
    Wire.endTransmission();
            return x;
    */

    // Serial.print("$"); Serial.print(reg, HEX);
    //  Serial.print(": 0x"); Serial.println(x, HEX);
}
