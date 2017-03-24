
#include "IS31FL3731.h"
#include "../twi_config.h"
#include "IS31FL3731_debug.h"
#include <util/delay.h>

#define ISSI_PICTUREMODE 0x00
#define ISSI_AUTOPLAYMODE 0x08
#define ISSI_AUDIOPLAYMODE 0x18

#define ISSI_REG_MODE 0x00
#define ISSI_REG_PICTUREFRAME 0x01
#define ISSI_REG_AUTOPLAY_1 0x02
#define ISSI_REG_AUTOPLAY_2 0x03
#define ISSI_REG_RESERVED 0x04
#define ISSI_REG_BLINK 0x05
#define ISSI_REG_AUDIOSYNC 0x06
#define ISSI_REG_FRAME_STATE_RO 0x07
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

#define IS31FL3731_HARDWARE_SHUTDOWN

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                                                            \
    {                                                                                                                  \
        int16_t t = a;                                                                                                 \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

uint8_t gamma_correction_table[GAMMA_STEPS] = {4, 18, 39, 69, 106, 149, 212, 255};

IS31FL3731::IS31FL3731(uint8_t x, uint8_t y) : Adafruit_GFX(x, y)
{
    _is_initialized = false;
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

    for (uint8_t i = 0; i <= 0x0C; i++)
        writeRegister8(ISSI_BANK_FUNCTIONREG, i, 0x0);

    enableHardwareShutdown(false);
    _delay_ms(1);

    reset();

    LS_("done");

    _is_initialized = true;

    return true;
}

bool IS31FL3731::is_initialized()
{
    return _is_initialized;
}

void IS31FL3731::reset()
{
    _frame = 0;

    LS_("reset");

    // shutdown
    enableSoftwareShutdown(true);
    _delay_ms(1);

    setPictureMode();
    displayFrame(_frame);
    audioSync(false);

    for (uint8_t f = 0; f < ISSI_TOTAL_FRAMES; f++)
        for (uint8_t i = 0; i <= 0x11; i++)
            writeRegister8(f, i, 0x00); // each 8 LEDs off

    for (uint8_t f = 0; f < ISSI_TOTAL_FRAMES; f++)
        for (uint8_t led = 0; led < ISSI_TOTAL_CHANNELS; led++)
            setLedBrightness(led, 0x00, f); // set each led to the default PWM

    // out of shutdown
    enableSoftwareShutdown(false);
}

void IS31FL3731::test()
{
    LS_("test");

    uint8_t bank = 0;

    for (uint8_t i = 0; i <= 0x11; i++)
        writeRegister8(bank, i, 0xff); // each 8 LEDs on

    for (uint8_t led = 0; led < ISSI_TOTAL_CHANNELS; led++)
        setLedBrightness(led, 0x32, bank); // set each led to PWM 0x32
}

void IS31FL3731::enableSoftwareShutdown(bool enabled)
{
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, enabled ? 0x00 : 0x01);
}

void IS31FL3731::enableHardwareShutdown(bool enabled)
{
#ifdef IS31FL3731_HARDWARE_SHUTDOWN
    if (enabled)
    {
        LS_("HardwareShutdown: on");
        // set SDB pin to LOW (PD2)
        DDRD |= (1 << 2);
        PORTD &= ~(1 << 2);
    }
    else
    {
        LS_("HardwareShutdown: off");
        // set SDB pin to HIGH (PD2)
        DDRD |= (1 << 2);
        PORTD |= (1 << 2);
    }
#endif
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
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUTOPLAY_1, ((loops & 0x7) << 4) | (frames & 0x7));
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUTOPLAY_2, delay | 0x3f);
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
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_BREATH_1, ((fade_out & 0x7) << 4) | (fade_in & 0x7));
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_BREATH_2, bcr2 | (extinguish & 0x7));
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
#ifdef IS31FL3731_SUPPORT_ROTATION
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
#endif

    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;

#ifdef IS31FL3731_DO_CHECKS
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

#if TWILIB == AVR315 || TWILIB == AVR315_SYNC

    TWI_write_data_to_register(_issi_address, ISSI__ENABLE_OFFSET, ledEnableMask, ISSI_LED_MASK_SIZE);

#elif TWILIB == BUFFTW

    i2cMasterSendCommandNI(_issi_address, ISSI__ENABLE_OFFSET, ISSI_LED_MASK_SIZE, ledEnableMask);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(ISSI__ENABLE_OFFSET);
    for (uint8_t p = 0; p < ISSI_LED_MASK_SIZE; ++p)
        i2c_write(ledEnableMask[p]);
    i2c_stop();

#endif
}

void IS31FL3731::setLedBrightness(uint8_t lednum, uint8_t pwm, uint8_t bank)
{
    if (lednum >= 144)
        return;

    writeRegister8(bank, ISSI__COLOR_OFFSET + lednum, pwm);
}

