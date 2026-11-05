/**
 * @file chali_bigint_schoolbook.c
 * @brief 面向小整数与平方的 C 学校乘法基线。
 */
#include "chali_bigint_internal.h"

/**
 * @brief 逐行累积原生分块乘积，跳过全零行。
 * @param output 独立工作目标。
 * @param left 左幅值。
 * @param right 右幅值。
 * @return 操作状态。
 */
ChaliCStatus cbn_schoolbook_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    const ChaliBigInt *outer = left->limbs.length <= right->limbs.length ? left : right;
    const ChaliBigInt *inner = outer == left ? right : left;
    size_t i, j;
    ChaliCStatus status;
    if (!outer->limbs.length) return CHALI_C_OK;
    if (left->limbs.length > SIZE_MAX - right->limbs.length) return CHALI_C_RANGE_ERROR;
    status = chali_c_buffer_resize(&output->limbs, left->limbs.length + right->limbs.length);
    if (status != CHALI_C_OK) return status;
    memset(output->limbs.data, 0, output->limbs.length * sizeof(uint32_t));
    for (i = 0; i < outer->limbs.length; ++i)
    {
        const uint32_t a = cbn_const_words(outer)[i];
        uint64_t carry = 0;
        if (!a) continue;
        for (j = 0; j < inner->limbs.length; ++j)
        {
            const uint64_t sum = (uint64_t)a * cbn_const_words(inner)[j] + cbn_words(output)[i + j] + carry;
            cbn_words(output)[i + j] = (uint32_t)sum;
            carry = sum >> 32;
        }
        cbn_words(output)[i + j] = (uint32_t)carry;
    }
    output->sign = 1;
    cbn_normalize(output);
    return CHALI_C_OK;
}
/**
 * @brief 对角平方先除二，交叉积只累积一次，最后整体倍增恢复奇偶位。
 * @param output 独立工作目标。
 * @param input 输入幅值。
 * @return 操作状态。
 */
ChaliCStatus cbn_schoolbook_square(ChaliBigInt *output, const ChaliBigInt *input)
{
    size_t i, j, count = input->limbs.length;
    uint32_t bit = 0;
    ChaliCStatus status;
    if (!count) return CHALI_C_OK;
    if (count > SIZE_MAX / 2) return CHALI_C_RANGE_ERROR;
    status = chali_c_buffer_resize(&output->limbs, count * 2);
    if (status != CHALI_C_OK) return status;
    for (i = 0; i < count; ++i)
    {
        const uint64_t product = (uint64_t)cbn_const_words(input)[i] * cbn_const_words(input)[i];
        cbn_words(output)[i * 2] = (uint32_t)product;
        cbn_words(output)[i * 2 + 1] = (uint32_t)(product >> 32);
    }
    for (i = count * 2; i-- > 0;)
    {
        const uint32_t word = cbn_words(output)[i];
        cbn_words(output)[i] = (word >> 1) | (bit << 31);
        bit = word & 1;
    }
    for (i = 0; i + 1 < count; ++i)
    {
        uint64_t carry = 0;
        const uint32_t a = cbn_const_words(input)[i];
        if (!a) continue;
        for (j = i + 1; j < count; ++j)
        {
            const uint64_t sum = (uint64_t)a * cbn_const_words(input)[j] + cbn_words(output)[i + j] + carry;
            cbn_words(output)[i + j] = (uint32_t)sum;
            carry = sum >> 32;
        }
        j += i;
        while (carry)
        {
            uint64_t sum;
            if (j >= output->limbs.length) return CHALI_C_RANGE_ERROR;
            sum = (uint64_t)cbn_words(output)[j] + carry;
            cbn_words(output)[j++] = (uint32_t)sum;
            carry = sum >> 32;
        }
    }
    bit = cbn_const_words(input)[0] & 1;
    for (i = 0; i < count * 2; ++i)
    {
        const uint32_t word = cbn_words(output)[i];
        cbn_words(output)[i] = (word << 1) | bit;
        bit = word >> 31;
    }
    if (bit) return CHALI_C_RANGE_ERROR;
    output->sign = 1;
    cbn_normalize(output);
    return CHALI_C_OK;
}
