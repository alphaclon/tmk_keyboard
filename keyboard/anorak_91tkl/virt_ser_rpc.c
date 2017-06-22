
#include "backlight/issi/is31fl3733_91tkl.h"
#include "backlight/issi/is31fl3733_twi.h"
#include "backlight/issi/is31fl3733_sdb.h"
#include "backlight/sector/sector_control.h"
#include "backlight/animations/animation.h"
#include "backlight/animations/animation_utils.h"
#include "backlight/key_led_map.h"
#include "backlight/backlight_91tkl.h"
#include "twi/avr315/TWI_Master.h"
#include "keymap.h"
#include "action_layer.h"
#include "bootloader.h"
#include "config.h"
#include "virtser.h"
#include "util.h"
#include "crc8.h"
#include "utils.h"
#include "nfo_led.h"
#include "timer.h"
#include "host.h"
#include "virt_ser_rpc.h"
#include "eeconfig.h"
#include "keyboard.h"
#include "keycode.h"
#include "keymap.h"
#include "backlight.h"
#include "pwm.h"
#include "keyboard_layout_json.h"
#include "backlight/eeconfig_backlight.h"
#include "../../tmk_core/common/avr/xprintf.h"
#include "mini-snprintf.h"
#ifdef SLEEP_LED_ENABLE
#include "sleep_led.h"
#include "led.h"
#endif
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_VIRTSER
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define VIRTSER_ENABLE_ECHO
#define VIRT_SER_PRINTF_BUFFER_SIZE 64

#define DATAGRAM_START 0x02
#define DATAGRAM_STOP 0x03

#define DATAGRAM_USER_START '!'
#define DATAGRAM_USER_STOP '\n'

#define DATAGRAM_CMD_SET_PWM_MODE 0x51
#define DATAGRAM_CMD_SET_PWM_ROW 0x52
#define DATAGRAM_CMD_UPDATE_PWM 0x53
#define DATAGRAM_CMD_SAVE_PWM 0x54
#define DATAGRAM_CMD_GET_PWM_MODE 0x55
#define DATAGRAM_CMD_GET_PWM_ROW 0x56

#define MAX_MSG_LENGTH 32

int virtser_printf_P(const char *fmt, ...);
int virtser_print_P(const char *s);

#if 0
#define vserprint(s) xfprintf(&virtser_send, s)
#define vserprintf(fmt, ...) xfprintf(&virtser_send, fmt, ##__VA_ARGS__)
#define vserprintfln(fmt, ...) xfprintf(&virtser_send, fmt "\n", ##__VA_ARGS__)
#define vserprintln(s) xfprintf(&virtser_send, s "\n")
#else
#define vserprint(s) virtser_print_P(PSTR(s))
#define vserprintf(fmt, ...) virtser_printf_P(PSTR(fmt), ##__VA_ARGS__)
#define vserprintfln(fmt, ...) virtser_printf_P(PSTR(fmt "\n"), ##__VA_ARGS__)
#define vserprintln(s) virtser_print_P(PSTR(s "\n"))
#endif

#undef print
#define print(s) vserprintf(s)
#undef print_dec
#define print_dec(i) vserprintf("%u", i)
#undef print_hex8
#define print_hex8(i) vserprintf("%02X", i)

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
bool cmd_user_led(uint8_t argc, char **argv);
bool cmd_user_pwm(uint8_t argc, char **argv);
bool cmd_user_rgb(uint8_t argc, char **argv);
bool cmd_user_hsv(uint8_t argc, char **argv);
bool cmd_user_key_hsv(uint8_t argc, char **argv);
bool cmd_user_sector(uint8_t argc, char **argv);
bool cmd_user_animation(uint8_t argc, char **argv);
bool cmd_user_debug_config(uint8_t argc, char **argv);
bool cmd_user_keymap_config(uint8_t argc, char **argv);
bool cmd_user_bootloader_jump(uint8_t argc, char **argv);
bool cmd_user_backlight(uint8_t argc, char **argv);
bool cmd_user_eeprom_clear(uint8_t argc, char **argv);
bool cmd_user_backlight_eeprom_clear(uint8_t argc, char **argv);
bool cmd_user_default_layer(uint8_t argc, char **argv);
bool cmd_user_keymap_json(uint8_t argc, char **argv);
bool cmd_user_keymap_led_map(uint8_t argc, char **argv);
bool cmd_user_test_issi(uint8_t argc, char **argv);
bool cmd_user_status_leds_pwm(uint8_t argc, char **argv);
bool cmd_user_test_sleep_led(uint8_t argc, char **argv);

