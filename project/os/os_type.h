/**
 * @file os_type.h
 * @brief OS 基础类型定义
 *
 * 定义 OS 模块中通用的基础类型，包括：
 * - os_handle : 事件/资源的句柄类型
 * - os_err_t  : 统一的错误码枚举
 */

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/** @brief 通用句柄类型，用于标识事件类型 */
typedef uint16_t os_handle;

/** @brief OS 统一错误码 */
typedef enum {
    OS_OK = 0,          /**< 操作成功 */
    OS_ERR_FULL,        /**< 资源已满（队列满、定时器池满等） */
    OS_ERR_INVALID,     /**< 参数无效（ID 越界、空指针等） */
    OS_ERR_NOT_FOUND,   /**< 资源未找到（未创建的定时器/任务等） */
} os_err_t;
