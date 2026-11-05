/** @file chali_sequence.h @brief 带元素生命周期回调的 C 连续对象存储。 */
#ifndef CHALI_C_SEQUENCE_H
#define CHALI_C_SEQUENCE_H
#include "chali_memory.h"
#ifdef __cplusplus
extern "C" {
#endif
/** @brief 不抛异常的移动构造与销毁回调，源槽移动后仍需销毁。 */
typedef struct ChaliCElementOps
{
    void (*move)(void *target, void *source);
    void (*destroy)(void *element);
} ChaliCElementOps;
/** @brief 捕获原分配器，独占有效元素及尚未构造的预留槽。 */
typedef struct ChaliCSequence
{
    void *data;
    size_t length, capacity, element_size;
    ChaliCAllocator allocator;
    ChaliCElementOps elements;
} ChaliCSequence;
/**
 * @brief 初始化空序列，不分配内存。
 * @param sequence 未持有存储的目标。
 * @param element_size 元素字节宽度。
 * @param allocator 分配接口；空指针使用 malloc/free。
 * @param elements 不抛异常的元素回调。
 * @return 参数有效返回成功。
 */
ChaliCStatus chali_c_sequence_init(ChaliCSequence *sequence, size_t element_size,
                                   const ChaliCAllocator *allocator, ChaliCElementOps elements);
/**
 * @brief 预留未构造槽；分配失败时原序列完全不变。
 * @param sequence 已初始化序列。
 * @param capacity 目标容量。
 * @return 操作状态。
 */
ChaliCStatus chali_c_sequence_reserve(ChaliCSequence *sequence, size_t capacity);
/**
 * @brief 插入外部元素的移动副本，并保持已有元素顺序。
 * @param sequence 已初始化序列。
 * @param index 插入下标，可以等于有效长度。
 * @param value 不与序列存储重叠的有效元素；返回后仍由调用方销毁。
 * @return 操作状态；失败时不移动输入和已有元素。
 */
ChaliCStatus chali_c_sequence_insert_move(ChaliCSequence *sequence, size_t index, void *value);
/**
 * @brief 销毁区间并将后续元素移到空槽。
 * @param sequence 已初始化序列。
 * @param index 区间起点。
 * @param count 删除数量。
 * @return 操作状态。
 */
ChaliCStatus chali_c_sequence_erase(ChaliCSequence *sequence, size_t index, size_t count);
/**
 * @brief 销毁全部有效元素，保留容量。
 * @param sequence 已初始化序列。
 */
void chali_c_sequence_clear(ChaliCSequence *sequence);
/**
 * @brief 销毁有效元素并通过原所有者释放容量，保留配置。
 * @param sequence 已初始化序列。
 */
void chali_c_sequence_destroy(ChaliCSequence *sequence);
#ifdef __cplusplus
}
#endif
#endif
