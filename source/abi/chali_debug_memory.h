/**
 * @file chali_debug_memory.h
 * @brief 原生调试器只读内存目录的固定宽度 C 布局，不提供执行或写入入口。
 */
#ifndef CHALI_DEBUG_MEMORY_H
#define CHALI_DEBUG_MEMORY_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/** @brief 一个活跃逻辑区间及其当前物理首地址，仅包含公开可访问范围。 */
typedef struct ChaliDebugAllocation
{
    uint64_t address, size, physical;
} ChaliDebugAllocation;
/** @brief 单线程的暂停点快照；state 为零表示线程结束，奇数表示正在发布。 */
typedef struct ChaliDebugMemoryThread
{
    uint64_t thread;
    volatile uint64_t state;
    uint64_t revision, revision_address;
    uint64_t data, data_size, allocations, count, complete, next;
    uint64_t context_address, context;
} ChaliDebugMemoryThread;
/** @brief 版本一使用 64 位小端字段和进程寿命内稳定的线程目录节点。 */
typedef struct ChaliDebugMemoryIndex
{
    uint64_t version, pointer_bytes;
    volatile uint64_t first;
} ChaliDebugMemoryIndex;
/** @brief 调试程序的只读观察入口；目录节点由正常执行路径发布。 */
extern ChaliDebugMemoryIndex chali_debug_memory;
/**
 * @brief 在源码停点之前发布当前线程的内存映射，失败仅使调试查看不可用。
 * @return 无返回值，不调用用户方法，不创建语言对象。
 */
void chali_debug_publish_memory(void);
#ifdef __cplusplus
}
#endif
#endif
