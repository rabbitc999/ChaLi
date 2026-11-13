#include "FreeRTOS.h"
#include "task.h"
#include "chali_mcu_hal.h"

extern int main(int, void *);
extern unsigned char data_start[], data_end[], data_load[], bss_start[], bss_end[];
static TaskHandle_t worker;
static size_t allocations, printed;

/** @brief 通过 QEMU semihosting 结束模拟并返回测试状态。 */
static _Noreturn void finish(unsigned status)
{
    unsigned args[] = {0x20026, status};
    register unsigned operation __asm("r0") = 0x20;
    register unsigned *parameter __asm("r1") = args;
    __asm volatile("bkpt 0xab" : "+r"(operation) : "r"(parameter) : "memory");
    __builtin_trap();
}

/** @brief 校验输出确实来自 FreeRTOS 工作任务。 */
size_t RTOS_HAL_Chali_Print(const void *data, size_t size)
{
    const char expected[] = "ChaLi RTOS";
    if (xTaskGetCurrentTaskHandle() != worker || size > sizeof(expected) - 1 - printed) finish(1);
    for (size_t i = 0; i < size; ++i)
        if (((const char *)data)[i] != expected[printed++]) finish(2);
    return size;
}

/** @brief 使用 FreeRTOS 堆分配，并满足 ChaLi 的对齐契约。 */
void *RTOS_HAL_Chali_Allocate(size_t size)
{
    if (xTaskGetCurrentTaskHandle() != worker || size > SIZE_MAX - 19) finish(3);
    void *raw = pvPortMalloc(size + 19);
    if (!raw) return NULL;
    void **aligned = (void **)(((uintptr_t)raw + 19u) & ~(uintptr_t)15u);
    aligned[-1] = raw;
    ++allocations;
    return aligned;
}

/** @brief 将 ChaLi 内存归还 FreeRTOS 堆。 */
void RTOS_HAL_Chali_Free(void *pointer)
{
    if (xTaskGetCurrentTaskHandle() != worker || !allocations) finish(4);
    --allocations;
    vPortFree(((void **)pointer)[-1]);
}

/** @brief 将 ChaLi 运行时错误报告给模拟器。 */
_Noreturn void HAL_Chali_Panic(const char *message)
{
    (void)message;
    finish(5);
}

/** @brief 通过 RTOS 延时恢复后执行 ChaLi 固件。 */
static void task(void *argument)
{
    (void)argument;
    vTaskDelay(2);
    if (main(0, NULL) || allocations || printed != 10) finish(6);
    finish(0);
}

/** @brief 初始化板级内存并启动 FreeRTOS 调度器。 */
_Noreturn void reset(void)
{
    for (unsigned char *p = data_start; p < data_end; ++p) *p = data_load[p - data_start];
    for (unsigned char *p = bss_start; p < bss_end; ++p) *p = 0;
    if (xTaskCreate(task, "chali", 512, NULL, 2, &worker) != pdPASS) finish(7);
    vTaskStartScheduler();
    finish(8);
}
