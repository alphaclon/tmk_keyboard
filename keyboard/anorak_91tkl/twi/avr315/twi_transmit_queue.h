#ifndef TX_QUEUE_H_
#define TX_QUEUE_H_

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TX_QUEUE_SIZE 16 // muss 2^n betragen (2, 4, 8, 16, 32, 64 ...)
#define TX_QUEUE_PAYLOAD_SIZE 16 + 2

struct _tx_queue_data_t
{
    uint8_t data_length;
    uint8_t data[TX_QUEUE_PAYLOAD_SIZE];
};

typedef struct _tx_queue_data_t tx_queue_data_t;

uint8_t tx_queue_size(void);
bool tx_queue_is_empty(void);
bool tx_queue_is_full(void);

bool tx_queue_get_empty_tail(tx_queue_data_t **tail);
bool tx_queue_push_tail(void);

bool tx_queue_front(tx_queue_data_t **head);
bool tx_queue_back(tx_queue_data_t **tail);

bool tx_queue_pop(void);

void tx_queue_print_status(void);
void tx_queue_test(void);

#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H_ */
