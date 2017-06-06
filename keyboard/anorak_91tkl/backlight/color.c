
#include "color.h"
#include <avr/pgmspace.h>

// Lightness curve using the CIE 1931 lightness formula
// Generated by the python script provided in http://jared.geek.nz/2013/feb/linear-led-pwm
const uint8_t g_cie_curve[] PROGMEM = {
    0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   3,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,   6,
    6,   6,   6,   7,   7,   7,   7,   8,   8,   8,   8,   9,   9,   9,   10,  10,  10,  10,  11,  11,  11,  12,
    12,  12,  13,  13,  13,  14,  14,  15,  15,  15,  16,  16,  17,  17,  17,  18,  18,  19,  19,  20,  20,  21,
    21,  22,  22,  23,  23,  24,  24,  25,  25,  26,  26,  27,  28,  28,  29,  29,  30,  31,  31,  32,  32,  33,
    34,  34,  35,  36,  37,  37,  38,  39,  39,  40,  41,  42,  43,  43,  44,  45,  46,  47,  47,  48,  49,  50,
    51,  52,  53,  54,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  70,  71,  72,
    73,  74,  75,  76,  77,  79,  80,  81,  82,  83,  85,  86,  87,  88,  90,  91,  92,  94,  95,  96,  98,  99,
    100, 102, 103, 105, 106, 108, 109, 110, 112, 113, 115, 116, 118, 120, 121, 123, 124, 126, 128, 129, 131, 132,
    134, 136, 138, 139, 141, 143, 145, 146, 148, 150, 152, 154, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173,
    175, 177, 179, 181, 183, 185, 187, 189, 191, 193, 196, 198, 200, 202, 204, 207, 209, 211, 214, 216, 218, 220,
    223, 225, 228, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255};

RGB hsv_to_rgb(HSV hsv)
{
    RGB rgb;
    uint8_t region, p, q, t;
    uint16_t h, s, v, remainder;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    h = hsv.h;
    s = hsv.s;
    v = hsv.v;

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        rgb.r = v;
        rgb.g = t;
        rgb.b = p;
        break;
    case 1:
        rgb.r = q;
        rgb.g = v;
        rgb.b = p;
        break;
    case 2:
        rgb.r = p;
        rgb.g = v;
        rgb.b = t;
        break;
    case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = v;
        break;
    case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = v;
        break;
    default:
        rgb.r = v;
        rgb.g = p;
        rgb.b = q;
        break;
    }

    rgb.r = pgm_read_byte(&g_cie_curve[rgb.r]);
    rgb.g = pgm_read_byte(&g_cie_curve[rgb.g]);
    rgb.b = pgm_read_byte(&g_cie_curve[rgb.b]);

    return rgb;
}

/*
 *
 * hsv library by Julien Vanier <jvanier@gmail.com>
 *
 * Original code from StackOverflow
 * http://stackoverflow.com/a/6930407/1901924
 *
 * https://github.com/monkbroc/particle-hsv/tree/master/src
 *
 */


HSV rgb_to_hsv(RGB rgb)
{
    HSV hsv;
    uint8_t rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * (long)(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}

/*
 * https://www.mikrocontroller.net/topic/158185
 *
 */

/*
RGB hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v)
{
    uint16_t vs = v * s, h6 = 6 * h, f;
    uint8_t i, p, u, r = v, g = v, b = v;

    p = (v << 8) - vs >> 8;
    i = h6 >> 8;
    f = ((i | 1) << 8) - h6;

    if (i & 1)
        f = -f;

    u = ((uint32_t)v << 16) - (uint32_t)vs * f >> 16;

    switch (i)
    {
    case 0:
        g = u;
        b = p;
        break;
    case 1:
        r = u;
        b = p;
        break;
    case 2:
        r = p;
        b = u;
        break;
    case 3:
        r = p;
        g = u;
        break;
    case 4:
        r = u;
        g = p;
        break;
    case 5:
        g = p;
        b = u;
        break;
    }

    ...

    return rgb;
}
*/

/*
 * https://www.mikrocontroller.net/attachment/182641/rgb_hsv.c
 *
RGB hsv_to_rgb(HSV hsv)
{
    RGB rgb;
    uint8_t region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = (uint8_t)( ( (uint16_t)(hsv.h) * 6 ) >> 8 );
    remainder = (uint8_t)( (uint16_t)(hsv.h) * 6 - ( (uint16_t)(region) << 8 ) ) ;

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            rgb.r = hsv.v;
                        rgb.g = t;
                        rgb.b = p;
            break;
        case 1:
            rgb.r = q;
                        rgb.g = hsv.v;
                        rgb.b = p;
            break;
        case 2:
            rgb.r = p;
                        rgb.g = hsv.v;
                        rgb.b = t;
            break;
        case 3:
            rgb.r = p;
                        rgb.g = q;
                        rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t;
                        rgb.g = p;
                        rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v;
                        rgb.g = p;
                        rgb.b = q;
            break;
    }

    return rgb;
}
*/