/**
 * @file event.c
 * @brief 事件子系统实现
 *
 * 使用环形缓冲区（circular buffer）实现 FIFO 事件队列。
 *
 * @par 环形缓冲区结构
 * @code
 *   [e0][e1][e2][e3][e4][e5][e6][e7]
 *            ^tail            ^head
 *   队列为空: head == tail
 *   队列已满: (head + 1) % SIZE == tail （牺牲一个槽位以区分满/空）
 * @endcode
 *
 * @par head / tail 指针说明
 * - head: 指向下一个写入位置（生产端更新）
 * - tail: 指向下一个读取位置（消费端更新）
 * - head 和 tail 都使用 volatile 修饰，支持中断安全的单生产者/单消费者模式
 *
 * @par 线程安全模型
 * - 中断投递（生产）/ 主循环消费的模式下无需锁
 * - head 更新必须在数据写入之后（保证内存可见性）
 * - 如果需要多个中断源投递，需要外层加锁
 */

#include "event.h"
#include "os_config.h"

/** @brief handler 注册表：handle 作为索引直接查找对应的回调函数
 *
 * 支持快速 O(1) 查找，但限制 handle 值不能超过 OS_EVT_HANDLER_MAX。
 * NULL 表示该 handle 尚未注册处理函数。
 */
static event_handler_t handler_table[OS_EVT_HANDLER_MAX];

/** @brief 环形事件队列
 *
 * 存储待处理的事件，由 os_event_post() 写入，os_event_get() 读出。
 */
static os_event_t      evt_queue[OS_EVT_QUEUE_SIZE];

/** @brief 写入索引（生产端）
 *
 * volatile: 可能在中断中修改，编译器不能优化掉相关访问
 * 范围: 0 ~ OS_EVT_QUEUE_SIZE-1，循环使用
 */
static volatile uint8_t head;

/** @brief 读取索引（消费端）
 *
 * volatile: 在中断和主循环间共享
 * 范围: 0 ~ OS_EVT_QUEUE_SIZE-1，循环使用
 */
static volatile uint8_t tail;

void os_event_init(void)
{
    /* 清空 handler 注册表 */
    memset(handler_table, 0, sizeof(handler_table));

    /* 初始化队列指针（空队列状态） */
    head = 0;
    tail = 0;
}

os_err_t os_event_post(os_handle handle, void* param)
{
    /* 计算下一个 head 位置 */
    uint8_t next = (head + 1) % OS_EVT_QUEUE_SIZE;

    /* 判满逻辑：next == tail 说明队列已满
     *
     * 环形队列通过牺牲一个槽位来区分满和空：
     * - 空: head == tail
     * - 满: (head + 1) % SIZE == tail
     *
     * 为什么需要牺牲一个槽位？
     * 如果完全填满 SIZE 个位置，那么 head 会与 tail 相等，无法区分满和空。
     * 所以当 next == tail 时，队列已满，不能再写入。
     */
    if (next == tail) {
        return OS_ERR_FULL;     /* 队列已满，丢弃事件 */
    }

    /* 写入事件数据 */
    evt_queue[head].handle = handle;
    evt_queue[head].param  = param;

    /* 原子更新 head 指针
     *
     * 重要：必须在数据写入之后更新 head，确保消费端读到的数据已完全写入。
     * 这样在单生产者/单消费者的无锁模式下：
     * - 主循环中的 tail 增长与 head 修改互不干扰
     * - 中断中的 head 增长对主循环可见（volatile 保证）
     */
    head = next;

    return OS_OK;
}

bool os_event_get(os_event_t* evt)
{
    /* 判空逻辑：tail == head 说明队列为空 */
    if (tail == head) {
        return false;           /* 队列为空，无事件取出 */
    }

    /* 读取当前 tail 指向的事件 */
    *evt = evt_queue[tail];

    /* 向前移动 tail 指针，指向下一个待读取的事件 */
    tail = (tail + 1) % OS_EVT_QUEUE_SIZE;

    return true;
}

void os_event_register(os_handle handle, event_handler_t handler)
{
    /* 边界检查：handle 必须在有效范围内 */
    if (handle < OS_EVT_HANDLER_MAX) {
        /* 直接设置注册表，同一 handle 重复注册会覆盖旧的 handler */
        handler_table[handle] = handler;
    }
    /* 超出范围的 handle 会被静默忽略，避免数组越界 */
}

void os_event_dispatch(os_event_t* evt)
{
    /* 安全性检查
     *
     * 两层防护：
     * 1. handle 范围检查：防止数组越界
     * 2. handler 有效性检查：防止调用 NULL 指针
     */
    if (evt->handle < OS_EVT_HANDLER_MAX && handler_table[evt->handle]) {
        /* 查表得到 handler，传入事件参数并执行 */
        handler_table[evt->handle](evt->param);
    }
    /* 如果 handle 越界或未注册，静默忽略该事件 */
}
