/**
 * @file chali_bigfloat_logarithm.c
 * @brief 十进制数量级对数、实数幂和精确余数的 C 服务。
 */
#include "chali_bigfloat_math_internal.h"

/**
 * @brief 分离十进制数量级，复用原对数级数。
 * @param output 目标。
 * @param input 正数。
 * @param digits 结果位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_ln(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{
    enum
    {
        COEFFICIENT,
        HALF,
        TWO,
        TEN,
        RESULT,
        LOG_TEN,
        SCALE,
        TEMP,
        FINAL,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int work, comparison;
    int64_t mantissa_digits, order;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    status = chali_bigfloat_work_precision(digits, 0, &work, error);
    if (status != CHALI_C_OK) return status;
    if (input->mantissa.sign <= 0)
        return cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR, "自然对数要求正数");
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(chali_bigfloat_copy(v + COEFFICIENT, input));
    CBF_TRY(cbf_constant_work(v + HALF, "0.5"));
    CBF_TRY(cbf_integer_work(v + TWO, 2));
    CBF_TRY(cbf_compare_work(v + COEFFICIENT, v + HALF, &comparison));
    if (comparison >= 0)
    {
        CBF_TRY(cbf_compare_work(v + COEFFICIENT, v + TWO, &comparison));
        if (comparison <= 0)
        {
            CBF_TRY(cbf_logarithm_series(v + RESULT, v + COEFFICIENT, work, error));
            goto rounded;
        }
    }
    CBF_TRY(cbf_digits(&input->mantissa, &mantissa_digits, &output->mantissa.limbs.allocator));
    if (mantissa_digits - 1 > INT_MAX)
    {
        status = CHALI_C_RANGE_ERROR;
        goto cleanup;
    }
    v[COEFFICIENT].exponent10 = -(int)(mantissa_digits - 1);
    CBF_TRY(cbf_normalize_work(v + COEFFICIENT));
    CBF_TRY(chali_bigfloat_decimal_order(input, &order, &output->mantissa.limbs.allocator));
    CBF_TRY(cbf_logarithm_series(v + RESULT, v + COEFFICIENT, work, error));
    if (order)
    {
        CBF_TRY(cbf_integer_work(v + TEN, 10));
        CBF_TRY(cbf_logarithm_series(v + LOG_TEN, v + TEN, work, error));
        CBF_TRY(cbf_integer_work(v + SCALE, order));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, v + SCALE, v + LOG_TEN));
        CBF_TRY(chali_bigfloat_add(v + RESULT, v + RESULT, v + TEMP, 0));
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, work));
    }
rounded:
    CBF_TRY(chali_bigfloat_round(v + FINAL, v + RESULT, digits));
    cbf_move(output, v + FINAL);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 通过自然对数比值计算常用对数，十的幂直接返回指数。
 * @param output 目标。
 * @param input 正数。
 * @param digits 有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_log10(ChaliBigFloat *output, const ChaliBigFloat *input, int digits, ChaliBigFloatError *error)
{
    enum
    {
        NUMERATOR,
        DENOMINATOR,
        TEN,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int work;
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    status = chali_bigfloat_work_precision(digits, 0, &work, error);
    if (status != CHALI_C_OK) return status;
    if (input->mantissa.sign <= 0)
        return cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR, "常用对数要求正数");
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    if (chali_bigint_bit_length(&input->mantissa) == 1)
    {
        CBF_TRY(cbf_integer_work(v + RESULT, input->exponent10));
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
    }
    else
    {
        CBF_TRY(chali_bigfloat_ln(v + NUMERATOR, input, digits + 16, error));
        CBF_TRY(cbf_integer_work(v + TEN, 10));
        CBF_TRY(cbf_logarithm_series(v + DENOMINATOR, v + TEN, work, error));
        CBF_TRY(chali_bigfloat_divide(v + RESULT, v + NUMERATOR, v + DENOMINATOR, work));
        CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, digits));
    }
    cbf_move(output, v + RESULT);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 保留定精度二进制幂与 exp/ln 两条实数幂路径。
 * @param output 目标。
 * @param input 底数。
 * @param exponent 实数指数。
 * @param digits 结果有效位数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_pow(ChaliBigFloat *output, const ChaliBigFloat *input, const ChaliBigFloat *exponent,
                                int digits, ChaliBigFloatError *error)
{
    enum
    {
        RESULT,
        BASE,
        INTEGER,
        ONE,
        TEMP,
        FINAL,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int work, integer;
    uint64_t remaining;
    if (!output || !input || !exponent) return CHALI_C_INVALID_ARGUMENT;
    status = chali_bigfloat_work_precision(digits, 0, &work, error);
    if (status != CHALI_C_OK) return status;
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(cbf_integer_work(v + ONE, 1));
    if (!exponent->mantissa.sign)
    {
        cbf_move(output, v + ONE);
        goto cleanup;
    }
    if (!input->mantissa.sign)
    {
        if (exponent->mantissa.sign < 0)
        {
            status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR, "零不能取负次幂");
            goto cleanup;
        }
        cbf_move(output, v + RESULT);
        goto cleanup;
    }
    CBF_TRY(chali_bigfloat_is_integer(exponent, &integer, &output->mantissa.limbs.allocator));
    if (integer)
    {
        CBF_TRY(chali_bigfloat_to_integer(&v[INTEGER].mantissa, exponent, 0));
        if (chali_bigint_bit_length(&v[INTEGER].mantissa) > 64)
        {
            status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_OVERFLOW, CHALI_C_RANGE_ERROR, "整数幂指数超出 64 位范围");
            goto cleanup;
        }
        remaining = chali_bigint_to_u64(&v[INTEGER].mantissa);
        CBF_TRY(chali_bigfloat_copy(v + RESULT, v + ONE));
        CBF_TRY(chali_bigfloat_round(v + BASE, input, work));
        while (remaining)
        {
            if (remaining & 1)
            {
                CBF_TRY(chali_bigfloat_multiply(v + RESULT, v + RESULT, v + BASE));
                CBF_TRY(chali_bigfloat_round(v + RESULT, v + RESULT, work));
            }
            remaining >>= 1;
            if (remaining)
            {
                CBF_TRY(chali_bigfloat_multiply(v + BASE, v + BASE, v + BASE));
                CBF_TRY(chali_bigfloat_round(v + BASE, v + BASE, work));
            }
        }
        if (exponent->mantissa.sign < 0) CBF_TRY(chali_bigfloat_divide(v + RESULT, v + ONE, v + RESULT, work));
        CBF_TRY(chali_bigfloat_round(v + FINAL, v + RESULT, digits));
    }
    else
    {
        if (input->mantissa.sign < 0)
        {
            status = cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR,
                                   "负底数的非整数次幂不属于实数");
            goto cleanup;
        }
        CBF_TRY(chali_bigfloat_ln(v + TEMP, input, digits + 8, error));
        CBF_TRY(chali_bigfloat_multiply(v + TEMP, exponent, v + TEMP));
        CBF_TRY(chali_bigfloat_round(v + TEMP, v + TEMP, work));
        CBF_TRY(chali_bigfloat_exp(v + FINAL, v + TEMP, digits, error));
    }
    cbf_move(output, v + FINAL);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
/**
 * @brief 对齐十进制指数并按截断商的余数规则计算精确模。
 * @param output 目标。
 * @param input 被除数。
 * @param divisor 非零除数。
 * @param error 可选诊断。
 * @return 操作状态。
 */
