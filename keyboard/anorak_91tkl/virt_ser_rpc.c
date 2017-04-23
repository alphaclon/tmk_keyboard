
#include "backlight/issi/is31fl3733_91tkl.h"
#include "backlight/sector/sector_control.h"
#include "backlight/animations/animation.h"
#include "backlight/animations/animation_utils.h"
#include "config.h"
#include "virtser.h"
#include "util.h"
#include "crc8.h"
#include "utils.h"
#include "nfo_led.h"
#include "timer.h"
#include "virt_ser_rpc.h"
#include "eeconfig.h"
#include "keyboard.h"
#include "keycode.h"
#include "keymap.h"
#include "backlight.h"
#include "backlight/eeconfig_backlight.h"
#include "../../tmk_core/common/avr/xprintf.h"
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_VIRTSER
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define DATAGRAM_START 0x02
#define DATAGRAM_STOP 0x03

#define DATAGRAM_USER_START '!'
#define DATAGRAM_USER_STOP '\n'

#define DATAGRAM_CMD_INFO 0x52

#define MAX_MSG_LENGTH 16
#define VIRT_SER_PRINTF_BUFFER_SIZE 256

#define vserprint(s) xfprintf(&virtser_send, s)
#define vserprintf(fmt, ...) xfprintf(&virtser_send, fmt, ##__VA_ARGS__)
#define vserprintfln(fmt, ...) xfprintf(&virtser_send, fmt "\n", ##__VA_ARGS__)
#define vserprintln(s) xfprintf(&virtser_send, s "\n")

uint16_t last_receive_ts = 0;
uint8_t recv_buffer[MAX_MSG_LENGTH];

enum recvStatus
{
    recvStatusIdle = 0,
    recvStatusFoundStart = 1,
    recvStatusFoundUserStart = 2,
    recvStatusRecvPayload = 3,
    recvStatusFindStop = 4
};

enum recvStatus recv_status = recvStatusIdle;

struct user_command_t
{
    char *cmd;
    bool (*pfn_command)(uint8_t argc, char **argv);
    char *help_args;
    char *help_msg;
};

typedef struct user_command_t user_command;

bool cmd_user_help(uint8_t argc, char **argv);
bool cmd_user_info(uint8_t argc, char **argv);
bool cmd_user_ram(uint8_t argc, char **argv);
bool cmd_user_dump_eeprom(uint8_t argc, char **argv);
bool cmd_user_init_issi(uint8_t argc, char **argv);
bool cmd_user_led(uint8_t argc, char **argv);
bool cmd_user_pwm(uint8_t argc, char **argv);
bool cmd_user_rgb(uint8_t argc, char **argv);
bool cmd_user_hsv(uint8_t argc, char **argv);
bool cmd_user_sector(uint8_t argc, char **argv);

const user_command user_command_table[] PROGMEM = {{"help", &cmd_user_help, "", "show help"},
                                                   {"h", &cmd_user_help, "", "show help"},
                                                   {"?", &cmd_user_help, "", "show help"},
                                                   {"info", &cmd_user_info, "", "show info"},
                                                   {"ram", &cmd_user_ram, "", "show free ram"},
                                                   {"eeprom", &cmd_user_dump_eeprom, "", "dump eeprom"},
                                                   {"issi", &cmd_user_init_issi, "", "init issi"},
                                                   {"led", &cmd_user_led, "dev cs sw on", "enable/disable led"},
                                                   {"pwm", &cmd_user_pwm, "dev cs sw bri", "set pwm"},
                                                   {"rgb", &cmd_user_rgb, "dev row col r g b", "set rgb"},
                                                   {"hsv", &cmd_user_hsv, "dev row col h s v", "set hsv"},
                                                   {"sector", &cmd_user_sector, "selected [on [h s v]]", "set sector"},
												   {"animation", &cmd_user_sector, "selected [on [c h s v]]", "set animation"},
                                                   {"", 0, "", ""}};

bool cmd_user_help(uint8_t argc, char **argv)
{
    uint8_t pos = 0;
    user_command cmd;

    vserprintf("\n\t- Anorak 91tkl - virtual console - help -\n");
    vserprintf("\n\nAll commmands must start with a '!'\n\n");

    memcpy_P(&cmd, (const void *)&user_command_table[pos++], sizeof(struct user_command_t));

    while (cmd.pfn_command)
    {
        vserprintfln("%s [%s]: %s", cmd.cmd, cmd.help_args, cmd.help_msg);
        memcpy_P(&cmd, (const void *)&user_command_table[pos++], sizeof(struct user_command_t));
    }

    vserprintf("\n\n");
    return true;
}

