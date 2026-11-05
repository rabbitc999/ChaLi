/**
 * @file chali_bigint_number_theory.c
 * @brief 二进制快速幂与欧几里得算法的 C 实现。
 */
#include "chali_bigint_internal.h"
#define CBN_TRY(expression)                     \
    do                                          \
    {                                           \
        status = (expression);                  \
        if (status != CHALI_C_OK) goto cleanup; \
    } while (0)

/**
 * @brief 交换独占工作区，不复制或分配分块。
 * @param left 工作区一。
 * @param right 工作区二。
 */
static void swap_integer(ChaliBigInt *left, ChaliBigInt *right)
{
    ChaliBigInt temporary = *left;
    *left = *right;
    *right = temporary;
}
/**
 * @brief 按原二进制快速幂次序计算整数幂，零的零次幂为一。
 * @param output 结果。
 * @param value 底数。
 * @param exponent 无符号指数。
 * @return 操作状态，失败保持原结果。
 */
ChaliCStatus chali_bigint_pow(ChaliBigInt *output, const ChaliBigInt *value, uint64_t exponent)
{
    ChaliBigInt result, base;
    ChaliCStatus status = CHALI_C_OK;
    if (!output || !value) return CHALI_C_INVALID_ARGUMENT;
    cbn_temporary(&result, output);
    cbn_temporary(&base, output);
    CBN_TRY(chali_bigint_set_u64(&result, 1));
    CBN_TRY(chali_bigint_copy(&base, value));
    while (exponent)
    {
        if (exponent & 1u) CBN_TRY(chali_bigint_multiply(&result, &result, &base));
        exponent >>= 1u;
        if (exponent) CBN_TRY(chali_bigint_multiply(&base, &base, &base));
    }
    cbn_move(output, &result);
cleanup:
    chali_bigint_clear(&result);
    chali_bigint_clear(&base);
    return status;
}
/**
 * @brief 在绝对值上运行原欧几里得公约数算法。
 * @param output 非负结果。
 * @param left 左值。
 * @param right 右值。
 * @return 操作状态，失败保持原结果。
 */
ChaliCStatus chali_bigint_gcd(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    ChaliBigInt a, b, remainder;
    ChaliCStatus status = CHALI_C_OK;
    if (!output || !left || !right) return CHALI_C_INVALID_ARGUMENT;
    cbn_temporary(&a, output);
    cbn_temporary(&b, output);
    cbn_temporary(&remainder, output);
    CBN_TRY(chali_bigint_copy(&a, left));
    CBN_TRY(chali_bigint_copy(&b, right));
    if (a.sign) a.sign = 1;
    if (b.sign) b.sign = 1;
    while (b.sign)
    {
        CBN_TRY(chali_bigint_divmod(NULL, &remainder, &a, &b));
        swap_integer(&a, &b);
        swap_integer(&b, &remainder);
    }
    cbn_move(output, &a);
cleanup:
    chali_bigint_clear(&a);
    chali_bigint_clear(&b);
    chali_bigint_clear(&remainder);
    return status;
}
/**
 * @brief 按先除公约数再相乘的原顺序计算最小公倍数。
 * @param output 非负结果。
 * @param left 左值。
 * @param right 右值。
 * @return 操作状态，失败保持原结果。
 */
ChaliCStatus chali_bigint_lcm(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    ChaliBigInt divisor, result;
    ChaliCStatus status = CHALI_C_OK;
    if (!output || !left || !right) return CHALI_C_INVALID_ARGUMENT;
    if (!left->sign || !right->sign) return chali_bigint_set_u64(output, 0);
    cbn_temporary(&divisor, output);
    cbn_temporary(&result, output);
    CBN_TRY(chali_bigint_gcd(&divisor, left, right));
    CBN_TRY(chali_bigint_divmod(&result, NULL, left, &divisor));
    CBN_TRY(chali_bigint_multiply(&result, &result, right));
    if (result.sign) result.sign = 1;
    cbn_move(output, &result);
cleanup:
    chali_bigint_clear(&divisor);
    chali_bigint_clear(&result);
    return status;
}
/**
 * @brief 将向零商余的负余数归一化到非负区间。
 * @param output 结果。
 * @param value 被除数。
 * @param modulus 非零模数，允许为负。
 * @return 操作状态，零模数返回定义域错误。
 */
ChaliCStatus chali_bigint_mod(ChaliBigInt *output, const ChaliBigInt *value, const ChaliBigInt *modulus)
{
    ChaliBigInt result, magnitude;
    ChaliCStatus status = CHALI_C_OK;
    if (!output || !value || !modulus) return CHALI_C_INVALID_ARGUMENT;
    if (!modulus->sign) return CHALI_C_DOMAIN_ERROR;
    magnitude = *modulus;
    magnitude.sign = 1;
    cbn_temporary(&result, output);
    CBN_TRY(chali_bigint_divmod(NULL, &result, value, &magnitude));
    if (result.sign < 0) CBN_TRY(chali_bigint_add(&result, &result, &magnitude));
    cbn_move(output, &result);
cleanup:
    chali_bigint_clear(&result);
    return status;
}
/**
 * @brief 按原大整数指数算法逐次平方、相乘和取模。
 * @param output 非负结果。
 * @param value 底数。
 * @param exponent 非负指数。
 * @param modulus 非零模数。
 * @return 操作状态，先检查模数，再检查指数。
 */
ChaliCStatus chali_bigint_powmod(ChaliBigInt *output, const ChaliBigInt *value,
                                 const ChaliBigInt *exponent, const ChaliBigInt *modulus)
{
    ChaliBigInt result, base, remaining, magnitude;
    ChaliCStatus status = CHALI_C_OK;
    if (!output || !value || !exponent || !modulus) return CHALI_C_INVALID_ARGUMENT;
    if (!modulus->sign) return CHALI_C_DOMAIN_ERROR;
    if (exponent->sign < 0) return CHALI_C_INVALID_ARGUMENT;
    magnitude = *modulus;
    magnitude.sign = 1;
    cbn_temporary(&result, output);
    cbn_temporary(&base, output);
    cbn_temporary(&remaining, output);
    CBN_TRY(chali_bigint_set_u64(&result, 1));
    CBN_TRY(chali_bigint_divmod(NULL, &result, &result, &magnitude));
    CBN_TRY(chali_bigint_mod(&base, value, &magnitude));
    CBN_TRY(chali_bigint_copy(&remaining, exponent));
    while (remaining.sign)
    {
        if (cbn_const_words(&remaining)[0] & 1u)
        {
            CBN_TRY(chali_bigint_multiply(&result, &result, &base));
            CBN_TRY(chali_bigint_divmod(NULL, &result, &result, &magnitude));
        }
        chali_bigint_shift_right(&remaining, 1);
        if (remaining.sign)
        {
            CBN_TRY(chali_bigint_multiply(&base, &base, &base));
            CBN_TRY(chali_bigint_divmod(NULL, &base, &base, &magnitude));
        }
    }
    cbn_move(output, &result);
cleanup:
    chali_bigint_clear(&result);
    chali_bigint_clear(&base);
    chali_bigint_clear(&remaining);
    return status;
}
#undef CBN_TRY
