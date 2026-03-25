/**
 * @file event.h
 * @brief 事件子系统
 *
 * 提供轻量级的事件队列机制，支持：
 * - 中断/任务中投递事件 (os_event_post)
 * - 主循环中取出事件 (os_event_get)
 * - 注册事件处理回调 (os_event_register)
 * - 派发事件到对应回调 (os_event_dispatch)
 *
 * 事件队列采用环形缓冲区实现，支持单生产者（中断）单消费者（主循环）模式。
 * 队列容量由 OS_EVT_QUEUE_SIZE 配置（见 os_config.h）。
 */

#pragma once

#include "os_type.h"

/** @brief 事件结构体 */
typedef struct {
    os_handle handle;   /**< 事件类型句柄（用于区分不同事件） */
    void*     param;    /**< 事件附带参数（由投递者传入，派发时传给回调） */
} os_event_t;

/** @brief 事件处理回调函数类型 */
typedef void (*event_handler_t)(void* param);

/**
 * @brief 初始化事件子系统
 *
 * 清空事件队列和 handler 注册表。
 * 必须在使用其他事件 API 之前调用。
 */
void os_event_init(void);

/**
 * @brief 投递事件到队列
 *
 * 可在中断上下文中安全调用（无锁设计，依赖 volatile 保证可见性）。
 * 队列满时不会阻塞，直接返回 OS_ERR_FULL。
 *
 * @param handle 事件类型句柄
 * @param param  附带参数（可为 NULL）
 * @return OS_OK 成功，OS_ERR_FULL 队列已满
 */
os_err_t os_event_post(os_handle handle, void* param);

/**
 * @brief 从队列中取出一个事件
 *
 * 仅在主循环上下文中调用。
 *
 * @param[out] evt 取出的事件存入此结构体
 * @return true 取到事件，false 队列为空
 */
bool os_event_get(os_event_t* evt);

/**
 * @brief 注册事件处理回调
 *
 * 同一个 handle 重复注册会覆盖旧的 handler。
 *
 * @param handle  事件类型句柄（必须 < OS_EVT_HANDLER_MAX）
 * @param handler 回调函数指针
 */
void os_event_register(os_handle handle, event_handler_t handler);

/**
 * @brief 派发事件到已注册的回调
 *
 * 根据 evt->handle 查找注册表，调用对应 handler 并传入 evt->param。
 * 如果该 handle 没有注册 handler，则静默忽略。
 *
 * @param evt 要派发的事件
 */
void os_event_dispatch(os_event_t* evt);
