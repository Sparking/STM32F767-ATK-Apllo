#ifndef __ARRAY_QUEUE_H_
#define __ARRAY_QUEUE_H_

#include <stdbool.h>

struct array_queue {
    unsigned int item_count;    /* 队列可容纳的数据项数 */
    unsigned int item_size;     /* 队列中的每个数据项的大小 */
    unsigned int remained_count;/* 队列中剩余的有效数据项数 */
    unsigned int front;         /* 指向第一个有效元素 */
    unsigned int rear;          /* 指向最后一个有效元素的下一项 */
    char data[];
};
typedef struct array_queue array_queue_t;

/*
 * @brief: 判断队列是否为空
 */
static inline bool array_queue_empty(struct array_queue *q)
{
    return q->remained_count == 0;
}

/*
 * @brief: 判断队列是否满
 */
static inline bool array_queue_full(struct array_queue * q)
{
    return q->remained_count == q->item_count;
}

/*
 * @brief: 查询队列中剩余的有效数据项数
 */
static inline unsigned int array_queue_count(struct array_queue *q)
{
    return q->remained_count;
}

/*
 * @brief: array_queue_init
 *  初始化队列, 队列可以由动态内存分配, 或者由一个合适的数组做一个队列
 * @param:
 *  q: 未初始化的队列
 *  item_count: 队列的数据项数
 *  item_size: 队列数据项的大小
 * @returns:
 *  返回q的值
 */
extern struct array_queue *array_queue_init(struct array_queue *restrict q,
    const unsigned int item_count, const unsigned int item_size);

/*
 * @brief: create_array_queue
 *  申请一片内存创建队列, 并初始化该队列
 * @param:
 *  item_count: 队列中数据项的个数
 *  item_size: 每个数据项的大小
 * @returns:
 *  成功返回创建的队列的地址, 失败返回NULL
 */
extern struct array_queue *create_array_queue(const unsigned int item_count,
    const unsigned int item_size);
/*
 * @brief: array_queue_enque
 *  数据入队
 * @param:
 *  q: 队列
 *  data: 连续数据项的首地址
 *  count: 数据项数
 * @returns:
 *  返回入队的数据项数
 */
extern unsigned int array_queue_enque(struct array_queue *restrict q,
    const void *restrict data, const unsigned int count);
/*
 * @breif: array_queue_deque
 *  数据出队
 * @param:
 *  q: 队列
 *  data: 出对数据的存放地址
 *  count: 出队数据项数
 * @returns:
 *  返回出队的数据项数
 */
extern unsigned int array_queue_deque(struct array_queue *restrict q,
    void *restrict data, const unsigned int count);

#endif /* __ARRAY_QUEUE_H_ */

