/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

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
#include <stdint.h>
#include "action.h"
#include <avr/pgmspace.h>
#include "keymap_common.h"

/*
 * Add different layouts. If no layout is defined the default layout will be set to ISO.
*/

#include "keymap_splitbrain.c"


#if 0
/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    if (layer < KEYMAPS_SIZE)
    {
        return pgm_read_byte(&keymaps[(layer)][(key.row)][(key.col)]);
    }
    else
    {
        // fall back to layer 0
        return pgm_read_byte(&keymaps[0][(key.row)][(key.col)]);
    }
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    action_t action;
    if (FN_INDEX(keycode) < FN_ACTIONS_SIZE)
    {
        action.code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]);
    }
    else
    {
        action.code = (action_t)ACTION_NO;
    }
    return action;
}
#endif