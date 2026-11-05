// SPDX-License-Identifier: MIT
// Copyright (c) 2026 小兔糖糖_OvO
/** @file node_structure.h @brief 不依赖 C++ 节点布局的 CAS 结构运算 C 接口。 */
#ifndef CAS_C_NODE_STRUCTURE_H
#define CAS_C_NODE_STRUCTURE_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct ChaliBigInt ChaliBigInt;
typedef struct ChaliCAllocator ChaliCAllocator;
/** @brief 与既有不可变表达式种类一一对应的 C 编号。 */
enum CasNodeKind
{
    CAS_NODE_INTEGER = 0,
    CAS_NODE_RATIONAL = 1,
    CAS_NODE_SYMBOL = 2,
    CAS_NODE_ADD = 3,
    CAS_NODE_MUL = 4,
    CAS_NODE_POW = 5,
    CAS_NODE_FUNCTION = 6,
    CAS_NODE_CONSTANT = 7
};
/** @brief 一次调用借用的只读节点字段，数值和名称由原节点持有。 */
typedef struct CasNodeView
{
    unsigned kind;
    uint32_t symbol, constant;
    const char *name;
    const ChaliBigInt *numerator, *denominator;
    size_t count;
} CasNodeView;
/** @brief 对既有节点的无分配、不抛异常读访问。 */
typedef struct CasNodeAccess
{
    void (*view)(const void *node, CasNodeView *result);
    const void *(*operand)(const void *node, size_t index);
    uint64_t *(*hash_cache)(const void *node);
} CasNodeAccess;
/**
 * @brief 按原有节点种类和完整子树判断结构相等。
 * @param left 左节点。
 * @param right 右节点。
 * @param access 不可变无环节点的读取接口。
 * @return 结构相等为一，否则为零。
 */
int cas_node_equals(const void *left, const void *right, const CasNodeAccess *access);
/**
 * @brief 按原有 FNV 混合顺序计算结构哈希，并沿用可选缓存。
 * @param node 节点。
 * @param access 读取接口。
 * @return 哈希；空节点返回零。
 */
uint64_t cas_node_hash(const void *node, const CasNodeAccess *access);
/**
 * @brief 按原种类权重、精确有理数和子树顺序比较。
 * @param left 左节点。
 * @param right 右节点。
 * @param access 读取接口。
 * @param allocator 数值交叉乘积的临时分配接口；空指针使用 C 堆。
 * @param result 成功时接收负一、零或一。
 * @return 零表示成功，其余沿用 C 数值服务状态码。
 */
int cas_node_compare(const void *left, const void *right, const CasNodeAccess *access,
                     const ChaliCAllocator *allocator, int *result);
#ifdef __cplusplus
}
#endif
#endif
