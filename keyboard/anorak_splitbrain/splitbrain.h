#pragma once

#include <stdint.h>
#include <stdbool.h>

void splitbrain_config_init(void);
bool is_left_side_of_keyboard(void);
bool is_right_side_of_keyboard(void);
void receive_data_from_other_side(void);
void send_row_to_other_side(uint8_t row_number, matrix_row_t row);
void send_ping_to_other_side(void);
void validate_communication_to_other_side(void);
matrix_row_t get_other_sides_row(uint8_t row_number);
