/**
 * @file main.c
 * @brief 应用入口 - OS 模块使用演示
 *
 * 演示三大功能的使用方式：
 * - 定时器：周期回调（500ms / 1s）
 * - 任务：周期调度（500ms LED 翻转 / 2s tick 打印）
 * - 事件：手动投递 + 注册回调派发
 */

#include <windows.h>
#include "os.h"

/* ========== 定时器 ID ========== */
#define TMR_1S      0
#define TMR_500MS   1

#if OS_USE_TASK
/* ========== 任务 ID ========== */
#define TASK_LED    0
#define TASK_PRINT  1
#endif

/* ========== 事件 handle ========== */
#define EVT_KEY     1   /**< 模拟按键事件 */

/* ---- 定时器回调 ---- */

static void on_timer_1s(void* param)
{
    printf("[TIMER] 1s callback\n");
}

static void on_timer_500ms(void* param)
{
    printf("[TIMER] 500ms callback\n");
}

#if OS_USE_TASK
/* ---- 任务回调 ---- */

static void task_led(void)
{
    static bool state = false;
    state = !state;
    printf("[TASK] LED %s\n", state ? "ON" : "OFF");
}

static void task_print(void)
{
    printf("[TASK] tick = %u ms\n", os_get_tick());
}
#endif

/* ---- 事件回调 ---- */

/**
 * @brief 按键事件处理函数
 *
 * 当 EVT_KEY 事件被派发时调用。
 * param 携带按键码（通过强转为整数使用）。
 */
static void on_key_event(void* param)
{
    int key = (int)(intptr_t)param;
    printf("[EVENT] key pressed: %d\n", key);
}

int main(void)
{
    /* -------- 初始化 OS -------- */
    os_init();

    /* -------- 定时器示例 -------- */
    os_timer_create(TMR_1S,    OS_TIMER_LOOP, 1000, on_timer_1s,    NULL);
    os_timer_create(TMR_500MS, OS_TIMER_LOOP, 500,  on_timer_500ms, NULL);
    os_timer_start(TMR_1S);
    os_timer_start(TMR_500MS);

#if OS_USE_TASK
    /* -------- 任务示例 -------- */
    os_task_create(TASK_LED,   task_led,   500,  0);
    os_task_create(TASK_PRINT, task_print, 2000, 1);
#endif

    /* -------- 事件示例 -------- */
    /*
     * 事件使用流程：
     *   1. os_event_register() 注册 handle 对应的回调
     *   2. os_event_post()     在任意位置投递事件（可在中断中调用）
     *   3. os_run() 主循环中自动取出并派发到回调
     *
     * 这里模拟投递一个按键事件，它会在第一次 os_run() 时被派发。
     */
    os_event_register(EVT_KEY, on_key_event);
    os_event_post(EVT_KEY, (void*)(intptr_t)42);   /* 模拟按键码 42 */

    /* -------- 主循环 -------- */
    while (1) {
        /*
         * GetTickCount() 返回 ms，直接传入 os_run。
         * OS 内部不做单位转换，传入什么单位就用什么单位。
         * 在单片机上可直接传入硬件定时器的计数值（us / ms / 其他）。
         */
        os_run(GetTickCount());
        Sleep(1);   /* 降低 CPU 占用 */
    }

    return 0;
}
