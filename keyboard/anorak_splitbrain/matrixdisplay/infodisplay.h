#ifndef KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_
#define KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_

#include "matrix.h"
#include "commands.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void mcpu_init(void);
void mcpu_set_initialized(void);
bool mcpu_is_initialized(void);
void mcpu_hardware_shutdown(bool enabled);
void mcpu_toggle_hardware_shutdown(void);

void mcpu_read_config(void);
void mcpu_read_and_dump_config(void);

void mcpu_send_text(char const *msg);
void mcpu_send_scroll_text(char const *msg, uint8_t direction, uint8_t duration);
void mcpu_send_animation_sweep(uint8_t direction, uint8_t duration);
void mcpu_send_animation_box(uint8_t direction, uint8_t duration);
void mcpu_send_animation_typematrix(void);

void mcpu_send_lock_state(uint8_t lock_state);
void mcpu_send_typematrix_row(uint8_t row_number, matrix_row_t row);
void mcpu_send_info_text(char const *msg);
void mcpu_send_info_text_P(char const *msg);

void mcpu_send_animation_stop(void);
void mcpu_send_animation_faster(void);
void mcpu_send_animation_slower(void);
void mcpu_send_brightness_inc(void);
void mcpu_send_brightness_dec(void);
void mcpu_send_brightness_set(uint8_t brightness);
void mcpu_send_sleep(uint8_t sleep);
void mcpu_send_animation_toggle(void);
void mcpu_send_animation_next(void);
void mcpu_send_animation_prev(void);

void mcpu_animation_toggle(void);
void mcpu_animation_next(void);
void mcpu_animation_prev(void);

void mcpu_send_command(uint8_t command, uint8_t const *data, uint8_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* KEYBOARD_ANORAK_SPLITBRAIN_INFODISPLAY_H_ */
