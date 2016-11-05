
#include "../../backlight/IS31FL3731/IS31FL3731.h"

#include <util/delay.h>

extern "C" {
#if TWILIB == AVR315
#include "../../backlight/avr315/TWI_Master.h"
#elif TWILIB == BUFFTW
#include "../../backlight/twi/twi_master.h"
#else
#include "../../backlight/i2cmaster/i2cmaster.h"
#endif
}

#define ISSI_PICTUREMODE 0x00
#define ISSI_AUTOPLAYMODE 0x08
#define ISSI_AUDIOPLAYMODE 0x18

#define ISSI_REG_MODE 0x00
#define ISSI_REG_PICTUREFRAME 0x01
#define ISSI_REG_AUTOPLAY_1 0x02
#define ISSI_REG_AUTOPLAY_2 0x03
#define ISSI_REG_BLINK 0x05
#define ISSI_REG_AUDIOSYNC 0x06
#define ISSI_REG_BREATH_1 0x08
#define ISSI_REG_BREATH_2 0x09
#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_GAIN_REGISTER 0x0B
#define ISSI_REG_ADC_REGISTER 0x0C

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B // helpfully called 'page nine'

#define ISSI__ENABLE_OFFSET 0x00
#define ISSI__BLINK_OFFSET 0x12
#define ISSI__COLOR_OFFSET 0x24

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

uint8_t gamma_correction_table[GAMMA_STEPS] = { 4, 18, 39, 69, 106, 149, 212, 255 };

IS31FL3731::IS31FL3731(uint8_t x, uint8_t y) : Adafruit_GFX(x, y)
{
    _issi_address = 0;
    _frame = 0;
}

IS31FL3731::~IS31FL3731()
{
}

bool IS31FL3731::begin(uint8_t issi_slave_address)
{
    _issi_address = (issi_slave_address << 1);
    _frame = 0;

    // shutdown
    enableSoftwareShutdown(1);

    _delay_ms(1);

    setPictureMode();
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
        for (uint8_t led = 0; led < ISSI_TOTAL_CHANNELS; led++)
            setLedBrightness(led, 0, 0); // set each led to the default PWM
    }

    // out of shutdown
    enableSoftwareShutdown(0);

    return true;
}

void IS31FL3731::enableSoftwareShutdown(bool enabled)
{
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, enabled ? 0x00 : 0x01);
}

void IS31FL3731::setPictureMode()
{
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_MODE, ISSI_PICTUREMODE);
}

void IS31FL3731::setAutoFramePlayMode(uint8_t frame_start)
{
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_MODE, ISSI_AUTOPLAYMODE | (frame_start & 0x07));
}

void IS31FL3731::setAutoFramePlayConfig(uint8_t loops, uint8_t frames, uint8_t delay)
{
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUTOPLAY_1, ((loops & 0x7)<< 4) | (frames & 0x7) );
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUTOPLAY_2, delay | 0x3f );
}

void IS31FL3731::setBreathMode(bool enable)
{
    uint8_t bcr2 = readRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_BREATH_2);

    if (enable)
        bcr2 |= (1 << 4);
    else
        bcr2 &= ~(1 << 4);

    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_BREATH_2, bcr2);
}

void IS31FL3731::setBreathConfig(uint8_t fade_in, uint8_t fade_out, uint8_t extinguish)
{
    uint8_t bcr2 = readRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_BREATH_2);
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_BREATH_1, ((fade_out & 0x7)<< 4) | (fade_in & 0x7) );
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_BREATH_2, bcr2 | (extinguish & 0x7) );
}

void IS31FL3731::setFrame(uint8_t frame)
{
    _frame = frame;
}

void IS31FL3731::displayFrame(uint8_t frame)
{
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, frame & 0x07);
}

void IS31FL3731::audioSync(bool sync)
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

void IS31FL3731::drawPixel(int16_t x, int16_t y, uint16_t color)
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

#ifdef IS31FL3731_DO_CHECKS
    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;
    if (color > 255)
        color = 255; // PWM 8bit max