bool cmd_user_animation(uint8_t argc, char **argv)
{
	// [[selected on] | [c h s v]]

	vserprintfln("animation: %u, running:%u", animation_current(), animation_is_running());

    if (argc == 0)
    {
    	vserprintfln("\t delay:%u, duration:%u", animation.delay_in_ms, animation.duration_in_ms);
    	vserprintfln("\t hsv1: h:%u, s:%u, v:%u", animation.hsv.h, animation.hsv.s, animation.hsv.v);
    	vserprintfln("\t hsv2: h:%u, s:%u, v:%u", animation.hsv2.h, animation.hsv2.s, animation.hsv2.v);
    	vserprintfln("\t  rgb: r:%u, g:%u, b:%u", animation.rgb.r, animation.rgb.g, animation.rgb.b);
    	return true;
    }

    if (argc == 1 && strcmp_P(PSTR("save"), argv[0]) == 0)
    {
    	vserprintfln("\t  save state");
    	animation_save_state();
    	return true;
    }

    if (argc == 1)
    {
    	uint16_t delay_in_ms = atoi(argv[0]);
    	animation_set_speed(delay_in_ms);
    	return true;
    }

    if (argc == 2)
    {
    	uint8_t selected_animation = atoi(argv[0]);
    	set_animation(selected_animation);

    	bool run_animation = atoi(argv[1]);
    	if (run_animation)
    		start_animation();
    	else
    		stop_animation();

    	vserprintfln("animation: set:%u, running:%u", animation_current(), animation_is_running());
    	return true;
    }

    if (argc >= 4)
    {
    	uint8_t pos = atoi(argv[0]);

        if (pos == 0)
        {
        	animation.hsv.h = atoi(argv[1]);
        	animation.hsv.s = atoi(argv[2]);
        	animation.hsv.v = atoi(argv[3]);
        }
        else
        {
        	animation.hsv2.h = atoi(argv[1]);
        	animation.hsv2.s = atoi(argv[2]);
        	animation.hsv2.v = atoi(argv[3]);
        }

    	vserprintfln("\t hsv1: h:%u, s:%u, v:%u", animation.hsv.h, animation.hsv.s, animation.hsv.v);
    	vserprintfln("\t hsv2: h:%u, s:%u, v:%u", animation.hsv2.h, animation.hsv2.s, animation.hsv2.v);
    	return true;
    }

    return false;
}

bool cmd_user_sector(uint8_t argc, char **argv)
{
    // selected [on [h s v]]

	vserprintf("sector ");

    if (argc == 0)
    {
    	vserprintfln(" state:");
    	for (uint8_t s = 0; s < SECTOR_MAX; ++s)
    		vserprintfln("\tsector %u: enabled:%u", s, sector_is_enabled(s));
    	return true;
    }

    if (argc == 1 && strcmp_P(PSTR("save"), argv[0]) == 0)
    {
    	vserprintfln("\n\t  save state");
    	sector_save_state();
    	return true;
    }

    uint8_t selected_sector = atoi(argv[0]);
    bool selected_sector_enabled = 0;

    if (argc >= 1)
    {
        vserprintfln("%u: enabled:%u", selected_sector, sector_is_enabled(selected_sector));
    }
    if (argc >= 2)
    {
        selected_sector_enabled = atoi(argv[1]);
        sector_select(selected_sector);
        sector_set_selected(selected_sector_enabled);

        vserprintfln("%u: enable:%u, enabled:%u", selected_sector, selected_sector_enabled, sector_is_enabled(selected_sector));
    }
    if (argc >= 5)
    {
        HSV hsv;

        hsv.h = atoi(argv[2]);
        hsv.s = atoi(argv[3]);
        hsv.v = atoi(argv[4]);

        sector_selected_set_hsv_color(hsv);
        is31fl3733_91tkl_update_led_pwm(&issi);

        vserprintfln("%u: set HSV: h:%u, s:%u, v:%u", selected_sector, hsv.h, hsv.s, hsv.v);
    }

    return true;
}

