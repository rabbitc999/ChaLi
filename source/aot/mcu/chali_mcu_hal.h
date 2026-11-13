#ifndef CHALI_MCU_HAL_H
#define CHALI_MCU_HAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 将字节写到板级输出，可由 C 或汇编实现。
 * @param data 借用到返回为止的字节缓冲区，允许包含零。
 * @param size 字节数。
 * @return 成功写出的字节数，不得超过 size。
 */
size_t HAL_Chali_Print(const void *data, size_t size);

/**
 * @brief 在板级堆申请至少 16 字节对齐的内存。
 * @param size 非零字节数。
 * @return 内存起始地址；失败返回空指针。
 */
void *HAL_Chali_Allocate(size_t size);

/**
 * @brief 释放来自同一板级堆的内存。
 * @param pointer 先前成功申请且尚未释放的非空地址。
 */
void HAL_Chali_Free(void *pointer);

/**
 * @brief 从 RTOS 任务写出字节；端口负责同步，不从中断调用。
 * @param data 借用到返回为止的字节缓冲区。
 * @param size 字节数。
 * @return 成功写出的字节数，不得超过 size。
 */
size_t RTOS_HAL_Chali_Print(const void *data, size_t size);

/**
 * @brief 从 RTOS 任务申请至少 16 字节对齐的内存；端口负责同步。
 * @param size 非零字节数。
 * @return 内存起始地址；失败返回空指针。
 */
void *RTOS_HAL_Chali_Allocate(size_t size);

/**
 * @brief 从 RTOS 任务释放来自同一端口的内存。
 * @param pointer 先前成功申请且尚未释放的非空地址。
 */
void RTOS_HAL_Chali_Free(void *pointer);

/**
 * @brief 通过选定端口写出字节。
 * @param data 借用到返回为止的字节缓冲区。
 * @param size 字节数；零字节时不调用端口。
 * @return 成功写出的字节数。
 */
size_t chali_mcu_print(const void *data, size_t size);

/**
 * @brief 通过选定端口申请内存。
 * @param size 字节数；零字节时不调用端口。
 * @return 内存起始地址；失败或零字节返回空指针。
 */
void *chali_mcu_allocate(size_t size);

/**
 * @brief 通过选定端口释放内存。
 * @param pointer 选定端口申请的地址；空指针时不调用端口。
 */
void chali_mcu_free(void *pointer);

#ifdef __cplusplus
}
#endif
#endif
