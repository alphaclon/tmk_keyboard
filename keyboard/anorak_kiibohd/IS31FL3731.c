
#include "IS31FL3731.h"
#include "TWIlib.h"

uint8_t _i2caddr;
uint8_t _frame;

typedef struct IS31FL3731_Buffer
{
    uint8_t i2c_addr;
    uint8_t reg_addr;
    uint8_t buffer[IS31FL3731_BufferLength];
} IS31FL3731_Buffer;

IS31FL3731_Buffer IS31FL3731_pageBuffer;

// A bit mask determining which LEDs are enabled in the ISSI chip
const uint8_t IS31FL3731_ledEnableMask1[] = {_i2caddr, // I2C address
                                             0x00,     // Starting register address
                                             ISSILedMask};

// Default LED brightness
const uint8_t IS31FL3731_defaultBrightness1[] = {_i2caddr, // I2C address
                                                 0x24,     // Starting register address
                                                 ISSILedDefaultBrightness};



uint8_t I2C_Send(uint8_t *data, uint8_t sendLen, uint8_t recvLen)
{
    TWITransmitData(data, sendLen, 0);

    if (recvLen)
    {
        TWIReadData(_i2caddr, 1, 1);
        // Wait until the TWI has finished before the data is available
        while (isTWIReady() == 0) {_delay_ms(1);}
    }

    return 0;
}

// Setup
/*
inline void IS31FL3731_setup()
{



    // Do not disable software shutdown of ISSI chip unless current is high enough
    // Require at least 150 mA
    // May be enabled/disabled at a later time
    if (Output_current_available() >= 150)
    {
        // Disable Software shutdown of ISSI chip
        IS31FL3731_writeReg(0x0A, 0x01, 0x0B);
    }
}
*/

void IS31FL3731_init(uint8_t addr)
{
    _i2caddr = addr;
    _frame = 0;

    // Zero out Frame Registers
    // This needs to be done before disabling the hardware shutdown (or the leds will do undefined things)
    IS31FL3731_zeroPages(0x0B, 1, 0x00, 0x0C); // Control Registers

    // Disable Hardware shutdown of ISSI chip (pull high)
    //GPIOB_PDDR |= (1 << 16);
    //PORTB_PCR16 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
    //GPIOB_PSOR |= (1 << 16);

    // shutdown
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);

    delay(10);

    // out of shutdown
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);

    // picture mode
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

    displayFrame(_frame);
    audioSync(false);

    // Clear LED Pages
    IS31FL3731_zeroPages(0x00, 8, 0x00, 0xB4); // LED Registers

    // Enable LEDs based upon mask
    IS31FL3731_sendPage((uint8_t *)IS31FL3731_ledEnableMask1, sizeof(IS31FL3731_ledEnableMask1), 0);

    // Set default brightness
    IS31FL3731_sendPage((uint8_t *)IS31FL3731_defaultBrightness1, sizeof(IS31FL3731_defaultBrightness1), 0);

    /*
    // all LEDs on & 0 PWM
    for (uint8_t f = 0; f < 8; f++)
    {
        for (uint8_t i = 0; i < 144; i++)
            setLEDPWM(i, 0x0, f); // set each led to 0 PWM

        for (uint8_t i = 0; i <= 0x11; i++)
            writeRegister8(f, i, 0xff); // each 8 LEDs on
    }
    */
}

void IS31FL3731_setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank)
{
    if (lednum >= 144)
        return;
    writeRegister8(bank, 0x24 + lednum, pwm);
}

void IS31FL3731_drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;
    if (color > 255)
        color = 255; // PWM 8bit max

    setLEDPWM(x + y * 16, color, _frame);
    return;
}

void IS31FL3731_setFrame(uint8_t f)
{
    _frame = f;
}

void IS31FL3731_displayFrame(uint8_t f)
{
    if (f > 7)
        f = 0;
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, f);
}

void IS31FL3731_selectBank(uint8_t b)
{
    uint8_t data[3] = { _i2caddr, ISSI_COMMANDREGISTER, b };
    TWITransmitData(data, 3, 0);

    /*
    Wire.beginTransmission(_i2caddr);
    Wire.write((byte)ISSI_COMMANDREGISTER);
    Wire.write(b);
    Wire.endTransmission();
    */
}

void IS31FL3731_audioSync(bool sync)
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

void IS31FL3731_writeRegister8(uint8_t b, uint8_t reg, uint8_t data)
{
    selectBank(b);

    uint8_t transmitdata[3] = { _i2caddr, reg, data };
    TWITransmitData(transmitdata, 3, 0);

    /*
    Wire.beginTransmission(_i2caddr);
    Wire.write((byte)reg);
    Wire.write((byte)data);
    Wire.endTransmission();
    */

    // Serial.print("$"); Serial.print(reg, HEX);
    // Serial.print(" = 0x"); Serial.println(data, HEX);
}

uint8_t IS31FL3731_readRegister8(uint8_t bank, uint8_t reg)
{
    uint8_t x;

    selectBank(bank);

    uint8_t data[2] = { _i2caddr, reg };
    TWITransmitData(data, 3, 0);

    TWIReadData(_i2caddr, 1, 1);
    // Wait until the TWI has finished before the data is available
    while (isTWIReady() == 0) {_delay_ms(1);}
    // TWI has finished, write the data to the LCD
    return TWIReceiveBuffer[0];

    /*
    Wire.beginTransmission(_i2caddr);
    Wire.write((byte)reg);
    Wire.endTransmission();

    Wire.beginTransmission(_i2caddr);
    Wire.requestFrom(_i2caddr, (byte)1);
    x = Wire.read();
    Wire.endTransmission();

    // Serial.print("$"); Serial.print(reg, HEX);
    //  Serial.print(": 0x"); Serial.println(x, HEX);

    return x;
    */
}

