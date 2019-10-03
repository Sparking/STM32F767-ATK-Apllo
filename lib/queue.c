#include <stdlib.h>
#include <string.h>
#include "queue.h"

struct queue *queue_create_static(void *restrict buf,
        const unsigned int size, const unsigned int block_size)
{
    struct queue *q;

    if (buf == NULL)
        return NULL;

    q = (struct queue *)buf;
    memset(q, 0, sizeof(*q) + size * block_size);
    q->size = size;
    q->block_size = block_size;
    q->used_size = 0;
    q->front = 0;
    q->rear = 0;

    return q;
}

unsigned int queue_enque(struct queue *restrict q,
        const void *restrict data, const unsigned int size)
{
    unsigned int i;

    if (q == NULL || data == NULL)
        return 0;

    for (i = 0; i < size; ++i) {
        if (queue_full(q))
            break;

        memcpy(q->data + q->rear * q->block_size, data, q->block_size);
        data = (char *)data + q->block_size;
        q->rear = (q->rear + 1) % q->size;
        ++q->used_size;
    }

    return i;
}

unsigned int queue_deque(struct queue *restrict q,
        void *restrict data, const unsigned int size)
{
    unsigned int i;

    if (q == NULL)
        return 0;

    for (i = 0; i < size; ++i) {
        if (queue_empty(q))
            break;

        if (data != NULL) {
            memcpy(data, q->data + q->front * q->block_size, q->block_size);
            data = (char *)data + q->block_size;
        }

        q->front = (q->front + 1) % q->size;
        --q->used_size;
    }

    return i;
}

int queue_front(const struct queue *restrict q,
        void *restrict data, const unsigned int size)
{
    unsigned int i, j;

    if (q == NULL)
        return 0;

    if (queue_empty(q))
        return 0;

    for (i = 0, j = q->front; i < size; ++i) {
        if (queue_empty(q))
            break;

        memcpy(data, q->data +j * q->block_size, q->block_size);
        data = (char *)data + q->block_size;
        j = (j + 1) % q->size;
    }

    return 1;
}

int queue_rear(const struct queue *restrict q, void *data)
{
    unsigned int pos;

    if (q == NULL)
        return 0;

    if (queue_empty(q))
        return 0;

    pos = (q->rear == 0 ? q->size : q->rear) - 1;
    memcpy(data, q->data + pos * q->block_size, q->block_size);

    return 1;
}

void queue_clear(struct queue *restrict q)
{
    if (q == NULL)
        return;

    q->used_size = 0;
    q->front = 0;
    q->rear = 0;
}
