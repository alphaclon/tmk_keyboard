#ifndef ANIMATION_OPTIONS_H
#define ANIMATION_OPTIONS_H

enum animation_options_t
{
    animation_option_none = 0x0000,
    animation_option_left_to_right = 0x0001,
    animation_option_right_to_left = 0x0002,
    animation_option_up_down = 0x0004,
    animation_option_down_up = 0x0008,
    animation_option_variant_1 = 0x0010,
    animation_option_variant_2 = 0x0020,
    animation_option_variant_3 = 0x0040,
    animation_option_monochrome = 0x0080,
    animation_option_color = 0x0100,
};

typedef enum animation_options_t animation_options;

#endif // ANIMATION_OPTIONS_H
