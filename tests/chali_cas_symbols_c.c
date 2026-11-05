/** @file chali_cas_symbols_c.c @brief CAS C 符号表的地址、并发和分配失败协议检查。 */
#include "cas/detail/symbol_table.h"
#include "../source/abi/chali_thread.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief 记录每个分配点与活动块，不使用 C++ 服务。 */
typedef struct Owner
{
    size_t calls, live;
    long failure;
} Owner;
/** @brief 保持 C 最大对齐并记录实际所有者。 */
typedef union Header
{
    max_align_t alignment;
    struct
    {
        Owner *owner;
        size_t bytes;
    } block;
} Header;
/**
 * @brief 逐分配点注入失败并记录所有者。
 * @param context 分配状态。
 * @param bytes 字节数。
 * @return 分配地址或空。
 */
static void *allocate(void *context, size_t bytes)
{
    Owner *owner = (Owner *)context;
    if ((long)owner->calls++ == owner->failure) return NULL;
    Header *header = (Header *)malloc(sizeof(Header) + bytes);
    assert(header);
    header->block.owner = owner;
    header->block.bytes = bytes;
    ++owner->live;
    return header + 1;
}
/**
 * @brief 核对释放的原所有者和原字节数。
 * @param context 原所有者。
 * @param pointer 分配地址。
 * @param bytes 原大小。
 */
static void release(void *context, void *pointer, size_t bytes)
{
    Owner *owner = (Owner *)context;
    Header *header = (Header *)pointer - 1;
    assert(header->block.owner == owner && header->block.bytes == bytes && owner->live);
    --owner->live;
    free(header);
}
/**
 * @brief 执行固定工作负载，并核对每个失败点之前的编号仍可查询。
 * @param failure 需要失败的分配序号；负数不失败。
 * @return 实际分配调用数。
 */
static size_t allocation_case(long failure)
{
    Owner owner = { 0, 0, failure };
    CasSymbolAllocator allocator = { &owner, allocate, release };
    CasSymbolTable *table = cas_symbols_create(&allocator);
    if (!table)
    {
        assert(failure == 0 && owner.live == 0);
        return owner.calls;
    }
    char name[128];
    size_t inserted = 0;
    for (; inserted < 64; ++inserted)
    {
        snprintf(name, sizeof(name), "symbol-%zu-with-a-stable-owned-name", inserted);
        uint32_t id = cas_symbols_intern(table, name, strlen(name));
        if (id == UINT32_MAX) break;
        assert(id == inserted);
    }
    assert(cas_symbols_count(table) == inserted);
    for (size_t i = 0; i < inserted; ++i)
    {
        snprintf(name, sizeof(name), "symbol-%zu-with-a-stable-owned-name", i);
        assert(cas_symbols_lookup(table, name, strlen(name)) == i);
        const char *stored = cas_symbols_name(table, (uint32_t)i, NULL);
        assert(strcmp(stored, name) == 0);
        const size_t before = owner.calls;
        assert(cas_symbols_intern(table, name, strlen(name)) == i && before == owner.calls);
        assert(cas_symbols_name(table, (uint32_t)i, NULL) == stored);
    }
    const size_t calls = owner.calls;
    cas_symbols_destroy(table);
    assert(owner.live == 0);
    return calls;
}
/** @brief 共享一个表的线程工作参数。 */
typedef struct Worker
{
    CasSymbolTable *table;
    int number;
} Worker;
/**
 * @brief 同时驻留共有名称和线程独立名称，核对重复编号一致。
 * @param argument 工作参数。
 */
static void worker(void *argument)
{
    Worker *work = (Worker *)argument;
    for (int i = 0; i < 500; ++i)
    {
        char shared[64], unique[64];
        snprintf(shared, sizeof(shared), "shared-%d", i);
        uint32_t id = cas_symbols_intern(work->table, shared, strlen(shared));
        assert(id != UINT32_MAX && cas_symbols_lookup(work->table, shared, strlen(shared)) == id);
        snprintf(unique, sizeof(unique), "thread-%d-name-%d", work->number, i);
        id = cas_symbols_intern(work->table, unique, strlen(unique));
        assert(id != UINT32_MAX && strcmp(cas_symbols_name(work->table, id, NULL), unique) == 0);
    }
}
/**
 * @brief 验证逐分配失败、空串、内嵌零、长名称和跨平台并发驻留。
 * @return 全部断言通过返回零。
 */
int main(void)
{
    const size_t failures = allocation_case(-1);
    for (size_t failure = 0; failure < failures; ++failure) allocation_case((long)failure);
    CasSymbolTable *table = cas_symbols_create(NULL);
    assert(table);
    uint32_t empty = cas_symbols_intern(table, NULL, 0);
    assert(empty == 0 && cas_symbols_lookup(table, "", 0) == empty);
    const char embedded[] = { 'x', '\0', 'y' };
    uint32_t binary = cas_symbols_intern(table, embedded, sizeof(embedded));
    uint32_t short_name = cas_symbols_intern(table, "x", 1);
    size_t length = 0;
    assert(binary != short_name && cas_symbols_lookup(table, embedded, sizeof(embedded)) == binary);
    assert(memcmp(cas_symbols_name(table, binary, &length), embedded, sizeof(embedded)) == 0 && length == 3);
    assert(strcmp(cas_symbols_name(table, UINT32_MAX, &length), "") == 0 && length == 0);
    char long_name[8193];
    memset(long_name, 'q', sizeof(long_name) - 1);
    long_name[8192] = '\0';
    uint32_t first = cas_symbols_intern(table, long_name, 8192);
    const char *stable = cas_symbols_name(table, first, NULL);
    ChaliThread *threads[8];
    Worker workers[8];
    for (int i = 0; i < 8; ++i)
    {
        workers[i] = (Worker){ table, i };
        assert(chali_thread_create(worker, &workers[i], &threads[i]) == 0);
    }
    for (int i = 0; i < 8; ++i) assert(chali_thread_join(threads[i]) == 0);
    assert(cas_symbols_count(table) == 4504 && cas_symbols_name(table, first, NULL) == stable);
    assert(memcmp(stable, long_name, 8193) == 0);
    cas_symbols_destroy(table);
    printf("C CAS symbols: %zu allocation failures, stable bytes, embedded NUL and 8 concurrent workers passed\n", failures);
}
