/**
 * @file chali_bigfloat.c
 * @brief 十进制 C 大浮点的所有权、解析、规范化与二进制输入。
 */
#include "chali_bigfloat_internal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 初始化无分配的零。
 * @param value 目标对象。
 * @param allocator 可选分配接口。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_init(ChaliBigFloat *value, const ChaliCAllocator *allocator)
{
    if (!value) return CHALI_C_INVALID_ARGUMENT;
    value->exponent10 = 0;
    return chali_bigint_init(&value->mantissa, allocator);
}
/**
 * @brief 释放存储并归零，保留分配器。
 * @param value 已初始化对象。
 */
void chali_bigfloat_clear(ChaliBigFloat *value)
{
    if (!value) return;
    chali_bigint_clear(&value->mantissa);
    value->exponent10 = 0;
}
/**
 * @brief 保留目标所有者复制数值。
 * @param output 目标对象。
 * @param input 输入对象。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_copy(ChaliBigFloat *output, const ChaliBigFloat *input)
{
    ChaliCStatus status;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    status = chali_bigint_copy(&output->mantissa, &input->mantissa);
    if (status == CHALI_C_OK) output->exponent10 = input->exponent10;
    return status;
}
/**
 * @brief 调用原大整数幂内核产生十的幂。
 * @param output 已初始化目标。
 * @param exponent 指数。
 * @return 操作状态。
 */
ChaliCStatus cbf_pow10(ChaliBigInt *output, uint64_t exponent)
{
    ChaliBigInt ten;
    ChaliCStatus status;
    cbn_temporary(&ten, output);
    status = chali_bigint_set_u64(&ten, 10);
    if (status == CHALI_C_OK) status = chali_bigint_pow(output, &ten, exponent);
    chali_bigint_clear(&ten);
    return status;
}
/**
 * @brief 查询尾数十进制位数并避免有符号窄化。
 * @param value 尾数。
 * @param digits 位数输出。
 * @param allocator 临时工作区分配接口。
 * @return 操作状态。
 */
ChaliCStatus cbf_digits(const ChaliBigInt *value, int64_t *digits, const ChaliCAllocator *allocator)
{
    size_t count;
    ChaliCStatus status = chali_bigint_decimal_digits(value, &count, allocator);
    if (status != CHALI_C_OK) return status;
    if (count > INT64_MAX) return CHALI_C_RANGE_ERROR;
    *digits = (int64_t)count;
    return CHALI_C_OK;
}
/**
 * @brief 原地按九位和一位两档剥除尾数零，工作值失败可丢弃。
 * @param value 已隔离工作值。
 * @return 操作状态。
 */
ChaliCStatus cbf_normalize_work(ChaliBigFloat *value)
{
    ChaliBigInt divisor, quotient, remainder;
    ChaliCStatus status = CHALI_C_OK;
    int phase;
    if (!value->mantissa.sign)
    {
        value->exponent10 = 0;
        return CHALI_C_OK;
    }
    cbn_temporary(&divisor, &value->mantissa);
    cbn_temporary(&quotient, &value->mantissa);
    cbn_temporary(&remainder, &value->mantissa);
    for (phase = 0; phase < 2; ++phase)
    {
        const int increment = phase == 0 ? 9 : 1;
        CBF_TRY(chali_bigint_set_u64(&divisor, phase == 0 ? 1000000000u : 10u));
        for (;;)
        {
            CBF_TRY(chali_bigint_divmod_work(&quotient, &remainder, &value->mantissa, &divisor,
                                             &value->mantissa.limbs.allocator));
            if (remainder.sign) break;
            if (value->exponent10 > INT_MAX - increment)
            {
                status = CHALI_C_RANGE_ERROR;
                goto cleanup;
            }
            cbn_move(&value->mantissa, &quotient);
            value->exponent10 += increment;
        }
    }
cleanup:
    chali_bigint_clear(&divisor);
    chali_bigint_clear(&quotient);
    chali_bigint_clear(&remainder);
    return status;
}
/**
 * @brief 复制后事务性规范化，失败保留调用者数值。
 * @param output 已初始化目标。
 * @param input 原值。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_normalized(ChaliBigFloat *output, const ChaliBigFloat *input)
{
    ChaliBigFloat work;
    ChaliCStatus status;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    chali_bigfloat_init(&work, &output->mantissa.limbs.allocator);
    CBF_TRY(chali_bigfloat_copy(&work, input));
    CBF_TRY(cbf_normalize_work(&work));
    cbf_move(output, &work);
cleanup:
    chali_bigfloat_clear(&work);
    return status;
}
/**
 * @brief 原地事务性规范化，失败保留原值。
 * @param value 已初始化对象。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_normalize(ChaliBigFloat *value) { return chali_bigfloat_normalized(value, value); }
/**
 * @brief 复制整数并规范化为十进制尾数和指数。
 * @param output 目标。
 * @param value 整数。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_from_integer(ChaliBigFloat *output, const ChaliBigInt *value)
{
    ChaliBigFloat work;
    ChaliCStatus status;
    if (!output || !value) return CHALI_C_INVALID_ARGUMENT;
    chali_bigfloat_init(&work, &output->mantissa.limbs.allocator);
    CBF_TRY(chali_bigint_copy(&work.mantissa, value));
    CBF_TRY(cbf_normalize_work(&work));
    cbf_move(output, &work);
cleanup:
    chali_bigfloat_clear(&work);
    return status;
}
/**
 * @brief 判断 ASCII 十进制数字，不受 locale 影响。
 * @param value 字节。
 * @return 属于零到九时返回一。
 */
