#ifndef KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_

#include <stdint.h>
#include "matrix.h"

void matrixcpu_init(void);

void mcpu_send_text(char const *msg);
void mcpu_send_scroll_text(char const *msg, uint8_t speed, uint8_t direction);
void mcpu_send_lock_state(uint8_t lock_state);
void mcpu_send_animation_sweep(uint8_t speed, uint8_t direction, uint8_t duration);

void mcpu_send_typematrix(uint8_t enable);
void mcpu_send_typematrix_row(uint8_t row_number, matrix_row_t row);

void mcpu_send_command(uint8_t command, uint8_t const *data, uint8_t data_length);

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_ */
