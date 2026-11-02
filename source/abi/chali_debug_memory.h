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
/** @brief 对象句柄快照；L／M／E 为容器，O 为装箱，F 为捕获，X 为存活但不透明的对象，first 是值表索引。 */
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
/** @brief 当前活跃存储的移动／销毁标记，类型号保留运行时实际身份。 */
typedef struct ChaliDebugMoved
{
    uint64_t address, size;
    int64_t type;
} ChaliDebugMoved;
/** @brief 原生帧所属的堆、数据区及对象表身份，不暴露其 C++ 布局。 */
typedef struct ChaliDebugFrameContext
{
    uint64_t heap, data, objects;
} ChaliDebugFrameContext;
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
    /** @brief 对象观察失效代次，版本五亦覆盖存储的移动及重新初始化。 */
    uint64_t object_revision_address, object_revision;
    /** @brief 版本五核对帧所属上下文成员及其当前值，防止跨实例逻辑地址碰撞。 */
    uint64_t heap_address, heap_identity, data_address, data_identity, objects_address, objects_identity;
    /** @brief 版本五追加的活跃移动标记及对象表完整标志。 */
    uint64_t moved, moved_count, objects_complete;
} ChaliDebugMemoryThread;
/** @brief 使用 64 位小端字段和稳定线程节点；二版容器、三版装箱、四版闭包、五版上下文及存活观察。 */
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
/**
 * @brief 在正常函数序言中记录该帧所属上下文，不由查看请求执行。
 * @param output 编译器预留的固定宽度观察槽。
 * @return 无返回值，不调用用户方法或分配语言对象。
 */
void chali_debug_capture_context(ChaliDebugFrameContext *output);
#ifdef __cplusplus
}
#endif
#endif
