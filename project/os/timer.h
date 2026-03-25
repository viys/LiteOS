#pragma once

#include "os_type.h"

#define OS_TIMER_TYPE_SUSPEND   0x00
#define OS_TIMER_TYPE_ONCE      0x01
#define OS_TIMER_TYPE_LOOP      0x02

typedef uint8_t os_timer_type;

typedef struct {
    os_handle handle;
    os_timer_type type;
    uint32_t timestamp;
    void* param;
} os_timer_t;

void os_time_run(void);
