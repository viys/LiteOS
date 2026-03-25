/**
 * @file timer.c
 * @brief 软件定时器子系统实现
 *
 * 内部使用一个静态数组（定时器池）管理所有定时器。
 * 每个定时器槽位包含：回调函数、间隔、到期时刻、类型、激活状态等。
 *
 * @par 到期判断算法
 * 使用无符号减法 + 半范围比较法：
 * @code
 *   elapsed = current_tick - deadline
 *   如果 elapsed < 0x80000000，说明 current_tick >= deadline（已到期）
 *   如果 elapsed >= 0x80000000，说明 current_tick < deadline（未到期）
 * @endcode
 * 这种方法天然支持 32 位溢出回绕，无需特殊处理。
 *
 * @par 时间单位
 * OS 不强制时间单位，传入 os_tick_update() 的时间戳是什么单位，
 * 所有 interval / deadline / period 就是什么单位。
 */

#include "timer.h"
#include "os_config.h"

/** @brief 定时器槽位结构（内部使用） */
typedef struct {
    os_timer_cb callback;       /**< 到期回调函数 */
    void*       param;          /**< 回调参数 */
    uint8_t     type;           /**< 定时器类型：OS_TIMER_ONCE / OS_TIMER_LOOP */
    uint32_t    interval;       /**< 定时间隔 */
    uint32_t    deadline;       /**< 下次到期的 tick 值 */
    bool        active;         /**< 是否正在运行 */
    bool        used;           /**< 该槽位是否已被创建 */
} timer_slot_t;

static timer_slot_t timer_pool[OS_TIMER_MAX];   /**< 定时器池 */
static uint32_t     sys_tick;                    /**< 系统 tick */

void os_timer_init(void)
{
    memset(timer_pool, 0, sizeof(timer_pool));
    sys_tick = 0;
}

uint32_t os_get_tick(void)
{
    return sys_tick;
}

void os_tick_update(uint32_t timestamp)
{
    static uint32_t pre_tick = 0;
    static bool     first    = true;

    /* 首次调用：记录基准时间戳，不产生 tick 增量 */
    if (first) {
        pre_tick = timestamp;
        first    = false;
        return;
    }

    /*
     * 计算差值（无符号减法天然处理溢出）
     * 例: 当 timestamp 从 0xFFFFFFFF 回绕到 0 时，
     *     (0 - 0xFFFFFFFF) 在无符号运算中等于 1，结果正确。
     */
    uint32_t delta = timestamp - pre_tick;
    pre_tick = timestamp;

    sys_tick += delta;
}

os_err_t os_timer_create(uint8_t id, uint8_t type, uint32_t interval,
                         os_timer_cb callback, void* param)
{
    if (id >= OS_TIMER_MAX) {
        return OS_ERR_INVALID;
    }

    timer_slot_t* t = &timer_pool[id];
    t->callback    = callback;
    t->param       = param;
    t->type        = type;
    t->interval = interval;
    t->active      = false;
    t->used        = true;

    return OS_OK;
}

os_err_t os_timer_start(uint8_t id)
{
    if (id >= OS_TIMER_MAX || !timer_pool[id].used) {
        return OS_ERR_INVALID;
    }

    /* 以当前 tick 为基准，计算首次到期时刻 */
    timer_pool[id].deadline = sys_tick + timer_pool[id].interval;
    timer_pool[id].active   = true;

    return OS_OK;
}

os_err_t os_timer_stop(uint8_t id)
{
    if (id >= OS_TIMER_MAX || !timer_pool[id].used) {
        return OS_ERR_INVALID;
    }

    timer_pool[id].active = false;
    return OS_OK;
}

os_err_t os_timer_delete(uint8_t id)
{
    if (id >= OS_TIMER_MAX) {
        return OS_ERR_INVALID;
    }

    memset(&timer_pool[id], 0, sizeof(timer_slot_t));
    return OS_OK;
}

void os_timer_tick(void)
{
    for (uint8_t i = 0; i < OS_TIMER_MAX; i++) {
        timer_slot_t* t = &timer_pool[i];

        if (!t->used || !t->active) {
            continue;
        }

        /*
         * 半范围到期判断：
         * elapsed = sys_tick - deadline （无符号减法）
         * 若 elapsed >= 0x80000000，说明 sys_tick 还没到 deadline，跳过
         */
        uint32_t elapsed = sys_tick - t->deadline;
        if (elapsed > 0x7FFFFFFF) {
            continue;
        }

        /* 到期：执行回调 */
        if (t->callback) {
            t->callback(t->param);
        }

        if (t->type == OS_TIMER_LOOP) {
            /* 循环定时器：基于上次 deadline 重载（避免累积误差） */
            t->deadline += t->interval;
        } else {
            /* 单次定时器：自动停止 */
            t->active = false;
        }
    }
}
