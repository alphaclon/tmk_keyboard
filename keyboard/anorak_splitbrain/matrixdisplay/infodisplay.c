
#include "infodisplay.h"
#include "commands.h"
#include "twi/twi_config.h"
#include <string.h>
#include <util/delay.h>

#ifdef DEBUG_INFODISPLAY
#include "debug.h"
#else
#include "nodebug.h"
#endif

static bool _is_initialized = false;
static uint8_t cmd_buffer[MAX_MSG_LENGTH];
static uint8_t current_animation = MATRIX_FIRST_ANIMATION;

void mcpu_init()
{
    dprintf("mcpu_init");

    mcpu_send_command(MATRIX_CMD_INITIALIZE, 0, 0);
    mcpu_read_config();

    //mcpu_send_scroll_text(PSTR("Anorak splitbrain"), MATRIX_ANIMATION_DIRECTION_LEFT, 5);

    _is_initialized = true;
}

bool mcpu_is_initialized()
{
	return _is_initialized;
}

uint8_t mcpu_read_config_register8(uint8_t reg)
{
    uint8_t data;

#if TWILIB == AVR315

    TWI_write_byte_to_register(MATRIX_TWI_ADDRESS, MATRIX_CMD_READ_CFG, reg);
    _delay_ms(2);
    TWI_read_data(MATRIX_TWI_ADDRESS, 1);
    TWI_get_data_from_transceiver(&data, 1);

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
    dprintf("mcpu_send_command: c:0x%X l:%u\n\r", command, data_length);

#if TWILIB == AVR315

    TWI_write_data_to_register(MATRIX_TWI_ADDRESS, command, data, data_length);

#elif TWILIB == BUFFTW

    i2cMasterSendCommand(MATRIX_TWI_ADDRESS, command, data_length, data);

#else

    i2c_start_wait(MATRIX_TWI_ADDRESS + I2C_WRITE);
    i2c_write(0x24);
    for (uint8_t p = 0; p < data_length; ++p)
        i2c_write(pwm[p]);
    i2c_stop();

#endif
}

void prepare_command(cmd_animation *cts, uint8_t speed, uint8_t direction, uint8_t duration, uint8_t color,
                     uint8_t font)
{
    cts->msg.direction = direction;
    cts->msg.speed = speed;
    cts->msg.color = color;
    cts->msg.font = font;
    cts->msg.duration = duration;
}

void mcpu_read_config()
{
    unsigned char cfg[MATRIX_MAX_CFG_REG];

    dprintf("mcpu cfg\r\n");

#if TWILIB == AVR315

    TWI_write_byte(MATRIX_TWI_ADDRESS, MATRIX_CMD_READ_CFG);
    _delay_ms(2);
    TWI_read_data(MATRIX_TWI_ADDRESS, MATRIX_MAX_CFG_REG);
    bool lastTransOK = TWI_get_data_from_transceiver(cfg, MATRIX_MAX_CFG_REG);

    if (!lastTransOK)
    {
        dprintf("transmission failed! 0x%X\r\n", TWI_Get_State_Info());
        TWI_Master_Initialise();
        return;
    }

#elif TWILIB == BUFFTW

    i2cMasterSendCommandNI(MATRIX_TWI_ADDRESS, MATRIX_CMD_READ_CFG, 0, 0);
    i2cMasterReceiveNI(MATRIX_TWI_ADDRESS, MATRIX_MAX_CFG_REG, &cfg[0]);

#endif

    // current_animation = cfg[MATRIX_CFG_REG_ANIMATION];

    dprintf("mcpu v%u.%u\r\n", cfg[MATRIX_CFG_REG_MAJOR_VERSION], cfg[MATRIX_CFG_REG_MINOR_VERSION]);
}

void mcpu_read_and_dump_config()
{
    dprintf("mcpu dmp\r\n");

    for (uint8_t i = 0; i < MATRIX_MAX_CFG_REG; ++i)
    {
        uint8_t cfg = mcpu_read_config_register8(i);
        dprintf("%u: %u 0x%X\r\n", i, cfg, cfg);
    }

    dprintf("running: %u\r\n", mcpu_read_config_register8(MATRIX_CFG_REG_ANIMATION_STATE));
}

