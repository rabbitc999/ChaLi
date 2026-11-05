/**
 * @file chali_bigfloat_math_internal.h
 * @brief C 高精度数学的状态与工作常量辅助。
 */
#ifndef CHALI_BIGFLOAT_MATH_INTERNAL_H
#define CHALI_BIGFLOAT_MATH_INTERNAL_H
#include "chali_bigfloat_math.h"
#include "chali_bigfloat_internal.h"
#include <string.h>
/**
 * @brief 清除本次运算的诊断，不访问线程全局状态。
 * @param error 可选诊断。
 */
static inline void cbf_math_reset(ChaliBigFloatError *error)
{
    if (error)
    {
        error->kind = CHALI_BIGFLOAT_ERROR_NONE;
        error->message = NULL;
    }
}
/**
 * @brief 保存静态错误消息并返回状态。
 * @param error 可选诊断。
 * @param kind 错误类别。
 * @param status C 状态。
 * @param message 静态 UTF-8 消息。
 * @return 原状态。
 */
static inline ChaliCStatus cbf_math_fail(ChaliBigFloatError *error, ChaliBigFloatErrorKind kind,
                                         ChaliCStatus status, const char *message)
{
    if (error)
    {
        error->kind = kind;
        error->message = message;
    }
    return status;
}
/**
 * @brief 在已隔离工作值上建立整数常量。
 * @param output 工作值。
 * @param number 整数常量。
 * @return 操作状态。
 */
static inline ChaliCStatus cbf_integer_work(ChaliBigFloat *output, int64_t number)
{
    ChaliCStatus status = chali_bigint_set_i64(&output->mantissa, number);
    if (status != CHALI_C_OK) return status;
    output->exponent10 = 0;
    return cbf_normalize_work(output);
}
/**
 * @brief 解析内部静态十进制常量。
 * @param output 工作值。
 * @param text 静态常量文本。
 * @return 操作状态。
 */
static inline ChaliCStatus cbf_constant_work(ChaliBigFloat *output, const char *text)
{
    int valid = 0;
    ChaliCStatus status = chali_bigfloat_parse(output, text, strlen(text), &valid);
    return status != CHALI_C_OK ? status : (valid ? CHALI_C_OK : CHALI_C_INVALID_ARGUMENT);
}
/**
 * @brief 比较两个工作数，使用左工作值的所有者。
 * @param left 左数。
 * @param right 右数。
 * @param order 比较结果。
 * @return 操作状态。
 */
static inline ChaliCStatus cbf_compare_work(const ChaliBigFloat *left, const ChaliBigFloat *right, int *order)
{
    return chali_bigfloat_compare(left, right, order, &left->mantissa.limbs.allocator);
}
/**
 * @brief 通过平方根约化和原有反双曲正切级数计算工作精度对数。
 * @param output 已初始化目标。
 * @param input 正输入。
 * @param work 工作有效位数。
 * @param error 可选错误细节。
 * @return 操作状态。
 */
ChaliCStatus cbf_logarithm_series(ChaliBigFloat *output, const ChaliBigFloat *input, int work, ChaliBigFloatError *error);
#endif
