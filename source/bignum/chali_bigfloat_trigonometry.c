/**
 * @file chali_bigfloat_trigonometry.c
 * @brief 保留原参数约化和级数的 C 三角函数内核。
 */
#include "chali_bigfloat_math_internal.h"

/**
 * @brief 为大角度约化及特殊角消去保留原有额外位数。
 * @param value 输入角度。
 * @param digits 请求位数。
 * @param work 工作位数输出。
 * @param allocator 工作区分配接口。
 * @param error 可选诊断。
 * @return 操作状态。
 */
static ChaliCStatus cbf_trig_precision(const ChaliBigFloat *value, int digits, int *work,
                                       const ChaliCAllocator *allocator, ChaliBigFloatError *error)
{
    int64_t order, count;
    ChaliCStatus status;
    cbf_math_reset(error);
    status = chali_bigfloat_decimal_order(value, &order, allocator);
    if (status != CHALI_C_OK) return status;
    status = cbf_digits(&value->mantissa, &count, allocator);
    if (status != CHALI_C_OK) return status;
    if (order < 0) order = 0;
    if (count > INT64_MAX - order)
        return cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_PRECISION, CHALI_C_RANGE_ERROR,
                             "高精度数学参数约化超出工作位数预算");
    return chali_bigfloat_work_precision(digits, order + count, work, error);
}
/**
 * @brief 先以圆周率约化，再用级数和倍角同时计算正弦及余弦。
 * @param sine_output 正弦目标，与余弦目标不同。
 * @param cosine_output 余弦目标。
 * @param input 输入弧度角。
 * @param work 工作位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
static ChaliCStatus cbf_sin_cos_working(ChaliBigFloat *sine_output, ChaliBigFloat *cosine_output,
                                        const ChaliBigFloat *input, int work, ChaliBigFloatError *error)
{
    enum
    {
        CIRCLE,
        PERIOD,
        QUOTIENT,
        REDUCED,
        ONE,
        TWO,
        THRESHOLD,
        SQUARE,
        SINE,
        COSINE,
        SINE_TERM,
        COSINE_TERM,
        NEXT_SINE,
        NEXT_COSINE,
        DENOM,
        TEMP,
        TEMP2,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int reductions = 0, index, order, converged = 0;
    cbf_temporaries(v, COUNT, &sine_output->mantissa.limbs.allocator);
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    if (!input->mantissa.sign)
    {
        cbf_move(sine_output, v + SINE);
        cbf_move(cosine_output, v + ONE);
        goto cleanup;
    }
    CBF_TRY(cbf_integer_work(v + TWO, 2));
    CBF_TRY(chali_bigfloat_pi(v + CIRCLE, work, error));
    CBF_TRY(chali_bigfloat_multiply(v + PERIOD, v + CIRCLE, v + TWO));
    CBF_TRY(chali_bigfloat_divide(v + TEMP, input, v + PERIOD, work));
    CBF_TRY(chali_bigfloat_to_integer(&v[QUOTIENT].mantissa, v + TEMP, 0));
    CBF_TRY(cbf_normalize_work(v + QUOTIENT));
    CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + QUOTIENT, v + PERIOD));
    CBF_TRY(chali_bigfloat_add(v + REDUCED, input, v + TEMP, 1));
    CBF_TRY(cbf_compare_work(v + REDUCED, v + CIRCLE, &order));
    if (order > 0) CBF_TRY(chali_bigfloat_add(v + REDUCED, v + REDUCED, v + PERIOD, 1));
    CBF_TRY(chali_bigfloat_sign_change(v + TEMP, v + CIRCLE, 0));
    CBF_TRY(cbf_compare_work(v + REDUCED, v + TEMP, &order));
    if (order < 0) CBF_TRY(chali_bigfloat_add(v + REDUCED, v + REDUCED, v + PERIOD, 0));
    CBF_TRY(cbf_constant_work(v + THRESHOLD, "0.125"));
    for (;;)
    {
        CBF_TRY(chali_bigfloat_sign_change(v + TEMP, v + REDUCED, 1));
        CBF_TRY(cbf_compare_work(v + TEMP, v + THRESHOLD, &order));
        if (order <= 0) break;
        CBF_TRY(chali_bigfloat_divide(v + REDUCED, v + REDUCED, v + TWO, work));
        ++reductions;
    }
    CBF_TRY(chali_bigfloat_multiply(v + SQUARE, v + REDUCED, v + REDUCED));
    CBF_TRY(chali_bigfloat_round(v + SQUARE, v + SQUARE, work));
    CBF_TRY(chali_bigfloat_copy(v + SINE, v + REDUCED));
    CBF_TRY(chali_bigfloat_copy(v + SINE_TERM, v + REDUCED));
    CBF_TRY(chali_bigfloat_copy(v + COSINE, v + ONE));
    CBF_TRY(chali_bigfloat_copy(v + COSINE_TERM, v + ONE));
    for (index = 1; index <= work * 2 + 64; ++index)
    {
        const int64_t twice = (int64_t)index * 2;
        CBF_TRY(chali_bigfloat_sign_change(v + TEMP, v + SINE_TERM, 0));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + TEMP, v + SQUARE));
        CBF_TRY(cbf_integer_work(v + DENOM, twice * (twice + 1)));
        CBF_TRY(chali_bigfloat_divide(v + SINE_TERM, v + TEMP, v + DENOM, work));
        CBF_TRY(chali_bigfloat_sign_change(v + TEMP, v + COSINE_TERM, 0));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + TEMP, v + SQUARE));
        CBF_TRY(cbf_integer_work(v + DENOM, (twice - 1) * twice));
        CBF_TRY(chali_bigfloat_divide(v + COSINE_TERM, v + TEMP, v + DENOM, work));
        CBF_TRY(chali_bigfloat_add(v + NEXT_SINE, v + SINE, v + SINE_TERM, 0));
        CBF_TRY(chali_bigfloat_round(v + NEXT_SINE, v + NEXT_SINE, work));
        CBF_TRY(chali_bigfloat_add(v + NEXT_COSINE, v + COSINE, v + COSINE_TERM, 0));
        CBF_TRY(chali_bigfloat_round(v + NEXT_COSINE, v + NEXT_COSINE, work));
        CBF_TRY(cbf_compare_work(v + NEXT_SINE, v + SINE, &order));
        if (!order)
        {
            CBF_TRY(cbf_compare_work(v + NEXT_COSINE, v + COSINE, &order));
            if (!order)
            {
                converged = 1;
                break;
            }
        }
        cbf_move(v + SINE, v + NEXT_SINE);
        cbf_move(v + COSINE, v + NEXT_COSINE);
    }
    if (!converged)
    {
        status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_CONVERGENCE, CHALI_C_DOMAIN_ERROR, "高精度三角级数未收敛");
        goto cleanup;
    }
    while (reductions-- > 0)
    {
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + TWO, v + SINE));
        CBF_TRY(chali_bigfloat_multiply(v + NEXT_SINE, v + TEMP, v + COSINE));
        CBF_TRY(chali_bigfloat_round(v + NEXT_SINE, v + NEXT_SINE, work));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + COSINE, v + COSINE));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP2, v + SINE, v + SINE));
        CBF_TRY(chali_bigfloat_add(v + COSINE, v + TEMP, v + TEMP2, 1));
        CBF_TRY(chali_bigfloat_round(v + COSINE, v + COSINE, work));
        cbf_move(v + SINE, v + NEXT_SINE);
    }
    cbf_move(sine_output, v + SINE);
    cbf_move(cosine_output, v + COSINE);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 用倒数、半角约化及原交错级数计算工作精度反正切。
 * @param output 目标。
 * @param input 实数输入。
 * @param work 工作位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
static ChaliCStatus cbf_atan_working(ChaliBigFloat *output, const ChaliBigFloat *input, int work, ChaliBigFloatError *error)
{
    enum
    {
        ABS_INPUT,
        REDUCED,
        ONE,
        TWO,
        THRESHOLD,
        SQUARE,
        TERM,
        SUM,
        DENOM,
        NEXT,
        TEMP,
        SCALE,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status = CHALI_C_OK;
    int inverse, reductions = 0, index, order, converged = 0;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    if (!input->mantissa.sign) goto commit;
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    CBF_TRY(cbf_integer_work(v + TWO, 2));
    CBF_TRY(cbf_constant_work(v + THRESHOLD, "0.1"));
    CBF_TRY(chali_bigfloat_sign_change(v + ABS_INPUT, input, 1));
    CBF_TRY(cbf_compare_work(v + ABS_INPUT, v + ONE, &order));
    inverse = order > 0;
    if (inverse)
        CBF_TRY(chali_bigfloat_divide(v + REDUCED, v + ONE, v + ABS_INPUT, work));
    else
        CBF_TRY(chali_bigfloat_copy(v + REDUCED, v + ABS_INPUT));
    for (;;)
    {
        CBF_TRY(cbf_compare_work(v + REDUCED, v + THRESHOLD, &order));
        if (order <= 0) break;
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + REDUCED, v + REDUCED));
        CBF_TRY(chali_bigfloat_add(v + TEMP, v + ONE, v + TEMP, 0));
        CBF_TRY(chali_bigfloat_sqrt(v + TEMP, v + TEMP, work));
        CBF_TRY(chali_bigfloat_add(v + DENOM, v + ONE, v + TEMP, 0));
        CBF_TRY(chali_bigfloat_divide(v + REDUCED, v + REDUCED, v + DENOM, work));
        ++reductions;
    }
    CBF_TRY(chali_bigfloat_multiply(v + SQUARE, v + REDUCED, v + REDUCED));
    CBF_TRY(chali_bigfloat_round(v + SQUARE, v + SQUARE, work));
    CBF_TRY(chali_bigfloat_copy(v + TERM, v + REDUCED));
    CBF_TRY(chali_bigfloat_copy(v + SUM, v + REDUCED));
    for (index = 1; index <= work * 2 + 64; ++index)
    {
        CBF_TRY(chali_bigfloat_sign_change(v + TEMP, v + TERM, 0));
        CBF_TRY(chali_bigfloat_multiply(v + TERM, v + TEMP, v + SQUARE));
        CBF_TRY(chali_bigfloat_round(v + TERM, v + TERM, work));
        CBF_TRY(cbf_integer_work(v + DENOM, (int64_t)index * 2 + 1));
        CBF_TRY(chali_bigfloat_divide(v + TEMP, v + TERM, v + DENOM, work));
        CBF_TRY(chali_bigfloat_add(v + NEXT, v + SUM, v + TEMP, 0));
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
        status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_CONVERGENCE, CHALI_C_DOMAIN_ERROR, "高精度反正切级数未收敛");
        goto cleanup;
    }
    CBF_TRY(cbf_integer_work(v + SCALE, 2));
    CBF_TRY(chali_bigfloat_pow_integer(v + SCALE, v + SCALE, (uint64_t)reductions));
    CBF_TRY(chali_bigfloat_multiply(v + SUM, v + SUM, v + SCALE));
    CBF_TRY(chali_bigfloat_round(v + SUM, v + SUM, work));
    if (inverse)
    {
        CBF_TRY(chali_bigfloat_pi(v + TEMP, work, error));
        CBF_TRY(chali_bigfloat_divide(v + TEMP, v + TEMP, v + TWO, work));
        CBF_TRY(chali_bigfloat_add(v + SUM, v + TEMP, v + SUM, 1));
    }
    if (input->mantissa.sign < 0)
        CBF_TRY(chali_bigfloat_sign_change(v + RESULT, v + SUM, 0));
    else
        cbf_move(v + RESULT, v + SUM);
commit:
    cbf_move(output, v + RESULT);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 计算带原有消去保护位的正弦、余弦或正切。
 * @param output 目标。
 * @param input 弧度输入。
 * @param digits 结果位数。
 * @param operation 零正弦、一余弦、二正切。
 * @param error 可选诊断。
 * @return 操作状态。
 */
