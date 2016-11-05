#pragma once

#include "IS31FL3731.h"

class IS31FL3731Buffered : public IS31FL3731
{
public:
	IS31FL3731Buffered(uint8_t x = ISSI_TOTAL_COLUMS, uint8_t y = ISSI_TOTAL_ROWS);
    virtual ~IS31FL3731Buffered();

	virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
	void blitToFrame(uint8_t frame);

private:
	uint8_t *_pwm_buffer;
	uint8_t _pwm_buffer_size;
};


