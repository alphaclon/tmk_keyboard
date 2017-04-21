
#include "utils.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// Deals with the messy details of incrementing an integer
uint8_t increment(uint8_t value, uint8_t step, uint8_t min, uint8_t max)
{
    int16_t new_value = value;
    new_value += step;
    return MIN(MAX(new_value, min), max);
}

uint8_t decrement(uint8_t value, uint8_t step, uint8_t min, uint8_t max)
{
    int16_t new_value = value;
    new_value -= step;
    return MIN(MAX(new_value, min), max);
}

uint16_t increment16(uint16_t value, uint8_t step, uint16_t min, uint16_t max)
{
    int32_t new_value = value;
    new_value += step;
    return MIN(MAX(new_value, min), max);
}

uint16_t decrement16(uint16_t value, uint8_t step, uint16_t min, uint16_t max)
{
    int32_t new_value = value;
    new_value -= step;
    return MIN(MAX(new_value, min), max);
}
