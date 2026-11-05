/** @file chali_c_index_test.c @brief C 索引随机模型与逐分配失败检查。 */
#include "../source/runtime_c/chali_index.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/** @brief 测试分配所有者，负数表示不注入失败。 */
typedef struct Owner
{
    int remaining;
    size_t live;
} Owner;
/** @brief 保持最大对齐并校验释放回调的所有者和字节数。 */
typedef union Header
{
    max_align_t alignment;
    struct
    {
        Owner *owner;
        size_t bytes;
    } allocation;
} Header;
/**
 * @brief 在指定调用点拒绝分配。
 * @param context 所有者。
 * @param bytes 字节数。
 * @return 独立存储；注入失败为空。
 */
static void *allocate(void *context, size_t bytes)
{
    Owner *owner = (Owner *)context;
    if (owner->remaining == 0) return NULL;
    if (owner->remaining > 0) --owner->remaining;
    Header *header = (Header *)malloc(sizeof(Header) + bytes);
    assert(header);
    header->allocation.owner = owner;
    header->allocation.bytes = bytes;
    ++owner->live;
    return header + 1;
}
/**
 * @brief 验证存储回到原所有者并核对原字节数。
 * @param context 原所有者。
 * @param pointer 有效分配。
 * @param bytes 原申请字节数。
 */
static void release(void *context, void *pointer, size_t bytes)
{
    Header *header = (Header *)pointer - 1;
    Owner *owner = (Owner *)context;
    assert(header->allocation.owner == owner && header->allocation.bytes == bytes && owner->live);
    --owner->live;
    free(header);
}
/**
 * @brief 为模型下标生成覆盖完整位宽的唯一键。
 * @param index 模型下标。
 * @return 键位模式。
 */
static uint64_t key_for(size_t index) { return (uint64_t)index * UINT64_C(0x9e3779b97f4a7c15) ^ UINT64_MAX; }
/**
 * @brief 将所有候选键与独立固定数组模型核对。
 * @param index 被测索引。
 * @param present 存在标记。
 * @param expected 值模型。
 */
static void verify(const ChaliCIndex *index, const unsigned char *present, const size_t *expected)
{
    size_t count = 0;
    for (size_t i = 0; i < 1024; ++i)
    {
        size_t actual = 0;
        assert(chali_c_index_get(index, key_for(i), &actual) == present[i]);
        if (present[i])
        {
            assert(actual == expected[i]);
            ++count;
        }
    }
    assert(index->length == count);
}
/**
 * @brief 执行随机增删改、删除链重建、复制及分配失败注入。
 * @return 断言全部通过返回零。
 */
int main(void)
{
    Owner first = { -1, 0 }, second = { -1, 0 };
    ChaliCAllocator a = { &first, allocate, release }, b = { &second, allocate, release };
    ChaliCIndex actual, copy;
    assert(chali_c_index_init(&actual, &a) == CHALI_C_OK);
    assert(chali_c_index_init(&copy, &b) == CHALI_C_OK);
    unsigned char present[1024] = { 0 };
    size_t expected[1024] = { 0 };
    uint64_t random = 1701411834604692317;
    for (size_t step = 0; step < 65536; ++step)
    {
        random ^= random << 13;
        random ^= random >> 7;
        random ^= random << 17;
        size_t position = (size_t)(random >> 16) % 1024;
        if (step % 3)
        {
            first.remaining = 0;
            ChaliCStatus status = chali_c_index_set(&actual, key_for(position), step);
            if (status == CHALI_C_OUT_OF_MEMORY)
            {
                verify(&actual, present, expected);
                first.remaining = -1;
                status = chali_c_index_set(&actual, key_for(position), step);
            }
            assert(status == CHALI_C_OK);
            first.remaining = -1;
            present[position] = 1;
            expected[position] = step;
        }
        else
        {
            assert(chali_c_index_remove(&actual, key_for(position)) == present[position]);
            present[position] = 0;
        }
        if (step % 127 == 0) verify(&actual, present, expected);
        if (step % 4096 == 0)
        {
            assert(chali_c_index_copy(&copy, &actual) == CHALI_C_OK);
            verify(&copy, present, expected);
            assert(copy.slots.allocator.context == &second);
        }
    }
    verify(&actual, present, expected);
    assert(chali_c_index_copy(&copy, &actual) == CHALI_C_OK);
    verify(&copy, present, expected);
    chali_c_index_destroy(&copy);
    second.remaining = 0;
    assert(chali_c_index_copy(&copy, &actual) == CHALI_C_OUT_OF_MEMORY);
    assert(copy.length == 0 && copy.slots.data == NULL && second.live == 0);
    second.remaining = -1;
    assert(chali_c_index_copy(&copy, &actual) == CHALI_C_OK);
    chali_c_index_clear(&actual);
    assert(actual.length == 0 && actual.deleted == 0);
    for (size_t i = 0; i < 1024; ++i) assert(!chali_c_index_get(&actual, key_for(i), NULL));
    chali_c_index_destroy(&actual);
    chali_c_index_destroy(&copy);
    assert(first.live == 0 && second.live == 0);
    puts("C index: 65536 model operations, growth failures, tombstones, copy and original owners passed");
}
