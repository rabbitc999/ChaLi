#include "chali_mcu_hal.h"

#if defined(CHALI_MCU_RTOS)
#define CHALI_HAL_PRINT RTOS_HAL_Chali_Print
#define CHALI_HAL_ALLOCATE RTOS_HAL_Chali_Allocate
#define CHALI_HAL_FREE RTOS_HAL_Chali_Free
#else
#define CHALI_HAL_PRINT HAL_Chali_Print
#define CHALI_HAL_ALLOCATE HAL_Chali_Allocate
#define CHALI_HAL_FREE HAL_Chali_Free
#endif

/**
 * @brief 向板级端口转发非空输出。
 * @param data 输出字节缓冲区。
 * @param size 字节数。
 * @return 板级端口报告的写入长度。
 */
size_t chali_mcu_print(const void *data, size_t size)
{
    return size ? CHALI_HAL_PRINT(data, size) : 0;
}

/**
 * @brief 向板级端口转发非零大小的内存申请。
 * @param size 申请的字节数。
 * @return 内存地址或空指针。
 */
void *chali_mcu_allocate(size_t size)
{
    return size ? CHALI_HAL_ALLOCATE(size) : NULL;
}

/**
 * @brief 将非空地址交给对应板级端口释放。
 * @param pointer 从对应板级端口申请的内存地址。
 */
void chali_mcu_free(void *pointer)
{
    if (pointer) CHALI_HAL_FREE(pointer);
}
