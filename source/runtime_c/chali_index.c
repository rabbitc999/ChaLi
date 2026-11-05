/** @file chali_index.c @brief 不分配节点的开放寻址索引，支持删除与失败回滚。 */
#include "chali_index.h"
#include <string.h>

/** @brief 状态零为空槽，一为有效，二为探测链上的删除槽。 */
typedef struct IndexSlot
{
    uint64_t key;
    size_t value;
    unsigned char state;
} IndexSlot;
/**
 * @brief 混合所有 64 位键，避免连续槽号集中碰撞。
 * @param key 输入位模式。
 * @return 混合结果。
 */
static uint64_t hash_key(uint64_t key)
{
    key ^= key >> 30;
    key *= UINT64_C(0xbf58476d1ce4e5b9);
    key ^= key >> 27;
    key *= UINT64_C(0x94d049bb133111eb);
    return key ^ (key >> 31);
}
/**
 * @brief 找到已有键或可插入槽，不改变索引。
 * @param index 非空索引。
 * @param key 键。
 * @param found 接收是否找到。
 * @return 槽下标。
 */
static size_t find_slot(const ChaliCIndex *index, uint64_t key, int *found)
{
    const IndexSlot *slots = (const IndexSlot *)index->slots.data;
    const size_t mask = index->slots.length - 1;
    size_t at = (size_t)hash_key(key) & mask, deleted = SIZE_MAX;
    for (size_t scanned = 0; scanned < index->slots.length; ++scanned)
    {
        if (slots[at].state == 0)
        {
            *found = 0;
            return deleted == SIZE_MAX ? at : deleted;
        }
        if (slots[at].state == 1 && slots[at].key == key)
        {
            *found = 1;
            return at;
        }
        if (slots[at].state == 2 && deleted == SIZE_MAX) deleted = at;
        at = (at + 1) & mask;
    }
    *found = 0;
    return deleted;
}
/**
 * @brief 初始化固定宽度的空槽表。
 * @param index 目标。
 * @param allocator 可选分配器。
 * @return 操作状态。
 */
ChaliCStatus chali_c_index_init(ChaliCIndex *index, const ChaliCAllocator *allocator)
{
    if (!index) return CHALI_C_INVALID_ARGUMENT;
    index->length = index->deleted = 0;
    return chali_c_buffer_init(&index->slots, sizeof(IndexSlot), allocator);
}
/**
 * @brief 查找有效键。
 * @param index 索引。
 * @param key 键。
 * @param value 可选结果。
 * @return 是否找到。
 */
int chali_c_index_get(const ChaliCIndex *index, uint64_t key, size_t *value)
{
    if (!index || !index->slots.length) return 0;
    int found;
    size_t at = find_slot(index, key, &found);
    if (found && value) *value = ((const IndexSlot *)index->slots.data)[at].value;
    return found;
}
/**
 * @brief 在不分配的已预留槽中设置键。
 * @param index 索引。
 * @param key 键。
 * @param value 值。
 */
static void set_reserved(ChaliCIndex *index, uint64_t key, size_t value)
{
    int found;
    size_t at = find_slot(index, key, &found);
    IndexSlot *slot = (IndexSlot *)index->slots.data + at;
    if (!found)
    {
        ++index->length;
        if (slot->state == 2) --index->deleted;
    }
    slot->key = key;
    slot->value = value;
    slot->state = 1;
}
/**
 * @brief 先分配完整新表，再重新散列并原子交换。
 * @param index 索引。
 * @param capacity 新槽数，为二的幂。
 * @return 分配失败不改变原表。
 */
static ChaliCStatus rehash(ChaliCIndex *index, size_t capacity)
{
    ChaliCIndex copy;
    ChaliCStatus status = chali_c_index_init(&copy, &index->slots.allocator);
    if (status == CHALI_C_OK) status = chali_c_buffer_resize(&copy.slots, capacity);
    if (status != CHALI_C_OK) return status;
    const IndexSlot *slots = (const IndexSlot *)index->slots.data;
    for (size_t i = 0; i < index->slots.length; ++i)
        if (slots[i].state == 1) set_reserved(&copy, slots[i].key, slots[i].value);
    ChaliCIndex old = *index;
    *index = copy;
    chali_c_index_destroy(&old);
    return CHALI_C_OK;
}
/**
 * @brief 按最多四分之三占用率插入，更新已有键不分配。
 * @param index 索引。
 * @param key 键。
 * @param value 值。
 * @return 操作状态。
 */
ChaliCStatus chali_c_index_set(ChaliCIndex *index, uint64_t key, size_t value)
{
    if (!index || index->slots.element_size != sizeof(IndexSlot)) return CHALI_C_INVALID_ARGUMENT;
    if (chali_c_index_get(index, key, NULL))
    {
        set_reserved(index, key, value);
        return CHALI_C_OK;
    }
    const size_t capacity = index->slots.length;
    if (!capacity || index->length + index->deleted >= capacity - capacity / 4)
    {
        size_t next = capacity ? capacity : 16;
        if (capacity && index->length >= capacity / 2)
        {
            if (capacity > SIZE_MAX / 2) return CHALI_C_RANGE_ERROR;
            next = capacity * 2;
        }
        ChaliCStatus status = rehash(index, next);
        if (status != CHALI_C_OK) return status;
    }
    set_reserved(index, key, value);
    return CHALI_C_OK;
}
/**
 * @brief 标记删除槽，保留碰撞查询链。
 * @param index 索引。
 * @param key 键。
 * @return 是否删除有效键。
 */
int chali_c_index_remove(ChaliCIndex *index, uint64_t key)
{
    if (!index || !index->slots.length) return 0;
    int found;
    size_t at = find_slot(index, key, &found);
    if (!found) return 0;
    ((IndexSlot *)index->slots.data)[at].state = 2;
    --index->length;
    ++index->deleted;
    return 1;
}
/**
 * @brief 使用目标原所有者复制完整槽表。
 * @param target 目标。
 * @param source 源。
 * @return 失败时原目标不变。
 */
ChaliCStatus chali_c_index_copy(ChaliCIndex *target, const ChaliCIndex *source)
{
    if (!target || !source) return CHALI_C_INVALID_ARGUMENT;
    if (target == source) return CHALI_C_OK;
    ChaliCStatus status = chali_c_buffer_copy(&target->slots, &source->slots);
    if (status == CHALI_C_OK)
    {
        target->length = source->length;
        target->deleted = source->deleted;
    }
    return status;
}
/**
 * @brief 清空有效项与删除标记，保留容量。
 * @param index 索引。
 */
void chali_c_index_clear(ChaliCIndex *index)
{
    if (!index) return;
    if (index->slots.data) memset(index->slots.data, 0, index->slots.length * sizeof(IndexSlot));
    index->length = index->deleted = 0;
}
/**
 * @brief 释放捕获分配器拥有的槽表。
 * @param index 索引。
 */
void chali_c_index_destroy(ChaliCIndex *index)
{
    if (!index) return;
    chali_c_buffer_clear(&index->slots);
    index->length = index->deleted = 0;
}