const user_command user_command_table[] PROGMEM = {
    {"?", &cmd_user_help, 0, "show help"},
    {"info", &cmd_user_info, 0, "show info"},
    {"ram", &cmd_user_ram, 0, "show free ram"},
    {"ee", &cmd_user_dump_eeprom, 0, "dump eeprom"},
    {"fee", &cmd_user_eeprom_clear, 0, "clear eeprom"},
    {"bee", &cmd_user_backlight_eeprom_clear, 0, "clear backlight eeprom"},
	{"bl", &cmd_user_backlight, 0, "backlight"},
    {"sector", &cmd_user_sector, "save | map # | # 0|1 | # h s v", "set sector"},
    {"animation", &cmd_user_animation, "save | delay ms | # 0|1 | c h s v", "set animation"},
	{"hsv", &cmd_user_key_hsv, "row col h s v", "set hsv"},
    {"debug", &cmd_user_debug_config, 0, "debug configuration"},
	{"keymap", &cmd_user_keymap_config, 0, "keymap configuration"},
    {"layer", &cmd_user_default_layer, 0, "default layer"},
    {"boot", &cmd_user_bootloader_jump, 0, "jump to bootloader"},
	{"pwm", &cmd_user_status_leds_pwm, 0, "set pwm for status leds"},

#ifdef SLEEP_LED_ENABLE
	{"sleepled", &cmd_user_test_sleep_led, 0, "test sleepled"},
#endif

#ifdef DEBUG_ISSI
    //{"keymap_json", &cmd_user_keymap_json, 0, "keymap json"},
    //{"ledmap", &cmd_user_keymap_led_map, 0, "row/col to led pos map"},
    {"issi", &cmd_user_test_issi, 0, "test issi"},
    {"tpwm", &cmd_user_pwm, "dev cs sw bri", "set pwm"},
    {"trgb", &cmd_user_rgb, "dev row col r g b", "set rgb"},
    {"thsv", &cmd_user_hsv, "dev row col h s v", "set hsv"},
    {"tled", &cmd_user_led, "dev cs sw on", "enable/disable led"},
#endif

    {0, 0, 0, 0}};

void dump_led_buffer(IS31FL3733 *device)
{
    uint8_t *led = is31fl3733_led_buffer(device);
    vserprintf("led buffer\n");
    for (uint8_t sw = 0; sw < IS31FL3733_SW; ++sw)
    {
        vserprintf("%02u: ", sw);
        for (uint8_t cs = 0; cs < (IS31FL3733_CS / 8); ++cs)
        {
            vserprintf("%02X ", led[sw * (IS31FL3733_CS / 8) + cs]);
        }
        vserprintf("\n");
    }
}

void dump_led_buffer_inverted(IS31FL3733 *device)
{
    uint8_t *led = is31fl3733_led_buffer(device);
    vserprintf("led buffer\n");
    for (uint8_t sw = 0; sw < IS31FL3733_SW; ++sw)
    {
        vserprintf("%02u: ", sw);
        for (uint8_t cs = 0; cs < (IS31FL3733_CS / 8); ++cs)
        {
            vserprintf("0x%02X ", (uint8_t) ~(led[sw * (IS31FL3733_CS / 8) + cs]));
            dprintf("b%b ", (uint8_t) ~(led[sw * (IS31FL3733_CS / 8) + cs]));
        }
        vserprintf("\n");
        dprintf("\n");
    }
}

void dump_pwm_buffer(IS31FL3733 *device)
{
    uint8_t *pwm = is31fl3733_pwm_buffer(device);
    vserprintf("issi: pwm buffer\n");
    for (uint8_t sw = 0; sw < IS31FL3733_SW; ++sw)
    {
        vserprintf("%02u: ", sw);
        for (uint8_t cs = 0; cs < IS31FL3733_CS; ++cs)
        {
            vserprintf("%02X ", pwm[sw * IS31FL3733_CS + cs]);
        }
        vserprintf("\n");
    }
}

void dump_args(uint8_t argc, char **argv)
{
    dprintf("args: ");
    for (uint8_t i = 0; i < argc; i++)
        dprintf("%s ", argv[i]);
    dprintf("\n");
}

bool cmd_user_help(uint8_t argc, char **argv)
{
    uint8_t pos = 0;
    user_command cmd;

    memcpy_P(&cmd, (const void *)&user_command_table[pos++], sizeof(struct user_command_t));

    while (cmd.pfn_command)
    {
        if (cmd.help_args)
            vserprintfln("%s [%s]: %s", cmd.cmd, cmd.help_args, cmd.help_msg);
        else
            vserprintfln("%s: %s", cmd.cmd, cmd.help_msg);

        memcpy_P(&cmd, (const void *)&user_command_table[pos++], sizeof(struct user_command_t));
    }

    vserprintf("\n");
    return true;
}

#ifdef SLEEP_LED_ENABLE
bool cmd_user_test_sleep_led(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        bool on = atoi(argv[0]);

        if (on)
        {
            sleep_led_enable();
        }
        else
        {
            sleep_led_disable();
            led_set(host_keyboard_leds());
        }

        return true;
    }

    return false;
}
#endif

bool cmd_user_status_leds_pwm(uint8_t argc, char **argv)
{
    if (argc == 1 && strcmp_P(argv[0], PSTR("init")) == 0)
    {
    	pwm_init();
    	return true;
    }

    if (argc == 1)
    {
    	uint8_t led = atoi(argv[0]);
    	uint8_t pwm;

    	if (led == 0)
    	{
    		pwm = get_pwm_caps_lock_led();
    	}
    	else
    	{
    		pwm = get_pwm_scroll_lock_led();
    	}

    	vserprintfln(".pwm %u %u", led, pwm);

    	return true;
    }

    if (argc == 2 && (strcmp_P(argv[1], PSTR("on")) == 0 || strcmp_P(argv[1], PSTR("off")) == 0))
    {
    	uint8_t led = atoi(argv[0]);
    	bool on = strcmp_P(argv[1], PSTR("on")) == 0;

    	if (led == 0)
    	{
    		pwm_caps_lock_led_enabled(on);
    	}
    	else
    	{
    		pwm_scroll_lock_led_enabled(on);
    	}

    	vserprintfln(".pwm %u %s", led, argv[1]);

    	return true;
    }

    if (argc == 2)
    {
    	uint8_t led = atoi(argv[0]);
    	uint8_t pwm = atoi(argv[1]);

    	if (led == 0)
    	{
    		pwm_caps_lock_led(pwm);
    	}
    	else
    	{
    		pwm_scroll_lock_led(pwm);
    	}

    	vserprintfln(".pwm %u %u", led, pwm);

    	return true;
    }

    return false;
}

