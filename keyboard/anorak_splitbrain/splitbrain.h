#pragma once

#include <stdint.h>
#include <stdbool.h>

void splitbrain_config_init(void);
bool is_left_side_of_keyboard(void);
void receive_data_from_other_side(void);
