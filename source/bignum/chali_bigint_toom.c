/**
 * @file chali_bigint_toom.c
 * @brief 三路 Toom-Cook：五个递归乘积，复用工作缓冲区和线性时间精确除法。
 *
 * 分层思路参照 OpenJDK BigInteger；按本项目的小端分块及 C 所有者接口独立实现。
 * 以标准多项式恒等式在 0、1、-1、2、无穷处评估与插值，不引入 Java 运行时。
 */
#include "chali_bigint_internal.h"
#define TOOM_TRY(expression)                    \
    do                                          \
    {                                           \
        status = (expression);                  \
        if (status != CHALI_C_OK) goto cleanup; \
    } while (0)

/**
 * @brief 在隔离工作值上按符号加减，允许与输入同对象并复用容量。
 * @param output 工作目标。
 * @param left 左工作值。
 * @param right 右工作值。
 * @param subtract 非零表示减法。
 * @return 操作状态；分配失败保持工作值不变。
 */
static ChaliCStatus toom_add_work(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right, int subtract)
{
    const size_t left_count = left->limbs.length, right_count = right->limbs.length;
    const int left_sign = left->sign, right_sign = subtract ? -right->sign : right->sign;
    size_t i, count;
    ChaliCStatus status;
    if (!left_sign && !right_sign)
    {
        output->limbs.length = 0;
        output->sign = 0;
        return CHALI_C_OK;
    }
    if (left_sign == right_sign)
    {
        uint64_t carry = 0;
        count = left_count > right_count ? left_count : right_count;
        if (count == SIZE_MAX) return CHALI_C_RANGE_ERROR;
        status = chali_c_buffer_resize(&output->limbs, count + 1);
        if (status != CHALI_C_OK) return status;
        for (i = 0; i < count; ++i)
        {
            const uint64_t a = i < left_count ? cbn_const_words(left)[i] : 0;
            const uint64_t b = i < right_count ? cbn_const_words(right)[i] : 0;
            const uint64_t sum = a + b + carry;
            cbn_words(output)[i] = (uint32_t)sum;
            carry = sum >> 32;
        }
        cbn_words(output)[count] = (uint32_t)carry;
        output->sign = left_sign;
    }
    else
    {
        const int order = cbn_compare_magnitude(left, right);
        const ChaliBigInt *large = order >= 0 ? left : right;
        const ChaliBigInt *small = order >= 0 ? right : left;
        const size_t small_count = order >= 0 ? right_count : left_count;
        const int result_sign = order >= 0 ? left_sign : right_sign;
        uint64_t borrow = 0;
        count = order >= 0 ? left_count : right_count;
        status = chali_c_buffer_resize(&output->limbs, count);
        if (status != CHALI_C_OK) return status;
        for (i = 0; i < count; ++i)
        {
            const uint64_t a = cbn_const_words(large)[i];
            const uint64_t b = (i < small_count ? cbn_const_words(small)[i] : 0) + borrow;
            cbn_words(output)[i] = (uint32_t)(a - b);
            borrow = a < b;
        }
        output->sign = result_sign;
    }
    cbn_normalize(output);
    return CHALI_C_OK;
}
/**
 * @brief 复制幅值的连续小端切片并去掉高位零。
 * @param output 工作目标。
 * @param input 输入幅值。
 * @param offset 起始分块。
 * @param width 最大分块数。
 * @return 操作状态。
 */
static ChaliCStatus toom_slice(ChaliBigInt *output, const ChaliBigInt *input, size_t offset, size_t width)
{
    size_t count;
    ChaliCStatus status;
    if (offset >= input->limbs.length) return CHALI_C_OK;
    count = input->limbs.length - offset;
    if (count > width) count = width;
    status = chali_c_buffer_resize(&output->limbs, count);
    if (status != CHALI_C_OK) return status;
    memcpy(output->limbs.data, cbn_const_words(input) + offset, count * sizeof(uint32_t));
    output->sign = 1;
    cbn_normalize(output);
    return CHALI_C_OK;
}
/**
 * @brief 用复用的工作缓冲在正负一或二处评估二次多项式。
 * @param output 工作目标。
 * @param low 常数项。
 * @param middle 一次项。
 * @param high 二次项。
 * @param point 评估点，必须是一、负一或二。
 * @return 操作状态。
 */
