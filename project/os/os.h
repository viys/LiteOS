/**
 * @file os.h
 * @brief OS 主模块接口
 *
 * 这是 OS 模块的统一入口头文件，包含了所有子系统：
 * - event : 事件队列（中断投递、主循环派发）
 * - timer : 软件定时器（到期直接回调）
 * - task  : 协作式任务调度（周期执行）
 *
 * 应用层只需 #include "os.h" 即可使用全部 OS 功能。
 *
 * @par 主循环调用流程
 * @code
 * os_init();
 * while (1) {
 *     os_run(get_timestamp_us());   // 传入当前时间戳（us）
 * }
 * @endcode
 *
 * @par os_run 内部执行顺序
 * 1. os_tick_update  → 接收 us 时间戳，累加转换为 ms 系统 tick
 * 2. os_timer_tick   → 检查定时器到期 → 执行回调
 * 3. os_task_run     → 调度到期任务 → 按优先级执行
 * 4. event dispatch  → 取出并派发所有待处理事件
 */

#pragma once

#include "os_type.h"
#include "os_config.h"
#include "event.h"
#include "timer.h"

#if OS_USE_TASK
#include "task.h"
#endif

/**
 * @brief 初始化 OS 所有子系统
 *
 * 必须在主循环之前调用一次，初始化事件队列、定时器池、任务池。
 */
void os_init(void);

/**
 * @brief OS 主循环调度函数
 *
 * 每次主循环迭代调用一次，驱动整个 OS 运转。
 * 内部依次执行：tick 更新 → 定时器检查 → 任务调度 → 事件派发。
 *
 * @param timestamp_us 外部时间源的当前时间戳（单位：微秒 us）
 */
void os_run(uint32_t timestamp_us);
