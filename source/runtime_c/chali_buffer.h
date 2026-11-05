/**
 * @file chali_buffer.h
 * @brief 可按需链接的 C 连续存储，用于分块整数与平凡元素数组。
 */
#ifndef CHALI_C_BUFFER_H
#define CHALI_C_BUFFER_H
#include "chali_memory.h"
#ifdef __cplusplus
extern "C" {
#endif
/** @brief 独占连续存储，元素按原始字节复制。 */
typedef struct ChaliCBuffer
{
    void *data;
    size_t length;
    size_t capacity;
    size_t element_size;
    ChaliCAllocator allocator;
} ChaliCBuffer;
/**
 * @brief 初始化空缓冲区。
 * @param buffer 未持有存储的目标。
 * @param element_size 非零元素宽度。
 * @param allocator 可选分配器，空时使用 C 默认实现。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_init(ChaliCBuffer *buffer, size_t element_size, const ChaliCAllocator *allocator);
/**
 * @brief 释放所有容量并保留元素宽度与分配接口。
 * @param buffer 已初始化缓冲区。
 */
void chali_c_buffer_clear(ChaliCBuffer *buffer);
/**
 * @brief 保证最小容量，失败时保留原有状态。
 * @param buffer 已初始化缓冲区。
 * @param capacity 需要的元素容量。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_reserve(ChaliCBuffer *buffer, size_t capacity);
/**
 * @brief 改变长度，新增区域填零。
 * @param buffer 已初始化缓冲区。
 * @param length 需要的元素个数。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_resize(ChaliCBuffer *buffer, size_t length);
/**
 * @brief 复制源中的有效元素，失败时不改变目标。
 * @param target 已初始化且元素宽度相同的目标。
 * @param source 源缓冲区。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_copy(ChaliCBuffer *target, const ChaliCBuffer *source);
/**
 * @brief 追加元素，支持借用自身有效区域。
 * @param buffer 已初始化缓冲区。
 * @param values 连续源元素。
 * @param count 源元素个数。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_append(ChaliCBuffer *buffer, const void *values, size_t count);
/**
 * @brief 在指定位置插入元素，支持源与当前存储重叠。
 * @param buffer 已初始化缓冲区。
 * @param index 插入位置，允许等于长度。
 * @param values 连续源元素。
 * @param count 元素个数。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_insert(ChaliCBuffer *buffer, size_t index, const void *values, size_t count);
/**
 * @brief 删除指定范围，保留其余元素顺序。
 * @param buffer 已初始化缓冲区。
 * @param index 起始下标。
 * @param count 删除元素个数。
 * @return 操作状态。
 */
ChaliCStatus chali_c_buffer_erase(ChaliCBuffer *buffer, size_t index, size_t count);
#ifdef __cplusplus
}
#endif
#endif