bool cmd_user_info(uint8_t argc, char **argv)
{
    vserprintf("\n\t- Anorak 91tkl - version -\n");
    vserprintf("DESC: " STR(DESCRIPTION) "\n");
    vserprintf("VID: " STR(VENDOR_ID) "(" STR(MANUFACTURER) ") "
                                                               "PID: " STR(PRODUCT_ID) "(" STR(
                                                                   PRODUCT) ") "
                                                                            "VER: " STR(DEVICE_VER) "\n");
    vserprintf("BUILD: " STR(VERSION) " (" __TIME__ " " __DATE__ ")\n");
    return true;
}

bool cmd_user_ram(uint8_t argc, char **argv)
{
    vserprintf("free ram: %d\n", freeRam());
    return true;
}

bool cmd_user_dump_eeprom(uint8_t argc, char **argv)
{
#undef print
#define print(s) vserprintf(s)
#undef print_dec
#define print_dec(i) vserprintf("%u", i)
#undef print_hex8
#define print_hex8(i) vserprintf("%02X", i)

#ifdef BOOTMAGIC_ENABLE
    print("default_layer: ");
    print_dec(eeconfig_read_default_layer());
    print("\n");

    debug_config_t dc;
    dc.raw = eeconfig_read_debug();
    print("debug_config.raw: ");
    print_hex8(dc.raw);
    print("\n");
    print(".enable: ");
    print_dec(dc.enable);
    print("\n");
    print(".matrix: ");
    print_dec(dc.matrix);
    print("\n");
    print(".keyboard: ");
    print_dec(dc.keyboard);
    print("\n");
    print(".mouse: ");
    print_dec(dc.mouse);
    print("\n");

    keymap_config_t kc;
    kc.raw = eeconfig_read_keymap();
    print("keymap_config.raw: ");
    print_hex8(kc.raw);
    print("\n");
    print(".swap_control_capslock: ");
    print_dec(kc.swap_control_capslock);
    print("\n");
    print(".capslock_to_control: ");
    print_dec(kc.capslock_to_control);
    print("\n");
    print(".swap_lalt_lgui: ");
    print_dec(kc.swap_lalt_lgui);
    print("\n");
    print(".swap_ralt_rgui: ");
    print_dec(kc.swap_ralt_rgui);
    print("\n");
    print(".no_gui: ");
    print_dec(kc.no_gui);
    print("\n");
    print(".swap_grave_esc: ");
    print_dec(kc.swap_grave_esc);
    print("\n");
    print(".swap_backslash_backspace: ");
    print_dec(kc.swap_backslash_backspace);
    print("\n");
    print(".nkro: ");
    print_dec(kc.nkro);
    print("\n");

#ifdef BACKLIGHT_ENABLE
    backlight_config_t bc;
    bc.raw = eeconfig_read_backlight();
    print("backlight_config.raw: ");
    print_hex8(bc.raw);
    print("\n");
    print(".enable: ");
    print_dec(bc.enable);
    print("\n");
    print(".level: ");
    print_dec(bc.level);
    print("\n");
#endif
#endif

    return true;
}

bool cmd_user_init_issi(uint8_t argc, char **argv)
{
    vserprintfln("init is31fl3733_91tkl");
    is31fl3733_91tkl_init(&issi);
    is31fl3733_91tkl_dump(&issi);
    return true;
}

bool cmd_user_led(uint8_t argc, char **argv)
{
    if (argc != 4)
        return false;

    uint8_t dev = atoi(argv[0]);
    uint8_t cs = atoi(argv[1]);
    uint8_t sw = atoi(argv[2]);
    bool on = atoi(argv[3]);

    vserprintfln("set led: dev:%u, cs:%u, sw:%u, on:%u", dev, cs, sw, on);

    IS31FL3733 *device = (dev ? issi.lower->device : issi.upper->device);

    is31fl3733_set_led(device, cs, sw, on);
    is31fl3733_update_led_enable(device);

    return true;
}

bool cmd_user_pwm(uint8_t argc, char **argv)
{
    if (argc != 4)
        return false;

    uint8_t dev = atoi(argv[0]);
    uint8_t cs = atoi(argv[1]);
    uint8_t sw = atoi(argv[2]);
    uint8_t pwm = atoi(argv[3]);

    vserprintfln("set pwm: dev:%u, cs:%u, sw:%u, pwm:%u", dev, cs, sw, pwm);

    IS31FL3733 *device = (dev ? issi.lower->device : issi.upper->device);

    is31fl3733_set_pwm(device, cs, sw, pwm);
    is31fl3733_update_led_pwm(device);

    return true;
}

