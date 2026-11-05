// SPDX-License-Identifier: MIT
// Copyright (c) 2026 小兔糖糖_OvO
/** @file symbol_table.h @brief 以 C 实现的稳定名称驻留和符号编号服务。 */
#ifndef CAS_C_SYMBOL_TABLE_H
#define CAS_C_SYMBOL_TABLE_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct CasSymbolTable CasSymbolTable;
/** @brief 可注入预算与失败的分配接口，所有存储归还原所有者。 */
typedef struct CasSymbolAllocator
{
    void *context;
    void *(*allocate)(void *context, size_t bytes);
    void (*release)(void *context, void *memory, size_t bytes);
} CasSymbolAllocator;
/**
 * @brief 创建独立符号表，供隔离运行时与生命周期测试使用。
 * @param allocator 可选分配接口；空指针使用 malloc/free。
 * @return 独占表；分配失败返回空。
 */
CasSymbolTable *cas_symbols_create(const CasSymbolAllocator *allocator);
/**
 * @brief 取得进程级表，名称地址持续有效到进程退出。
 * @return 不应销毁的默认表。
 */
CasSymbolTable *cas_symbols_default(void);
/**
 * @brief 释放独立表全部名称及索引；默认表保持进程级寿命。
 * @param table 独立表；允许为空。
 */
void cas_symbols_destroy(CasSymbolTable *table);
/**
 * @brief 驻留完整字节名称，重复名称返回原编号。
 * @param table 符号表。
 * @param name 名称字节，允许含零字节。
 * @param length 字节数。
 * @return 符号编号；参数、容量或分配失败返回 UINT32_MAX，已有编号不变。
 */
uint32_t cas_symbols_intern(CasSymbolTable *table, const char *name, size_t length);
/**
 * @brief 按完整字节名称查找编号。
 * @param table 符号表。
 * @param name 名称。
 * @param length 长度。
 * @return 编号；不存在返回 UINT32_MAX。
 */
uint32_t cas_symbols_lookup(CasSymbolTable *table, const char *name, size_t length);
/**
 * @brief 查询不可变名称，表扩容不会移动名称字节。
 * @param table 符号表。
 * @param id 符号编号。
 * @param length 可选长度输出。
 * @return 零结尾名称；无效编号返回空字符串并将长度置零。
 */
const char *cas_symbols_name(CasSymbolTable *table, uint32_t id, size_t *length);
/**
 * @brief 查询当前编号范围。
 * @param table 符号表。
 * @return 编号数量；有效编号为零至数量减一。
 */
size_t cas_symbols_count(CasSymbolTable *table);
#ifdef __cplusplus
}
#endif
#endif
