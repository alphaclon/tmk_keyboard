#pragma once

#include "matrix.h"
#include <stdint.h>

#define MATRIX_CMD_INITIALIZE 0x01

#define MATRIX_CMD_CONTROL 0x10

#define MATRIX_CMD_BRIGHTNESS_SET (MATRIX_CMD_CONTROL | 0x01)
#define MATRIX_CMD_BRIGHTNESS_INC (MATRIX_CMD_CONTROL | 0x02)
#define MATRIX_CMD_BRIGHTNESS_DEC (MATRIX_CMD_CONTROL | 0x03)
#define MATRIX_CMD_SLEEP_SET (MATRIX_CMD_CONTROL | 0x04)
#define MATRIX_CMD_INVERT_SET (MATRIX_CMD_CONTROL | 0x05)

#define MATRIX_CMD_ANIMATION 0x80

#define MATRIX_CMD_ANIMATE_SHOW_TEXT (MATRIX_CMD_ANIMATION | 0x01)
#define MATRIX_CMD_ANIMATE_SCROLL_TEXT (MATRIX_CMD_ANIMATION | 0x02)
#define MATRIX_CMD_ANIMATE_SWEEP (MATRIX_CMD_ANIMATION | 0x03)
#define MATRIX_CMD_ANIMATE_TYPEMATRIX (MATRIX_CMD_ANIMATION | 0x04)

#define MATRIX_FIRST_ANIMATION MATRIX_CMD_ANIMATE_SWEEP
#define MATRIX_LAST_ANIMATION MATRIX_CMD_ANIMATE_TYPEMATRIX

#define MATRIX_CMD_ANIMATE_CONTROL 0xC0

#define MATRIX_CMD_ANIMATE_STOP (MATRIX_CMD_ANIMATE_CONTROL | 0x01)
#define MATRIX_CMD_ANIMATE_RESTART (MATRIX_CMD_ANIMATE_CONTROL | 0x02)
#define MATRIX_CMD_ANIMATE_TOGGLE (MATRIX_CMD_ANIMATE_CONTROL | 0x03)
#define MATRIX_CMD_ANIMATE_START_PREV (MATRIX_CMD_ANIMATE_CONTROL | 0x04)
#define MATRIX_CMD_ANIMATE_START_NEXT  (MATRIX_CMD_ANIMATE_CONTROL | 0x05)
#define MATRIX_CMD_ANIMATE_SLOWER (MATRIX_CMD_ANIMATE_CONTROL | 0x06)
#define MATRIX_CMD_ANIMATE_FASTER (MATRIX_CMD_ANIMATE_CONTROL | 0x07)
#define MATRIX_CMD_ANIMATE_SET_SPEED (MATRIX_CMD_ANIMATE_CONTROL | 0x08)
#define MATRIX_CMD_ANIMATE_TYPEMATRIX_KEY (MATRIX_CMD_ANIMATE_CONTROL | 0x09)
#define MATRIX_CMD_ANIMATE_LOCK_STATE (MATRIX_CMD_ANIMATE_CONTROL | 0x0A)

#define MATRIX_CMD_READ_CFG 0xFD

#define MATRIX_CFG_REG_BRIGHTNESS 0
#define MATRIX_CFG_REG_ANIMATION 1
#define MATRIX_CFG_REG_ANIMATION_STATE 2
#define MATRIX_CFG_REG_ANIMATION_SPEED 3
#define MATRIX_CFG_REG_FAULT 4
#define MATRIX_CFG_REG_WIDTH 5
#define MATRIX_CFG_REG_HEIGHT 6
#define MATRIX_CFG_REG_MAJOR_VERSION 7
#define MATRIX_CFG_REG_MINOR_VERSION 8

#define MATRIX_MAX_CFG_REG 9

#define MATRIX_ANIMATION_DIRECTION_LEFT 1
#define MATRIX_ANIMATION_DIRECTION_RIGHT 2
#define MATRIX_ANIMATION_DIRECTION_UP 3
#define MATRIX_ANIMATION_DIRECTION_DOWN 4

#define MATRIX_ANIMATION_RUN_FOREVER 0xff

#define MATRIX_ANIMATION_MIN_SPEED 10
#define MATRIX_ANIMATION_CHANGE_SPEED 10

#define MATRIX_TWI_ADDRESS (0x16 << 1)
#define MAX_TEXT_LENGTH 24
#define MAX_MSG_LENGTH 24 + 6

union _cmd_text {
    struct _msg
    {
        uint8_t speed;
        uint8_t direction;
        uint8_t duration;
        uint8_t color;
        uint8_t font;
        char text[MAX_TEXT_LENGTH];
    } msg;
    uint8_t raw[sizeof(struct _msg)];
};

typedef union _cmd_text cmd_text;

union _cmd_scroll_text {
    struct _msg_scroll_text
    {
        uint8_t speed;
        uint8_t direction;
        uint8_t duration;
        uint8_t color;
        uint8_t font;
        char text[MAX_TEXT_LENGTH];
    } msg;
    uint8_t raw[sizeof(struct _msg_scroll_text)];
};

typedef union _cmd_scroll_text cmd_scroll_text;

union _cmd_typematrix_key {
    struct _msg_typematrix_key
    {
        uint8_t row_number;
        matrix_row_t row;
    } msg;
    uint8_t raw[sizeof(struct _msg_typematrix_key)];
};

typedef union _cmd_typematrix_key cmd_typematrix_key;

union _cmd_lock_state {
    struct _msg_lock_state
    {
        uint8_t locks;
    } msg;
    uint8_t raw[sizeof(struct _msg_lock_state)];
};

typedef union _cmd_lock_state cmd_lock_state;

union _cmd_animation {
    struct _msg_animation
    {
        uint8_t speed;
        uint8_t direction;
        uint8_t duration;
        uint8_t color;
        uint8_t font;
    } msg;
    uint8_t raw[sizeof(struct _msg_animation)];
};

typedef union _cmd_animation cmd_animation;
