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
/** @brief 对象句柄快照；L／M／E 为容器，O 为类型号及装箱值，F 为函数标签及有序捕获，first 是值表索引。 */
typedef struct ChaliDebugObject
{
    uint64_t handle, kind, count, first;
} ChaliDebugObject;
/**
 * @brief 无所有权单值快照，flags 的低四位分别表示聚合、文本、浮点、闭包稳定存储地址。
 * @note first/count 为子字段索引范围，text/length 为 UTF-8 字节表范围，不是机器指针。
 */
typedef struct ChaliDebugValue
{
    uint64_t flags, bits, first, count, text, length;
} ChaliDebugValue;
/** @brief 单线程的暂停点快照；state 为零表示线程结束，奇数表示正在发布。 */
typedef struct ChaliDebugMemoryThread
{
    uint64_t thread;
    volatile uint64_t state;
    uint64_t revision, revision_address;
    uint64_t data, data_size, allocations, count, complete, next;
    uint64_t context_address, context;
    /** @brief 版本二追加的只读容器快照及对象表失效代次，失败时表数量为零。 */
    uint64_t objects, object_count, values, value_count, text, text_size;
    uint64_t object_revision_address, object_revision;
} ChaliDebugMemoryThread;
/** @brief 使用 64 位小端字段和稳定线程节点；二版追加容器、三版增加装箱、四版增加闭包观察。 */
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