bool cmd_user_rgb(uint8_t argc, char **argv)
{
    if (argc != 6)
        return false;

    RGB rgb;

    uint8_t dev = atoi(argv[0]);
    uint8_t row = atoi(argv[1]);
    uint8_t col = atoi(argv[2]);
    rgb.r = atoi(argv[3]);
    rgb.g = atoi(argv[4]);
    rgb.b = atoi(argv[5]);

    vserprintfln("set RGB: dev:%u, r:%u, c:%u, r:%u, g:%u, b:%u", dev, row, col, rgb.r, rgb.g, rgb.b);

    IS31FL3733_RGB *device = (dev ? issi.lower : issi.upper);

    is31fl3733_rgb_set_pwm(device, row, col, rgb);
    is31fl3733_update_led_pwm(device->device);

    return true;
}

bool cmd_user_hsv(uint8_t argc, char **argv)
{
    if (argc != 6)
        return false;

    HSV hsv;

    uint8_t dev = atoi(argv[0]);
    uint8_t row = atoi(argv[1]);
    uint8_t col = atoi(argv[2]);
    hsv.h = atoi(argv[3]);
    hsv.s = atoi(argv[4]);
    hsv.v = atoi(argv[5]);

    vserprintfln("set HSV: dev:%u, r:%u, c:%u, h:%u, s:%u, v:%u", dev, row, col, hsv.h, hsv.s, hsv.v);

    IS31FL3733_RGB *device = (dev ? issi.lower : issi.upper);

    is31fl3733_hsv_set_pwm(device, row, col, hsv);
    is31fl3733_update_led_pwm(device->device);

    return true;
}

#if 0
void virtser_send_data(uint8_t const *data, uint8_t length)
{
    // dprintf("send ");
    // dump_buffer(data, length);

    for (uint8_t i = 0; i < length; ++i)
        virtser_send(*data++);
}

/*
        %%       - print '%'
        %c       - character
        %s       - string
        %d, %u   - decimal integer
        %x, %X   - hex integer
*/

int virtser_printf(const char *fmt, ...)
{
    static char buffer[VIRT_SER_PRINTF_BUFFER_SIZE];

    int ret;
    va_list va;
    va_start(va, fmt);
    ret = mini_vsnprintf(buffer, VIRT_SER_PRINTF_BUFFER_SIZE, fmt, va);
    va_end(va);

    virtser_send_data(buffer, ret);

    return ret;
}

int virtser_printf_P(const char *fmt, ...)
{
    static char buffer[VIRT_SER_PRINTF_BUFFER_SIZE];
    static char fbuffer[32];

    strcpy_P(fbuffer, fmt);

    int ret;
    va_list va;
    va_start(va, fmt);
    ret = mini_vsnprintf(buffer, VIRT_SER_PRINTF_BUFFER_SIZE, fbuffer, va);
    va_end(va);

    virtser_send_data(buffer, ret);

    return ret;
}

int virtser_print(const char *s)
{
    int ret = 0;

    while (*s)
    {
        uart_putc(*s++);
        ret++;
    }

    return ret;
}

int virtser_print_P(const char *progmem_s)
{
    int ret = 0;
    register char c;

    while ((c = pgm_read_byte(progmem_s++)))
    {
        virtser_send(c);
        ret++;
    }

    return ret;
}
#endif

void interpret_user_command(uint8_t *buffer, uint8_t length)
{
    uint8_t pos = 0;
    char *str = (char*)buffer;
    char *token;
    char *command;
    uint8_t argc = 0;
    char *argv[10];
    struct user_command_t user_command;

    command = strsep(&str, " ");
    dprintf("user_command: %s\n", command);

    while ((token = strsep(&str, " ")))
    {
        argv[argc] = token;
        argc++;
    }

    memcpy_P(&user_command, &user_command_table[pos++], sizeof(struct user_command_t));

    while (user_command.pfn_command)
    {
        if (strcmp(user_command.cmd, command))
        {
            bool success = user_command.pfn_command(argc, argv);
            if (success)
            {
                print("OK\n");
            }
            else
            {
                print("cmd failed!\n");
            }
        }

        memcpy_P(&user_command, &user_command_table[pos++], sizeof(struct user_command_t));
    }
}

uint8_t get_datagram_cmd(uint8_t const *buffer)
{
	return buffer[2];
}

