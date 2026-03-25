/**
 * @file os_config.h
 * @brief OS 模块配置文件
 *
 * 所有 OS 子系统的容量参数统一在此配置。
 * 修改后需要重新编译整个 OS 模块。
 */

#pragma once

#define OS_EVT_QUEUE_SIZE   32      /**< 事件队列容量（环形缓冲区大小） */
#define OS_EVT_HANDLER_MAX  32      /**< 事件 handler 注册表最大数量 */
#define OS_TIMER_MAX        16      /**< 软件定时器最大数量 */

/**
 * @brief 任务调度器开关
 *
 * 设为 1 启用协作式任务调度功能（task.c 参与编译）。
 * 设为 0 关闭任务调度，仅使用定时器 + 事件，减小代码量。
 */
#define OS_USE_TASK         1

#if OS_USE_TASK
#define OS_TASK_MAX         8       /**< 协作式任务最大数量 */
#endif
