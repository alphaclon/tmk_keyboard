#include "twi_transmit_queue.h"
#include <string.h>

//#define USE_ATOMIC_BLOCK

#ifdef USE_ATOMIC_BLOCK
#include <util/atomic.h>
#else
#define ATOMIC_BLOCK(arg)
#define ATOMIC_RESTORESTATE 0
#endif

#if defined(DEBUG_TX_QUEUE)
#include "debug.h"
#else
#include "nodebug.h"
#endif

#if defined(DEBUG_TX_QUEUE)
#define LS_(arg) dprintf(arg "\r\n")
#define LV_(s, args...) dprintf(s "\r\n", args)
#define LVN_(s, args...) dprintf(s, args)
#else
#define LS_(arg)
#define LV_(s, args...)
#define LVN_(s, args...)
#endif

#define QUEUE_MASK (TX_QUEUE_SIZE - 1) // Klammern auf keinen Fall vergessen

static struct _queue_t
{
    tx_queue_data_t data[TX_QUEUE_SIZE];
    uint8_t read;  // Start, zeigt auf das Feld mit dem ältesten Inhalt
    uint8_t write; // Ende, zeigt immer auf leeres Feld
} tx_queue = {{}, 0, 0};

bool tx_queue_is_empty()
{
    LS_("tx_queue_is_empty");
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (tx_queue.read == tx_queue.write)
        {
            return true;
        }
    }
    return false;
}

bool tx_queue_is_full()
{
    LS_("tx_queue_is_full");
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t next = ((tx_queue.write + 1) & QUEUE_MASK);
        if (tx_queue.read == next)
        {
            return true;
        }
    }
    return false;
}

bool tx_queue_get_empty_tail(tx_queue_data_t **data)
{
    LS_("tx_queue_get_empty_tail");
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t next = ((tx_queue.write + 1) & QUEUE_MASK);
        if (tx_queue.read == next)
            return false;

        *data = &(tx_queue.data[tx_queue.write]);
    }
    return true;
}

bool tx_queue_push_tail()
{
    LS_("tx_queue_push_tail");
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t next = ((tx_queue.write + 1) & QUEUE_MASK);
        if (tx_queue.read == next)
            return false;

        tx_queue.write = next;
    }
    return true;
}

bool tx_queue_push(uint8_t const *data, uint8_t data_length)
{
    LS_("tx_queue_push");
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t next = ((tx_queue.write + 1) & QUEUE_MASK);
        if (tx_queue.read == next)
            return false;

        if (data == 0 || data_length == 0 || data_length > TX_QUEUE_PAYLOAD_SIZE)
            return false;

        // queue.data[queue.write & QUEUE_MASK] = byte; // absolut sicher

        memcpy(tx_queue.data[tx_queue.write].data, data, data_length);
        tx_queue.data[tx_queue.write].data_length = data_length;

        tx_queue.write = next;
    }
    return true;
}

bool tx_queue_front(tx_queue_data_t **head)
{
    LS_("tx_get_queue_head");
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (tx_queue.read == tx_queue.write)
        {
            *head = 0;
            return false;
        }

        *head = &(tx_queue.data[tx_queue.read]);
    }
    return true;
}

bool tx_queue_back(tx_queue_data_t **tail)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (tx_queue.read == tx_queue.write)
        {
            *tail = 0;
            return false;
        }

        *tail = &(tx_queue.data[((tx_queue.write - 1) & QUEUE_MASK)]);
    }
    return true;
}

bool tx_queue_pop()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (tx_queue.read == tx_queue.write)
        {
            return false;
        }

        tx_queue.read = (tx_queue.read + 1) & QUEUE_MASK;
    }
    return true;
}

void tx_queue_print_status(void)
{
    LV_("read: %u\n", tx_queue.read);
    LV_("write: %u\n", tx_queue.write);
    LV_("empty: %u\n", tx_queue_is_empty());
    LV_("full: %u\n", tx_queue_is_full());
}

void tx_queue_test()
{
    uint8_t buffer[16];
    tx_queue_data_t *data;

    char *stringptr = (char *)buffer;

    LS_("start\n");
    tx_queue_print_status();

    LV_("pop empty: %d\n", tx_queue_pop());

    LS_("push\n");
    strcpy_P(stringptr, PSTR("Element1"));
    tx_queue_push(buffer, strlen(stringptr));
    tx_queue_print_status();

    LS_("push\n");
    strcpy_P(stringptr, PSTR("Element2"));
    tx_queue_push(buffer, strlen(stringptr));
    tx_queue_print_status();

    LS_("push\n");
    strcpy_P(stringptr, PSTR("Element3"));
    tx_queue_push(buffer, strlen(stringptr));
    tx_queue_print_status();

    LS_("front\n");
    tx_queue_front(&data);
    LV_("front: [%s]\n", data->data);
    tx_queue_print_status();

    LS_("push\n");
    strcpy_P(stringptr, PSTR("Element4"));
    tx_queue_push(buffer, strlen(stringptr));
    tx_queue_print_status();

    LS_("front\n");
    tx_queue_front(&data);
    LV_("front: [%s]\n", data->data);
    tx_queue_print_status();

    LS_("pop until empty\n");
    while (!tx_queue_is_empty())
    {
        LS_("front\n");
        tx_queue_front(&data);
        LV_("front: [%s]\n", data->data);
        LS_("pop\n");
        tx_queue_pop();
        tx_queue_print_status();
    }

    LS_("push\n");
    strcpy_P(stringptr, PSTR("Element5"));
    tx_queue_push(buffer, strlen(stringptr));
    tx_queue_print_status();

    LS_("front\n");
    tx_queue_front(&data);
    LV_("front: [%s]\n", data->data);
    tx_queue_print_status();

    tx_queue_get_empty_tail(&data);
    strcpy_P((char*)data->data, PSTR("Element6"));
    data->data_length = strlen((char*)data->data);
    tx_queue_push_tail();

    LS_("front\n");
    tx_queue_front(&data);
    LV_("front: [%s]\n", data->data);
    tx_queue_print_status();

    LS_("pop until empty\n");
    while (!tx_queue_is_empty())
    {
        LS_("front\n");
        tx_queue_front(&data);
        LV_("front: [%s]\n", data->data);
        LS_("pop\n");
        tx_queue_pop();
        tx_queue_print_status();
    }

    LS_("push until full\n");
    while (!tx_queue_is_full())
    {
        tx_queue_get_empty_tail(&data);
        strcpy_P((char*)data->data, PSTR("Element"));
        data->data_length = strlen((char*)data->data);
        tx_queue_push_tail();
        tx_queue_print_status();
    }

    LS_("pop until empty\n");
    while (!tx_queue_is_empty())
    {
        LS_("front\n");
        tx_queue_front(&data);
        LV_("front: [%s]\n", data->data);
        LS_("pop\n");
        tx_queue_pop();
        tx_queue_print_status();
    }

    LS_("push_tail until full\n");
    while (!tx_queue_is_full())
    {
        tx_queue_get_empty_tail(&data);
        strcpy_P((char*)data->data, PSTR("Element"));
        data->data_length = strlen((char*)data->data);
        tx_queue_push_tail();
        tx_queue_print_status();
    }

    LS_("pop until empty\n");
    while (!tx_queue_is_empty())
    {
        LS_("front\n");
        tx_queue_front(&data);
        LV_("front: [%s]\n", data->data);
        LS_("pop\n");
        tx_queue_pop();
        tx_queue_print_status();
    }
}