void mcpu_send_text(char const *msg)
{
    cmd_animation *cmd = (cmd_animation *)cmd_buffer;
    prepare_command(cmd, 0, 0, 0, 3, 0);

    cmd_text *txt = (cmd_text *)cmd_buffer;

    uint8_t length = strlen_P(msg);
    strncpy_P(txt->msg.text, msg, MAX_TEXT_LENGTH);
    txt->msg.text[MAX_TEXT_LENGTH - 1] = '\0';

    mcpu_send_command(MATRIX_CMD_ANIMATE_SHOW_TEXT, cmd_buffer, sizeof(cmd_animation) + length + 1);
}

void mcpu_send_scroll_text(char const *msg, uint8_t direction, uint8_t duration)
{
    cmd_animation *cmd = (cmd_animation *)cmd_buffer;
    prepare_command(cmd, 3, direction, duration, 3, 0);

    cmd_scroll_text *txt = (cmd_scroll_text *)cmd_buffer;

    uint8_t length = strlen_P(msg);
    strncpy_P(txt->msg.text, msg, MAX_TEXT_LENGTH);
    txt->msg.text[MAX_TEXT_LENGTH - 1] = '\0';

    mcpu_send_command(MATRIX_CMD_ANIMATE_SCROLL_TEXT, cmd_buffer, sizeof(cmd_animation) + length + 1);
}

void mcpu_send_lock_state(uint8_t lock_state)
{
    cmd_lock_state *cmd = (cmd_lock_state *)cmd_buffer;
    cmd->msg.locks = lock_state;

    mcpu_send_command(MATRIX_CMD_ANIMATE_LOCK_STATE, cmd_buffer, sizeof(cmd_lock_state));
}

void mcpu_send_animation(uint8_t animation, uint8_t speed, uint8_t direction, uint8_t duration, uint8_t color,
                         uint8_t font)
{
    dprintf("mcpu_send_animation: c:0x%X %u\n\r", animation, animation);
    cmd_animation *cmd = (cmd_animation *)cmd_buffer;
    prepare_command(cmd, speed, direction, duration, color, font);
    mcpu_send_command(animation, cmd_buffer, sizeof(cmd_animation));
}

void mcpu_send_animation_sweep(uint8_t direction, uint8_t duration)
{
    dprintf("sweep\r\n");
    mcpu_send_animation(MATRIX_CMD_ANIMATE_SWEEP, 3, direction, duration, 3, 0);
}

void mcpu_send_animation_box(uint8_t direction, uint8_t duration)
{
    dprintf("box\r\n");
    mcpu_send_animation(MATRIX_CMD_ANIMATE_BOX, 3, direction, duration, 3, 0);
}

void mcpu_send_animation_typematrix()
{
    dprintf("typematrix\r\n");
    mcpu_send_animation(MATRIX_CMD_ANIMATE_TYPEMATRIX, 3, MATRIX_ANIMATION_DIRECTION_LEFT, MATRIX_ANIMATION_RUN_FOREVER,
                        3, 0);
}

void mcpu_send_animation_stop()
{
    mcpu_send_command(MATRIX_CMD_ANIMATE_STOP, 0, 0);
}

void mcpu_send_animation_faster()
{
    mcpu_send_command(MATRIX_CMD_ANIMATE_FASTER, 0, 0);
}

void mcpu_send_animation_slower()
{
    mcpu_send_command(MATRIX_CMD_ANIMATE_SLOWER, 0, 0);
}

void mcpu_send_brightness_inc()
{
    mcpu_send_command(MATRIX_CMD_BRIGHTNESS_INC, 0, 0);
}

void mcpu_send_brightness_dec()
{
    mcpu_send_command(MATRIX_CMD_BRIGHTNESS_DEC, 0, 0);
}

void mcpu_send_brightness_set(uint8_t brightness)
{
    cmd_buffer[0] = brightness;
    mcpu_send_command(MATRIX_CMD_BRIGHTNESS_INC, cmd_buffer, 1);
}

