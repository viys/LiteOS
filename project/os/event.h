#pragma once

#include "os_type.h"

typedef struct {
    os_handle handle;    /* 事件类型 */
    void* param;   /* 附带参数 */
} os_event_t;

enum {
    EVT_NONE = 0,
    EVT_KEY_PRESS,
    EVT_KEY_RELEASE,
    EVT_UART_RX,
    EVT_TIMER_TICK,
    EVT_ADC_DONE,
};

typedef void (*event_handler_t)(void* param);

void os_event_post(os_handle handle, void* param);

bool os_event_get(os_event_t* evt);

void os_event_register(os_handle handle, event_handler_t handler);

void os_event_dispatch(os_event_t* evt);