bool cmd_user_test_issi(uint8_t argc, char **argv)
{
    bool found = false;

    if (argc == 1 && strcmp_P(argv[0], PSTR("detect")) == 0)
    {
        unsigned char slave_address;
        unsigned char device_present;

        // TWI_detect: 0 = device accessible, 1= failed to access device

        slave_address = IS31FL3733_I2C_ADDR(ADDR_GND, ADDR_GND);
        vserprintf("issi: upper device at 0x%X (GND, GND): ", slave_address);
        device_present = TWI_detect(slave_address);
        vserprintfln("%u", device_present ? 0 : 1);

        slave_address = IS31FL3733_I2C_ADDR(ADDR_VCC, ADDR_GND);
        vserprintf("issi: lower device at 0x%X (VCC, GNC): ", slave_address);
        device_present = TWI_detect(slave_address);
        vserprintfln("%u", device_present ? 0 : 1);

        /*
        for (uint8_t i = 0; i <= 3; i++)
        {
                for (uint8_t j = 0; j <= 3; j++)
                {
                        slave_address = IS31FL3733_I2C_ADDR(i, j);
                                vserprintf("issi: lower device at 0x%X (%u/%u): ", slave_address, i, j);
                                device_present = TWI_detect(slave_address);
                                vserprintfln("%u", device_present ? 0 : 1);
                }
        }
        */

        found = true;
    }
    else if (argc == 1 && strcmp_P(argv[0], PSTR("init")) == 0)
    {
        vserprintfln("init is31fl3733_91tkl");

        is31fl3733_91tkl_init(&issi);
        is31fl3733_91tkl_power_target(&issi, 450);

        sector_control_init();
        fix_backlight_level();
        initialize_animation();

        found = true;
    }
    else if (argc == 2 && strcmp_P(argv[0], PSTR("pwm")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);
        dump_pwm_buffer(device);
        found = true;
    }
    else if (argc == 3 && strcmp_P(argv[0], PSTR("pwm")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);
        uint8_t pwm = atoi(argv[2]);

        is31fl3733_fill(device, pwm);
        is31fl3733_update_led_pwm(device);

        found = true;
    }
    else if (argc == 2 && strcmp_P(argv[0], PSTR("led")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);
        dump_led_buffer(device);
        found = true;
    }
    else if (argc == 3 && strcmp_P(argv[0], PSTR("led")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);
        bool on = atoi(argv[2]);

        if (on)
        {
            is31fl3733_led_enable_all(device);
        }
        else
        {
            is31fl3733_led_disable_all(device);
        }

        is31fl3733_update_led_enable(device);

        found = true;
    }
    else if (argc == 2 && (strcmp_P(argv[0], PSTR("open")) == 0 || strcmp_P(argv[0], PSTR("short")) == 0))
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);

        is31fl3733_detect_led_open_short_states(device);

        vserprintfln("open states:");
        is31fl3733_read_led_open_states(device);
        dump_led_buffer(device);
        //dump_led_buffer_inverted(device);

        vserprintfln("short states:");
        is31fl3733_read_led_short_states(device);
        dump_led_buffer(device);

        found = true;
    }
    else if (argc == 2 && strcmp_P(argv[0], PSTR("power")) == 0)
    {
        uint16_t power = atoi(argv[1]);
        vserprintfln("issi: power: %u", power);
        is31fl3733_91tkl_power_target(&issi, power);

        found = true;
    }
    else if (argc == 3 && strcmp_P(argv[0], PSTR("gcc")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);

        device->gcc = atoi(argv[2]);
        vserprintfln("issi: gcc: %u", device->gcc);
        is31fl3733_update_global_current_control(device);

        found = true;
    }
    else if (argc == 3 && strcmp_P(argv[0], PSTR("hsd")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);

        bool enable = atoi(argv[2]);
        vserprintfln("issi: hsd: %u", enable);
        is31fl3733_hardware_shutdown(device, enable);

        found = true;
    }
    else if (argc == 3 && strcmp_P(argv[0], PSTR("ssd")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);

        bool enable = atoi(argv[2]);
        vserprintfln("issi: ssd: %u", enable);
        is31fl3733_software_shutdown(device, enable);

        found = true;
    }
    else if (argc == 4 && strcmp_P(argv[0], PSTR("res")) == 0)
    {
        IS31FL3733 *device = ((atoi(argv[1]) == 0) ? issi.lower->device : issi.upper->device);

        uint8_t swpur = atoi(argv[2]);
        uint8_t cspdr = atoi(argv[3]);

        is31fl3733_set_resistor_values(device, swpur, cspdr);

        found = true;
    }
    else if (argc == 1 && strcmp_P(argv[0], PSTR("map")) == 0)
    {
        uint8_t device_number;
        IS31FL3733_RGB *device;
        uint8_t row;
        uint8_t col;

        HSV hsv = {.h = 0, .s = 255, .v = 128};

        is31fl3733_fill(issi.upper->device, 0);
        is31fl3733_fill(issi.lower->device, 0);
        is31fl3733_led_enable_all(issi.upper->device);
        is31fl3733_led_enable_all(issi.lower->device);
        is31fl3733_91tkl_update(&issi);

        for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
        {
            for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
            {
                if (getLedPosByMatrixKey(key_row, key_col, &device_number, &row, &col))
                {
                    device = (device_number ? issi.upper : issi.lower);
                    is31fl3733_hsv_set_pwm(device, col, row, hsv);

                    is31fl3733_91tkl_update_led_pwm(&issi);

                    _delay_ms(500);
                    hsv.h += 3;
                }
            }
        }

        found = true;
    }
    else if (argc == 1 && strcmp_P(argv[0], PSTR("fast")) == 0)
    {
        is31fl3733_update_led_pwm_fast(issi.upper->device);
        found = true;
    }
    else if (argc == 1 && strcmp_P(argv[0], PSTR("que")) == 0)
    {
        queued_twi_stats();
#ifdef DEBUG_TX_QUEUE
        tx_queue_print_status();
#endif
        found = true;
    }

    return found;
}

