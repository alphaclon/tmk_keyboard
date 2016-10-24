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

class Adafruit_IS31FL3731 : public Adafruit_GFX
{
public:
    Adafruit_IS31FL3731(uint8_t x = ISSI_TOTAL_COLUMS, uint8_t y = ISSI_TOTAL_ROWS);
    virtual ~Adafruit_IS31FL3731();

    bool begin(uint8_t addr = ISSI_ADDR_DEFAULT);

    void drawPixel(int16_t x, int16_t y, uint16_t color);

    void setLEDEnable(uint8_t lednum, uint8_t enabled, uint8_t bank = 0);
    void setLEDEnableMask(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE], uint8_t bank = 0);
    void setLEDEnableMaskForAllBanks(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE]);

    void setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank = 0);
    void setLEDPWM(uint8_t const pwm[ISSI_TOTAL_CHANNELS], uint8_t bank = 0);

    void setFrame(uint8_t b);
    void displayFrame(uint8_t frame);

    void audioSync(bool sync);
    void setPictureMode();
    void setAutoFramePlayMode(uint8_t frame_start);
    void setAutoFramePlayConfig(uint8_t loops, uint8_t frames, uint8_t delay_ms_x_11);
    void setBreathMode(uint8_t enable);
    void setBreathConfig(uint8_t fade_in, uint8_t fade_out, uint8_t extinguish);

    void setSoftwareShutdown(uint8_t shutdown);

protected:
    typedef union _i2c_command
    {
        struct _data
        {
            uint8_t cmd;
            uint8_t data[ISSI_USED_CHANNELS];
        } parts;
        uint8_t raw[ISSI_USED_CHANNELS + 1];
    } i2c_command;

    void selectBank(uint8_t bank);
    void writeRegister8(uint8_t bank, uint8_t reg, uint8_t data);
    void writeRegister16(uint8_t bank, uint8_t reg, uint16_t data);
    uint8_t readRegister8(uint8_t bank, uint8_t reg);

    uint8_t _i2caddr;
    uint8_t _frame;
};

#endif
