#include <stdlib.h>
#include <string.h>
#include "array_queue.h"

struct array_queue *array_queue_init(struct array_queue *restrict q, const unsigned int item_count, const unsigned int item_size)
{
    if (q) {
        q->remained_count = 0;
        q->front = q->rear = 0;
        q->item_count = item_count;
        q->item_size = item_size;
    }

    return q;
}

struct array_queue *create_array_queue(const unsigned int item_count, const unsigned int item_size)
{
    unsigned int size;
    struct array_queue *q;

    if (item_count == 0 || item_size == 0) /* 异常? */
        return NULL;

    size = item_count * item_size + sizeof(struct array_queue); /* 避免重复计算 */
    q = (struct array_queue *)malloc(size); /* 根据需要替换malloc函数 */
    if (q)
        array_queue_init(q, item_count, item_size);

    return q;
}

unsigned int array_queue_enque(struct array_queue *restrict q, const void *restrict data, const unsigned int count)
{
    unsigned int enque_n;
    unsigned int que_offset, data_offset;

    enque_n = 0;
    if (q) {
        data_offset = 0;
        while (!array_queue_full(q) && enque_n < count) {  /* 循环入队 */
            enque_n++;
            que_offset = q->rear * q->item_size;
            memcpy(((char *)q->data) + que_offset, ((char *)data) + data_offset, q->item_size);
            q->rear = (q->rear + 1) % q->item_count;
            data_offset = data_offset + q->item_size;
            q->remained_count++;
        }
    }

    return enque_n;
}

unsigned int array_queue_deque(struct array_queue *restrict q, void *restrict data, const unsigned int count)
{
    unsigned int deque_n;
    unsigned int que_offset, data_offset;

    deque_n = 0;
    if (q) {
        data_offset = 0;
        while (!array_queue_empty(q) && deque_n < count) {
            deque_n++;
            que_offset = q->front * q->item_size;
            memcpy(((char *)data) + data_offset, ((char *)q->data) + que_offset, q->item_size);
            q->front = (q->front + 1) % q->item_count;
            data_offset = data_offset + q->item_size;
            q->remained_count--;
        }
    }

    return deque_n;
}

