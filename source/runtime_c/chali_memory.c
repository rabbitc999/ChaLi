/**
 * @file chali_memory.c
 * @brief 默认 C 内存分配器。
 */
#include "chali_memory.h"
#include <stdlib.h>
/**
 * @brief 使用 C 运行库分配字节。
 * @param context 保留上下文。
 * @param bytes 请求字节数。
 * @return 分配结果，失败时为空。
 */
static void *allocate_memory(void *context, size_t bytes)
{
    (void)context;
    return malloc(bytes);
}
/**
 * @brief 释放默认分配器拥有的字节。
 * @param context 保留上下文。
 * @param memory 分配结果。
 * @param bytes 原请求大小。
 */
static void release_memory(void *context, void *memory, size_t bytes)
{
    (void)context;
    (void)bytes;
    free(memory);
}
/**
 * @brief 返回独立于 C++ 分配器的默认实现。
 * @return malloc/free 回调集合。
 */
ChaliCAllocator chali_c_default_allocator(void)
{
    ChaliCAllocator allocator = {NULL, allocate_memory, release_memory};
    return allocator;
}
/**
 * @brief 验证分配和释放均存在。
 * @param allocator 待验证接口。
 * @return 有效时返回一。
 */
int chali_c_allocator_valid(const ChaliCAllocator *allocator)
{
    return allocator && allocator->allocate && allocator->release;
}
