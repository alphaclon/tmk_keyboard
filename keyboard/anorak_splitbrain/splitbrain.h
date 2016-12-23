#pragma once

#include "matrix.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void splitbrain_config_init(void);
void splitbrain_communication_task(void);
bool is_left_side_of_keyboard(void);
bool is_right_side_of_keyboard(void);
void receive_data_from_other_side(void);
void send_row_to_other_side(uint8_t row_number, matrix_row_t row);
void send_ping_to_other_side(void);
void send_sync_to_other_side(void);
void send_command_to_other_side(char const *cmd);
void communication_watchdog(void);
matrix_row_t get_other_sides_row(uint8_t row_number);

#ifdef __cplusplus
}
#endif
