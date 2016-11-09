#ifndef KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_

#include <stdint.h>
#include "matrix.h"

#define MATRIX_CMD_SHOW_VERSION 0x01
#define MATRIX_CMD_TYPEMATRIX 0x10
#define MATRIX_CMD_TYPEMATRIX_KEY 0x11

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
	struct _msg
	{
		uint8_t length;
		uint8_t speed;
		uint8_t direction;
		char text[MAX_TEXT_LENGTH];
	} msg;
	uint8_t raw[sizeof(struct _msg)] ;
};

typedef union _cmd_scroll_text cmd_scroll_text;

union _cmd_typematrix_key
{
	struct _msg
	{
		uint8_t row_number;
		matrix_row_t row;
	} msg;
	uint8_t raw[sizeof(struct _msg)] ;
};

typedef union _cmd_typematrix_key cmd_typematrix_key;

union _cmd_lock_state
{
	struct _msg
	{
		uint8_t locks;
	} msg;
	uint8_t raw[sizeof(struct _msg)] ;
};

typedef union _cmd_lock_state cmd_lock_state;

union _cmd_animation
{
	struct _msg
	{
		uint8_t speed;
		uint8_t direction;
		uint8_t duration;
	} msg;
	uint8_t raw[sizeof(struct _msg)] ;
};

typedef union _cmd_animation cmd_animation;

void mcpu_send_command(uint8_t command, uint8_t const *data, uint8_t data_length);

void mcpu_send_text(char const *msg);
void mcpu_send_scroll_text(char const *msg, uint8_t speed, uint8_t direction);
void mcpu_send_lock_state(uint8_t lock_state);
void mcpu_send_animation_sweep(uint8_t speed, uint8_t direction, uint8_t duration);

void mcpu_send_typematrix(uint8_t enable);
void mcpu_send_typematrix_row(uint8_t row_number, matrix_row_t row);


#endif /* KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_ */
