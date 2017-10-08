
#if 1
#include "backlight/animations/animation.h"
#include "backlight/animations/animation_utils.h"
#include "backlight/key_led_map.h"
#include "backlight/backlight_kiibohd.h"
#include "matrixdisplay/infodisplay.h"
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

#define VIRTSER_MAX_MSG_LENGTH 32

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

uint16_t virtser_last_receive_ts = 0;
uint8_t recv_buffer[VIRTSER_MAX_MSG_LENGTH];

extern backlight_config_t backlight_config;

enum virtserRecvStatus
{
    recvStatusIdle = 0,
    recvStatusFoundStart = 1,
    recvStatusFoundUserStart = 2,
    recvStatusRecvPayload = 3,
    recvStatusFindStop = 4
};

enum virtserRecvStatus virtser_recv_status = recvStatusIdle;

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
bool cmd_user_sector(uint8_t argc, char **argv);
bool cmd_user_animation(uint8_t argc, char **argv);
bool cmd_user_debug_config(uint8_t argc, char **argv);
bool cmd_user_keymap_config(uint8_t argc, char **argv);
bool cmd_user_bootloader_jump(uint8_t argc, char **argv);
bool cmd_user_backlight(uint8_t argc, char **argv);
bool cmd_user_eeprom_clear(uint8_t argc, char **argv);
bool cmd_user_backlight_eeprom_clear(uint8_t argc, char **argv);
bool cmd_user_default_layer(uint8_t argc, char **argv);
bool cmd_user_infodisplay(uint8_t argc, char **argv);

const user_command user_command_table[] PROGMEM = {
    {"?", &cmd_user_help, 0, "show help"},
    {"info", &cmd_user_info, 0, "show info"},
    {"ram", &cmd_user_ram, 0, "show free ram"},
    {"ee", &cmd_user_dump_eeprom, 0, "dump eeprom"},
    {"fee", &cmd_user_eeprom_clear, 0, "clear eeprom"},
    {"bee", &cmd_user_backlight_eeprom_clear, 0, "clear backlight eeprom"},
	{"bl", &cmd_user_backlight, 0, "enable backlight"},
    {"sector", &cmd_user_sector, "save | map # | # 0|1 | # h s v", "set sector"},
    {"animation", &cmd_user_animation, "save | delay ms | # 0|1 | c h s v", "set animation"},
    {"debug", &cmd_user_debug_config, 0, "debug configuration"},
	{"keymap", &cmd_user_keymap_config, 0, "keymap configuration"},
    {"layer", &cmd_user_default_layer, 0, "default layer"},
    {"boot", &cmd_user_bootloader_jump, 0, "jump to bootloader"},
	{"mx", &cmd_user_infodisplay, 0, "infodisplay" },

    {0, 0, 0, 0}};

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

bool cmd_user_infodisplay(uint8_t argc, char **argv)
{
#ifdef DEBUG
	if (argc == 0 || strcmp_P(argv[0], PSTR("status")) == 0)
	{
		mcpu_read_and_dump_config();
	}
#endif

	if (argc >= 2 && strcmp_P(argv[0], PSTR("text")) == 0)
	{
		mcpu_send_scroll_text(argv[1], 0, 100);
		return true;
	}

	if (argc >= 2 && strcmp_P(argv[0], PSTR("stop")) == 0)
	{
		mcpu_send_animation_stop();
		return true;
	}

	if (argc >= 2 && strcmp_P(argv[0], PSTR("animation")) == 0)
	{

		uint8_t animation = atoi(argv[1]);

		uint8_t direction = (argc >= 3 ? atoi(argv[2]) : 0);
		uint8_t duration = (argc >= 4 ? atoi(argv[3]) : 0);

		mcpu_send_animation(animation, 3, direction, duration, 3, 0);

		return true;
	}

	return false;
}

