/**
 * @file task.h
 * @brief 协作式任务调度器
 *
 * 提供基于时间片的协作式任务调度，支持：
 * - 周期性任务：每隔固定时间执行一次
 * - 优先级调度：数值越小优先级越高，到期任务按优先级顺序执行
 * - 挂起/恢复：动态控制任务的运行状态
 *
 * 时间单位：毫秒（ms）
 *
 * @note 这是协作式调度，不是抢占式。每个任务函数必须尽快返回，
 *       不能在任务中长时间阻塞，否则会影响其他任务和事件的处理。
 *
 * @par 典型用法
 * @code
 * os_task_create(0, led_toggle, 500, 0);   // 每 500ms 执行，优先级 0（最高）
 * os_task_create(1, log_print, 2000, 1);    // 每 2s 执行，优先级 1
 * // ... 主循环中 os_run() 会自动调度到期任务 ...
 * @endcode
 */

#pragma once

#include "os_type.h"

/** @brief 任务函数类型（无参数、无返回值，必须尽快返回） */
typedef void (*os_task_fn)(void);

/**
 * @brief 初始化任务调度器
 *
 * 清空任务池。必须在使用其他任务 API 之前调用。
 */
void os_task_init(void);

/**
 * @brief 创建周期任务
 *
 * 创建后任务自动处于激活状态，立即参与调度。
 *
 * @param id        任务 ID（0 ~ OS_TASK_MAX-1，由调用者分配）
 * @param handler   任务函数
 * @param period_ms 执行周期（毫秒）
 * @param priority  优先级（0 = 最高，数值越大优先级越低）
 * @return OS_OK 成功，OS_ERR_INVALID ID 越界
 */
os_err_t os_task_create(uint8_t id, os_task_fn handler,
                        uint32_t period_ms, uint8_t priority);

/**
 * @brief 挂起任务
 *
 * 挂起后任务不再被调度，但不会删除。
 *
 * @param id 任务 ID
 * @return OS_OK 成功，OS_ERR_INVALID ID 无效
 */
os_err_t os_task_suspend(uint8_t id);

/**
 * @brief 恢复任务
 *
 * 恢复后以当前 tick 为基准重新计算下次执行时刻。
 *
 * @param id 任务 ID
 * @return OS_OK 成功，OS_ERR_INVALID ID 无效
 */
os_err_t os_task_resume(uint8_t id);

/**
 * @brief 执行到期任务
 *
 * 遍历任务池，找到所有到期且激活的任务，按优先级从高到低执行。
 * 每个任务执行后自动重载下次执行时刻。
 *
 * 由 os_run() 内部调用，应用层无需直接使用。
 */
void os_task_run(void);
