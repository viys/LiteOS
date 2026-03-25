#include "event.h"

#define EVT_QUEUE_SIZE  32
#define EVT_MAX 32

/* 处理函数注册表 */
static event_handler_t handler_table[EVT_MAX] = { NULL };

static os_event_t evt_queue[EVT_QUEUE_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

/* 中断中调用：投递事件 */
void os_event_post(os_handle handle, void* param)
{
    uint8_t next = (head + 1) % EVT_QUEUE_SIZE;
    if (next != tail) {          /* 队列没满 */
        evt_queue[head].handle  = handle;
        evt_queue[head].param = param;
        head = next;
    }
}

/* 主循环中调用：取出事件 */
bool os_event_get(os_event_t* evt)
{
    if (tail == head) {
        return false;            /* 队列为空 */
    }

    *evt = evt_queue[tail];
    tail = (tail + 1) % EVT_QUEUE_SIZE;
    return true;
}

void os_event_register(os_handle handle, event_handler_t handler)
{
    if (handle < EVT_MAX) {
        handler_table[handle] = handler;
    }
}

// 要改成标志位或者循环队列
void os_event_dispatch(os_event_t* evt)
{
    if (evt->handle < EVT_MAX && handler_table[evt->handle]) {
        handler_table[evt->handle](evt->param);
    }
}