static int cbf_is_digit(char value) { return value >= '0' && value <= '9'; }
/**
 * @brief 按原语法解析十进制浮点，语法错返回零及 valid=0。
 * @param output 已初始化目标。
 * @param text 输入字节。
 * @param length 输入长度。
 * @param valid 解析成功标志。
 * @return 操作状态；内存失败保留原目标。
 */
ChaliCStatus chali_bigfloat_parse(ChaliBigFloat *output, const char *text, size_t length, int *valid)
{
    ChaliBigFloat work;
    ChaliCBuffer digits;
    ChaliCStatus status = CHALI_C_OK;
    size_t index = 0, integer_start, integer_length, fraction_start = 0, fraction_length = 0;
    int sign = 1, exponent_sign = 1;
    int64_t exponent = 0;
    if (!output || (!text && length) || !valid) return CHALI_C_INVALID_ARGUMENT;
    *valid = 0;
    chali_bigfloat_init(&work, &output->mantissa.limbs.allocator);
    chali_c_buffer_init(&digits, 1, &output->mantissa.limbs.allocator);
    while (index < length && (text[index] == ' ' || text[index] == '\t')) ++index;
    if (index < length && (text[index] == '+' || text[index] == '-'))
    {
        if (text[index] == '-') sign = -1;
        ++index;
    }
    integer_start = index;
    while (index < length && cbf_is_digit(text[index])) ++index;
    integer_length = index - integer_start;
    if (index < length && text[index] == '.')
    {
        fraction_start = ++index;
        while (index < length && cbf_is_digit(text[index])) ++index;
        fraction_length = index - fraction_start;
    }
    if (!integer_length && !fraction_length) goto invalid;
    if (index < length && (text[index] == 'e' || text[index] == 'E'))
    {
        ++index;
        if (index < length && (text[index] == '+' || text[index] == '-'))
        {
            if (text[index] == '-') exponent_sign = -1;
            ++index;
        }
        if (index >= length || !cbf_is_digit(text[index])) goto invalid;
        while (index < length && cbf_is_digit(text[index]))
        {
            exponent = exponent * 10 + text[index++] - '0';
            if (exponent > 1000000) goto invalid;
        }
        exponent *= exponent_sign;
    }
    while (index < length && (text[index] == ' ' || text[index] == '\t')) ++index;
    if (index != length || fraction_length > INT64_MAX) goto invalid;
    exponent -= (int64_t)fraction_length;
    if (exponent < INT_MIN || exponent > INT_MAX) goto invalid;
    CBF_TRY(chali_c_buffer_append(&digits, text + integer_start, integer_length));
    if (fraction_length) CBF_TRY(chali_c_buffer_append(&digits, text + fraction_start, fraction_length));
    CBF_TRY(chali_bigint_parse(&work.mantissa, (const char *)digits.data, digits.length, 10));
    if (work.mantissa.sign) work.mantissa.sign = sign;
    work.exponent10 = (int)exponent;
    CBF_TRY(cbf_normalize_work(&work));
    *valid = 1;
invalid:
    cbf_move(output, &work);
cleanup:
    chali_c_buffer_clear(&digits);
    chali_bigfloat_clear(&work);
    return status;
}
/**
 * @brief 原样保留逐有效位试写并读回 double 的转换算法。
 * @param output 目标。
 * @param value 双精度输入。
 * @param digits 有效位数上限。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_from_double(ChaliBigFloat *output, double value, int digits)
{
    ChaliBigFloat work;
    ChaliCStatus status = CHALI_C_OK;
    char buffer[64];
    int precision, valid, limit = digits > 0 && digits < 17 ? digits : 17;
    if (!output) return CHALI_C_INVALID_ARGUMENT;
    chali_bigfloat_init(&work, &output->mantissa.limbs.allocator);
    if (isfinite(value))
    {
        for (precision = 1; precision <= limit; ++precision)
        {
            int length = snprintf(buffer, sizeof(buffer), "%.*g", precision, value);
            if (length < 0 || (size_t)length >= sizeof(buffer))
            {
                status = CHALI_C_RANGE_ERROR;
                goto cleanup;
            }
            CBF_TRY(chali_bigfloat_parse(&work, buffer, (size_t)length, &valid));
            if (!valid || strtod(buffer, NULL) == value) break;
        }
    }
    cbf_move(output, &work);
cleanup:
    chali_bigfloat_clear(&work);
    return status;
}
