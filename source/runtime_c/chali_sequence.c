/** @file chali_sequence.c @brief 对象数组的 C 扩容、插入、删除和所有权。 */
#include "chali_sequence.h"
#include <stdint.h>

/**
 * @brief 取得已验证下标的槽地址。
 * @param sequence 序列。
 * @param index 下标。
 * @return 槽地址。
 */
static void *slot(const ChaliCSequence *sequence, size_t index)
{
    return (unsigned char *)sequence->data + index * sequence->element_size;
}
/**
 * @brief 初始化带不抛异常回调的连续对象存储。
 * @param sequence 空目标。
 * @param element_size 元素宽度。
 * @param allocator 可选分配器。
 * @param elements 移动构造与析构回调。
 * @return 操作状态。
 */
ChaliCStatus chali_c_sequence_init(ChaliCSequence *sequence, size_t element_size,
                                   const ChaliCAllocator *allocator, ChaliCElementOps elements)
{
    if (!sequence || !element_size || !elements.move || !elements.destroy || (allocator && !chali_c_allocator_valid(allocator))) return CHALI_C_INVALID_ARGUMENT;
    *sequence = (ChaliCSequence){ NULL, 0, 0, element_size,
                                  allocator ? *allocator : chali_c_default_allocator(), elements };
    return CHALI_C_OK;
}
/**
 * @brief 分配成功后移动原元素，最后发布新存储。
 * @param sequence 已初始化序列。
 * @param capacity 目标容量。
 * @return 分配失败时保持输入不变。
 */
ChaliCStatus chali_c_sequence_reserve(ChaliCSequence *sequence, size_t capacity)
{
    if (!sequence || !sequence->element_size) return CHALI_C_INVALID_ARGUMENT;
    if (capacity <= sequence->capacity) return CHALI_C_OK;
    if (capacity > SIZE_MAX / sequence->element_size) return CHALI_C_RANGE_ERROR;
    void *data = sequence->allocator.allocate(sequence->allocator.context, capacity * sequence->element_size);
    if (!data) return CHALI_C_OUT_OF_MEMORY;
    for (size_t i = 0; i < sequence->length; ++i)
    {
        sequence->elements.move((unsigned char *)data + i * sequence->element_size, slot(sequence, i));
        sequence->elements.destroy(slot(sequence, i));
    }
    if (sequence->data)
        sequence->allocator.release(sequence->allocator.context, sequence->data,
                                    sequence->capacity * sequence->element_size);
    sequence->data = data;
    sequence->capacity = capacity;
    return CHALI_C_OK;
}
/**
 * @brief 在已经预留的槽间从后向前移动，插入外部元素。
 * @param sequence 序列。
 * @param index 插入位置。
 * @param value 外部有效对象。
 * @return 失败时不移动输入或改变序列。
 */
ChaliCStatus chali_c_sequence_insert_move(ChaliCSequence *sequence, size_t index, void *value)
{
    if (!sequence || !sequence->element_size || !value || index > sequence->length)
        return CHALI_C_INVALID_ARGUMENT;
    const size_t maximum = SIZE_MAX / sequence->element_size;
    if (sequence->length == maximum) return CHALI_C_RANGE_ERROR;
    const uintptr_t address = (uintptr_t)value, begin = (uintptr_t)sequence->data;
    if (sequence->data && address >= begin && address - begin < sequence->capacity * sequence->element_size)
        return CHALI_C_INVALID_ARGUMENT;
    if (sequence->length == sequence->capacity)
    {
        size_t capacity = sequence->length > maximum / 2 ? maximum : sequence->length * 2;
        if (!capacity) capacity = 1;
        ChaliCStatus status = chali_c_sequence_reserve(sequence, capacity);
        if (status != CHALI_C_OK) return status;
    }
    for (size_t i = sequence->length; i > index; --i)
    {
        sequence->elements.move(slot(sequence, i), slot(sequence, i - 1));
        sequence->elements.destroy(slot(sequence, i - 1));
    }
    sequence->elements.move(slot(sequence, index), value);
    ++sequence->length;
    return CHALI_C_OK;
}
/**
 * @brief 销毁指定范围并向前移动余下元素。
 * @param sequence 序列。
 * @param index 起点。
 * @param count 数量。
 * @return 范围无效时不执行回调。
 */
ChaliCStatus chali_c_sequence_erase(ChaliCSequence *sequence, size_t index, size_t count)
{
    if (!sequence || !sequence->element_size || index > sequence->length || count > sequence->length - index)
        return CHALI_C_INVALID_ARGUMENT;
    if (!count) return CHALI_C_OK;
    for (size_t i = index; i < index + count; ++i) sequence->elements.destroy(slot(sequence, i));
    for (size_t i = index + count; i < sequence->length; ++i)
    {
        sequence->elements.move(slot(sequence, i - count), slot(sequence, i));
        sequence->elements.destroy(slot(sequence, i));
    }
    sequence->length -= count;
    return CHALI_C_OK;
}
/**
 * @brief 销毁所有有效对象并保留存储。
 * @param sequence 序列。
 */
void chali_c_sequence_clear(ChaliCSequence *sequence)
{
    if (!sequence || !sequence->element_size) return;
    while (sequence->length) sequence->elements.destroy(slot(sequence, --sequence->length));
}
/**
 * @brief 将存储归还原分配器。
 * @param sequence 序列。
 */
void chali_c_sequence_destroy(ChaliCSequence *sequence)
{
    if (!sequence || !sequence->element_size) return;
    chali_c_sequence_clear(sequence);
    if (sequence->data)
        sequence->allocator.release(sequence->allocator.context, sequence->data,
                                    sequence->capacity * sequence->element_size);
    sequence->data = NULL;
    sequence->capacity = 0;
}