void IS31FL3731::setLedsBrightness(uint8_t const pwm[ISSI_TOTAL_CHANNELS], uint8_t bank)
{
    selectBank(bank);

#if TWILIB == AVR315 || TWILIB == AVR315_SYNC

    for (uint8_t offset = 0; offset < ISSI_USED_CHANNELS; offset += (ISSI_USED_CHANNELS / ISSI_USED_ROWS))
    {
        TWI_write_data_to_register(_issi_address, ISSI__COLOR_OFFSET + offset, pwm + offset,
                                   (ISSI_USED_CHANNELS / ISSI_USED_ROWS));
    }

    // TWI_write_data_to_register(_issi_address, ISSI__COLOR_OFFSET, pwm, ISSI_USED_CHANNELS);

#elif TWILIB == BUFFTW

    for (uint8_t offset = 0; offset < ISSI_USED_CHANNELS; offset += (ISSI_USED_CHANNELS / ISSI_USED_ROWS))
    {
        i2cMasterSendCommandNI(_issi_address, ISSI__COLOR_OFFSET + offset, (ISSI_USED_CHANNELS / ISSI_USED_ROWS),
                               pwm + offset);
    }

    // i2cMasterSendCommandNI(_issi_address, ISSI__COLOR_OFFSET, ISSI_USED_CHANNELS, pwm);

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(ISSI__COLOR_OFFSET);
    for (uint8_t p = 0; p < ISSI_USED_CHANNELS; ++p)
        i2c_write(pwm[p]);
    i2c_stop();

#endif
}

/*************/

void IS31FL3731::selectBank(uint8_t bank)
{
#if TWILIB == AVR315_SYNC

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    i2c_start_wait(_issi_address + I2C_WRITE);
	i2c_write(ISSI_COMMANDREGISTER);
	i2c_write(bank);
	i2c_stop();

#elif TWILIB == AVR315

    TWI_write_byte_to_register(_issi_address, ISSI_COMMANDREGISTER, bank);

#elif TWILIB == BUFFTW

    i2cMasterSendCommandNI(_issi_address, ISSI_COMMANDREGISTER, 1, &bank);

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

#if TWILIB == AVR315_SYNC

    while (TWI_Transceiver_Busy())
        ; // Wait until TWI is ready for next transmission.

    i2c_start_wait(_issi_address + I2C_WRITE);
	i2c_write(reg);
	i2c_write(data);
	i2c_stop();

#elif TWILIB == AVR315

    TWI_write_byte_to_register(_issi_address, reg, data);

#elif TWILIB == BUFFTW

    uint8_t retval = i2cMasterSendCommandNI(_issi_address, reg, 1, &data);

#ifdef TWI_HANDLE_ERRORS
    if (retval != I2C_OK)
    {
        LV_("writeRegister8 i2c error: 0x%X", retval);
    }
#endif

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

#if TWILIB == AVR315 || TWILIB == AVR315_SYNC

    TWI_write_data_to_register(_issi_address, reg, (unsigned char *)&data, 2);

#elif TWILIB == BUFFTW

    uint8_t retval = i2cMasterSendCommandNI(_issi_address, reg, 2, (uint8_t const *)&data);

#ifdef TWI_HANDLE_ERRORS
    if (retval != I2C_OK)
    {
        LV_("writeRegister16 i2c error: 0x%X", retval);
    }
#endif

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

#if TWILIB == AVR315 || TWILIB == AVR315_SYNC

    TWI_write_byte(_issi_address, reg);
    TWI_read_data(_issi_address, 1);
    bool lastTransOK = TWI_get_data_from_transceiver(&data, 1);

#ifdef TWI_HANDLE_ERRORS
    if (!lastTransOK)
    {
        dprintf("transmission failed! reg:%u\r\n", reg);
        return 0;
    }
#endif

#elif TWILIB == BUFFTW

    uint8_t retval = i2cMasterSendCommandNI(_issi_address, reg, 0, 0);

#ifdef TWI_HANDLE_ERRORS
    if (retval != I2C_OK)
    {
        LV_("readRegister8 send i2c error: 0x%X", retval);
    }
#endif

    retval = i2cMasterReceiveNI(_issi_address, 1, &data);

#ifdef TWI_HANDLE_ERRORS
    if (retval != I2C_OK)
    {
        LV_("readRegister8 recv i2c error: 0x%X", retval);
    }
#endif

#else

    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(reg);
    i2c_rep_start(_issi_address + I2C_READ);
    data = i2c_readNak();
    i2c_stop();

#endif

    return data;
}

void IS31FL3731::dumpConfiguration()
{
#ifdef DEBUG_ISSI
    LS_("register bank");
    uint8_t reg;
    reg = readRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN);
    LV_("soft shutdown: %s", (reg ? "no" : "yes"));

    for (uint8_t i = 0; i < 0x0D; i++)
    {
        reg = readRegister8(ISSI_BANK_FUNCTIONREG, i);
        LV_("%02X: 0x%02X\r\n", i, reg);
    }
