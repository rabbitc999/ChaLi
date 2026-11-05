/**
 * @file chali_bigint_modular.c
 * @brief 扩展欧几里得和模逆元的 C 实现。
 */
#include "chali_bigint_internal.h"
#define CBN_TRY(expression)                     \
    do                                          \
    {                                           \
        status = (expression);                  \
        if (status != CHALI_C_OK) goto cleanup; \
    } while (0)
/**
 * @brief 转移迭代变量而不分配存储。
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
 * @brief 按原迭代扩展欧几里得算法计算公约数及有符号裴蜀系数。
 * @param gcd 非负公约数目标。
 * @param x 左系数目标，须与另两个目标不同。
 * @param y 右系数目标，须与另两个目标不同。
 * @param left 左值，允许与任意目标别名。
 * @param right 右值，允许与任意目标别名。
 * @return 状态，分配失败时三个目标均保持不变。
 */
ChaliCStatus chali_bigint_extended_gcd(ChaliBigInt *gcd, ChaliBigInt *x, ChaliBigInt *y,
                                       const ChaliBigInt *left, const ChaliBigInt *right)
{
    ChaliBigInt work[11], publish_gcd, publish_x, publish_y;
    ChaliBigInt *old_r = &work[0], *r = &work[1], *old_s = &work[2], *s = &work[3];
    ChaliBigInt *old_t = &work[4], *t = &work[5], *quotient = &work[6], *remainder = &work[7];
    ChaliBigInt *next_s = &work[8], *next_t = &work[9], *product = &work[10];
    ChaliCStatus status = CHALI_C_OK;
    size_t index;
    if (!gcd || !x || !y || !left || !right || gcd == x || gcd == y || x == y)
        return CHALI_C_INVALID_ARGUMENT;
    for (index = 0; index < 11; ++index) cbn_temporary(&work[index], gcd);
    cbn_temporary(&publish_gcd, gcd);
    cbn_temporary(&publish_x, x);
    cbn_temporary(&publish_y, y);
    CBN_TRY(chali_bigint_copy(old_r, left));
    CBN_TRY(chali_bigint_copy(r, right));
    if (old_r->sign) old_r->sign = 1;
    if (r->sign) r->sign = 1;
    CBN_TRY(chali_bigint_set_u64(old_s, 1));
    CBN_TRY(chali_bigint_set_u64(t, 1));
    while (r->sign)
    {
        CBN_TRY(chali_bigint_divmod(quotient, remainder, old_r, r));
        swap_integer(old_r, r);
        swap_integer(r, remainder);
        CBN_TRY(chali_bigint_multiply(product, quotient, s));
        CBN_TRY(chali_bigint_subtract(next_s, old_s, product));
        swap_integer(old_s, s);
        swap_integer(s, next_s);
        CBN_TRY(chali_bigint_multiply(product, quotient, t));
        CBN_TRY(chali_bigint_subtract(next_t, old_t, product));
        swap_integer(old_t, t);
        swap_integer(t, next_t);
    }
    if (left->sign < 0) old_s->sign = -old_s->sign;
    if (right->sign < 0) old_t->sign = -old_t->sign;
    CBN_TRY(chali_bigint_copy(&publish_gcd, old_r));
    CBN_TRY(chali_bigint_copy(&publish_x, old_s));
    CBN_TRY(chali_bigint_copy(&publish_y, old_t));
    cbn_move(gcd, &publish_gcd);
    cbn_move(x, &publish_x);
    cbn_move(y, &publish_y);
cleanup:
    for (index = 0; index < 11; ++index) chali_bigint_clear(&work[index]);
    chali_bigint_clear(&publish_gcd);
    chali_bigint_clear(&publish_x);
    chali_bigint_clear(&publish_y);
    return status;
}
/**
 * @brief 使用原扩展欧几里得及非负取模计算模逆元。
 * @param output 结果。
 * @param value 数值。
 * @param modulus 非零模数。
 * @return 状态，零模数或不互素返回定义域错误。
 */
ChaliCStatus chali_bigint_inverse(ChaliBigInt *output, const ChaliBigInt *value, const ChaliBigInt *modulus)
{
    ChaliBigInt gcd, x, y, result;
    ChaliCStatus status = CHALI_C_OK;
    if (!output || !value || !modulus) return CHALI_C_INVALID_ARGUMENT;
    if (!modulus->sign) return CHALI_C_DOMAIN_ERROR;
    cbn_temporary(&gcd, output);
    cbn_temporary(&x, output);
    cbn_temporary(&y, output);
    cbn_temporary(&result, output);
    CBN_TRY(chali_bigint_extended_gcd(&gcd, &x, &y, value, modulus));
    if (gcd.sign != 1 || gcd.limbs.length != 1 || cbn_const_words(&gcd)[0] != 1)
    {
        status = CHALI_C_DOMAIN_ERROR;
        goto cleanup;
    }
    CBN_TRY(chali_bigint_mod(&result, &x, modulus));
    cbn_move(output, &result);
cleanup:
    chali_bigint_clear(&gcd);
    chali_bigint_clear(&x);
    chali_bigint_clear(&y);
    chali_bigint_clear(&result);
    return status;
}
#undef CBN_TRY
