/**
 * @file chali_bigfloat_internal.h
 * @brief 十进制 C 服务共用的临时存储与事务提交。
 */
#ifndef CHALI_BIGFLOAT_INTERNAL_H
#define CHALI_BIGFLOAT_INTERNAL_H
#include "chali_bigfloat.h"
#include "chali_bigint_internal.h"
#include <limits.h>
#define CBF_TRY(expression)                     \
    do                                          \
    {                                           \
        status = (expression);                  \
        if (status != CHALI_C_OK) goto cleanup; \
    } while (0)
/**
 * @brief 转移临时结果，源变为零且不再持有尾数。
 * @param output 已初始化目标。
 * @param input 可移动源。
 */
static inline void cbf_move(ChaliBigFloat *output, ChaliBigFloat *input)
{
    if (output == input) return;
    cbn_move(&output->mantissa, &input->mantissa);
    output->exponent10 = input->exponent10;
    input->exponent10 = 0;
}
/**
 * @brief 用同一所有者建立多个无分配临时浮点数。
 * @param values 目标数组。
 * @param count 数组长度。
 * @param allocator 所有者分配接口。
 */
static inline void cbf_temporaries(ChaliBigFloat *values, size_t count, const ChaliCAllocator *allocator)
{
    size_t i;
    for (i = 0; i < count; ++i) chali_bigfloat_init(values + i, allocator);
}
/**
 * @brief 归还临时浮点数组。
 * @param values 已初始化数组。
 * @param count 数组长度。
 */
static inline void cbf_clear_temporaries(ChaliBigFloat *values, size_t count)
{
    size_t i;
    for (i = 0; i < count; ++i) chali_bigfloat_clear(values + i);
}
/**
 * @brief 在已隔离的工作值上移除十进制尾随零。
 * @param value 可修改工作值。
 * @return 操作状态。
 */
ChaliCStatus cbf_normalize_work(ChaliBigFloat *value);
/**
 * @brief 以原有大整数幂算法产生十的幂。
 * @param output 已初始化整数。
 * @param exponent 非负指数。
 * @return 操作状态。
 */
ChaliCStatus cbf_pow10(ChaliBigInt *output, uint64_t exponent);
/**
 * @brief 取得十进制尾数的有效位数，零为一位。
 * @param value 输入尾数。
 * @param digits 写入位数。
 * @param allocator 临时工作区分配接口。
 * @return 操作状态。
 */
ChaliCStatus cbf_digits(const ChaliBigInt *value, int64_t *digits, const ChaliCAllocator *allocator);
#endif
