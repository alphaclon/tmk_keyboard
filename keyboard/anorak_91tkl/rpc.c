

#include "rpc.h"
#include "raw_hid.h"
#include "config.h"

#ifndef RAW_ENABLE

void raw_hid_receive(uint8_t *data, uint8_t length)
{
    uint8_t command = data[0];
    switch (command)
    {
    case id_protocol_version:
    {
        msg_protocol_version *msg = (msg_protocol_version *)&data[1];
        msg->version = PROTOCOL_VERSION;
        break;
    }
#if USE_KEYMAPS_IN_EEPROM
    case id_keymap_keycode_load:
    {
        msg_keymap_keycode_load *msg = (msg_keymap_keycode_load *)&data[1];
        msg->keycode = keymap_keycode_load(msg->layer, msg->row, msg->column);
        break;
    }
    case id_keymap_keycode_save:
    {
        msg_keymap_keycode_save *msg = (msg_keymap_keycode_save *)&data[1];
        keymap_keycode_save(msg->layer, msg->row, msg->column, msg->keycode);
        break;
    }
    case id_keymap_default_save:
    {
        keymap_default_save();
        break;
    }
#endif // USE_KEYMAPS_IN_EEPROM
    case id_backlight_config_set_values:
    {
        msg_backlight_config_set_values *msg = (msg_backlight_config_set_values *)&data[1];
        backlight_config_set_values(msg);
        backlight_config_save();
        backlight_init_drivers();
        break;
    }
    case id_backlight_config_set_alphas_mods:
    {
        msg_backlight_config_set_alphas_mods *msg = (msg_backlight_config_set_alphas_mods *)&data[1];
        backlight_config_set_alphas_mods(msg->alphas_mods);
        backlight_config_save();
        break;
    }
    case id_backlight_set_key_color:
    {
        msg_backlight_set_key_color *msg = (msg_backlight_set_key_color *)&data[1];
        backlight_set_key_color(msg->row, msg->column, msg->hsv);
        break;
    }
    default:
    {
        // Unhandled message.
        data[0] = id_unhandled;
        break;
    }
    }

    // Return same buffer with values changed
    raw_hid_send(data, length);
}

#endif
