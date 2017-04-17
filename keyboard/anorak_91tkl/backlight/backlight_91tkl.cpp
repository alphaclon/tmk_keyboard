/*
Copyright 2016 Moritz Wenk <MoritzWenk@web.de>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "backlight_91tkl.h"
#include "backlight.h"
#include "config.h"
#include "eeconfig.h"
#include "eeconfig_backlight.h"
#include "issi/is31fl3733_91tkl.h"
#include "sector/sector_control.h"

#ifdef DEBUG_BACKLIGHT
#include "debug.h"
#else
#include "nodebug.h"
#endif

// Defined in tmk_core/common/backlight.h
void backlight_set(uint8_t level)
{
    dprintf("backlight_set level:%d\n", level);

     if (level == 0)
    {
		sector_set_all_off();
    }
    else
    {
    	sector_restore_state();
    }
}

void fix_backlight_level()
{
#ifdef BACKLIGHT_ENABLE
    if (!eeconfig_backlight_is_enabled())
    {
        eeconfig_backlight_init();
    }

    backlight_config_t backlight_config;
    backlight_config.raw = eeconfig_read_backlight();

    dprintf("backlight_setup on:%u, level:%u\r\n", backlight_config.enable, backlight_config.level);

    if (backlight_config.level == 0)
    {
        dprintf("fix level\r\n");
        backlight_config.level = BACKLIGHT_LEVELS - 1;
        eeconfig_write_backlight(backlight_config.raw);
    }
#endif
}

void backlight_setup()
{
    dprintf("backlight_setup\r\n");

#ifdef BACKLIGHT_ENABLE
    is31fl3733_91tkl_init(&issi);
    is31fl3733_91tkl_power_target(&issi, 450);

    sector_control_init();
    fix_backlight_level();
#endif
}

void backlight_setup_finish()
{
    dprintf("backlight_setup_finish\r\n");
}
