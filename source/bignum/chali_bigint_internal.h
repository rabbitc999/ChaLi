/**
 * @file chali_bigint_internal.h
 * @brief C 大整数实现共用的规范化、幅值与存储操作。
 */
#ifndef CHALI_BIGINT_INTERNAL_H
#define CHALI_BIGINT_INTERNAL_H
#include "chali_bigint.h"
#include <string.h>
/**
 * @brief 可修改分块数组。
 * @param value 数值。
 * @return 分块起点。
 */
static inline uint32_t *cbn_words(ChaliBigInt *value) { return (uint32_t *)value->limbs.data; }
/**
 * @brief 只读分块数组。
 * @param value 数值。
 * @return 分块起点。
 */
static inline const uint32_t *cbn_const_words(const ChaliBigInt *value) { return (const uint32_t *)value->limbs.data; }
/**
 * @brief 移除高位零块并同步零的符号。
 * @param value 数值。
 */
static inline void cbn_normalize(ChaliBigInt *value)
{
    while (value->limbs.length && cbn_words(value)[value->limbs.length - 1] == 0) --value->limbs.length;
    if (!value->limbs.length) value->sign = 0;
}
/**
 * @brief 移动数值存储并清空源所有权。
 * @param output 已初始化目标。
 * @param input 可移动源。
 */
static inline void cbn_move(ChaliBigInt *output, ChaliBigInt *input)
{
    if (output == input) return;
    chali_bigint_clear(output);
    *output = *input;
    input->sign = 0;
    input->limbs.data = NULL;
    input->limbs.length = 0;
    input->limbs.capacity = 0;
}
/**
 * @brief 用目标分配器建立临时数值。
 * @param output 临时数值。
 * @param owner 分配所有者。
 */
static inline void cbn_temporary(ChaliBigInt *output, const ChaliBigInt *owner)
{
    chali_bigint_init(output, &owner->limbs.allocator);
}
/**
 * @brief 比较两个幅值。
 * @param left 左值。
 * @param right 右值。
 * @return 负一、零或一。
 */
int cbn_compare_magnitude(const ChaliBigInt *left, const ChaliBigInt *right);
/**
 * @brief 乘一个 32 位数再加一个 32 位数。
 * @param value 原地数值。
 * @param multiplier 乘数。
 * @param addend 加数。
 * @return 操作状态。
 */
ChaliCStatus cbn_multiply_small_add(ChaliBigInt *value, uint32_t multiplier, uint32_t addend);
/**
 * @brief 原地除以非零 32 位数。
 * @param value 幅值。
 * @param divisor 非零除数。
 * @return 余数。
 */
uint32_t cbn_divide_small(ChaliBigInt *value, uint32_t divisor);
/**
 * @brief 以原生分块执行学校乘法，输出为独立工作值。
 * @param output 独立目标。
 * @param left 左幅值。
 * @param right 右幅值。
 * @return 操作状态。
 */
ChaliCStatus cbn_schoolbook_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right);
/**
 * @brief 用对角项半移和交叉项累积执行学校平方。
 * @param output 独立目标。
 * @param input 输入幅值。
 * @return 操作状态。
 */
ChaliCStatus cbn_schoolbook_square(ChaliBigInt *output, const ChaliBigInt *input);
/**
 * @brief 在零、正负一、二和无穷点评估三路 Toom-Cook 多项式。
 * @param output 独立目标。
 * @param left 左幅值。
 * @param right 右幅值。
 * @param square 是否为相同幅值的平方。
 * @return 操作状态。
 */
ChaliCStatus cbn_toom3_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right, int square);
#endif