bool cmd_user_led(uint8_t argc, char **argv)
{
    if (argc != 4)
        return false;

    uint8_t dev = atoi(argv[0]);
    uint8_t cs = atoi(argv[1]);
    uint8_t sw = atoi(argv[2]);
    bool on = atoi(argv[3]);

    IS31FL3733 *device = (dev == 0 ? issi.lower->device : issi.upper->device);
    vserprintfln("set led: dev:%u %X, cs:%u, sw:%u, on:%u", dev, device->address, cs, sw, on);
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

    IS31FL3733 *device = (dev == 0 ? issi.lower->device : issi.upper->device);
    vserprintfln("set pwm: dev:%u %X, cs:%u, sw:%u, pwm:%u", dev, device->address, cs, sw, pwm);
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

    IS31FL3733_RGB *device = (dev == 0 ? issi.lower : issi.upper);

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

    IS31FL3733_RGB *device = (dev == 0 ? issi.lower : issi.upper);

    is31fl3733_hsv_set_pwm(device, row, col, hsv);
    is31fl3733_update_led_pwm(device->device);

    return true;
}

bool cmd_user_backlight(uint8_t argc, char **argv)
{
#ifdef BACKLIGHT_ENABLE
	if (argc == 0)
	{
	    backlight_config_t bc;
	    bc.raw = eeconfig_read_backlight();
	    print(".backlight_config.raw: ");
	    print_hex8(bc.raw);
	    print("\n");
	    print(".enable: ");
	    print_dec(bc.enable);
	    print("\n");
	    print(".level: ");
	    print_dec(bc.level);
	    print("\n");
		return true;
	}

	uint8_t level = atoi(argv[0]);
	backlight_level(level);
#endif
	return true;
}

bool cmd_user_key_hsv(uint8_t argc, char **argv)
{
    if (argc != 5)
        return false;

    HSV hsv;
    uint8_t row;
    uint8_t col;
    uint8_t device_number;

    uint8_t col_to_change = atoi(argv[0]);
    uint8_t row_to_change = atoi(argv[1]);
    hsv.h = atoi(argv[2]);
    hsv.s = atoi(argv[3]);
    hsv.v = atoi(argv[4]);

    if (!getLedPosByMatrixKey(row_to_change, col_to_change, &device_number, &row, &col))
        return false;

    IS31FL3733_RGB *device = (device_number == 0 ? issi.lower : issi.upper);

    // vserprintfln("HSV: d:%u, r:%u, c:%u, h:%u, s:%u, v:%u", device_number, row, col, hsv.h, hsv.s, hsv.v);

    is31fl3733_hsv_direct_set_pwm(device, col, row, hsv);

    return true;
}