void mcpu_send_sleep(uint8_t sleep)
{
    cmd_buffer[0] = sleep;
    mcpu_send_command(MATRIX_CMD_SLEEP_SET, cmd_buffer, 1);
}

void mcpu_send_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    cmd_typematrix_key *cmd = (cmd_typematrix_key *)cmd_buffer;
    cmd->msg.row_number = row_number;
    cmd->msg.row = row;

    mcpu_send_command(MATRIX_CMD_ANIMATE_TYPEMATRIX_KEY, cmd_buffer, sizeof(cmd_typematrix_key));
}

void mcpu_send_info_text_P(char const *msg)
{
    cmd_animation *cmd = (cmd_animation *)cmd_buffer;
    prepare_command(cmd, 0, 0, 0, 3, 0);

    cmd_text *txt = (cmd_text *)cmd_buffer;

    uint8_t length = strlen_P(msg);
    strncpy_P(txt->msg.text, msg, MAX_TEXT_LENGTH);
    txt->msg.text[MAX_TEXT_LENGTH - 1] = '\0';

    mcpu_send_command(MATRIX_CMD_ANIMATE_INFO_TEXT, cmd_buffer, sizeof(cmd_animation) + length + 1);
}

void mcpu_send_info_text(char const *msg)
{
    cmd_animation *cmd = (cmd_animation *)cmd_buffer;
    prepare_command(cmd, 0, 0, 0, 3, 0);

    cmd_text *txt = (cmd_text *)cmd_buffer;

    uint8_t length = strlen(msg);
    strncpy(txt->msg.text, msg, MAX_TEXT_LENGTH);
    txt->msg.text[MAX_TEXT_LENGTH - 1] = '\0';

    mcpu_send_command(MATRIX_CMD_ANIMATE_INFO_TEXT, cmd_buffer, sizeof(cmd_animation) + length + 1);
}

void mcpu_send_animation_next(void)
{
    mcpu_send_command(MATRIX_CMD_ANIMATE_START_NEXT, 0, 0);
}

void mcpu_send_animation_prev(void)
{
    mcpu_send_command(MATRIX_CMD_ANIMATE_START_PREV, 0, 0);
}

void mcpu_send_animation_toggle(void)
{
    mcpu_send_command(MATRIX_CMD_ANIMATE_TOGGLE, 0, 0);
}

void mcpu_start_animation(uint8_t animation_number)
{
    mcpu_send_animation_stop();
    _delay_ms(2);

    switch (animation_number)
    {
    case MATRIX_CMD_ANIMATE_SWEEP:
        mcpu_send_animation_sweep(MATRIX_ANIMATION_DIRECTION_LEFT, MATRIX_ANIMATION_RUN_FOREVER);
        break;
    case MATRIX_CMD_ANIMATE_BOX:
        mcpu_send_animation_box(MATRIX_ANIMATION_DIRECTION_LEFT, MATRIX_ANIMATION_RUN_FOREVER);
        break;
    case MATRIX_CMD_ANIMATE_TYPEMATRIX:
        mcpu_send_animation_typematrix();
        break;
    default:
        dprintf("set_animation: not found: %u\r\n", animation_number);
        break;
    }
}

void mcpu_animation_toggle(void)
{
    uint8_t running = mcpu_read_config_register8(MATRIX_CFG_REG_ANIMATION_STATE);

    dprintf("mcpu_animation_toggle: running: %u\r\n", running);

    if (running)
    {
        mcpu_send_animation_stop();
    }
    else
    {
        mcpu_start_animation(current_animation);
    }
}

void mcpu_animation_next()
{
    current_animation++;
    if (current_animation > MATRIX_LAST_ANIMATION)
        current_animation = MATRIX_FIRST_ANIMATION;

    dprintf("animation_next: %u\r\n", current_animation);

    mcpu_start_animation(current_animation);
}

void mcpu_animation_prev()
{
    if (current_animation <= MATRIX_FIRST_ANIMATION)
        current_animation = MATRIX_LAST_ANIMATION;
    else
        current_animation--;

    dprintf("animation_previous: %u\r\n", current_animation);

    mcpu_start_animation(current_animation);
}
