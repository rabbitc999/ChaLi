/**
 * @file chali_bigfloat_transcendental.c
 * @brief 原 AGM、Taylor 与对数级数的 C 实现。
 */
#include "chali_bigfloat_math_internal.h"

/**
 * @brief 验证工作位数并加入原有保护位。
 * @param digits 请求位数。
 * @param extra 约化所需额外位数。
 * @param precision 工作位数输出。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_work_precision(int digits, int64_t extra, int *precision, ChaliBigFloatError *error)
{
    int64_t work;
    cbf_math_reset(error);
    if (!precision) return CHALI_C_INVALID_ARGUMENT;
    if (digits < 1 || digits > 250000)
        return cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_PRECISION, CHALI_C_RANGE_ERROR,
                             "高精度数学工作有效位数必须在 1..250000 之间");
    if (extra < 0 || extra > 300032)
        return cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_PRECISION, CHALI_C_RANGE_ERROR,
                             "高精度数学参数约化超出工作位数预算");
    work = (int64_t)digits + 32 + extra;
    if (work > 300032)
        return cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_PRECISION, CHALI_C_RANGE_ERROR,
                             "高精度数学参数约化超出工作位数预算");
    *precision = (int)work;
    return CHALI_C_OK;
}
/**
 * @brief 由尾数位数与十进制指数计算数量级。
 * @param value 输入。
 * @param order 结果输出。
 * @param allocator 工作区分配接口。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_decimal_order(const ChaliBigFloat *value, int64_t *order, const ChaliCAllocator *allocator)
{
    ChaliCStatus status;
    int64_t digits;
    if (!value || !order) return CHALI_C_INVALID_ARGUMENT;
    if (!value->mantissa.sign)
    {
        *order = 0;
        return CHALI_C_OK;
    }
    status = cbf_digits(&value->mantissa, &digits, allocator);
    if (status != CHALI_C_OK) return status;
    if (digits > INT64_MAX - INT_MAX) return CHALI_C_RANGE_ERROR;
    *order = digits - 1 + value->exponent10;
    return CHALI_C_OK;
}
/**
 * @brief 平方根约化至 0.9..1.1，再累加反双曲正切级数。
 * @param output 目标。
 * @param input 正输入。
 * @param work 工作有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus cbf_logarithm_series(ChaliBigFloat *output, const ChaliBigFloat *input, int work, ChaliBigFloatError *error)
{
    enum
    {
        VALUE,
        LOW,
        HIGH,
        ONE,
        RATIO,
        SQUARE,
        TERM,
        SUM,
        DENOM,
        NEXT,
        SCALE,
        TEMP,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int reductions = 0, order, index;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(chali_bigfloat_copy(v + VALUE, input));
    CBF_TRY(cbf_constant_work(v + LOW, "0.9"));
    CBF_TRY(cbf_constant_work(v + HIGH, "1.1"));
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    for (;;)
    {
        CBF_TRY(cbf_compare_work(v + VALUE, v + HIGH, &order));
        if (order <= 0)
        {
            CBF_TRY(cbf_compare_work(v + VALUE, v + LOW, &order));
            if (order >= 0) break;
        }
        CBF_TRY(chali_bigfloat_sqrt(v + VALUE, v + VALUE, work));
        ++reductions;
    }
    CBF_TRY(chali_bigfloat_add(v + TEMP, v + VALUE, v + ONE, 1));
    CBF_TRY(chali_bigfloat_add(v + DENOM, v + VALUE, v + ONE, 0));
    CBF_TRY(chali_bigfloat_divide(v + RATIO, v + TEMP, v + DENOM, work));
    CBF_TRY(chali_bigfloat_multiply(v + SQUARE, v + RATIO, v + RATIO));
    CBF_TRY(chali_bigfloat_round(v + SQUARE, v + SQUARE, work));
    CBF_TRY(chali_bigfloat_copy(v + TERM, v + RATIO));
    CBF_TRY(chali_bigfloat_copy(v + SUM, v + RATIO));
    for (index = 1; index <= work * 2 + 64; ++index)
    {
        CBF_TRY(chali_bigfloat_multiply(v + TERM, v + TERM, v + SQUARE));
        CBF_TRY(chali_bigfloat_round(v + TERM, v + TERM, work));
        CBF_TRY(cbf_integer_work(v + DENOM, (int64_t)index * 2 + 1));
        CBF_TRY(chali_bigfloat_divide(v + TEMP, v + TERM, v + DENOM, work));
        CBF_TRY(chali_bigfloat_add(v + NEXT, v + SUM, v + TEMP, 0));
        CBF_TRY(chali_bigfloat_round(v + NEXT, v + NEXT, work));
        CBF_TRY(cbf_compare_work(v + NEXT, v + SUM, &order));
        if (!order)
        {
            CBF_TRY(cbf_integer_work(v + SCALE, 2));
            CBF_TRY(chali_bigfloat_pow_integer(v + SCALE, v + SCALE, (uint64_t)reductions + 1));
            CBF_TRY(chali_bigfloat_multiply(v + RESULT, v + SUM, v + SCALE));
            CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, work));
            cbf_move(output, v + RESULT);
            goto cleanup;
        }
        cbf_move(v + SUM, v + NEXT);
    }
    status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_CONVERGENCE, CHALI_C_DOMAIN_ERROR,
                           "高精度对数级数未在预算内收敛");
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 原 Gauss-Legendre AGM 圆周率算法，保留六十四轮上限。
 * @param output 目标。
 * @param digits 结果有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_pi(ChaliBigFloat *output, int digits, ChaliBigFloatError *error)
{
    enum
    {
        A,
        B,
        T,
        SCALE,
        ONE,
        TWO,
        FOUR,
        NEXT_A,
        NEXT_B,
        DELTA,
        TEMP,
        DENOM,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int work, iteration, order;
    if (!output) return CHALI_C_INVALID_ARGUMENT;
    status = chali_bigfloat_work_precision(digits, 0, &work, error);
    if (status != CHALI_C_OK) return status;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    CBF_TRY(cbf_integer_work(v + TWO, 2));
    CBF_TRY(cbf_integer_work(v + FOUR, 4));
    CBF_TRY(chali_bigfloat_copy(v + A, v + ONE));
    CBF_TRY(chali_bigfloat_sqrt(v + TEMP, v + TWO, work));
    CBF_TRY(chali_bigfloat_divide(v + B, v + ONE, v + TEMP, work));
    CBF_TRY(cbf_constant_work(v + T, "0.25"));
    CBF_TRY(chali_bigfloat_copy(v + SCALE, v + ONE));
    for (iteration = 0; iteration < 64; ++iteration)
    {
        CBF_TRY(chali_bigfloat_add(v + TEMP, v + A, v + B, 0));
        CBF_TRY(chali_bigfloat_divide(v + NEXT_A, v + TEMP, v + TWO, work));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + A, v + B));
        CBF_TRY(chali_bigfloat_round(v + TEMP, v + TEMP, work));
        CBF_TRY(chali_bigfloat_sqrt(v + NEXT_B, v + TEMP, work));
        CBF_TRY(chali_bigfloat_add(v + DELTA, v + A, v + NEXT_A, 1));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + DELTA, v + DELTA));
        CBF_TRY(chali_bigfloat_round(v + TEMP, v + TEMP, work));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + SCALE, v + TEMP));
        CBF_TRY(chali_bigfloat_add(v + T, v + T, v + TEMP, 1));
        CBF_TRY(chali_bigfloat_round(v + T, v + T, work));
        CBF_TRY(cbf_compare_work(v + NEXT_A, v + A, &order));
        if (!order)
        {
            CBF_TRY(cbf_compare_work(v + NEXT_B, v + B, &order));
            if (!order)
            {
                CBF_TRY(chali_bigfloat_add(v + TEMP, v + NEXT_A, v + NEXT_B, 0));
                CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + TEMP, v + TEMP));
                CBF_TRY(chali_bigfloat_multiply(v + DENOM, v + FOUR, v + T));
                CBF_TRY(chali_bigfloat_divide(v + RESULT, v + TEMP, v + DENOM, work));
                CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
                cbf_move(output, v + RESULT);
                goto cleanup;
            }
        }
        cbf_move(v + A, v + NEXT_A);
        cbf_move(v + B, v + NEXT_B);
        CBF_TRY(chali_bigfloat_multiply(v + SCALE, v + SCALE, v + TWO));
    }
    status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_CONVERGENCE, CHALI_C_DOMAIN_ERROR,
                           "高精度圆周率迭代未收敛");
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 参数约化后求 Taylor 级数，再重复平方恢复自然指数。
 * @param output 目标。
 * @param input 实数输入。
 * @param digits 结果有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_exp(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{
    enum
    {
        REDUCED,
        ONE,
        TWO,
        LIMIT,
        THRESHOLD,
        TERM,
        SUM,
        DENOM,
        NEXT,
        TEMP,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int work, reductions = 0, index, order, converged = 0;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    status = chali_bigfloat_work_precision(digits, 16, &work, error);
    if (status != CHALI_C_OK) return status;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    if (!input->mantissa.sign)
    {
        cbf_move(output, v + ONE);
        goto cleanup;
    }
    CBF_TRY(chali_bigfloat_sign_change(v + REDUCED, input, 1));
    CBF_TRY(cbf_constant_work(v + LIMIT, "4944763830"));
    CBF_TRY(cbf_compare_work(v + REDUCED, v + LIMIT, &order));
    if (order > 0)
    {
        status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_OVERFLOW, CHALI_C_RANGE_ERROR,
                               "自然指数结果超出十进制指数范围");
        goto cleanup;
    }
    CBF_TRY(cbf_integer_work(v + TWO, 2));
    CBF_TRY(cbf_constant_work(v + THRESHOLD, "0.125"));
    for (;;)
    {
        CBF_TRY(cbf_compare_work(v + REDUCED, v + THRESHOLD, &order));
        if (order <= 0) break;
        CBF_TRY(chali_bigfloat_divide(v + REDUCED, v + REDUCED, v + TWO, work));
        ++reductions;
    }
    CBF_TRY(chali_bigfloat_copy(v + TERM, v + ONE));
    CBF_TRY(chali_bigfloat_copy(v + SUM, v + ONE));
    for (index = 1; index <= work * 2 + 64; ++index)
    {
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + TERM, v + REDUCED));
        CBF_TRY(cbf_integer_work(v + DENOM, index));
        CBF_TRY(chali_bigfloat_divide(v + TERM, v + TEMP, v + DENOM, work));
        CBF_TRY(chali_bigfloat_add(v + NEXT, v + SUM, v + TERM, 0));
        CBF_TRY(chali_bigfloat_round(v + NEXT, v + NEXT, work));
        CBF_TRY(cbf_compare_work(v + NEXT, v + SUM, &order));
        if (!order)
        {
            converged = 1;
            break;
        }
        cbf_move(v + SUM, v + NEXT);
    }
    if (!converged)
    {
        status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_CONVERGENCE, CHALI_C_DOMAIN_ERROR,
                               "高精度指数级数未收敛");
        goto cleanup;
    }
    while (reductions-- > 0)
    {
        CBF_TRY(chali_bigfloat_multiply(v + SUM, v + SUM, v + SUM));
        CBF_TRY(chali_bigfloat_round(v + SUM, v + SUM, work));
    }
    if (input->mantissa.sign < 0) CBF_TRY(chali_bigfloat_divide(v + SUM, v + ONE, v + SUM, work));
    CBF_TRY(chali_bigfloat_round(v + RESULT, v + SUM, digits));
    cbf_move(output, v + RESULT);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
