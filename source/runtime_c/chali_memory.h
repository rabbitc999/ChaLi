/**
 * @file chali_memory.h
 * @brief C 运行时共用的分配契约与错误码。
 */
#ifndef CHALI_C_MEMORY_H
#define CHALI_C_MEMORY_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/** @brief C 服务返回的操作状态。 */
typedef enum ChaliCStatus
{
    CHALI_C_OK = 0,
    CHALI_C_INVALID_ARGUMENT = 1,
    CHALI_C_OUT_OF_MEMORY = 2,
    CHALI_C_RANGE_ERROR = 3,
    CHALI_C_DOMAIN_ERROR = 4
} ChaliCStatus;
/** @brief 由调用方拥有的分配器，回调失败时返回空指针。 */
typedef struct ChaliCAllocator
{
    void *context;
    void *(*allocate)(void *context, size_t bytes);
    void (*release)(void *context, void *memory, size_t bytes);
} ChaliCAllocator;
/**
 * @brief 取得使用 malloc/free 的独立 C 分配器。
 * @return 不借用调用方状态的分配接口。
 */
ChaliCAllocator chali_c_default_allocator(void);
/**
 * @brief 检查回调集合是否完整。
 * @param allocator 分配接口。
 * @return 回调均可用时返回一。
 */
int chali_c_allocator_valid(const ChaliCAllocator *allocator);
#ifdef __cplusplus
}
#endif
#endif
