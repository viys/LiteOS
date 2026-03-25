/**
 * @file os.c
 * @brief OS 主模块实现
 *
 * 协调所有子系统的初始化和调度。
 * os_run() 是整个系统的心跳，由主循环反复调用驱动。
 */

#include "os.h"

void os_init(void)
{
    os_event_init();
    os_timer_init();
#if OS_USE_TASK
    os_task_init();
#endif
}

void os_run(uint32_t timestamp)
{
    os_event_t evt;

    /* 1. 更新系统 tick */
    os_tick_update(timestamp);

    /* 2. 检查定时器：到期的定时器会执行回调 */
    os_timer_tick();

#if OS_USE_TASK
    /* 3. 任务调度：到期任务按优先级执行 */
    os_task_run();
#endif

    /* 4. 派发事件：取出队列中所有事件并调用对应 handler */
    while (os_event_get(&evt)) {
        os_event_dispatch(&evt);
    }
}
