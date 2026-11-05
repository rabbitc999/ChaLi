// SPDX-License-Identifier: MIT
// Copyright (c) 2026 小兔糖糖_OvO
/** @file node_structure.c @brief CAS 的结构相等、哈希与精确排序 C 算法。 */
#include "cas/detail/node_structure.h"
#include "chali_bigint.h"
#include <string.h>

/**
 * @brief 沿用原节点种类的规范化排序权重。
 * @param kind 种类。
 * @return 权重。
 */
static int kind_rank(unsigned kind)
{
    switch (kind)
    {
    case CAS_NODE_INTEGER:
    case CAS_NODE_RATIONAL: return 0;
    case CAS_NODE_SYMBOL: return 1;
    case CAS_NODE_CONSTANT: return 2;
    case CAS_NODE_POW: return 3;
    case CAS_NODE_FUNCTION: return 4;
    case CAS_NODE_MUL: return 5;
    case CAS_NODE_ADD: return 6;
    default: return 7;
    }
}
/**
 * @brief 按原空指针和字节字典序比较名称。
 * @param left 左名称。
 * @param right 右名称。
 * @return 负一、零或一。
 */
static int compare_text(const char *left, const char *right)
{
    if (left == right) return 0;
    if (!left) return -1;
    if (!right) return 1;
    int result = strcmp(left, right);
    return result < 0 ? -1 : result > 0;
}
/**
 * @brief 按节点种类和全部子树精确比较结构。
 * @param left 左节点。
 * @param right 右节点。
 * @param access 读取接口。
 * @return 结构相等为一。
 */
int cas_node_equals(const void *left, const void *right, const CasNodeAccess *access)
{
    if (left == right) return 1;
    if (!left || !right) return 0;
    CasNodeView a, b;
    access->view(left, &a);
    access->view(right, &b);
    if (a.kind != b.kind) return 0;
    switch (a.kind)
    {
    case CAS_NODE_INTEGER: return chali_bigint_compare(a.numerator, b.numerator) == 0;
    case CAS_NODE_RATIONAL:
        return chali_bigint_compare(a.numerator, b.numerator) == 0 && chali_bigint_compare(a.denominator, b.denominator) == 0;
    case CAS_NODE_SYMBOL: return a.symbol == b.symbol;
    case CAS_NODE_CONSTANT: return a.constant == b.constant;
    default: break;
    }
    if (compare_text(a.name, b.name) || a.count != b.count) return 0;
    for (size_t i = 0; i < a.count; ++i)
        if (!cas_node_equals(access->operand(left, i), access->operand(right, i), access)) return 0;
    return 1;
}
/**
 * @brief 沿用原有 64 位哈希的单项混合。
 * @param seed 当前种子。
 * @param value 输入项。
 * @return 新种子。
 */
static uint64_t mix(uint64_t seed, uint64_t value) { return (seed ^ value) * UINT64_C(1099511628211); }
/**
 * @brief 按从低到高的原大整数分块顺序混合哈希。
 * @param seed 种子。
 * @param value 大整数。
 * @return 新种子。
 */
static uint64_t mix_limbs(uint64_t seed, const ChaliBigInt *value)
{
    const uint32_t *words = (const uint32_t *)value->limbs.data;
    for (size_t i = 0; i < value->limbs.length; ++i) seed = mix(seed, words[i]);
    return seed;
}
/**
 * @brief 使用相同常数、种类偏移和子树顺序生成并缓存结构哈希。
 * @param node 节点。
 * @param access 读取接口。
 * @return 哈希，零结果规范为一；空节点为零。
 */