bool cmd_user_animation(uint8_t argc, char **argv)
{
    // "save | fps ms | # 0|1 | 0|1 h s v"

	vserprintf(".animation ");

    if (argc == 0)
    {
    	vserprintfln("%u", animation_current());
        vserprintfln(".running %u", animation_is_running());
        vserprintfln(".delay %u", animation.delay_in_ms);
        vserprintfln(".duration %u", animation.duration_in_ms);
        vserprintfln(".hsv1 %X %X %X", animation.hsv.h, animation.hsv.s, animation.hsv.v);
        vserprintfln(".hsv2 %X %X %X", animation.hsv2.h, animation.hsv2.s, animation.hsv2.v);
        vserprintfln(".rgb %X %X %X", animation.rgb.r, animation.rgb.g, animation.rgb.b);
        return true;
    }

    if (argc == 1)
	{
		uint8_t selected_animation = atoi(argv[0]);
		vserprintfln("%u %u", selected_animation, animation_is_running());
		return true;
	}

    if (argc == 2 && strcmp_P(argv[1], PSTR("save")) == 0)
    {
    	uint8_t selected_animation = atoi(argv[0]);
        vserprintfln("%u saved", selected_animation);
        animation_save_state();
        return true;
    }

    if (argc == 2)
    {
        uint8_t selected_animation = atoi(argv[0]);
        bool run_animation = atoi(argv[1]);

        if (run_animation)
            set_and_start_animation(selected_animation);
        else
            stop_animation();

        vserprintfln("%u %u", animation_current(), animation_is_running());
        return true;
    }

    if (argc == 3 && strcmp_P(argv[1], PSTR("fps")) == 0)
    {
    	//uint8_t selected_animation = atoi(argv[0]);
        uint16_t delay_in_ms = atoi(argv[2]);
        animation_set_speed(delay_in_ms);
        return true;
    }

    if (argc == 3 && strcmp_P(argv[1], PSTR("c")) == 0)
    {
    	uint8_t selected_animation = atoi(argv[0]);
    	uint8_t colorid = atoi(argv[2]);

    	if (colorid == 0)
    	{
    		vserprintfln("%u c %u %X %X %X", selected_animation, colorid, animation.hsv.h, animation.hsv.s, animation.hsv.v);
    	}
    	else
    	{
    		vserprintfln("%u c %u %X %X %X", selected_animation, colorid, animation.hsv2.h, animation.hsv2.s, animation.hsv2.v);
    	}

        return true;
    }

    if (argc >= 5 && strcmp_P(argv[1], PSTR("c")) == 0)
    {
    	uint8_t selected_animation = atoi(argv[0]);
        uint8_t colorid = atoi(argv[2]);

        if (colorid == 0)
        {
            animation.hsv.h = atoi(argv[3]);
            animation.hsv.s = atoi(argv[4]);
            animation.hsv.v = atoi(argv[5]);

            animation.rgb = hsv_to_rgb(animation.hsv);

            vserprintfln("%u c %u %X %X %X", selected_animation, colorid, animation.hsv.h, animation.hsv.s, animation.hsv.v);
        }
        else
        {
            animation.hsv2.h = atoi(argv[3]);
            animation.hsv2.s = atoi(argv[4]);
            animation.hsv2.v = atoi(argv[5]);

            vserprintfln("%u c %u %X %X %X", selected_animation, colorid, animation.hsv2.h, animation.hsv2.s, animation.hsv2.v);
        }

        return true;
    }

    return false;
}

bool cmd_user_sector(uint8_t argc, char **argv)
{
    // save | map # | # 0|1 | # h s v

	vserprintf(".sector");

    if (argc == 0)
    {
        for (uint8_t s = 0; s < SECTOR_MAX; ++s)
        {
            HSV hsv = sector_get_hsv_color(s);
            vserprintfln(" %u %u %X %X %X", s, sector_is_enabled(s), hsv.h, hsv.s, hsv.v);
        }
        return true;
    }

    if (argc == 1 && strcmp_P(argv[0], PSTR("save")) == 0)
    {
        vserprintfln(".saved");
        sector_save_state();
        return true;
    }

    if (argc == 1 && strcmp_P(argv[0], PSTR("map")) == 0)
    {
        vserprintfln(" %u", sector_get_custom_map());
        return true;
    }

    if (argc == 1)
    {
    	uint8_t s = atoi(argv[0]);
        HSV hsv = sector_get_hsv_color(s);
        vserprintfln(" %u %u %X %X %X", s, sector_is_enabled(s), hsv.h, hsv.s, hsv.v);
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("map")) == 0)
    {
        uint8_t custom_map = atoi(argv[1]);
        vserprintfln(" %u", custom_map);
        sector_set_custom_map(custom_map);
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("+")) == 0)
    {
        uint8_t inc = atoi(argv[1]);
        vserprintfln(".inc %u", inc);
        sector_selected_increase_hsv_color(inc);
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("-")) == 0)
    {
        uint8_t inc = atoi(argv[1]);
        vserprintfln(".dec %u", inc);
        sector_selected_decrease_hsv_color(inc);
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("++")) == 0)
    {
        uint8_t inc = atoi(argv[1]);
        vserprintfln(".ainc %u", inc);
        sector_all_increase_hsv_color(inc);
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("--")) == 0)
    {
        uint8_t inc = atoi(argv[1]);
        vserprintfln(".adec %u", inc);
        sector_all_decrease_hsv_color(inc);
        return true;
    }

    if (argc == 2)
    {
    	uint8_t selected_sector = atoi(argv[0]);
		bool selected_sector_enabled = atoi(argv[1]);
        sector_select(selected_sector);
        sector_set_selected(selected_sector_enabled);

        vserprintfln(" %u %u", selected_sector, sector_is_enabled(selected_sector));
        return true;
    }

    if (argc == 4)
    {
        HSV hsv;
        hsv.h = atoi(argv[1]);
        hsv.s = atoi(argv[2]);
        hsv.v = atoi(argv[3]);

        uint8_t s = atoi(argv[0]);

        sector_set_hsv_color(s, hsv);
        is31fl3733_91tkl_update_led_pwm(&issi);

        vserprintfln(" %u %X %X %X", s, hsv.h, hsv.s, hsv.v);
        return true;
    }

    return false;
}

bool cmd_user_info(uint8_t argc, char **argv)
{
	vserprintf(".device\n");
    vserprintf(".desc: " STR(DESCRIPTION) "\n");
    vserprintf(".vid: " STR(VENDOR_ID) "\n");
    vserprintf(".manufacturer: "  STR(MANUFACTURER) "\n");
    vserprintf(".pid: " STR(PRODUCT_ID) "\n");
    vserprintf(".product: " STR(PRODUCT) "\n");
    vserprintf(".version: " STR(DEVICE_VER) "\n");
    vserprintf(".build: " STR(VERSION) " (" __TIME__ " " __DATE__ ")\n");
    return true;
}