bool cmd_user_backlight(uint8_t argc, char **argv)
{
#ifdef BACKLIGHT_ENABLE
    if (argc == 1 && strcmp_P(argv[0], PSTR("save")) == 0)
    {
    	if (backlight_config.raw != eeconfig_read_backlight())
    		eeconfig_write_backlight(backlight_config.raw);
    	return true;
    }
    else if (argc == 2)
    {
        if (strcmp_P(argv[0], PSTR("enable")) == 0)
        {
        	bool enable = atoi(argv[1]);
        	uint8_t level = enable ? backlight_config.level : 0;
        	backlight_level(level);
        	return true;
        }
        else if (strcmp_P(argv[0], PSTR("level")) == 0)
        {
        	uint8_t level = atoi(argv[0]);
        	backlight_level(level);
        	return true;
        }
        else if (strcmp_P(argv[0], PSTR("ee")) == 0)
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
    }
    else if (argc == 0)
	{
    	backlight_config_t bc;
	    bc.raw = backlight_config.raw;
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
#endif
	return false;
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
        vserprintfln(".hsv1 %X %X %X", 0x3c, 0xff, 0xff);
        vserprintfln(".hsv2 %X %X %X", 0x3c, 0xff, 0xff);
        vserprintfln(".rgb %X %X %X", 0xff, 0xff, 0x00);
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
    		vserprintfln("%u c %u %X %X %X", selected_animation, colorid, 0x3c, 0xff, 0xff);
    	}
    	else
    	{
    		vserprintfln("%u c %u %X %X %X", selected_animation, colorid, 0x3c, 0xff, 0xff);
    	}

        return true;
    }

    if (argc >= 5 && strcmp_P(argv[1], PSTR("c")) == 0)
    {
    	uint8_t selected_animation = atoi(argv[0]);
        uint8_t colorid = atoi(argv[2]);

        if (colorid == 0)
        {
        	/*
            animation.hsv.h = atoi(argv[3]);
            animation.hsv.s = atoi(argv[4]);
            animation.hsv.v = atoi(argv[5]);

            animation.rgb = hsv_to_rgb(animation.hsv);
			*/

            vserprintfln("%u c %u %X %X %X", selected_animation, colorid, 0x3c, 0xff, 0xff);
        }
        else
        {
        	/*
            animation.hsv2.h = atoi(argv[3]);
            animation.hsv2.s = atoi(argv[4]);
            animation.hsv2.v = atoi(argv[5]);
            */

            vserprintfln("%u c %u %X %X %X", selected_animation, colorid, 0x3c, 0xff, 0xff);
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
        for (uint8_t s = 0; s < BACKLIGHT_MAX_REGIONS; ++s)
        {
        	bool enabled = backlight_is_region_enabled(BACKLIGHT_BV(s));
        	uint8_t brightness = backlight_get_brightness_for_region(BACKLIGHT_BV(s));
        	vserprintfln(" %u %u %X %X %X", s, enabled, 0x3c, 0xff, brightness);
        }
        return true;
    }

    if (argc == 1 && strcmp_P(argv[0], PSTR("save")) == 0)
    {
        vserprintfln(".saved");
        backlight_save_region_states();
        return true;
    }

    if (argc == 1 && strcmp_P(argv[0], PSTR("map")) == 0)
    {
        vserprintfln(" %u", 0xff);
        return true;
    }

    if (argc == 1)
    {
    	uint8_t selected_sector = atoi(argv[0]);
    	bool enabled = backlight_is_region_enabled(BACKLIGHT_BV(selected_sector));
    	uint8_t brightness = backlight_get_brightness_for_region(BACKLIGHT_BV(selected_sector));
    	vserprintfln(" %u %u %X %X %X", selected_sector, enabled, 0x3c, 0xff, brightness);
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("map")) == 0)
    {
        //uint8_t custom_map = atoi(argv[1]);
        vserprintfln(" %u", 0xff);
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("+")) == 0)
    {
        uint8_t inc = atoi(argv[1]);
        vserprintfln(".inc %u", inc);
        backlight_increase_brightness_selected_region();
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("-")) == 0)
    {
        uint8_t dec = atoi(argv[1]);
        vserprintfln(".dec %u", dec);
        backlight_decrease_brightness_selected_region();
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("++")) == 0)
    {
        uint8_t inc = atoi(argv[1]);
        vserprintfln(".ainc %u", inc);
        backlight_increase_brightness();
        return true;
    }

    if (argc == 2 && strcmp_P(argv[0], PSTR("--")) == 0)
    {
        uint8_t inc = atoi(argv[1]);
        vserprintfln(".adec %u", inc);
        backlight_decrease_brightness();
        return true;
    }

    if (argc == 2)
    {
    	uint8_t selected_sector = atoi(argv[0]);
		bool selected_sector_enabled = atoi(argv[1]);

		backlight_select_region(BACKLIGHT_BV(selected_sector));
		if (selected_sector_enabled)
			backlight_selected_region_on();
		else
			backlight_selected_region_off();

		bool enabled = backlight_is_region_enabled(BACKLIGHT_BV(selected_sector));

        vserprintfln(" %u %u", selected_sector, enabled);
        return true;
    }

    if (argc == 4)
    {
    	uint8_t brightness = atoi(argv[3]);
        uint8_t selected_sector = atoi(argv[0]);

    	backlight_set_brightness_for_region(BACKLIGHT_BV(selected_sector), brightness);
    	brightness = backlight_get_brightness_for_region(BACKLIGHT_BV(selected_sector));
        vserprintfln(" %u %X %X %X", selected_sector, 0x3c, 0xff, brightness);

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

#ifdef STATUS_LED_PWM_ENABLE
bool cmd_user_statusled_eeprom_clear(uint8_t argc, char **argv)
{
    vserprintfln(".cleared");
    eeconfig_statusled_brightness_init();
    return true;
}
#endif

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

void virtser_send_data(uint8_t const *data, uint8_t length)
{
    // dprintf("send ");
    // virtser_dump_buffer(data, length);

	//LedInfo1_On();
    for (uint8_t i = 0; i < length; ++i)
    {
        virtser_send(*data++);
    }
    //LedInfo1_Off();
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
	/*
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
    */
}

void command_update_pwm_row(uint8_t const *buffer, uint8_t length)
{
	/*
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
    */
}

void command_set_pwm_mode(uint8_t const *buffer, uint8_t length)
{
	/*
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
    */
}

void command_get_pwm_mode(uint8_t const *buffer, uint8_t length)
{
	/*
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
    */
}

void command_get_pwm_row(uint8_t const *buffer, uint8_t length)
{
	/*
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
    */
}

void command_save_pwm(uint8_t const *buffer, uint8_t length)
{
    //sector_save_custom_pwm_map();
}

void command_update_pwm(uint8_t const *buffer, uint8_t length)
{
    // sector_save_custom_pwm_map();
}

uint8_t virtser_get_datagram_cmd(uint8_t const *buffer)
{
    return buffer[2];
}

void virtser_interpret_command(uint8_t const *buffer, uint8_t length)
{
    uint8_t cmd = virtser_get_datagram_cmd(buffer);
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

void virtser_dump_buffer(uint8_t const *buffer, uint8_t length)
{
    dprintf("[");
    for (uint8_t i = 0; i < length; ++i)
        dprintf("%02X ", buffer[i]);
    dprintf("]\n");
}

bool virtser_datagram_check_crc(uint8_t const *buffer, uint8_t length)
{
    uint8_t crc = crc8_calc(buffer, 0x2D, length - 1);

    if (crc != buffer[length - 1])
    {
        dprintf("crc error: [%X] [%X]\n", crc, buffer[length - 1]);
        dprint("msg: ");
        virtser_dump_buffer(buffer, length);
        return false;
    }

    return true;
}

bool virtser_datagram_is_valid(uint8_t buffer_pos, uint8_t expected_length)
{
    return (buffer_pos == expected_length && virtser_datagram_check_crc(recv_buffer, buffer_pos));
}

bool virtser_datagram_is_data_start(uint8_t ucData)
{
    return (ucData == DATAGRAM_START && virtser_recv_status == recvStatusIdle);
}

bool virtser_datagram_is_userdata_start(uint8_t ucData)
{
    return (ucData == DATAGRAM_USER_START && virtser_recv_status == recvStatusIdle);
}

void virtser_recv(uint8_t ucData)
{
    static uint8_t buffer_pos = 0;
    static uint8_t expected_length = 0;

    virtser_last_receive_ts = timer_read();

    //LedInfo2_On();

#ifdef VIRTSER_ENABLE_ECHO
    virtser_send(ucData);
#endif

    // dprintf("recv: [%02X] <%c> s:%u\n", ucData, (char)ucData, virtser_recv_status);

    if (virtser_datagram_is_data_start(ucData))
    {
        dprintf("recv: start\n");

        buffer_pos = 0;

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        virtser_recv_status = recvStatusFoundStart;
    }
    else if (virtser_datagram_is_userdata_start(ucData))
    {
        // dprintf("recv: user start\n");

        buffer_pos = 0;

        // do not store the user command start byte
        // recv_buffer[buffer_pos] = ucData;
        // buffer_pos++;

        virtser_recv_status = recvStatusFoundUserStart;
    }

    else if (virtser_recv_status == recvStatusFoundStart)
    {
        // start + len + cmd + payload + crc
        expected_length = ucData + 4;

        dprintf("recv: expected len %u\n", expected_length);

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        virtser_recv_status = recvStatusRecvPayload;

        if (expected_length >= VIRTSER_MAX_MSG_LENGTH)
        {
            // bail out
            buffer_pos = 0;
            virtser_recv_status = recvStatusIdle;
            print("recv: payload too long\n");
        }
    }
    else if (virtser_recv_status == recvStatusFoundUserStart)
    {
        if (buffer_pos + 1 >= VIRTSER_MAX_MSG_LENGTH)
        {
            // bail out
            buffer_pos = 0;
            virtser_recv_status = recvStatusIdle;
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
            virtser_recv_status = recvStatusIdle;
        }
    }

    else if (virtser_recv_status == recvStatusRecvPayload && buffer_pos < expected_length)
    {
        dprintf("recv: bpos %u\n", buffer_pos);

        recv_buffer[buffer_pos] = ucData;
        buffer_pos++;

        if (buffer_pos >= expected_length)
            virtser_recv_status = recvStatusFindStop;
    }

    else if (virtser_recv_status == recvStatusFindStop && ucData == DATAGRAM_STOP)
    {
        // dprintf("recv: stop %u\n", buffer_pos);

        if (virtser_datagram_is_valid(buffer_pos, expected_length))
        {
            dprintf("recv: ");
            virtser_dump_buffer(recv_buffer, buffer_pos);
            virtser_interpret_command(recv_buffer, buffer_pos);
        }
        else
        {
            print("recv: crc invalid: ");
            virtser_dump_buffer(recv_buffer, buffer_pos);
        }

        buffer_pos = 0;
        virtser_recv_status = recvStatusIdle;
    }

    if ((virtser_recv_status == recvStatusRecvPayload || virtser_recv_status == recvStatusFindStop) &&
        (buffer_pos > expected_length || buffer_pos >= VIRTSER_MAX_MSG_LENGTH))
    {
        buffer_pos = 0;
        expected_length = 0;
        virtser_recv_status = recvStatusIdle;
        print("recv: msg invalid: ");
        virtser_dump_buffer(recv_buffer, buffer_pos);
    }

    //LedInfo2_Off();
}
#endif
