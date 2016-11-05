
#include "../../backlight/control.h"

// The lookup table to make the brightness changes be more visible
uint8_t sweep[] = {1, 2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 60, 60, 40, 30, 20, 15, 10, 8, 6, 4, 3, 2, 1};

void loop()
{
    // animate over all the pixels, and set the brightness from the sweep table
    for (uint8_t incr = 0; incr < 24; incr++)
    {
        for (uint8_t x = 0; x < 16; x++)
            for (uint8_t y = 0; y < 9; y++)
                issi.drawPixel(x, y, sweep[(x + y + incr) % 24]);
        issi.blitToFrame(0);
    }
}
