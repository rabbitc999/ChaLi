/* 仅供 C 参考的开放寻址整数表；计时壳不依赖该实现。 */
#ifndef CHALI_PERF_HASH_INT_H
#define CHALI_PERF_HASH_INT_H
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
/** @brief C 参考表的单个键值槽，零为空，负一为删除标记。 */
typedef struct PerfHashEntry { int64_t key, value; } PerfHashEntry;
/** @brief 从空表动态增长的 C 参考存储，使用系统 calloc/free。 */
typedef struct PerfHashTable { PerfHashEntry *entries; size_t capacity, size, occupied; } PerfHashTable;
/**
 * @brief 混合整数键的全部位。
 * @param key 查找键。
 * @return 未取模的哈希值。
 */
static uint64_t perf_hash(int64_t key)
{
    uint64_t x=(uint64_t)key;
    x=(x^(x>>30))*UINT64_C(0xbf58476d1ce4e5b9);
    x=(x^(x>>27))*UINT64_C(0x94d049bb133111eb);
    return x^(x>>31);
}
/**
 * @brief 查找已有键或可插入的位置。
 * @param table 已分配且至少有一个空槽的表。
 * @param key 非零且非负一的键。
 * @return 已有槽或首个可复用槽的下标。
 */
static size_t perf_slot(const PerfHashTable *table,int64_t key)
{
    size_t at=(size_t)perf_hash(key)&(table->capacity-1), deleted=table->capacity;
    while (table->entries[at].key!=0)
    {
        if (table->entries[at].key==key) return at;
        if (table->entries[at].key==-1 && deleted==table->capacity) deleted=at;
        at=(at+1)&(table->capacity-1);
    }
    return deleted!=table->capacity ? deleted : at;
}
/**
 * @brief 扩容并迁移存活槽，清除删除标记。
 * @param table 当前表。
 * @param capacity 新的二次幂容量。
 */
static void perf_grow(PerfHashTable *table,size_t capacity)
{
    PerfHashEntry *old=table->entries;
    size_t oldCapacity=table->capacity;
    table->entries=(PerfHashEntry *)calloc(capacity,sizeof(PerfHashEntry));
    if (!table->entries) abort();
    table->capacity=capacity;
    table->occupied=table->size;
    for (size_t i=0;i<oldCapacity;++i)
        if (old[i].key!=0 && old[i].key!=-1) table->entries[perf_slot(table,old[i].key)]=old[i];
    free(old);
}
/**
 * @brief 插入或覆盖整数键值。
 * @param table 动态表。
 * @param key 正整数键。
 * @param value 保存的整数。
 */
static void perf_put(PerfHashTable *table,int64_t key,int64_t value)
{
    if (!table->capacity || (table->occupied+1)*4>table->capacity*3)
        perf_grow(table,table->capacity ? table->capacity*2 : 8);
    size_t at=perf_slot(table,key);
    if (table->entries[at].key!=key)
    {
        if (!table->entries[at].key) ++table->occupied;
        ++table->size;
    }
    table->entries[at]=(PerfHashEntry){ key,value };
}
/**
 * @brief 查询整数键，缺失时返回空。
 * @param table 动态表。
 * @param key 查询键。
 * @return 存活槽或空指针。
 */
static PerfHashEntry *perf_get(PerfHashTable *table,int64_t key)
{
    if (!table->capacity) return NULL;
    size_t at=perf_slot(table,key);
    return table->entries[at].key==key ? table->entries+at : NULL;
}
/**
 * @brief 删除已有键并保留探测链。
 * @param table 动态表。
 * @param key 删除键。
 * @return 删除成功为一，缺失为零。
 */
static int64_t perf_remove(PerfHashTable *table,int64_t key)
{
    PerfHashEntry *entry=perf_get(table,key);
    if (!entry) return 0;
    entry->key=-1;
    --table->size;
    return 1;
}
#endif
