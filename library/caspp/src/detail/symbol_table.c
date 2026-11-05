// SPDX-License-Identifier: MIT
// Copyright (c) 2026 小兔糖糖_OvO
/** @file symbol_table.c @brief C 哈希驻留表，名称地址稳定且插入失败不发布编号。 */
#include "cas/detail/symbol_table.h"
#include "chali_index.h"
#include <stdatomic.h>
#include <string.h>

/** @brief 稳定名称与同哈希冲突链，索引为编号加一。 */
typedef struct Symbol
{
    char *name;
    size_t length, next;
} Symbol;
/** @brief 容器字节、索引和名称均由同一 C 分配器拥有。 */
struct CasSymbolTable
{
    atomic_flag guard;
    ChaliCAllocator allocator;
    ChaliCBuffer names;
    ChaliCIndex hashes;
};
static CasSymbolTable default_table = { .guard = ATOMIC_FLAG_INIT };

/**
 * @brief 串行化表状态，不引入 C++ 锁或线程运行库。
 * @param table 符号表。
 */
static void lock_table(CasSymbolTable *table)
{
    while (atomic_flag_test_and_set_explicit(&table->guard, memory_order_acquire)) {}
}
/**
 * @brief 发布表状态。
 * @param table 符号表。
 */
static void unlock_table(CasSymbolTable *table)
{
    atomic_flag_clear_explicit(&table->guard, memory_order_release);
}
/**
 * @brief 为已独占的空表配置 C 存储，不分配容量。
 * @param table 符号表。
 * @param allocator 分配接口。
 */
static void initialize(CasSymbolTable *table, ChaliCAllocator allocator)
{
    table->allocator = allocator;
    (void)chali_c_buffer_init(&table->names, sizeof(Symbol), &allocator);
    (void)chali_c_index_init(&table->hashes, &allocator);
}
/**
 * @brief 计算完整名称的哈希，含零字节也参与计算。
 * @param name 字节。
 * @param length 长度。
 * @return 哈希位模式。
 */
static uint64_t hash_name(const char *name, size_t length)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    for (size_t i = 0; i < length; ++i) hash = (hash ^ (unsigned char)name[i]) * UINT64_C(1099511628211);
#ifdef CAS_SYMBOL_TABLE_TESTING_COLLISIONS
    return hash & 15;
#else
    return hash;
#endif
}
/**
 * @brief 在锁内按冲突链精确比较名称。
 * @param table 表。
 * @param name 名称。
 * @param length 长度。
 * @param hash 名称哈希。
 * @return 已有编号；不存在返回 UINT32_MAX。
 */
static uint32_t find_locked(CasSymbolTable *table, const char *name, size_t length, uint64_t hash)
{
    size_t next = 0;
    (void)chali_c_index_get(&table->hashes, hash, &next);
    const Symbol *names = (const Symbol *)table->names.data;
    while (next)
    {
        const Symbol *symbol = names + next - 1;
        if (symbol->length == length && (!length || memcmp(symbol->name, name, length) == 0)) return (uint32_t)(next - 1);
        next = symbol->next;
    }
    return UINT32_MAX;
}
/**
 * @brief 创建独立表并捕获分配接口。
 * @param allocator 可选分配接口。
 * @return 独占表或空。
 */
CasSymbolTable *cas_symbols_create(const CasSymbolAllocator *allocator)
{
    ChaliCAllocator owner = chali_c_default_allocator();
    if (allocator)
    {
        owner = (ChaliCAllocator){ allocator->context, allocator->allocate, allocator->release };
        if (!chali_c_allocator_valid(&owner)) return NULL;
    }
    CasSymbolTable *table = (CasSymbolTable *)owner.allocate(owner.context, sizeof(CasSymbolTable));
    if (!table) return NULL;
    table->guard = (atomic_flag)ATOMIC_FLAG_INIT;
    initialize(table, owner);
    return table;
}
/**
 * @brief 在 C 内初始化进程级符号表，不需要 C++ 静态对象和析构。
 * @return 进程级表。
 */