#endif

    setLedBrightness(x + y * 16, color, _frame);
}

void IS31FL3731::enableLed(uint8_t lednum, uint8_t enable, uint8_t bank)
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

void IS31FL3731::enableLeds(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE], uint8_t bank)
{
    selectBank(bank);

#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_2(_issi_address, 0, ledEnableMask, ISSI_LED_MASK_SIZE);

#elif TWILIB == BUFFTW

    i2c_command.parts.cmd = 0;
    memcpy(i2c_command.parts.data, ledEnableMask, ISSI_LED_MASK_SIZE);
    i2cMasterSendNI(_issi_address, ISSI_LED_MASK_SIZE + 1, i2c_command.raw);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(0x0);
    for (uint8_t p = 0; p < ISSI_LED_MASK_SIZE; ++p)
        i2c_write(ledEnableMask[p]);
    i2c_stop();

#endif
}

void IS31FL3731::setLedBrightness(uint8_t lednum, uint8_t pwm, uint8_t bank)
{
    if (lednum >= 144)
        return;

    writeRegister8(bank, 0x24 + lednum, pwm);
}

void IS31FL3731::setLedsBrightness(uint8_t const pwm[ISSI_TOTAL_CHANNELS], uint8_t bank)
{
    selectBank(bank);

#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_2(_issi_address, 0x24, pwm, ISSI_USED_CHANNELS);

#elif TWILIB == BUFFTW

    i2c_command.parts.cmd = 0x24;
    memcpy(i2c_command.parts.data, pwm, ISSI_USED_CHANNELS);
    i2cMasterSendNI(_issi_address, ISSI_USED_CHANNELS + 1, i2c_command.raw);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(0x24);
    for (uint8_t p = 0; p < ISSI_USED_CHANNELS; ++p)
        i2c_write(pwm[p]);
    i2c_stop();

#endif
}

/*************/

void IS31FL3731::selectBank(uint8_t bank)
{
#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_1(_issi_address, ISSI_COMMANDREGISTER, bank);

#elif TWILIB == BUFFTW

    uint8_t cmd[2] = {ISSI_COMMANDREGISTER, bank};
    i2cMasterSendNI(_issi_address, 2, cmd);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(ISSI_COMMANDREGISTER);
    i2c_write(bank);
    i2c_stop();

#endif
}

void IS31FL3731::writeRegister8(uint8_t b, uint8_t reg, uint8_t data)
{
    selectBank(b);

#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_1(_issi_address, ISSI_COMMANDREGISTER, data);

#elif TWILIB == BUFFTW

    uint8_t cmd[2] = {reg, data};
    i2cMasterSendNI(_issi_address, 2, cmd);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(reg);
    i2c_write(data);
    i2c_stop();

#endif
}

void IS31FL3731::writeRegister16(uint8_t b, uint8_t reg, uint16_t data)
{
    selectBank(b);

#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_2(_issi_address, reg, (unsigned char *)&data, 2);

#elif TWILIB == BUFFTW

    uint8_t cmd[3] = {reg, data >> 8, data & 0xFF};
    i2cMasterSendNI(_issi_address, 3, cmd);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(reg);
    i2c_write(data >> 8);
    i2c_write(data & 0xFF);
    i2c_stop();

#endif
}

uint8_t IS31FL3731::readRegister8(uint8_t bank, uint8_t reg)
{
    selectBank(bank);

    uint8_t data;

#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_2(_issi_address | (1 << TWI_READ_BIT), reg, &data, 1);
    TWI_Get_Data_From_Transceiver(&data, 1);

#elif TWILIB == BUFFTW

    uint8_t cmd[1] = {reg};
    i2cMasterSendNI(_issi_address, 1, cmd);
    i2cMasterReceiveNI(_issi_address, 1, &data);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(reg);
    i2c_rep_start(_issi_address + I2C_READ);
    data = i2c_readNak();
    i2c_stop();

#endif

    return data;
}