static ChaliCStatus cbf_trig_result(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, int operation,
                                    ChaliBigFloatError *error)
{
    enum
    {
        SINE,
        COSINE,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int work;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    status = cbf_trig_precision(input, digits, &work, &output->mantissa.limbs.allocator, error);
    if (status != CHALI_C_OK) return status;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(cbf_sin_cos_working(v + SINE, v + COSINE, input, work, error));
    if (operation == 2)
    {
        if (!v[COSINE].mantissa.sign)
        {
            status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR, "正切在当前输入处未定义");
            goto cleanup;
        }
        CBF_TRY(chali_bigfloat_divide(v + RESULT, v + SINE, v + COSINE, work));
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
    }
    else
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + (operation == 0 ? SINE : COSINE), digits));
    cbf_move(output, v + RESULT);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 计算高精度正弦。
 * @param output 目标。
 * @param input 弧度输入。
 * @param digits 有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_sin(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{ return cbf_trig_result(output, input, digits, 0, error); }
/**
 * @brief 计算高精度余弦。
 * @param output 目标。
 * @param input 弧度输入。
 * @param digits 有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_cos(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{ return cbf_trig_result(output, input, digits, 1, error); }
/**
 * @brief 计算高精度正切。
 * @param output 目标。
 * @param input 弧度输入。
 * @param digits 有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_tan(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{ return cbf_trig_result(output, input, digits, 2, error); }
/**
 * @brief 计算反正切主值。
 * @param output 目标。
 * @param input 实数输入。
 * @param digits 有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_atan(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{
    ChaliBigFloat work_value;
    ChaliCStatus status;
    int work;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    status = chali_bigfloat_work_precision(digits, 0, &work, error);
    if (status != CHALI_C_OK) return status;
    chali_bigfloat_init(&work_value, &output->mantissa.limbs.allocator);
    CBF_TRY(cbf_atan_working(&work_value, input, work, error));
    CBF_TRY(chali_bigfloat_round(&work_value, &work_value, digits));
    cbf_move(output, &work_value);
cleanup:
    chali_bigfloat_clear(&work_value);
    return status;
}
/**
 * @brief 用平方根与反正切求反正弦，并保留端点附近的消去位数。
 * @param output 目标。
 * @param input 输入。
 * @param digits 有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_asin(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{
    enum
    {
        ABS_INPUT,
        ONE,
        TWO,
        DENOMINATOR,
        RATIO,
        TEMP,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int64_t count;
    int work, order;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    cbf_math_reset(error);
    status = cbf_digits(&input->mantissa, &count, &output->mantissa.limbs.allocator);
    if (status != CHALI_C_OK) return status;
    status = chali_bigfloat_work_precision(digits, count, &work, error);
    if (status != CHALI_C_OK) return status;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    CBF_TRY(chali_bigfloat_sign_change(v + ABS_INPUT, input, 1));
    CBF_TRY(cbf_compare_work(v + ABS_INPUT, v + ONE, &order));
    if (order > 0)
    {
        status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR, "反正弦要求输入位于 [-1,1]");
        goto cleanup;
    }
    if (!order)
    {
        CBF_TRY(chali_bigfloat_pi(v + TEMP, work, error));
        CBF_TRY(cbf_integer_work(v + TWO, 2));
        CBF_TRY(chali_bigfloat_divide(v + RESULT, v + TEMP, v + TWO, work));
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
        if (input->mantissa.sign < 0) CBF_TRY(chali_bigfloat_sign_change(v + RESULT, v + RESULT, 0));
    }
    else
    {
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, input, input));
        CBF_TRY(chali_bigfloat_add(v + TEMP, v + ONE, v + TEMP, 1));
        CBF_TRY(chali_bigfloat_sqrt(v + DENOMINATOR, v + TEMP, work));
        CBF_TRY(chali_bigfloat_divide(v + RATIO, input, v + DENOMINATOR, work));
        CBF_TRY(cbf_atan_working(v + RESULT, v + RATIO, work, error));
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
    }
    cbf_move(output, v + RESULT);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 用原半角恒等式计算反余弦，避免一附近的消去误差。
 * @param output 目标。
 * @param input 输入。
 * @param digits 有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_acos(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{
    enum
    {
        ABS_INPUT,
        ONE,
        TWO,
        NUMERATOR,
        DENOMINATOR,
        RATIO,
        TEMP,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int64_t count;
    int work, order;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    cbf_math_reset(error);
    status = cbf_digits(&input->mantissa, &count, &output->mantissa.limbs.allocator);
    if (status != CHALI_C_OK) return status;
    status = chali_bigfloat_work_precision(digits, count, &work, error);
    if (status != CHALI_C_OK) return status;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    CBF_TRY(chali_bigfloat_sign_change(v + ABS_INPUT, input, 1));
    CBF_TRY(cbf_compare_work(v + ABS_INPUT, v + ONE, &order));
    if (order > 0)
    {
        status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR, "反余弦要求输入位于 [-1,1]");
        goto cleanup;
    }
    if (!order)
    {
        if (input->mantissa.sign < 0)
        {
            CBF_TRY(chali_bigfloat_pi(v + RESULT, work, error));
            CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
        }
    }
    else
    {
        CBF_TRY(chali_bigfloat_add(v + NUMERATOR, v + ONE, input, 1));
        CBF_TRY(chali_bigfloat_add(v + DENOMINATOR, v + ONE, input, 0));
        CBF_TRY(chali_bigfloat_divide(v + RATIO, v + NUMERATOR, v + DENOMINATOR, work));
        CBF_TRY(chali_bigfloat_sqrt(v + RATIO, v + RATIO, work));
        CBF_TRY(cbf_atan_working(v + TEMP, v + RATIO, work, error));
        CBF_TRY(cbf_integer_work(v + TWO, 2));
        CBF_TRY(chali_bigfloat_multiply(v + RESULT, v + TWO, v + TEMP));
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
    }
    cbf_move(output, v + RESULT);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
