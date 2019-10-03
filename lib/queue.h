#pragma once

struct queue {
    unsigned int size;          /* 数据块的个数 */
    unsigned int block_size;    /* 数据块的大小 */
    unsigned int used_size;     /* 已被使用的数据块个数 */
    unsigned int front;         /* 头指针 */
    unsigned int rear;          /* 尾指针 */

    char data[0];                   /* 数据段, 指向一片内存 */
};

/**
 * @brief queue_create 创建一个指定大小的循环队列
 * @param buf 队列使用的内存
 * @param size 数据块的个数
 * @param block_size 数据块的大小
 * @return 返回循环队列指针
 */
extern struct queue *queue_create_static(void *restrict buf,
        const unsigned int size, const unsigned int block_size);

/**
 * @brief queue_full 判断队列是否已满
 */
static inline int queue_full(const struct queue *restrict q)
{
    if (q == NULL)
        return 0;

    return q->size == q->used_size;
}

/**
 * @brief queue_empty 判断队列是否为空
 */
static inline int queue_empty(const struct queue *restrict q)
{
    if (q == NULL)
        return 0;

    return q->used_size == 0;
}

/**
 * @brief queue_used_size 检查队列已使用的大小
 */
static inline unsigned int queue_used_size(const struct queue *restrict q)
{
    if (q == NULL)
        return 0;

    return q->used_size;
}

/**
 * @brief queue_avaiable_size 检查队列可用的剩余空间大小
 */
static inline unsigned int queue_avaiable_size(const struct queue *restrict q)
{
    if (q == NULL)
        return 0;

    return q->size - q->used_size;
}

/**
 * @brief queue_enque 将数据入队
 * @param q 循环队列
 * @param data 连续的数据首地址
 * @param size 入队数据个数
 * @return 返回入队的数据个数
 */
extern unsigned int queue_enque(struct queue *q, const void *data, const unsigned int size);

/**
 * @brief queue_enque 将数据出队
 * @param q 循环队列
 * @param data 接受数据的地址
 * @param size 出队的数据个数
 * @return 返回出队的数据个数
 */
extern unsigned int queue_deque(struct queue *q, void *data, const unsigned int size);

/**
 * @brief queue_front 只读取队列头的数据, 不出队该数据
 * @param q 循环队列
 * @param data 接受数据的地址
 * @param size 数据个数
 * @return 失败返回0
 */
extern int queue_front(const struct queue *q, void *data, const unsigned int size);

/**
 * @brief queue_rear 只读取队列尾的数据, 不出对该数据
 * @return 失败返回0
 */
extern int queue_rear(const struct  queue *q, void *data);

/**
 * @brief queue_clear 清空整个队列
 */
extern void queue_clear(struct queue *q);
