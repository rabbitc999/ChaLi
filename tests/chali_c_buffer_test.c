/**
 * @file chali_c_buffer_test.c
 * @brief 验证 C 缓冲区的别名、容量溢出和分配失败恢复。
 */
#include "../source/runtime_c/chali_buffer.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** @brief 可注入一次分配失败的测试分配器。 */
struct AllocationState { int fail; size_t active; };
/**
 * @brief 分配测试存储并记录活跃块。
 * @param context 失败注入状态。
 * @param bytes 请求大小。
 * @return 分配结果。
 */
static void *allocate_test(void *context, size_t bytes)
{
    struct AllocationState *state = context;
    if (state->fail) { state->fail = 0; return NULL; }
    void *result = malloc(bytes);
    if (result) ++state->active;
    return result;
}
/**
 * @brief 回收并记录测试存储。
 * @param context 分配状态。
 * @param memory 存储。
 * @param bytes 原请求大小。
 */
static void release_test(void *context, void *memory, size_t bytes)
{
    (void)bytes;
    struct AllocationState *state = context;
    if (memory) { assert(state->active); --state->active; free(memory); }
}
/**
 * @brief 检查连续存储、别名复制和失败原子性。
 * @return 全部通过时为零。
 */
int main(void)
{
    struct AllocationState state = {0,0};
    ChaliCAllocator allocator = {&state, allocate_test, release_test};
    ChaliCBuffer value, copy;
    assert(chali_c_buffer_init(&value,sizeof(int),&allocator) == CHALI_C_OK);
    assert(chali_c_buffer_init(&copy,sizeof(int),&allocator) == CHALI_C_OK);
    int input[] = {1,2,3,4};
    assert(chali_c_buffer_append(&value,input,4) == CHALI_C_OK);
    assert(chali_c_buffer_append(&value,(int *)value.data+1,3) == CHALI_C_OK);
    int expected[] = {1,2,3,4,2,3,4};
    assert(value.length == 7 && !memcmp(value.data,expected,sizeof(expected)));
    assert(chali_c_buffer_insert(&value,2,(int *)value.data+4,3) == CHALI_C_OK);
    int inserted[] = {1,2,2,3,4,3,4,2,3,4};
    assert(value.length == 10 && !memcmp(value.data,inserted,sizeof(inserted)));
    assert(chali_c_buffer_erase(&value,2,3) == CHALI_C_OK);
    assert(value.length == 7 && !memcmp(value.data,expected,sizeof(expected)));
    assert(chali_c_buffer_copy(&copy,&value) == CHALI_C_OK);
    void *old = value.data;
    state.fail = 1;
    assert(chali_c_buffer_reserve(&value,100) == CHALI_C_OUT_OF_MEMORY);
    assert(value.data == old && value.length == 7 && !memcmp(value.data,expected,sizeof(expected)));
    assert(chali_c_buffer_reserve(&value,SIZE_MAX) == CHALI_C_RANGE_ERROR);
    state.fail = 1;
    assert(chali_c_buffer_copy(&copy,&value) == CHALI_C_OUT_OF_MEMORY);
    assert(copy.length == 7 && !memcmp(copy.data,expected,sizeof(expected)));
    assert(chali_c_buffer_copy(&value,&value) == CHALI_C_OK && value.data == old);
    chali_c_buffer_clear(&value);
    chali_c_buffer_clear(&copy);
    assert(state.active == 0);
    puts("C buffer aliasing, bounds and allocation rollback passed");
    return 0;
}