uint64_t cas_node_hash(const void *node, const CasNodeAccess *access)
{
    if (!node) return 0;
    uint64_t *cache = access->hash_cache ? access->hash_cache(node) : NULL;
    if (cache && *cache) return *cache;
    CasNodeView view;
    access->view(node, &view);
    uint64_t seed = mix(UINT64_C(1469598103934665603), (uint64_t)view.kind + UINT64_C(0x9e3779b97f4a7c15));
    switch (view.kind)
    {
    case CAS_NODE_INTEGER:
        seed = mix(mix_limbs(seed, view.numerator), (uint64_t)(view.numerator->sign + 1));
        break;
    case CAS_NODE_RATIONAL:
        seed = mix(mix_limbs(seed, view.numerator), (uint64_t)(view.numerator->sign + 1));
        seed = mix_limbs(seed, view.denominator);
        break;
    case CAS_NODE_SYMBOL: seed = mix(seed, view.symbol); break;
    case CAS_NODE_CONSTANT: seed = mix(seed, view.constant); break;
    default:
        if (view.name)
            for (const unsigned char *p = (const unsigned char *)view.name; *p; ++p) seed = mix(seed, *p);
        for (size_t i = 0; i < view.count; ++i) seed = mix(seed, cas_node_hash(access->operand(node, i), access));
        break;
    }
    if (!seed) seed = 1;
    if (cache) *cache = seed;
    return seed;
}
/**
 * @brief 通过 C 大整数交叉乘积精确比较两个有理数。
 * @param left 左视图。
 * @param right 右视图。
 * @param allocator 临时所有者。
 * @param result 接收比较结果。
 * @return C 数值操作状态。
 */
static int compare_numbers(const CasNodeView *left, const CasNodeView *right,
                           const ChaliCAllocator *allocator, int *result)
{
    ChaliBigInt a, b;
    ChaliCStatus status = chali_bigint_init(&a, allocator);
    if (status != CHALI_C_OK) return status;
    status = chali_bigint_init(&b, allocator);
    if (status != CHALI_C_OK)
    {
        chali_bigint_clear(&a);
        return status;
    }
    status = chali_bigint_multiply(&a, left->numerator, right->denominator);
    if (status == CHALI_C_OK) status = chali_bigint_multiply(&b, right->numerator, left->denominator);
    if (status == CHALI_C_OK) *result = chali_bigint_compare(&a, &b);
    chali_bigint_clear(&a);
    chali_bigint_clear(&b);
    return status;
}
/**
 * @brief 按原权重、名称和子树字典序比较，不改变节点。
 * @param left 左节点。
 * @param right 右节点。
 * @param access 读取接口。
 * @param allocator 数值临时所有者。
 * @param result 接收排序结果。
 * @return 成功为零；失败时保留调用方输出。
 */
int cas_node_compare(const void *left, const void *right, const CasNodeAccess *access,
                     const ChaliCAllocator *allocator, int *result)
{
    if (!access || !access->view || !access->operand || !result) return CHALI_C_INVALID_ARGUMENT;
    if (left == right)
    {
        *result = 0;
        return CHALI_C_OK;
    }
    if (!left || !right)
    {
        *result = left ? 1 : -1;
        return CHALI_C_OK;
    }
    CasNodeView a, b;
    access->view(left, &a);
    access->view(right, &b);
    int left_rank = kind_rank(a.kind), right_rank = kind_rank(b.kind);
    if (left_rank != right_rank)
    {
        *result = left_rank < right_rank ? -1 : 1;
        return CHALI_C_OK;
    }
    switch (a.kind)
    {
    case CAS_NODE_INTEGER:
    case CAS_NODE_RATIONAL: return compare_numbers(&a, &b, allocator, result);
    case CAS_NODE_SYMBOL:
        *result = a.symbol == b.symbol ? 0 : compare_text(a.name, b.name);
        return CHALI_C_OK;
    case CAS_NODE_CONSTANT:
        *result = a.constant == b.constant ? 0 : a.constant < b.constant ? -1 :
                                                                           1;
        return CHALI_C_OK;
    default: break;
    }
    int order = compare_text(a.name, b.name);
    if (order)
    {
        *result = order;
        return CHALI_C_OK;
    }
    const size_t count = a.count < b.count ? a.count : b.count;
    for (size_t i = 0; i < count; ++i)
    {
        int status = cas_node_compare(access->operand(left, i), access->operand(right, i), access, allocator, &order);
        if (status != CHALI_C_OK) return status;
        if (order)
        {
            *result = order;
            return CHALI_C_OK;
        }
    }
    *result = a.count == b.count ? 0 : a.count < b.count ? -1 :
                                                           1;
    return CHALI_C_OK;
}