bool cmd_user_ram(uint8_t argc, char **argv)
{
    vserprintf(".free %d\n", freeRam());
    return true;
}

bool cmd_user_bootloader_jump(uint8_t argc, char **argv)
{
    bootloader_jump();
    return true;
}

bool cmd_user_eeprom_clear(uint8_t argc, char **argv)
{
    vserprintfln(".cleared");
    eeconfig_init();
    return true;
}

bool cmd_user_backlight_eeprom_clear(uint8_t argc, char **argv)
{
    vserprintfln(".cleared");
    eeconfig_backlight_init();
    return true;
}

bool cmd_user_default_layer(uint8_t argc, char **argv)
{
#ifdef BOOTMAGIC_ENABLE
    if (argc == 1 && strcmp_P(argv[0], PSTR("save")) == 0)
    {
        if (debug_config.raw != eeconfig_read_debug())
            eeconfig_write_debug(debug_config.raw);
    }
    else if (argc == 2 && strcmp_P(argv[0], PSTR("set")) == 0)
    {
        uint8_t default_layer = atoi(argv[1]);

        if (default_layer)
        {
            eeconfig_write_default_layer(default_layer);
            default_layer_set((uint32_t)default_layer);
        }
        else
        {
            default_layer = eeconfig_read_default_layer();
            default_layer_set((uint32_t)default_layer);
        }
    }
    else
    {
        print(".default_layer ");
        print_dec(eeconfig_read_default_layer());
        print("\n");
    }
#endif
    return true;
}

bool cmd_user_debug_config(uint8_t argc, char **argv)
{
#ifdef BOOTMAGIC_ENABLE
    if (argc == 1 && strcmp_P(argv[0], PSTR("save")) == 0)
    {
        if (debug_config.raw != eeconfig_read_debug())
            eeconfig_write_debug(debug_config.raw);
    }
    else if (argc == 2)
    {
        if (strcmp_P(argv[0], PSTR("enable")) == 0)
        {
            debug_config.enable = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("matrix")) == 0)
        {
            debug_config.matrix = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("keyboard")) == 0)
        {
            debug_config.keyboard = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("mouse")) == 0)
        {
            debug_config.mouse = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("raw")) == 0)
        {
            debug_config.raw = atoi(argv[1]);
        }
    }
    else
    {
        print(".debug_config.raw ");
        print_hex8(debug_config.raw);
        print("\n");
        print(".enable ");
        print_dec(debug_config.enable);
        print("\n");
        print(".matrix ");
        print_dec(debug_config.matrix);
        print("\n");
        print(".keyboard ");
        print_dec(debug_config.keyboard);
        print("\n");
        print(".mouse ");
        print_dec(debug_config.mouse);
        print("\n");
    }
#endif
    return true;
}

#ifdef BOOTMAGIC_ENABLE
extern keymap_config_t keymap_config;
#endif

bool cmd_user_keymap_config(uint8_t argc, char **argv)
{
#ifdef BOOTMAGIC_ENABLE
    if (argc == 1 && strcmp_P(argv[0], PSTR("save")) == 0)
    {
        if (keymap_config.raw != eeconfig_read_keymap())
            eeconfig_write_keymap(keymap_config.raw);
    }
    else if (argc == 2)
    {
        if (strcmp_P(argv[0], PSTR("swap_control_capslock")) == 0)
        {
            keymap_config.swap_control_capslock = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("capslock_to_control")) == 0)
        {
            keymap_config.capslock_to_control = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("swap_lalt_lgui")) == 0)
        {
            keymap_config.swap_lalt_lgui = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("swap_ralt_rgui")) == 0)
        {
            keymap_config.swap_ralt_rgui = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("no_gui")) == 0)
        {
            keymap_config.no_gui = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("swap_grave_esc")) == 0)
        {
            keymap_config.swap_grave_esc = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("swap_backslash_backspace")) == 0)
        {
            keymap_config.swap_backslash_backspace = atoi(argv[1]);
        }
        else if (strcmp_P(argv[0], PSTR("nkro")) == 0)
        {
            keymap_config.nkro = atoi(argv[1]);
#ifdef NKRO_ENABLE
            keyboard_nkro = keymap_config.nkro;
#endif
        }
        else if (strcmp_P(argv[0], PSTR("raw")) == 0)
        {
            keymap_config.raw = atoi(argv[1]);
        }
    }
    else
    {
        print(".keymap_config.raw ");
        print_hex8(keymap_config.raw);
        print("\n");
        print(".swap_control_capslock ");
        print_dec(keymap_config.swap_control_capslock);
        print("\n");
        print(".capslock_to_control ");
        print_dec(keymap_config.capslock_to_control);
        print("\n");
        print(".swap_lalt_lgui ");
        print_dec(keymap_config.swap_lalt_lgui);
        print("\n");
        print(".swap_ralt_rgui ");
        print_dec(keymap_config.swap_ralt_rgui);
        print("\n");
        print(".no_gui ");
        print_dec(keymap_config.no_gui);
        print("\n");
        print(".swap_grave_esc ");
        print_dec(keymap_config.swap_grave_esc);
        print("\n");
        print(".swap_backslash_backspace ");
        print_dec(keymap_config.swap_backslash_backspace);
        print("\n");
        print(".nkro ");
        print_dec(keymap_config.nkro);
        print("\n");
    }