ChaliCStatus chali_bigfloat_fmod(ChaliBigFloat *output, const ChaliBigFloat *input, const ChaliBigFloat *divisor,
                                 ChaliBigFloatError *error)
{
    enum
    {
        ABS_INPUT,
        ABS_DIVISOR,
        SCALE,
        EXPONENT,
        TEN,
        NUMERATOR,
        DENOMINATOR,
        RESULT,
        COUNT
    };
    ChaliBigFloat v[COUNT];
    ChaliCStatus status;
    int comparison;
    int64_t shift;
    cbf_math_reset(error);
    if (!output || !input || !divisor) return CHALI_C_INVALID_ARGUMENT;
    if (!divisor->mantissa.sign)
        return cbf_math_fail(error, CHALI_BIGFLOAT_ERROR_DOMAIN, CHALI_C_DOMAIN_ERROR, "十进制余数的除数不能为零");
    if (!input->mantissa.sign) return chali_bigfloat_copy(output, input);
    cbf_temporaries(v, COUNT, &output->mantissa.limbs.allocator);
    CBF_TRY(chali_bigfloat_sign_change(v + ABS_INPUT, input, 1));
    CBF_TRY(chali_bigfloat_sign_change(v + ABS_DIVISOR, divisor, 1));
    CBF_TRY(cbf_compare_work(v + ABS_INPUT, v + ABS_DIVISOR, &comparison));
    if (comparison < 0)
    {
        CBF_TRY(chali_bigfloat_copy(v + RESULT, input));
        goto commit;
    }
    shift = (int64_t)input->exponent10 - divisor->exponent10;
    if (shift >= 0)
    {
        CBF_TRY(chali_bigint_set_u64(&v[TEN].mantissa, 10));
        CBF_TRY(chali_bigint_set_u64(&v[EXPONENT].mantissa, (uint64_t)shift));
        CBF_TRY(chali_bigint_powmod(&v[SCALE].mantissa, &v[TEN].mantissa, &v[EXPONENT].mantissa,
                                    &v[ABS_DIVISOR].mantissa));
        CBF_TRY(chali_bigint_multiply(&v[NUMERATOR].mantissa, &input->mantissa, &v[SCALE].mantissa));
        CBF_TRY(chali_bigint_divmod_work(NULL, &v[RESULT].mantissa, &v[NUMERATOR].mantissa, &divisor->mantissa,
                                         &output->mantissa.limbs.allocator));
        v[RESULT].exponent10 = divisor->exponent10;
    }
    else
    {
        CBF_TRY(cbf_pow10(&v[SCALE].mantissa, (uint64_t)-shift));
        CBF_TRY(chali_bigint_multiply(&v[DENOMINATOR].mantissa, &divisor->mantissa, &v[SCALE].mantissa));
        CBF_TRY(chali_bigint_divmod_work(NULL, &v[RESULT].mantissa, &input->mantissa, &v[DENOMINATOR].mantissa,
                                         &output->mantissa.limbs.allocator));
        v[RESULT].exponent10 = input->exponent10;
    }
    CBF_TRY(cbf_normalize_work(v + RESULT));
commit:
    cbf_move(output, v + RESULT);
cleanup:
    cbf_clear_temporaries(v, COUNT);
    return status;
}
