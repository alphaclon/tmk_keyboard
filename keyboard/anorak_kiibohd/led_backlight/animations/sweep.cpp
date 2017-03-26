
#include "matrix.h"
#include "../control.h"
#include "animation_utils.h"
#include "../avr315/twi_transmit_queue.h"

#if defined(DEBUG_ANIMATION) || defined(DEBUG_ISSI_PERFORMANCE)
#include "debug.h"
#else
#include "nodebug.h"
#endif

// The lookup table to make the brightness changes be more visible
//uint8_t const sweep[] PROGMEM = {1, 2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 60, 60, 40, 30, 20, 15, 10, 8, 6, 4, 3, 2, 1};
uint8_t const sweep[] PROGMEM = {1, 3, 6, 10, 20, 30, 40, 50, 60, 70, 80, 100, 100, 80, 70, 60, 50, 40, 30, 20, 10, 6, 3, 1};
static uint8_t animation_frame;
static uint8_t incr;

void sweep_animation_start()
{
	incr = 0;
	animation_frame = 1;

	animation_prepare(animation_frame);
}

void sweep_animation_stop()
{
	animation_postpare(animation_frame);
}

#ifdef DEBUG_ISSI_PERFORMANCE
uint8_t max_size = 0;
uint8_t min_size = 255;
uint8_t loopcounter = 0;
#endif

void sweep_animation_loop()
{
    // animate over all the pixels, and set the brightness from the sweep table
	//dprintf(".");

#ifdef DEBUG_ISSI_PERFORMANCE
	loopcounter++;

	if (loopcounter >= 10)
	{
		uint8_t queue_size = tx_queue_size();
		dprintf("> s: %u, min: %u, max: %u\n", queue_size, min_size, max_size);
	}
#endif

	incr++;
	if (incr >= 24)
		incr = 0;

	for (uint8_t x = 0; x < 16; x++)
		for (uint8_t y = 0; y < 9; y++)
			issi.drawPixel(x, y, pgm_read_byte(&sweep[(x + y + incr) % 24]));

	issi.blitToFrame(1);

#ifdef DEBUG_ISSI_PERFORMANCE
	uint8_t queue_size = tx_queue_size();

	if (queue_size > max_size)
		max_size = queue_size;

	if (queue_size > 0 && queue_size < min_size)
			min_size = queue_size;

	if (loopcounter >= 10)
	{
		loopcounter = 0;
		dprintf("< s: %u, min: %u, max: %u\n", queue_size, min_size, max_size);
	}
#endif
}