static ChaliCStatus toom_evaluate(ChaliBigInt *output, const ChaliBigInt *low,
                                  const ChaliBigInt *middle, const ChaliBigInt *high, int point)
{
    ChaliCStatus status = chali_bigint_copy(output, high);
    if (status != CHALI_C_OK) return status;
    if (point == 2)
    {
        status = chali_bigint_shift_left(output, 1);
        if (status != CHALI_C_OK) return status;
        status = toom_add_work(output, output, middle, 0);
        if (status != CHALI_C_OK) return status;
        status = chali_bigint_shift_left(output, 1);
        if (status != CHALI_C_OK) return status;
        return toom_add_work(output, output, low, 0);
    }
    status = toom_add_work(output, output, low, 0);
    if (status != CHALI_C_OK) return status;
    return toom_add_work(output, output, middle, point < 0);
}
/**
 * @brief 用三在模 2^32 下的逆元逐块进行线性时间精确除三。
 * @param value 非负且可被三整除的工作值。
 * @return 精确整除时成功，否则报告内部定义域错误。
 */
static ChaliCStatus toom_divide_exact_three(ChaliBigInt *value)
{
    uint64_t carry = 0;
    size_t i;
    if (value->sign < 0) return CHALI_C_DOMAIN_ERROR;
    for (i = 0; i < value->limbs.length; ++i)
    {
        const uint32_t word = cbn_words(value)[i];
        const uint32_t quotient = (uint32_t)(word - (uint32_t)carry) * UINT32_C(0xaaaaaaab);
        carry = ((uint64_t)quotient * 3 + carry) >> 32;
        cbn_words(value)[i] = quotient;
    }
    if (carry) return CHALI_C_DOMAIN_ERROR;
    cbn_normalize(value);
    return CHALI_C_OK;
}
/**
 * @brief 对非负偶数工作值执行精确除二。
 * @param value 可被二整除的工作值。
 * @return 操作状态。
 */
static ChaliCStatus toom_divide_exact_two(ChaliBigInt *value)
{
    if (value->sign < 0 || (value->limbs.length && (cbn_const_words(value)[0] & 1))) return CHALI_C_DOMAIN_ERROR;
    chali_bigint_shift_right(value, 1);
    return CHALI_C_OK;
}
/**
 * @brief 将非负多项式系数移至指定分块位置并累加。
 * @param output 已预留足够长度并清零的目标。
 * @param coefficient 非负系数。
 * @param offset 小端分块偏移。
 * @return 操作状态。
 */
