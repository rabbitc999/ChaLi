/**
 * @file chali_bigfloat_roots.c
 * @brief 十进制 C 整数幂、整数转换与方根。
 */
#include "chali_bigfloat_internal.h"

/**
 * @brief 按原有指数界限与大整数幂算法求整数次幂。
 * @param output 目标。
 * @param input 输入值。
 * @param exponent 非负指数。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_pow_integer(ChaliBigFloat *output, const ChaliBigFloat *input, uint64_t exponent)
{
    ChaliBigFloat work;
    ChaliCStatus status;
    uint64_t magnitude, limit;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    if (input->exponent10)
    {
        magnitude = input->exponent10 < 0 ? (uint64_t)-(int64_t)input->exponent10 : (uint64_t)input->exponent10;
        limit = input->exponent10 < 0 ? (uint64_t)-(int64_t)(INT_MIN / 2) : (uint64_t)(INT_MAX / 2);
        if (exponent > limit / magnitude) return CHALI_C_RANGE_ERROR;
    }
    chali_bigfloat_init(&work, &output->mantissa.limbs.allocator);
    CBF_TRY(chali_bigint_pow(&work.mantissa, &input->mantissa, exponent));
    work.exponent10 = input->exponent10 ? (int)((int64_t)input->exponent10 * (int64_t)exponent) : 0;
    CBF_TRY(cbf_normalize_work(&work));
    cbf_move(output, &work);
cleanup:
    chali_bigfloat_clear(&work);
    return status;
}
/**
 * @brief 按指数放大或用整数商余数处理定向取整。
 * @param output 整数目标。
 * @param input 原数值。
 * @param mode 零截断、负数向下、正数向上。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_to_integer(ChaliBigInt *output, const ChaliBigFloat *input, int mode)
{
    ChaliBigInt work, scale, remainder, unit;
    ChaliCStatus status = CHALI_C_OK;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    cbn_temporary(&work, output);
    cbn_temporary(&scale, output);
    cbn_temporary(&remainder, output);
    cbn_temporary(&unit, output);
    if (!input->mantissa.sign) goto commit;
    CBF_TRY(cbf_pow10(&scale, input->exponent10 < 0 ? (uint64_t)-(int64_t)input->exponent10 : (uint64_t)input->exponent10));
    if (input->exponent10 >= 0)
        CBF_TRY(chali_bigint_multiply(&work, &input->mantissa, &scale));
    else
    {
        CBF_TRY(chali_bigint_divmod_work(&work, &remainder, &input->mantissa, &scale, &output->limbs.allocator));
        if (remainder.sign && ((mode < 0 && input->mantissa.sign < 0) || (mode > 0 && input->mantissa.sign > 0)))
        {
            CBF_TRY(chali_bigint_set_i64(&unit, input->mantissa.sign < 0 ? -1 : 1));
            CBF_TRY(chali_bigint_add(&work, &work, &unit));
        }
    }
commit:
    cbn_move(output, &work);
cleanup:
    chali_bigint_clear(&work);
    chali_bigint_clear(&scale);
    chali_bigint_clear(&remainder);
    chali_bigint_clear(&unit);
    return status;
}
/**
 * @brief 检查尾数能否被负指数对应的十次幂整除。
 * @param input 原值。
 * @param integer 整数标志输出。
 * @param allocator 工作区分配接口。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_is_integer(const ChaliBigFloat *input, int *integer, const ChaliCAllocator *allocator)
{
    ChaliBigInt scale, remainder;
    ChaliCStatus status;
    if (!input || !integer) return CHALI_C_INVALID_ARGUMENT;
    if (!input->mantissa.sign || input->exponent10 >= 0)
    {
        *integer = 1;
        return CHALI_C_OK;
    }
    status = chali_bigint_init(&scale, allocator);
    if (status != CHALI_C_OK) return status;
    chali_bigint_init(&remainder, &scale.limbs.allocator);
    CBF_TRY(cbf_pow10(&scale, (uint64_t)-(int64_t)input->exponent10));
    CBF_TRY(chali_bigint_divmod_work(NULL, &remainder, &input->mantissa, &scale, &scale.limbs.allocator));
    *integer = !remainder.sign;
cleanup:
    chali_bigint_clear(&scale);
    chali_bigint_clear(&remainder);
    return status;
}
/**
 * @brief 凑偶指数并放大尾数，通过原整数平方根和保护位舍入求根。
 * @param output 目标。
 * @param input 非负输入。
 * @param digits 正有效位数。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_sqrt(ChaliBigFloat *output, const ChaliBigFloat *input, int digits)
{
    ChaliBigFloat work;
    ChaliBigInt magnitude, scale;
    ChaliCStatus status = CHALI_C_OK;
    int64_t exponent, have, want, extra;
    if (!output || !input || digits <= 0) return CHALI_C_INVALID_ARGUMENT;
    if (input->mantissa.sign < 0) return CHALI_C_DOMAIN_ERROR;
    chali_bigfloat_init(&work, &output->mantissa.limbs.allocator);
    cbn_temporary(&magnitude, &output->mantissa);
    cbn_temporary(&scale, &output->mantissa);
    if (!input->mantissa.sign) goto commit;
    CBF_TRY(chali_bigint_copy(&magnitude, &input->mantissa));
    exponent = input->exponent10;
    if (exponent % 2)
    {
        CBF_TRY(chali_bigint_set_u64(&scale, 10));
        CBF_TRY(chali_bigint_multiply(&magnitude, &magnitude, &scale));
        --exponent;
    }
    want = 2 * (int64_t)digits + 2;
    CBF_TRY(cbf_digits(&magnitude, &have, &output->mantissa.limbs.allocator));
    if (have < want)
    {
        extra = want - have;
        if ((extra + exponent) % 2) ++extra;
        CBF_TRY(cbf_pow10(&scale, (uint64_t)extra));
        CBF_TRY(chali_bigint_multiply(&magnitude, &magnitude, &scale));
        exponent -= extra;
    }
    if (exponent / 2 < INT_MIN || exponent / 2 > INT_MAX)
    {
        status = CHALI_C_RANGE_ERROR;
        goto cleanup;
    }
    CBF_TRY(chali_bigint_sqrt(&work.mantissa, &magnitude));
    work.exponent10 = (int)(exponent / 2);
    CBF_TRY(cbf_normalize_work(&work));
    CBF_TRY(chali_bigfloat_round(&work, &work, digits));
commit:
    cbf_move(output, &work);
cleanup:
    chali_bigfloat_clear(&work);
    chali_bigint_clear(&magnitude);
    chali_bigint_clear(&scale);
    return status;
}
/**
 * @brief 保留八轮低精度预热及逐级精度翻倍的牛顿方根。
 * @param output 目标。
 * @param input 非负输入。
 * @param degree 正根次。
 * @param digits 正有效位数。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_root(ChaliBigFloat *output, const ChaliBigFloat *input, uint64_t degree, int digits)
{
    ChaliBigFloat values[9];
    ChaliBigFloat *x = values, *previous = values + 1, *target = values + 2, *k = values + 3,
                  *km1 = values + 4, *power = values + 5, *quotient = values + 6,
                  *numerator = values + 7, *result = values + 8;
    ChaliCStatus status;
    int target_digits, precision, warmup, order;
    if (!output || !input || !degree || digits <= 0) return CHALI_C_INVALID_ARGUMENT;
    if (digits > (INT_MAX - 6) / 2) return CHALI_C_RANGE_ERROR;
    if (input->mantissa.sign < 0) return CHALI_C_DOMAIN_ERROR;
    if (degree == 1) return chali_bigfloat_round(output, input, digits);
    if (degree == 2) return chali_bigfloat_sqrt(output, input, digits);
    cbf_temporaries(values, 9, &output->mantissa.limbs.allocator);
    if (!input->mantissa.sign)
    {
        status = CHALI_C_OK;
        goto commit;
    }
    CBF_TRY(chali_bigint_root(&x->mantissa, &input->mantissa, degree));
    if (!x->mantissa.sign) CBF_TRY(chali_bigint_set_u64(&x->mantissa, 1));
    x->exponent10 = degree > INT64_MAX ? 0 : (int)((int64_t)input->exponent10 / (int64_t)degree);
    CBF_TRY(cbf_normalize_work(x));
    CBF_TRY(chali_bigfloat_copy(target, input));
    CBF_TRY(chali_bigint_set_u64(&k->mantissa, degree));
    CBF_TRY(cbf_normalize_work(k));
    CBF_TRY(chali_bigint_set_u64(&km1->mantissa, degree - 1));
    CBF_TRY(cbf_normalize_work(km1));
    for (warmup = 0; warmup < 8; ++warmup)
    {
        CBF_TRY(chali_bigfloat_copy(previous, x));
        CBF_TRY(chali_bigfloat_pow_integer(power, x, degree - 1));
        CBF_TRY(chali_bigfloat_divide(quotient, target, power, 10));
        CBF_TRY(chali_bigfloat_multiply(numerator, km1, x));
        CBF_TRY(chali_bigfloat_add(numerator, numerator, quotient, 0));
        CBF_TRY(chali_bigfloat_divide(x, numerator, k, 10));
        CBF_TRY(chali_bigfloat_compare(x, previous, &order, &output->mantissa.limbs.allocator));
        if (!order) break;
    }
    target_digits = 2 * digits + 4;
    precision = 8;
    while (precision < target_digits)
    {
        precision += precision < target_digits - precision ? precision : target_digits - precision;
        CBF_TRY(chali_bigfloat_pow_integer(power, x, degree - 1));
        CBF_TRY(chali_bigfloat_divide(quotient, target, power, precision + 2));
        CBF_TRY(chali_bigfloat_multiply(numerator, km1, x));
        CBF_TRY(chali_bigfloat_add(numerator, numerator, quotient, 0));
        CBF_TRY(chali_bigfloat_divide(x, numerator, k, precision + 2));
    }
    CBF_TRY(chali_bigfloat_round(result, x, digits));
commit:
    cbf_move(output, result);
cleanup:
    cbf_clear_temporaries(values, 9);
    return status;
}
