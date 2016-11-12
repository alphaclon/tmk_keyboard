#include "infodisplay.h"
#include "twi_config.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#if TWILIB == AVR315
#include "backlight/avr315/TWI_Master.h"
#elif TWILIB == BUFFTW
#include "backlight/twi/twi_master.h"
#else
#include "backlight/i2cmaster/i2cmaster.h"
#endif
#ifdef __cplusplus
}
#endif


uint8_t cmd_buffer[TWI_SEND_DATA_BUFFER_SIZE];


void mcpu_init()
{

}

uint8_t mcpu_read_register8(uint8_t reg)
{
    uint8_t data;

#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_2(MATRIX_TWI_ADDRESS | (1 << TWI_READ_BIT), reg, &data, 1);
    TWI_Get_Data_From_Transceiver(&data, 1);

#elif TWILIB == BUFFTW

    i2cMasterSendCommandNI(MATRIX_TWI_ADDRESS, reg, 0, 0);
    i2cMasterReceiveNI(MATRIX_TWI_ADDRESS, 1, &data);

#else

    i2c_start_wait(MATRIX_TWI_ADDRESS + I2C_WRITE);
    i2c_write(reg);
    i2c_rep_start(MATRIX_TWI_ADDRESS + I2C_READ);
    data = i2c_readNak();
    i2c_stop();

#endif

    return data;
}

void mcpu_send_command(uint8_t command, uint8_t const *data, uint8_t data_length)
{
#if TWILIB == AVR315

    TWI_Start_Transceiver_With_Data_2(MATRIX_TWI_ADDRESS, command, data, data_length);

#elif TWILIB == BUFFTW

    //i2c_command.parts.cmd = 0x24;
    //memcpy(i2c_command.parts.data, pwm, ISSI_USED_CHANNELS);
    //i2cMasterSendNI(_issi_address, ISSI_USED_CHANNELS + 1, i2c_command.raw);

    i2cMasterSendCommand(MATRIX_TWI_ADDRESS, command, data_length, data);

#else

    i2c_start_wait(MATRIX_TWI_ADDRESS + I2C_WRITE);
    i2c_write(0x24);
    for (uint8_t p = 0; p < data_length; ++p)
        i2c_write(pwm[p]);
    i2c_stop();

#endif
}

void mcpu_send_text(char const *msg)
{
	cmd_text *cmd = (cmd_text *)cmd_buffer;

	cmd->msg.length = strlen(msg);
	strncpy(cmd->msg.text, msg, MAX_TEXT_LENGTH);

	mcpu_send_command(MATRIX_CMD_SHOW_TEXT, cmd_buffer, sizeof(cmd_text) - MAX_TEXT_LENGTH + cmd->msg.length /*1 + cmd->msg.length*/);
}

void mcpu_send_scroll_text(char const *msg, uint8_t speed, uint8_t direction)
{
	cmd_scroll_text *cmd = (cmd_scroll_text *)cmd_buffer;

	cmd->msg.length = strlen(msg);
	cmd->msg.speed = speed;
	cmd->msg.direction = direction;
	strncpy(cmd->msg.text, msg, MAX_TEXT_LENGTH);

	mcpu_send_command(MATRIX_CMD_SHOW_TEXT, cmd_buffer, sizeof(cmd_scroll_text) - MAX_TEXT_LENGTH + cmd->msg.length /*3 + cmd->msg.length*/);
}

void mcpu_send_lock_state(uint8_t lock_state)
{
	cmd_lock_state *cmd = (cmd_lock_state *)cmd_buffer;
	cmd->msg.locks = lock_state;

	mcpu_send_command(MATRIX_CMD_SCROLL_TEXT, cmd_buffer, sizeof(cmd_lock_state));
}

void mcpu_send_animation(uint8_t animation, uint8_t speed, uint8_t direction, uint8_t duration)
{
	cmd_animation *cmd = (cmd_animation *)cmd_buffer;

	cmd->msg.speed = speed;
	cmd->msg.direction = direction;
	cmd->msg.duration = duration;

	mcpu_send_command(animation, cmd_buffer, sizeof(cmd_animation));
}

void mcpu_send_animation_sweep(uint8_t speed, uint8_t direction, uint8_t duration)
{
	mcpu_send_animation(MATRIX_CMD_ANIMATE_SWEEP, speed, direction, duration);
}

void mcpu_send_typematrix(uint8_t enable)
{
	mcpu_send_command(MATRIX_CMD_TYPEMATRIX, &enable, 1);
}

void mcpu_send_typematrix_row(uint8_t row_number, matrix_row_t row)
{
	cmd_typematrix_key *cmd = (cmd_typematrix_key *)cmd_buffer;
	cmd->msg.row_number = row_number;
	cmd->msg.row = row;

	mcpu_send_command(MATRIX_CMD_TYPEMATRIX_KEY, cmd_buffer, sizeof(cmd_typematrix_key));
}
