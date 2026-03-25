#include "timer.h"

uint32_t u32_tick = 0;

uint32_t os_get_system_tick(void)
{
    os_time_run();
    return u32_tick;
}

uint8_t os_time_exceed(uint64_t ref, uint32_t span_us)
{
    uint32_t tick_temp = os_get_system_tick();
    uint8_t ret = 0;

    if (tick_temp >= ref) {
        ret = ((tick_temp - ref) > span_us);
    } else {
        ret = ((0xFFFFFFFF - ref + tick_temp) > span_us);
    }

    return ret;
}

void os_time_run(void)
{
    static uint32_t pre_tick = 0;

    uint32_t cur_tick = 0;
    // uint32_t cur_tick = REG_RD(0x42000104);// (SYS_CTRL->AON_RTC);

    if (cur_tick >= pre_tick) {
        u32_tick += (cur_tick - pre_tick);
        pre_tick = cur_tick;
    } else {
        u32_tick += (0xFFFFFFFF - pre_tick + cur_tick);
        pre_tick = cur_tick;
    }
}

void os_timer_start(os_handle handle, os_timer_type type, uint32_t ms)
{
}

void os_timer_stop(os_handle handle)
{
}

void os_timer_dispatch(os_timer_t timer)
{
}