#endif
    return true;
}

bool cmd_user_dump_eeprom(uint8_t argc, char **argv)
{
	cmd_user_keymap_config(0, 0);
	cmd_user_debug_config(0, 0);
	cmd_user_backlight(0, 0);

    return true;
}

bool cmd_user_keymap_led_map(uint8_t argc, char **argv)
{
    uint8_t row;
    uint8_t col;
    uint8_t dev;

    vserprint("[");

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; key_row++)
    {
        vserprint("[");

        for (uint8_t key_col = 0; key_col < MATRIX_COLS; key_col++)
        {
            if (getLedPosByMatrixKey(key_row, key_col, &dev, &row, &col))
            {
                vserprintf("{\"kr\":%u,\"kc\":%u,\"d\":%u,\"r\":%u,\"c\":%u},", key_row, key_col, dev, row, col);
            }
            else
            {
                vserprintf("{\"kr\":%u,\"kc\":%u,\"d\":255,\"r:\"255,\"c\":255},", key_row, key_col);
            }
        }

        vserprint("],");
    }

    vserprintln("]");

    return true;
}

bool cmd_user_keymap_json(uint8_t argc, char **argv)
{
    virtser_print_P(keyboard_layout_json);
    return true;
}

void virtser_send_data(uint8_t const *data, uint8_t length)
{
    // dprintf("send ");
    // dump_buffer(data, length);

	LedInfo1_On();
    for (uint8_t i = 0; i < length; ++i)
    {
        virtser_send(*data++);
    }
    LedInfo1_Off();
}

/*
        %%       - print '%'
        %c       - character
        %s       - string
        %d, %u   - decimal integer
        %x, %X   - hex integer
*/

#if 0
int virtser_printf(const char *fmt, ...)
{
    static char buffer[VIRT_SER_PRINTF_BUFFER_SIZE];

    int ret;
    va_list va;
    va_start(va, fmt);
    ret = mini_vsnprintf(buffer, VIRT_SER_PRINTF_BUFFER_SIZE, fmt, va);
    va_end(va);

    virtser_send_data((uint8_t const *)buffer, ret);

    return ret;
}
#endif

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

    virtser_send_data((uint8_t const *)buffer, ret);

    return ret;
}

#if 0
int virtser_print(const char *s)
{
    int ret = 0;

    while (*s)
    {
        virtser_send(*s++);
        ret++;
    }

    return ret;
}
#endif

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

void interpret_user_command(uint8_t *buffer, uint8_t length)
{
    uint8_t pos = 0;
    char *str = (char *)buffer;
    char *token;
    char *command;
    uint8_t argc = 0;
    char *argv[10];
    struct user_command_t user_command;

    // dprintf("buffer: <%s> l:%u\n", buffer, length);
    command = strsep(&str, " ");

    while ((token = strsep(&str, " ")))
    {
        argv[argc] = token;
        argc++;
    }

    memcpy_P(&user_command, &user_command_table[pos++], sizeof(struct user_command_t));

    // dprintf("uc: [%s] [%s]\n", user_command.cmd, user_command.help_msg);

    while (user_command.pfn_command)
    {
        if (strcmp(user_command.cmd, command) == 0)
        {
            dprintf("exec: %s\n", command);
            dump_args(argc, argv);

            bool success = user_command.pfn_command(argc, argv);
            if (success)
            {
                vserprintfln(">OK");
            }
            else
            {
                vserprintfln(">ERR");
            }

            break;
        }

        memcpy_P(&user_command, &user_command_table[pos++], sizeof(struct user_command_t));

        // dprintf("uc: [%s] [%s]\n", user_command.cmd, user_command.help_msg);
    }

    if (!user_command.pfn_command)
    {
    	vserprintfln(">NC");
    }
}

void command_set_pwm_row(uint8_t const *buffer, uint8_t length)
{
    struct cmd_set_pwm_row
    {
        // start + len + cmd + payload + crc8
        uint8_t start;
        uint8_t len;
        uint8_t cmd;
        uint8_t dev;
        uint8_t row;
        uint8_t pwm[IS31FL3733_CS];
    };

    struct cmd_set_pwm_row *cmd = (struct cmd_set_pwm_row *)buffer;

    if (cmd->len != IS31FL3733_CS + 3)
    {
        dprintf("invalid length: %d\n", cmd->len);
        return;
    }

    IS31FL3733 *device = (cmd->dev == 0 ? issi.lower->device : issi.upper->device);
    uint8_t *pwm_buffer = is31fl3733_pwm_buffer(device);
    memcpy(pwm_buffer + (cmd->row * IS31FL3733_CS), cmd->pwm, IS31FL3733_CS);
}

void command_update_pwm_row(uint8_t const *buffer, uint8_t length)
{
    struct cmd_update_pwm_row
    {
        // start + len + cmd + payload + crc8
        uint8_t start;
        uint8_t len;
        uint8_t cmd;
        uint8_t dev;
    };

    struct cmd_update_pwm_row *cmd = (struct cmd_update_pwm_row *)buffer;

    if (cmd->len != 2)
    {
        dprintf("invalid length: %d\n", cmd->len);
        return;
    }

    IS31FL3733 *device = (cmd->dev == 0 ? issi.lower->device : issi.upper->device);
    is31fl3733_update_led_pwm(device);
}

