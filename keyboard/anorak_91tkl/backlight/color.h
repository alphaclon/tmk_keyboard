
#ifndef KEYBOARD_ANORAK_91TKL_BACKLIGHT_ISSI_COLOR_H_
#define KEYBOARD_ANORAK_91TKL_BACKLIGHT_ISSI_COLOR_H_

#include <stdint.h>
#include <stdbool.h>

#if defined(__GNUC__)
#define PACKED __attribute__((__packed__))
#else
#define PACKED
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif

typedef union PACKED {
    struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint8_t rgb[3];
} RGB;

/*
 * hue             (Farbton) :   0 ... 359 °
 * saturation  (Saettingung) :   0 ... 100 %
 * value          (Hellwert) :   0 ... 100 %
*/

typedef union PACKED {
    struct
    {
        uint8_t h;
        uint8_t s;
        uint8_t v;
    };
    uint8_t hsv[3];
} HSV;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif

enum RGBColorName_t
{
    red = 0,
    green,
    blue
};

typedef enum RGBColorName_t RGBColorName;

enum HSVColorName_t
{
    hue = 0,
    saturation,
	value
};

typedef enum HSVColorName_t HSVColorName;

RGB hsv_to_rgb(HSV hsv);
HSV rgb_to_hsv(RGB rgb);

#endif /* KEYBOARD_ANORAK_91TKL_BACKLIGHT_ISSI_COLOR_H_ */
