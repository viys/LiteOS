/**
 * @file event.c
 * @brief 事件子系统实现
 *
 * 使用环形缓冲区（circular buffer）实现 FIFO 事件队列。
 *
 * 结构示意：
 * @code
 *   [  ][  ][tail][  ][  ][head][  ][  ]
 *               ^-- 消费端      ^-- 生产端
 * @endcode
 *
 * - head 指向下一个写入位置
 * - tail 指向下一个读取位置
 * - head == tail 表示队列为空
 * - (head + 1) % SIZE == tail 表示队列已满（牺牲一个槽位判满）
 */

#include "event.h"
#include "os_config.h"

/** @brief handler 注册表：handle 作为索引直接查找对应的回调函数 */
static event_handler_t handler_table[OS_EVT_HANDLER_MAX];

/** @brief 环形事件队列 */
static os_event_t      evt_queue[OS_EVT_QUEUE_SIZE];
static volatile uint8_t head;   /**< 写入索引（volatile: 可能在中断中修改） */
static volatile uint8_t tail;   /**< 读取索引 */

void os_event_init(void)
{
    memset(handler_table, 0, sizeof(handler_table));
    head = 0;
    tail = 0;
}

os_err_t os_event_post(os_handle handle, void* param)
{
    uint8_t next = (head + 1) % OS_EVT_QUEUE_SIZE;

    if (next == tail) {
        return OS_ERR_FULL;     /* 队列已满，丢弃事件 */
    }

    evt_queue[head].handle = handle;
    evt_queue[head].param  = param;
    head = next;                /* 更新 head 必须在数据写入之后 */

    return OS_OK;
}

bool os_event_get(os_event_t* evt)
{
    if (tail == head) {
        return false;           /* 队列为空 */
    }

    *evt = evt_queue[tail];
    tail = (tail + 1) % OS_EVT_QUEUE_SIZE;
    return true;
}

void os_event_register(os_handle handle, event_handler_t handler)
{
    if (handle < OS_EVT_HANDLER_MAX) {
        handler_table[handle] = handler;
    }
}

void os_event_dispatch(os_event_t* evt)
{
    if (evt->handle < OS_EVT_HANDLER_MAX && handler_table[evt->handle]) {
        handler_table[evt->handle](evt->param);
    }
}