void command_set_pwm_mode(uint8_t const *buffer, uint8_t length)
{
    struct cmd_set_pwm_row
    {
        // start + len + cmd + payload + crc8
        uint8_t start;
        uint8_t len;
        uint8_t cmd;
        uint8_t map;
    };

    struct cmd_set_pwm_row *cmd = (struct cmd_set_pwm_row *)buffer;

    if (cmd->len != 2)
    {
        dprintf("invalid length: %d\n", cmd->len);
        return;
    }

    vserprintfln("\n    custom map %u", cmd->map);
    sector_set_custom_map(cmd->map);
}

void command_get_pwm_mode(uint8_t const *buffer, uint8_t length)
{
    struct cmd_get_pwm_mode
    {
        // start + len + cmd + payload + crc8
        uint8_t start;
        uint8_t len;
        uint8_t cmd;
        uint8_t map;
        uint8_t crc;
        uint8_t stop;
    };

    struct cmd_get_pwm_mode *cmd = (struct cmd_get_pwm_mode *)recv_buffer;

    uint8_t payload_length = sizeof(struct cmd_get_pwm_mode) - 2;

    cmd->start = DATAGRAM_START;
    cmd->stop = DATAGRAM_START;
    cmd->cmd = DATAGRAM_CMD_GET_PWM_MODE;
    cmd->len = payload_length;
    cmd->map = sector_get_custom_map();
    cmd->crc = crc8_calc(recv_buffer, 0x2D, payload_length);

    virtser_send_data(recv_buffer, sizeof(struct cmd_get_pwm_mode));
}

void command_get_pwm_row(uint8_t const *buffer, uint8_t length)
{
    struct cmd_get_pwm_row
    {
        // start + len + cmd + payload + crc8
        uint8_t start;
        uint8_t len;
        uint8_t cmd;
        uint8_t dev;
        uint8_t row;
        uint8_t pwm[IS31FL3733_CS];
        uint8_t crc;
        uint8_t stop;
    };

    struct cmd_get_pwm_row *cmd = (struct cmd_get_pwm_row *)recv_buffer;

    IS31FL3733 *device = (cmd->dev == 0 ? issi.lower->device : issi.upper->device);
    uint8_t *pwm_buffer = is31fl3733_pwm_buffer(device);
    memcpy(cmd->pwm, pwm_buffer + (cmd->row * IS31FL3733_CS), IS31FL3733_CS);

    uint8_t payload_length = sizeof(struct cmd_get_pwm_row) - 4;

    cmd->start = DATAGRAM_START;
    cmd->stop = DATAGRAM_START;
    cmd->cmd = DATAGRAM_CMD_GET_PWM_MODE;
    cmd->len = payload_length;
    cmd->crc = crc8_calc(recv_buffer, 0x2D, payload_length);

    virtser_send_data(recv_buffer, sizeof(struct cmd_get_pwm_row));
}

void command_save_pwm(uint8_t const *buffer, uint8_t length)
{
    sector_save_custom_pwm_map();
}

void command_update_pwm(uint8_t const *buffer, uint8_t length)
{
    // sector_save_custom_pwm_map();
}

uint8_t get_datagram_cmd(uint8_t const *buffer)
{
    return buffer[2];
}

void interpret_command(uint8_t const *buffer, uint8_t length)
{
    uint8_t cmd = get_datagram_cmd(buffer);
    dprintf("cmd: %x %c, l:%u\n", cmd, cmd, length);

    if (cmd == DATAGRAM_CMD_SET_PWM_ROW)
    {
        dprintf("recv set_pwm_row\n");
        command_set_pwm_row(buffer, length);
    }
    else if (cmd == DATAGRAM_CMD_UPDATE_PWM)
    {
        dprintf("recv update_pwm\n");
        command_update_pwm_row(buffer, length);
    }
    else if (cmd == DATAGRAM_CMD_SET_PWM_MODE)
    {
        dprintf("recv set_pwm_mode\n");
        command_set_pwm_mode(buffer, length);
    }
    else if (cmd == DATAGRAM_CMD_SAVE_PWM)
    {
        dprintf("recv save_pwm\n");
        command_save_pwm(buffer, length);
    }
    if (cmd == DATAGRAM_CMD_GET_PWM_ROW)
    {
        dprintf("recv get_pwm_row\n");
        command_get_pwm_row(buffer, length);
    }
    else if (cmd == DATAGRAM_CMD_GET_PWM_MODE)
    {
        dprintf("recv get_pwm_mode\n");
        command_get_pwm_mode(buffer, length);
    }
    else
    {
        dprintf("recv unknown cmd: %u\n", cmd);
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

#ifdef VIRTSER_ENABLE_ECHO
    virtser_send(ucData);
#endif

    // dprintf("recv: [%02X] <%c> s:%u\n", ucData, (char)ucData, recv_status);

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
        // dprintf("recv: user start\n");

        buffer_pos = 0;

        // do not store the user command start byte
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
        else
        {
            // dprintf("recv: user stop\n");
            recv_buffer[buffer_pos] = '\0';
            interpret_user_command(recv_buffer, buffer_pos);

            buffer_pos = 0;
            recv_status = recvStatusIdle;
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
        // dprintf("recv: stop %u\n", buffer_pos);

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
