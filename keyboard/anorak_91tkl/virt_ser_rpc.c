
#include "backlight/issi/is31fl3733_91tkl.h"
#include "mini-snprintf.h"
#include "virtser.h"
#include "utils.h"
#include "nfo_led.h"
#include "timer.h"
#include "virt_ser_rpc.h"
#include <avr/pgmspace.h>
//#include <cstdarg>

#ifdef VIRTSER_ENABLE

#define DATAGRAM_START 0x02
#define DATAGRAM_STOP 0x03

#define DATAGRAM_USER_START '!'
#define DATAGRAM_USER_STOP '\n'

#define DATAGRAM_CMD_INFO 0x52

#define MAX_MSG_LENGTH 16
#define VIRT_SER_PRINTF_BUFFER_SIZE 256

#define vsprint(s) virtser_print(PSTR(s))
#define vsprintf(arg, ...) virtser_printf(PSTR(s), ...)
#define vsprintln(s) virtser_print(PSTR(s "\n"))

/* decimal */
#define print_dec(i) vsprintf("%u", i)
#define print_decs(i) vsprintf("%d", i)
/* hex */
#define print_hex4(i) vsprintf("%X", i)
#define print_hex8(i) vsprintf("%02X", i)
#define print_hex16(i) vsprintf("%04X", i)
#define print_hex32(i) vsprintf("%08lX", i)
/* binary */
#define print_bin4(i) vsprintf("%04b", i)
#define print_bin8(i) vsprintf("%08b", i)
#define print_bin16(i) vsprintf("%016b", i)
#define print_bin32(i) vsprintf("%032lb", i)
#define print_bin_reverse8(i) vsprintf("%08b", bitrev(i))
#define print_bin_reverse16(i) vsprintf("%016b", bitrev16(i))
#define print_bin_reverse32(i) vsprintf("%032lb", bitrev32(i))
/* print value utility */
#define print_val_dec(v) vsprintf(#v ": %u\n", v)
#define print_val_decs(v) vsprintf(#v ": %d\n", v)
#define print_val_hex8(v) vsprintf(#v ": %X\n", v)
#define print_val_hex16(v) vsprintf(#v ": %02X\n", v)
#define print_val_hex32(v) vsprintf(#v ": %04lX\n", v)
#define print_val_bin8(v) vsprintf(#v ": %08b\n", v)
#define print_val_bin16(v) vsprintf(#v ": %016b\n", v)
#define print_val_bin32(v) vsprintf(#v ": %032lb\n", v)
#define print_val_bin_reverse8(v) vsprintf(#v ": %08b\n", bitrev(v))
#define print_val_bin_reverse16(v) vsprintf(#v ": %016b\n", bitrev16(v))
#define print_val_bin_reverse32(v) vsprintf(#v ": %032lb\n", bitrev32(v))

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

int virtser_printf(const char *fmt, ...);

bool cmd_user_nfo(uint8_t argc, char **argv);
bool cmd_user_ram(uint8_t argc, char **argv);
bool cmd_user_eep(uint8_t argc, char **argv);
bool cmd_user_led(uint8_t argc, char **argv);
bool cmd_user_pwm(uint8_t argc, char **argv);

struct user_command_t user_command_table[] = {{"nfo", &cmd_user_ram, "", "show info"},
                                              {"ram", &cmd_user_ram, "", "show free ram"},
                                              {"eep", &cmd_user_eep, "", "show eeprom"},
                                              {"led", &cmd_user_led, "dev cs sw on", "enable/disable led"},
                                              {"pwm", &cmd_user_pwm, "dev cs sw bri", "set pwm"},
                                              {"", 0, "", ""}};

bool cmd_user_nfo(uint8_t argc, char **argv)
{
    print("\n\t- Version -\n");
    print("DESC: " STR(DESCRIPTION) "\n");
    print("VID: " STR(VENDOR_ID) "(" STR(MANUFACTURER) ") "
                                                       "PID: " STR(PRODUCT_ID) "(" STR(PRODUCT) ") "
                                                                                                "VER: " STR(
                                                                                                    DEVICE_VER) "\n");
    print("BUILD: " STR(VERSION) " (" __TIME__ " " __DATE__ ")\n");
    return true;
}

bool cmd_user_ram(uint8_t argc, char **argv)
{
    printf("free ram: %s\n", freeRam());
    return true;
}

bool cmd_user_eep(uint8_t argc, char **argv)
{
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

bool cmd_user_led(uint8_t argc, char **argv)
{
    if (argc != 4)
        return false;

    uint8_t dev = atoi(argv[0]);
    uint8_t cs = atoi(argv[1]);
    uint8_t sw = atoi(argv[2]);
    bool on = atoi(argv[3]);

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

    IS31FL3733 *device = (dev ? issi.lower->device : issi.upper->device);

    is31fl3733_set_pwm(device, cs, sw, pwm);
    is31fl3733_update_led_pwm(device);

    return true;
}

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

void interpret_user_command(uint8_t const *buffer, uint8_t length)
{
    char *str = buffer;
    char *token;
    char *command;
    uint8_t argc = 0;
    char *argv[10];
    struct user_command_t user_command;

    // user_command =

    command = strsep(&str, " ");

    dprintf("user_command: %s\n", command);

    while ((token = strsep(&str, " ")))
    {
        argv[argc] = token;
        argc++;
    }

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
        // user_command =
    }
}

void interpret_command(uint8_t const *buffer, uint8_t length)
{
    uint8_t cmd = get_datagram_cmd(buffer);
    // dprintf("cmd: %x %c, l:%u\n", cmd, cmd, length);

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
        // dprintf("recv: start\n");

        buffer_pos = 0;

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        recv_status = recvStatusFoundStart;
    }
    else if (datagram_is_userdata_start(ucData))
    {
        // dprintf("recv: user start\n");

        buffer_pos = 0;

        // recv_buffer[buffer_pos] = ucData;
        // buffer_pos++;

        recv_status = recvStatusFoundUserStart;
    }

    else if (recv_status == recvStatusFoundStart)
    {
        // start + len + cmd + payload + crc
        expected_length = ucData + 4;

        // dprintf("recv: expected len %u\n", expected_length);

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
        // dprintf("recv: bpos %u\n", buffer_pos);

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        if (buffer_pos >= expected_length)
            recv_status = recvStatusFindStop;
    }

    else if (recv_status == recvStatusFindStop && ucData == DATAGRAM_STOP)
    {
        // dprintf("recv: stop %u\n", buffer_pos);

        if (datagram_is_valid(buffer_pos, expected_length))
        {
            // dprintf("recv: ");
            // dump_buffer(recv_buffer, buffer_pos);
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
#endif
