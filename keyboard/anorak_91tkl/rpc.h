
#ifndef KEYBOARD_ANORAK_91TKL_RPC_H_
#define KEYBOARD_ANORAK_91TKL_RPC_H_

#include "backlight/color.h"

#define PROTOCOL_VERSION 0x0003

enum
{
    id_protocol_version = 0x01, // always 0x01
    id_keymap_keycode_load,
    id_keymap_keycode_save,
    id_keymap_default_save,
    id_backlight_config_set_values,
    id_backlight_config_set_alphas_mods,
    id_backlight_set_key_color,

    id_unhandled = 0xFF,
};

#if defined(__GNUC__)
#define PACKED __attribute__((__packed__))
#else
#define PACKED
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif

typedef struct PACKED
{
    uint16_t version;
} msg_protocol_version;

typedef struct PACKED
{
    uint8_t layer;
    uint8_t row;
    uint8_t column;
    uint16_t keycode;
} msg_keymap_keycode_load;

typedef struct PACKED
{
    uint8_t layer;
    uint8_t row;
    uint8_t column;
    uint16_t keycode;
} msg_keymap_keycode_save;

typedef struct PACKED
{
    bool use_split_backspace : 1;
    bool use_split_left_shift : 1;
    bool use_split_right_shift : 1;
    bool use_7u_spacebar : 1;
    bool use_iso_enter : 1;
    bool disable_when_usb_suspended : 1;
    uint8_t disable_after_timeout;
    uint8_t brightness;
    uint8_t effect;
    HSV color_1;
    HSV color_2;
    HSV caps_lock_indicator_color;
    uint8_t caps_lock_indicator_row;
    uint8_t caps_lock_indicator_column;
    HSV layer_1_indicator_color;
    uint8_t layer_1_indicator_row;
    uint8_t layer_1_indicator_column;
    HSV layer_2_indicator_color;
    uint8_t layer_2_indicator_row;
    uint8_t layer_2_indicator_column;
    HSV layer_3_indicator_color;
    uint8_t layer_3_indicator_row;
    uint8_t layer_3_indicator_column;
    // This is close to the packet size limit of 32
} msg_backlight_config_set_values;

typedef struct PACKED
{
    uint16_t alphas_mods[5];
} msg_backlight_config_set_alphas_mods;

typedef struct PACKED
{
    uint8_t row;
    uint8_t column;
    HSV hsv;
} msg_backlight_set_key_color;

#endif /* KEYBOARD_ANORAK_91TKL_RPC_H_ */