CasSymbolTable *cas_symbols_default(void)
{
    lock_table(&default_table);
    if (!default_table.names.element_size) initialize(&default_table, chali_c_default_allocator());
    unlock_table(&default_table);
    return &default_table;
}
/**
 * @brief 释放独立表所有名称与索引，调用方保证无并发访问。
 * @param table 独立表。
 */
void cas_symbols_destroy(CasSymbolTable *table)
{
    if (!table || table == &default_table) return;
    const ChaliCAllocator owner = table->allocator;
    Symbol *names = (Symbol *)table->names.data;
    for (size_t i = 0; i < table->names.length; ++i)
        owner.release(owner.context, names[i].name, names[i].length + 1);
    chali_c_buffer_clear(&table->names);
    chali_c_index_destroy(&table->hashes);
    owner.release(owner.context, table, sizeof(CasSymbolTable));
}
/**
 * @brief 先完成全部可能失败的分配，再发布新编号和哈希链头。
 * @param table 符号表。
 * @param name 名称字节。
 * @param length 长度。
 * @return 已有或新编号；失败返回 UINT32_MAX。
 */
uint32_t cas_symbols_intern(CasSymbolTable *table, const char *name, size_t length)
{
    if (!table || (!name && length) || length == SIZE_MAX) return UINT32_MAX;
    lock_table(table);
    const uint64_t hash = hash_name(name, length);
    uint32_t id = find_locked(table, name, length, hash);
    if (id != UINT32_MAX)
    {
        unlock_table(table);
        return id;
    }
    if (table->names.length >= UINT32_MAX)
    {
        unlock_table(table);
        return UINT32_MAX;
    }
    const size_t count = table->names.length;
    size_t capacity = table->names.capacity;
    ChaliCStatus status = CHALI_C_OK;
    if (count == capacity)
    {
        capacity = count > (SIZE_MAX / sizeof(Symbol)) / 2 ? count + 1 : count ? count * 2 :
                                                                                 16;
        status = chali_c_buffer_reserve(&table->names, capacity);
    }
    char *copy = status == CHALI_C_OK ? (char *)table->allocator.allocate(table->allocator.context, length + 1) : NULL;
    if (!copy)
    {
        unlock_table(table);
        return UINT32_MAX;
    }
    if (length) memcpy(copy, name, length);
    copy[length] = '\0';
    size_t previous = 0;
    (void)chali_c_index_get(&table->hashes, hash, &previous);
    status = chali_c_index_set(&table->hashes, hash, count + 1);
    if (status != CHALI_C_OK)
    {
        table->allocator.release(table->allocator.context, copy, length + 1);
        unlock_table(table);
        return UINT32_MAX;
    }
    ((Symbol *)table->names.data)[count] = (Symbol){ copy, length, previous };
    ++table->names.length;
    unlock_table(table);
    return (uint32_t)count;
}
/**
 * @brief 查询现有编号，不新增名称。
 * @param table 符号表。
 * @param name 名称。
 * @param length 长度。
 * @return 编号或 UINT32_MAX。
 */
uint32_t cas_symbols_lookup(CasSymbolTable *table, const char *name, size_t length)
{
    if (!table || (!name && length)) return UINT32_MAX;
    lock_table(table);
    const uint32_t id = find_locked(table, name, length, hash_name(name, length));
    unlock_table(table);
    return id;
}
/**
 * @brief 借用不可变名称，在表扩容和并发驻留后仍有效。
 * @param table 表。
 * @param id 编号。
 * @param length 可选长度输出。
 * @return 名称或空字符串。
 */
const char *cas_symbols_name(CasSymbolTable *table, uint32_t id, size_t *length)
{
    if (length) *length = 0;
    if (!table) return "";
    lock_table(table);
    const char *name = "";
    if (id < table->names.length)
    {
        const Symbol *symbol = (const Symbol *)table->names.data + id;
        name = symbol->name;
        if (length) *length = symbol->length;
    }
    unlock_table(table);
    return name;
}
/**
 * @brief 读取稳定编号范围，已有编号不会改变。
 * @param table 表。
 * @return 当前名称数。
 */
size_t cas_symbols_count(CasSymbolTable *table)
{
    if (!table) return 0;
    lock_table(table);
    const size_t count = table->names.length;
    unlock_table(table);
    return count;
}
