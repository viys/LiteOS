/**
 * @file task.c
 * @brief 协作式任务调度器实现
 *
 * 使用静态数组（任务池）管理所有任务。
 *
 * @par 调度算法
 * 每次 os_task_run() 调用时，循环查找到期任务中优先级最高的执行，
 * 直到没有到期任务为止。同一次 run 中可能执行多个不同任务。
 *
 * 到期判断采用与 timer 相同的半范围比较法（支持 32 位溢出回绕）。
 */

#include "os_config.h"

#if OS_USE_TASK

#include "task.h"
#include "timer.h"      /* 需要 os_get_tick() */

/** @brief 任务槽位结构（内部使用） */
typedef struct {
    os_task_fn    handler;      /**< 任务函数 */
    uint32_t      period_ms;    /**< 执行周期（ms） */
    uint32_t      deadline;     /**< 下次执行的 tick 值 */
    uint8_t       priority;     /**< 优先级（0 = 最高） */
    bool          active;       /**< 是否激活 */
    bool          used;         /**< 该槽位是否已被创建 */
} task_slot_t;

static task_slot_t task_pool[OS_TASK_MAX];  /**< 任务池 */

void os_task_init(void)
{
    memset(task_pool, 0, sizeof(task_pool));
}

os_err_t os_task_create(uint8_t id, os_task_fn handler,
                        uint32_t period_ms, uint8_t priority)
{
    if (id >= OS_TASK_MAX) {
        return OS_ERR_INVALID;
    }

    task_slot_t* t = &task_pool[id];
    t->handler    = handler;
    t->period_ms  = period_ms;
    t->deadline   = os_get_tick() + period_ms;
    t->priority   = priority;
    t->active     = true;
    t->used       = true;

    return OS_OK;
}

os_err_t os_task_suspend(uint8_t id)
{
    if (id >= OS_TASK_MAX || !task_pool[id].used) {
        return OS_ERR_INVALID;
    }
    task_pool[id].active = false;
    return OS_OK;
}

os_err_t os_task_resume(uint8_t id)
{
    if (id >= OS_TASK_MAX || !task_pool[id].used) {
        return OS_ERR_INVALID;
    }

    /* 以当前 tick 为基准重新开始计时，避免恢复后立即触发 */
    task_pool[id].deadline = os_get_tick() + task_pool[id].period_ms;
    task_pool[id].active   = true;

    return OS_OK;
}

void os_task_run(void)
{
    uint32_t tick = os_get_tick();

    /*
     * 调度策略：每轮从所有到期任务中选出优先级最高的执行。
     * 执行完一个任务后重新扫描，直到没有到期任务。
     * 这样保证高优先级任务总是先于低优先级任务执行。
     */
    for (;;) {
        task_slot_t* best = NULL;

        for (uint8_t i = 0; i < OS_TASK_MAX; i++) {
            task_slot_t* t = &task_pool[i];

            if (!t->used || !t->active) {
                continue;
            }

            /* 半范围到期判断（同 timer） */
            uint32_t elapsed = tick - t->deadline;
            if (elapsed > 0x7FFFFFFF) {
                continue;   /* 未到期 */
            }

            /* 选择优先级最高（数值最小）的到期任务 */
            if (!best || t->priority < best->priority) {
                best = t;
            }
        }

        if (!best) {
            break;          /* 没有到期任务了 */
        }

        best->handler();

        /* 重载下次执行时刻（基于 deadline 而非当前 tick，避免累积误差） */
        best->deadline += best->period_ms;
    }
}

#endif /* OS_USE_TASK */