#endif
}

void IS31FL3731::dumpLeds(uint8_t bank)
{
#ifdef DEBUG_ISSI
    uint8_t leds[ISSI_TOTAL_LED_MASK_SIZE];
    memset(leds, 0, ISSI_TOTAL_LED_MASK_SIZE);

    selectBank(bank);

#if TWILIB == AVR315 || TWILIB == AVR315_SYNC
    TWI_write_byte(_issi_address, ISSI__ENABLE_OFFSET);
    TWI_read_data(_issi_address, ISSI_TOTAL_LED_MASK_SIZE);
    bool lastTransOK = TWI_get_data_from_transceiver(leds, ISSI_TOTAL_LED_MASK_SIZE);

    if (!lastTransOK)
    {
        dprintf("transmission failed! 0x%X\r\n", TWI_Get_State_Info());
        TWI_Master_Initialise();
        return;
    }
#elif TWILIB == BUFFTW
    i2cMasterSendCommandNI(_issi_address, ISSI__ENABLE_OFFSET, 0, 0);
    i2cMasterReceiveNI(_issi_address, ISSI_TOTAL_LED_MASK_SIZE, leds);
#else
    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(ISSI__ENABLE_OFFSET);
    i2c_rep_start(_issi_address + I2C_READ);
    for (uint8_t r = 0; r < ISSI_TOTAL_LED_MASK_SIZE; ++r)
        leds[r] = i2c_read(r != ISSI_TOTAL_LED_MASK_SIZE - 1);
    i2c_stop();
#endif

    LV_("leds, bank %u", bank);
    for (uint8_t r = 0; r < ISSI_TOTAL_ROWS; ++r)
    {
        LVN_(("%02d: "), r);
        for (uint8_t c = 0; c < 2; ++c)
        {
            uint8_t led = leds[r * 2 + c];
            LVN_(("%02X "), led);
        }
        LS_("");
    }
#endif
}

void IS31FL3731::dumpBrightness(uint8_t bank)
{
#ifdef DEBUG_ISSI
    uint8_t pwm[ISSI_TOTAL_CHANNELS];
    memset(pwm, 0, ISSI_TOTAL_CHANNELS);

    selectBank(bank);

#if TWILIB == AVR315 || TWILIB == AVR315_SYNC

    /*
    TWI_write_byte(_issi_address, ISSI__COLOR_OFFSET);
    TWI_read_data(_issi_address, ISSI_TOTAL_CHANNELS);
    bool lastTransOK = TWI_get_data_from_transceiver(pwm, ISSI_TOTAL_CHANNELS);

    if (!lastTransOK)
    {
        dprintf("transmission failed! 0x%X\r\n", TWI_Get_State_Info());
        TWI_Master_Initialise();
        return;
    }
    */

    for (uint8_t offset = 0; offset < ISSI_TOTAL_CHANNELS; offset += (ISSI_TOTAL_CHANNELS / ISSI_TOTAL_ROWS))
    {
        TWI_write_byte(_issi_address, ISSI__COLOR_OFFSET + offset);
        TWI_read_data(_issi_address, (ISSI_TOTAL_CHANNELS / ISSI_TOTAL_ROWS));
        bool lastTransOK = TWI_get_data_from_transceiver(pwm + offset, (ISSI_TOTAL_CHANNELS / ISSI_TOTAL_ROWS));

        if (!lastTransOK)
        {
            dprintf("transmission failed! 0x%X\r\n", TWI_Get_State_Info());
            TWI_Master_Initialise();
            return;
        }
    }

#elif TWILIB == BUFFTW
    i2cMasterSendCommandNI(_issi_address, ISSI__COLOR_OFFSET, 0, 0);
    i2cMasterReceiveNI(_issi_address, ISSI_TOTAL_CHANNELS, pwm);
#else
    i2c_start_wait(_issi_address + I2C_WRITE);
    i2c_write(ISSI__COLOR_OFFSET);
    i2c_rep_start(_issi_address + I2C_READ);
    for (uint8_t r = 0; r < ISSI_TOTAL_CHANNELS; ++r)
        pwm[r] = i2c_read(r != ISSI_TOTAL_CHANNELS - 1);
    i2c_stop();
#endif

    LV_("brightness, bank %u", bank);
    for (uint8_t r = 0; r < ISSI_TOTAL_ROWS; ++r)
    {
        LVN_("%02u: ", r);
        for (uint8_t c = 0; c < ISSI_TOTAL_COLUMS; ++c)
        {
            LVN_("%03u ", pwm[r * 16 + c]);
        }
        LS_("");
    }
#endif
}