void IS31FL3731_zeroPages(uint8_t startPage, uint8_t numPages, uint8_t startReg, uint8_t endReg)
{
    // Page Setup
    uint8_t pageSetup[] = {_i2caddr, ISSI_COMMANDREGISTER, 0x00};

    // Max length of a page + chip id + reg start
    uint8_t fullPage[0xB4 + 2] = {0}; // Max size of page
    fullPage[0] = _i2caddr;           // Set chip id
    fullPage[1] = startReg;           // Set start reg

    // Iterate through given pages, zero'ing out the given register regions
    for (uint8_t page = startPage; page < startPage + numPages; page++)
    {
        // Set page
        pageSetup[2] = page;

        // Setup page
        while (I2C_Send(pageSetup, sizeof(pageSetup), 0) == 0)
            delay(1);

        // Zero out page
        while (I2C_Send(fullPage, endReg - startReg + 2, 0) == 0)
            delay(1);
    }
}

void IS31FL3731_sendPage(uint8_t *buffer, uint8_t len, uint8_t page)
{
    // Page Setup
    uint8_t pageSetup[] = {_i2caddr, ISSI_COMMANDREGISTER, page};

    // Setup page
    while (I2C_Send(pageSetup, sizeof(pageSetup), 0) == 0)
        delay(1);

    // Write page to I2C Tx Buffer
    while (I2C_Send(buffer, len, 0) == 0)
        delay(1);
}

void IS31FL3731_writeReg(uint8_t reg, uint8_t val, uint8_t page)
{
    // Page Setup
    uint8_t pageSetup[] = {_i2caddr, ISSI_COMMANDREGISTER, page};

    // Reg Write Setup
    uint8_t writeData[] = {_i2caddr, reg, val};

    // Setup page
    while (I2C_Send(pageSetup, sizeof(pageSetup), 0) == 0)
        delay(1);

    while (I2C_Send(writeData, sizeof(writeData), 0) == 0)
        delay(1);
}

void IS31FL3731_readPage(uint8_t len, uint8_t page)
{
    // Software shutdown must be enabled to read registers
    IS31FL3731_writeReg(0x0A, 0x00, 0x0B);

    // Page Setup
    uint8_t pageSetup[] = {_i2caddr, ISSI_COMMANDREGISTER, page};

    // Setup page
    while (I2C_Send(pageSetup, sizeof(pageSetup), 0) == 0)
        delay(1);

    // Register Setup
    uint8_t regSetup[] = {_i2caddr, 0x00};

    // Read each register in the page
    for (uint8_t reg = 0; reg < len; reg++)
    {
        // Update register to read
        regSetup[1] = reg;

        // Configure register
        while (I2C_Send(regSetup, sizeof(regSetup), 0) == 0)
            delay(1);

        // Register Read Command
        uint8_t regReadCmd[] = {0xE9};

        // Request single register byte
        while (I2C_Send(regReadCmd, sizeof(regReadCmd), 1) == 0)
            delay(1);
        dbug_print("NEXT");
    }

    // Disable software shutdown
    IS31FL3731_writeReg(0x0A, 0x01, 0x0B);
}

// LED State processing loop
unsigned int IS31FL3731_currentEvent = 0;
inline uint8_t IS31FL3731_scan()
{
    // Check for current change event
    if (IS31FL3731_currentEvent)
    {
        // TODO dim LEDs in low power mode instead of shutting off
        if (IS31FL3731_currentEvent < 150)
        {
            // Enable Software shutdown of ISSI chip
            IS31FL3731_writeReg(0x0A, 0x00, 0x0B);
        }
        else
        {
            // Disable Software shutdown of ISSI chip
            IS31FL3731_writeReg(0x0A, 0x01, 0x0B);
        }

        IS31FL3731_currentEvent = 0;
    }

    return 0;
}

void IS31FL3731_control(LedControl *control)
{
    // Only send if we've completed all other transactions
    /*
    if ( I2C_TxBuffer.sequencePos > 0 )
        return;
    */

    // Configure based upon the given mode
    // TODO Perhaps do gamma adjustment?
    switch (control->mode)
    {
    case LedControlMode_brightness_decrease:
        IS31FL3731_pageBuffer.buffer[control->index] -= control->amount;
        break;

    case LedControlMode_brightness_increase:
        IS31FL3731_pageBuffer.buffer[control->index] += control->amount;
        break;

    case LedControlMode_brightness_set:
        IS31FL3731_pageBuffer.buffer[control->index] = control->amount;
        break;

    case LedControlMode_brightness_decrease_all:
        for (uint8_t channel = 0; channel < IS31FL3731_TotalChannels; channel++)
        {
            IS31FL3731_pageBuffer.buffer[channel] -= control->amount;
        }
        break;

    case LedControlMode_brightness_increase_all:
        for (uint8_t channel = 0; channel < IS31FL3731_TotalChannels; channel++)
        {
            IS31FL3731_pageBuffer.buffer[channel] += control->amount;
        }
        break;

    case LedControlMode_brightness_set_all:
        for (uint8_t channel = 0; channel < IS31FL3731_TotalChannels; channel++)
        {
            IS31FL3731_pageBuffer.buffer[channel] = control->amount;
        }
        break;
    }

    // Sync LED buffer with ISSI chip buffer
    // TODO Support multiple frames
    IS31FL3731_pageBuffer.i2c_addr = _i2caddr; // Chip Address
    IS31FL3731_pageBuffer.reg_addr = 0x24;     // Brightness section
    IS31FL3731_sendPage((uint8_t *)&IS31FL3731_pageBuffer, sizeof(IS31FL3731_Buffer), 0);
}