static ChaliCStatus toom_accumulate(ChaliBigInt *output, const ChaliBigInt *coefficient, size_t offset)
{
    size_t i;
    uint64_t carry = 0;
    if (!coefficient->sign) return CHALI_C_OK;
    if (coefficient->sign < 0) return CHALI_C_DOMAIN_ERROR;
    if (offset > output->limbs.length || coefficient->limbs.length > output->limbs.length - offset)
        return CHALI_C_RANGE_ERROR;
    for (i = 0; i < coefficient->limbs.length; ++i)
    {
        const uint64_t sum = (uint64_t)cbn_words(output)[offset + i] + cbn_const_words(coefficient)[i] + carry;
        cbn_words(output)[offset + i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    while (carry)
    {
        uint64_t sum;
        if (offset + i >= output->limbs.length) return CHALI_C_RANGE_ERROR;
        sum = (uint64_t)cbn_words(output)[offset + i] + carry;
        cbn_words(output)[offset + i++] = (uint32_t)sum;
        carry = sum >> 32;
    }
    return CHALI_C_OK;
}
/**
 * @brief 以五个递归乘积计算三路多项式卷积，并用独立恒等式插值。
 * @param output 独立工作目标。
 * @param left 左幅值。
 * @param right 右幅值。
 * @param square 是否可复用相同输入的切片与评估。
 * @return 操作状态；调用方仅在成功时发布结果。
 */
ChaliCStatus cbn_toom3_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right, int square)
{
    enum
    {
        A0,
        A1,
        A2,
        B0,
        B1,
        B2,
        P0,
        P1,
        PM1,
        P2,
        PINF,
        EA,
        EB,
        SUM,
        C2,
        TEMP,
        C3,
        C1,
        COUNT
    };
    ChaliBigInt v[COUNT];
    const ChaliBigInt *b0, *b1, *b2;
    const size_t largest = left->limbs.length > right->limbs.length ? left->limbs.length : right->limbs.length;
    size_t width, i;
    ChaliCStatus status = CHALI_C_OK;
    if (!left->limbs.length || !right->limbs.length) return CHALI_C_OK;
    if (largest > SIZE_MAX - 2 || left->limbs.length > SIZE_MAX - right->limbs.length - 1)
        return CHALI_C_RANGE_ERROR;
    width = (largest + 2) / 3;
    if (width > SIZE_MAX / 4) return CHALI_C_RANGE_ERROR;
    for (i = 0; i < COUNT; ++i) cbn_temporary(v + i, output);
    TOOM_TRY(toom_slice(v + A0, left, 0, width));
    TOOM_TRY(toom_slice(v + A1, left, width, width));
    TOOM_TRY(toom_slice(v + A2, left, width * 2, width));
    if (square)
    {
        b0 = v + A0;
        b1 = v + A1;
        b2 = v + A2;
    }
    else
    {
        TOOM_TRY(toom_slice(v + B0, right, 0, width));
        TOOM_TRY(toom_slice(v + B1, right, width, width));
        TOOM_TRY(toom_slice(v + B2, right, width * 2, width));
        b0 = v + B0;
        b1 = v + B1;
        b2 = v + B2;
    }
    TOOM_TRY(chali_bigint_multiply(v + P0, v + A0, b0));
    TOOM_TRY(chali_bigint_multiply(v + PINF, v + A2, b2));
    for (i = 0; i < 3; ++i)
    {
        const int point = i == 0 ? 1 : (i == 1 ? -1 : 2);
        const int target = i == 0 ? P1 : (i == 1 ? PM1 : P2);
        TOOM_TRY(toom_evaluate(v + EA, v + A0, v + A1, v + A2, point));
        if (!square) TOOM_TRY(toom_evaluate(v + EB, b0, b1, b2, point));
        TOOM_TRY(chali_bigint_multiply(v + target, v + EA, square ? v + EA : v + EB));
    }
    for (i = A0; i <= B2; ++i) chali_bigint_clear(v + i);
    chali_bigint_clear(v + EA);
    chali_bigint_clear(v + EB);
    /* s=(p(1)-p(-1))/2=c1+c3；c2=(p(1)+p(-1))/2-c0-c4。 */
    TOOM_TRY(toom_add_work(v + SUM, v + P1, v + PM1, 1));
    TOOM_TRY(toom_divide_exact_two(v + SUM));
    TOOM_TRY(toom_add_work(v + C2, v + P1, v + PM1, 0));
    TOOM_TRY(toom_divide_exact_two(v + C2));
    TOOM_TRY(toom_add_work(v + C2, v + C2, v + P0, 1));
    TOOM_TRY(toom_add_work(v + C2, v + C2, v + PINF, 1));
    /* t=(p(2)-c0-16*c4)/2-2*c2=c1+4*c3；c3=(t-s)/3。 */
    TOOM_TRY(chali_bigint_copy(v + TEMP, v + PINF));
    TOOM_TRY(chali_bigint_shift_left(v + TEMP, 4));
    TOOM_TRY(toom_add_work(v + C3, v + P2, v + P0, 1));
    TOOM_TRY(toom_add_work(v + C3, v + C3, v + TEMP, 1));
    TOOM_TRY(toom_divide_exact_two(v + C3));
    TOOM_TRY(chali_bigint_copy(v + TEMP, v + C2));
    TOOM_TRY(chali_bigint_shift_left(v + TEMP, 1));
    TOOM_TRY(toom_add_work(v + C3, v + C3, v + TEMP, 1));
    TOOM_TRY(toom_add_work(v + C3, v + C3, v + SUM, 1));
    TOOM_TRY(toom_divide_exact_three(v + C3));
    TOOM_TRY(toom_add_work(v + C1, v + SUM, v + C3, 1));
    TOOM_TRY(chali_c_buffer_resize(&output->limbs, left->limbs.length + right->limbs.length + 1));
    memset(output->limbs.data, 0, output->limbs.length * sizeof(uint32_t));
    TOOM_TRY(toom_accumulate(output, v + P0, 0));
    TOOM_TRY(toom_accumulate(output, v + C1, width));
    TOOM_TRY(toom_accumulate(output, v + C2, width * 2));
    TOOM_TRY(toom_accumulate(output, v + C3, width * 3));
    TOOM_TRY(toom_accumulate(output, v + PINF, width * 4));
    output->sign = 1;
    cbn_normalize(output);
cleanup:
    for (i = 0; i < COUNT; ++i) chali_bigint_clear(v + i);
    return status;
}
