#ifndef _ADAFRUIT_IS31FL3731_H_
#define _ADAFRUIT_IS31FL3731_H_

#include <inttypes.h>
#include "../gfx/Adafruit_GFX.h"

#define USE_BUFFERED_TWI

#define ISSI_ADDR_DEFAULT 0x74

#define ISSI_TOTAL_CHANNELS 144
#define ISSI_TOTAL_ROWS 9
#define ISSI_TOTAL_COLUMS 16
#define ISSI_TOTAL_LED_MASK_SIZE (ISSI_TOTAL_ROWS * 2)
#define ISSI_TOTAL_FRAMES 8

#define ISSI_USED_ROWS 6
#define ISSI_USED_CHANNELS (ISSI_USED_ROWS * ISSI_TOTAL_COLUMS)
#define ISSI_LED_MASK_SIZE (ISSI_USED_ROWS * 2)

#define ISSI_REG_CONFIG 0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME 0x01

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B // helpfully called 'page nine'

class Adafruit_IS31FL3731 : public Adafruit_GFX
{
public:
    Adafruit_IS31FL3731(uint8_t x = ISSI_TOTAL_COLUMS, uint8_t y = ISSI_TOTAL_ROWS);
    virtual ~Adafruit_IS31FL3731();

    bool begin(uint8_t addr = ISSI_ADDR_DEFAULT);

    void drawPixel(int16_t x, int16_t y, uint16_t color);

    void setLEDEnable(uint8_t lednum, uint8_t enable, uint8_t bank = 0);
    void setLEDEnableMask(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE], uint8_t bank = 0);
    void setLEDEnableMaskForAllBanks(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE]);

    void setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank = 0);
    void setLEDPWM(uint8_t const pwm[ISSI_TOTAL_CHANNELS], uint8_t bank = 0);

    void setFrame(uint8_t b);
    void displayFrame(uint8_t frame);

    void audioSync(bool sync);

    void setSoftwareShutdown(uint8_t shutdown);

protected:
    void selectBank(uint8_t bank);
    void writeRegister8(uint8_t bank, uint8_t reg, uint8_t data);
    void writeRegister16(uint8_t bank, uint8_t reg, uint16_t data);
    uint8_t readRegister8(uint8_t bank, uint8_t reg);
    uint8_t _i2caddr, _frame;
};

#endif
