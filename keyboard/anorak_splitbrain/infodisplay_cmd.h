#ifndef KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_CMD_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_CMD_H_

#include <stdint.h>
#include "matrix.h"

#define MATRIX_CMD_SHOW_VERSION 0x01
#define MATRIX_CMD_STOP_CURRENT 0x02

#define MATRIX_CMD_SHOW_TEXT 0x10
#define MATRIX_CMD_SCROLL_TEXT 0x11

#define MATRIX_CMD_ANIMATE_SWEEP 0x20

#define MATRIX_CMD_TYPEMATRIX 0xF0
#define MATRIX_CMD_TYPEMATRIX_KEY 0xF1

#define MATRIX_CMD_READ_CFG 0xFD

#define MATRIX_CFG_REG_HEIGHT 0
#define MATRIX_CFG_REG_WIDTH 1
#define MATRIX_CFG_REG_MAJOR_VERSION 2
#define MATRIX_CFG_REG_MINOR_VERSION 3

#define MATRIX_TWI_ADDRESS (0x16 << 1)
#define MAX_TEXT_LENGTH 32

union _cmd_text
{
	struct _msg
	{
		uint8_t length;
		char text[MAX_TEXT_LENGTH];
	} msg;
	uint8_t raw[sizeof(struct _msg)] ;
};

typedef union _cmd_text cmd_text;

union _cmd_scroll_text
{
	struct _msg_scroll_text
	{
		uint8_t length;
		uint8_t speed;
		uint8_t direction;
		char text[MAX_TEXT_LENGTH];
	} msg;
	uint8_t raw[sizeof(struct _msg_scroll_text)] ;
};

typedef union _cmd_scroll_text cmd_scroll_text;

union _cmd_typematrix_key
{
	struct _msg_typematrix_key
	{
		uint8_t row_number;
		matrix_row_t row;
	} msg;
	uint8_t raw[sizeof(struct _msg_typematrix_key)] ;
};

typedef union _cmd_typematrix_key cmd_typematrix_key;

union _cmd_lock_state
{
	struct _msg_lock_state
	{
		uint8_t locks;
	} msg;
	uint8_t raw[sizeof(struct _msg_lock_state)] ;
};

typedef union _cmd_lock_state cmd_lock_state;

union _cmd_animation
{
	struct _msg_animation
	{
		uint8_t speed;
		uint8_t direction;
		uint8_t duration;
	} msg;
	uint8_t raw[sizeof(struct _msg_animation)] ;
};

typedef union _cmd_animation cmd_animation;

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_CMD_H_ */
