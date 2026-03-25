/**
 * @file timer.h
 * @brief 软件定时器子系统
 *
 * 提供基于系统 tick 的软件定时器功能，支持：
 * - 单次定时器（OS_TIMER_ONCE）：到期执行一次回调后自动停止
 * - 循环定时器（OS_TIMER_LOOP）：到期执行回调后自动重载，周期性触发
 *
 * 定时器 API 的时间单位由外部传入 os_tick_update() 的时间戳决定，
 * OS 内部不做单位转换。
 *
 * 定时器到期后直接调用注册的回调函数（在主循环上下文中执行）。
 * 定时器数量上限由 OS_TIMER_MAX 配置（见 os_config.h）。
 *
 * @par 典型用法
 * @code
 * os_timer_create(0, OS_TIMER_LOOP, 1000, my_callback, NULL);  // 1 秒循环定时器
 * os_timer_start(0);
 * // ... 主循环中 os_run() 会自动检查并触发回调 ...
 * os_timer_stop(0);
 * @endcode
 */

#pragma once

#include "os_type.h"

/** @brief 定时器回调函数类型 */
typedef void (*os_timer_cb)(void* param);

#define OS_TIMER_ONCE   0x01    /**< 单次：到期执行一次后自动停止 */
#define OS_TIMER_LOOP   0x02    /**< 循环：到期后自动重载，周期性触发 */

/**
 * @brief 初始化定时器子系统
 *
 * 清空定时器池和系统 tick 计数器。
 * 必须在使用其他定时器 API 之前调用。
 */
void os_timer_init(void);

/**
 * @brief 创建定时器
 *
 * 创建后定时器处于停止状态，需调用 os_timer_start() 启动。
 *
 * @param id          定时器 ID（0 ~ OS_TIMER_MAX-1，由调用者分配）
 * @param type        定时器类型（OS_TIMER_ONCE 或 OS_TIMER_LOOP）
 * @param interval    定时间隔（单位与 os_tick_update 传入的时间戳一致）
 * @param callback    到期回调函数
 * @param param       回调参数（可为 NULL）
 * @return OS_OK 成功，OS_ERR_INVALID ID 越界
 */
os_err_t os_timer_create(uint8_t id, uint8_t type, uint32_t interval,
                         os_timer_cb callback, void* param);

/**
 * @brief 启动定时器
 *
 * 以当前系统 tick 为基准，计算首次到期时刻。
 * 对已启动的定时器再次调用会重新计时。
 *
 * @param id 定时器 ID
 * @return OS_OK 成功，OS_ERR_INVALID ID 无效或未创建
 */
os_err_t os_timer_start(uint8_t id);

/**
 * @brief 停止定时器
 *
 * 停止后定时器不再触发，但不会删除，可以再次 start。
 *
 * @param id 定时器 ID
 * @return OS_OK 成功，OS_ERR_INVALID ID 无效或未创建
 */
os_err_t os_timer_stop(uint8_t id);

/**
 * @brief 删除定时器
 *
 * 删除后该 ID 可以被重新 create 使用。
 *
 * @param id 定时器 ID
 * @return OS_OK 成功，OS_ERR_INVALID ID 越界
 */
os_err_t os_timer_delete(uint8_t id);

/**
 * @brief 获取当前系统 tick
 * @return 当前系统 tick 值
 */
uint32_t os_get_tick(void);

/**
 * @brief 更新系统 tick
 *
 * 通过计算与上次的差值来累加系统 tick，支持 32 位溢出回绕。
 * 首次调用时仅记录基准值，不产生 tick 增量。
 * 由 os_run() 内部调用，应用层无需直接使用。
 *
 * @param timestamp 外部时间源的当前时间戳（单位由调用者决定）
 */
void os_tick_update(uint32_t timestamp);

/**
 * @brief 检查并触发到期的定时器
 *
 * 遍历定时器池，对所有到期的活跃定时器：
 * - 调用其回调函数
 * - ONCE 类型：自动停止
 * - LOOP 类型：重新计算下次到期时刻
 *
 * 由 os_run() 内部调用，应用层无需直接使用。
 */
void os_timer_tick(void);
