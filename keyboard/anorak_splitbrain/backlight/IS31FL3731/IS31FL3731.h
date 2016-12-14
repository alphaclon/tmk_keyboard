#ifndef _ADAFRUIT_IS31FL3731_H_
#define _ADAFRUIT_IS31FL3731_H_

#include <inttypes.h>

#include "../gfx/Adafruit_GFX.h"
#include "../IS31FL3732_config.h"

/*
 * IS31FL3731 supports 144 = 9*16 LEDs
 *
 */

#define ISSI_TOTAL_ROWS 9
#define ISSI_TOTAL_COLUMS 16
#define ISSI_TOTAL_CHANNELS (ISSI_TOTAL_ROWS * ISSI_TOTAL_COLUMS)
#define ISSI_TOTAL_LED_MASK_SIZE (ISSI_TOTAL_ROWS * 2)
#define ISSI_TOTAL_FRAMES 8

#define GAMMA_STEPS 8
extern uint8_t gamma_correction_table[GAMMA_STEPS];

class IS31FL3731 : public Adafruit_GFX
{
public:
    IS31FL3731(uint8_t x = ISSI_TOTAL_COLUMS, uint8_t y = ISSI_TOTAL_ROWS);
    virtual ~IS31FL3731();

    bool begin(uint8_t issi_slave_address = ISSI_ADDR_DEFAULT);
    void reset();

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);

    void enableLed(uint8_t lednum, uint8_t enabled, uint8_t bank = 0);
    void enableLeds(uint8_t const ledEnableMask[ISSI_LED_MASK_SIZE], uint8_t bank = 0);

    void setLedBrightness(uint8_t lednum, uint8_t pwm, uint8_t bank = 0);
    void setLedsBrightness(uint8_t const pwm[ISSI_TOTAL_CHANNELS], uint8_t bank = 0);

    void setFrame(uint8_t b);
    void displayFrame(uint8_t frame);

    void audioSync(bool sync);
    /** picture mode
     *  enables picture mode and
     *  disables frame play mode
     */
    void setPictureMode();
    /** auto frame play control
     * enable auto frame play mode and start with frame #frame_start#
     * Movie starts with the given frame.
     */
    void setAutoFramePlayMode(uint8_t frame_start);
    /** auto play control
     * frame delay time: a = 1..63, a = 0 is equivalent to a = 64!
     * FDT = t * a with t = 11ms
     * e. g. a = 23: FDT is 11ms * 23 = 253ms
     *
     * loops: number of loops to play
     *     0 : endless
     *  1..7 : 1..7 loops
     *
     *  frames: number of frames to play
     *      0 : all frames
     *   1..7 : 1..7 frames
     */
    void setAutoFramePlayConfig(uint8_t loops, uint8_t frames, uint8_t delay);
    /**  breath control
     *   enable/disable breath function in auto frame play mode and picture mode
     */
    void setBreathMode(bool enable);
    /** breath control
     * Set the extinguish time
     * extinguish: a = 0..7
     * extinguish time ET = t * 2^a with t = 3.5ms
     * e.g. a = 4: ET is 3.5ms = 2^4 = 56ms
     *
     * Set the fade in / fade out time
     *
     * fade_in: b = 0..7
     * fade in time FIT = t * 2^b with t = 26ms
     * e. g. b = 4: FIT is 26ms * 2^4 = 416ms
     *
     * fade_out: c = 0..7
     * fade out time FOT = t * 2^c with t = 26ms
     * e. g. c = 4: FOT is 26ms * 2^4 = 416ms
     *
     */
    void setBreathConfig(uint8_t fade_in, uint8_t fade_out, uint8_t extinguish);

    /** set shutdown register
     *  enable/disable software shutdown
     */
    void enableSoftwareShutdown(bool enabled);

    /** set hardware shutdown (pin SDB)
     *  enable/disable hardware shutdown
     */
    void enableHardwareShutdown(bool enabled);



    void test();
    void dumpConfiguration();
    void dumpLeds(uint8_t bank);
    void dumpBrightness(uint8_t bank);



protected:
#if TWILIB == BUFFTW
    union _i2c_command
    {
        struct _data
        {
            uint8_t cmd;
            uint8_t data[ISSI_USED_CHANNELS];
        } parts;
        uint8_t raw[ISSI_USED_CHANNELS + 1];
    } i2c_command;
#endif

    void selectBank(uint8_t bank);
    void writeRegister8(uint8_t bank, uint8_t reg, uint8_t data);
    void writeRegister16(uint8_t bank, uint8_t reg, uint16_t data);
    uint8_t readRegister8(uint8_t bank, uint8_t reg);

    uint8_t _issi_address;
    uint8_t _frame;
};

#endif