void interpret_command(uint8_t const *buffer, uint8_t length)
{
    uint8_t cmd = get_datagram_cmd(buffer);
    dprintf("cmd: %x %c, l:%u\n", cmd, cmd, length);

    if (cmd == DATAGRAM_CMD_INFO)
    {
        dprintf("recv info\n");
    }
}

void dump_buffer(uint8_t const *buffer, uint8_t length)
{
    dprintf("[");
    for (uint8_t i = 0; i < length; ++i)
        dprintf("%02X ", buffer[i]);
    dprintf("]\n");
}

bool check_crc(uint8_t const *buffer, uint8_t length)
{
    uint8_t crc = crc8_calc(buffer, 0x2D, length - 1);

    if (crc != buffer[length - 1])
    {
        dprintf("crc error: [%X] [%X]\n", crc, buffer[length - 1]);
        dprint("msg: ");
        dump_buffer(buffer, length);
        return false;
    }

    return true;
}

bool datagram_is_valid(uint8_t buffer_pos, uint8_t expected_length)
{
    return (buffer_pos == expected_length && check_crc(recv_buffer, buffer_pos));
}

bool datagram_is_data_start(uint8_t ucData)
{
    return (ucData == DATAGRAM_START && recv_status == recvStatusIdle);
}

bool datagram_is_userdata_start(uint8_t ucData)
{
    return (ucData == DATAGRAM_USER_START && recv_status == recvStatusIdle);
}

void virtser_recv(uint8_t ucData)
{
    static uint8_t buffer_pos = 0;
    static uint8_t expected_length = 0;

    last_receive_ts = timer_read();

    LedInfo2_On();
    // dprintf("recv: [%02X] s:%u\n", ucData, recv_status);

    if (datagram_is_data_start(ucData))
    {
        dprintf("recv: start\n");

        buffer_pos = 0;

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        recv_status = recvStatusFoundStart;
    }
    else if (datagram_is_userdata_start(ucData))
    {
        dprintf("recv: user start\n");

        buffer_pos = 0;

        // recv_buffer[buffer_pos] = ucData;
        // buffer_pos++;

        recv_status = recvStatusFoundUserStart;
    }

    else if (recv_status == recvStatusFoundStart)
    {
        // start + len + cmd + payload + crc
        expected_length = ucData + 4;

        dprintf("recv: expected len %u\n", expected_length);

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        recv_status = recvStatusRecvPayload;

        if (expected_length >= MAX_MSG_LENGTH)
        {
            // bail out
            buffer_pos = 0;
            recv_status = recvStatusIdle;
            print("recv: payload too long\n");
        }
    }
    else if (recv_status == recvStatusFoundUserStart)
    {
        if (ucData == DATAGRAM_USER_STOP)
        {
            interpret_user_command(recv_buffer, buffer_pos);

            buffer_pos = 0;
            recv_status = recvStatusIdle;
        }

        if (buffer_pos + 1 >= MAX_MSG_LENGTH)
        {
            // bail out
            buffer_pos = 0;
            recv_status = recvStatusIdle;
            print("recv: payload too long\n");
        }

        if (ucData != DATAGRAM_USER_STOP)
        {
            recv_buffer[buffer_pos] = ucData;
            buffer_pos++;
        }
    }

    else if (recv_status == recvStatusRecvPayload && buffer_pos < expected_length)
    {
        dprintf("recv: bpos %u\n", buffer_pos);

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        if (buffer_pos >= expected_length)
            recv_status = recvStatusFindStop;
    }

    else if (recv_status == recvStatusFindStop && ucData == DATAGRAM_STOP)
    {
        dprintf("recv: stop %u\n", buffer_pos);

        if (datagram_is_valid(buffer_pos, expected_length))
        {
            dprintf("recv: ");
            dump_buffer(recv_buffer, buffer_pos);
            interpret_command(recv_buffer, buffer_pos);
        }
        else
        {
            print("recv: crc invalid: ");
            dump_buffer(recv_buffer, buffer_pos);
        }

        buffer_pos = 0;
        recv_status = recvStatusIdle;
    }

    if ((recv_status == recvStatusRecvPayload || recv_status == recvStatusFindStop) &&
        (buffer_pos > expected_length || buffer_pos >= MAX_MSG_LENGTH))
    {
        buffer_pos = 0;
        expected_length = 0;
        recv_status = recvStatusIdle;
        print("recv: msg invalid: ");
        dump_buffer(recv_buffer, buffer_pos);
    }

    LedInfo2_Off();
}
