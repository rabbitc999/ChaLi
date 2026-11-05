/**
 * @file chali_bigint_bits.c
 * @brief 保持原非负按位运算与带符号幅值位修改的 C 实现。
 */
#include "chali_bigint_internal.h"
/**
 * @brief 按位与、或、异或，负数输入返回定义域错误。
 * @param output 目标。
 * @param left 左值。
 * @param right 右值。
 * @param operation 零为与，一为或，二为异或。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_bitwise(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right, int operation)
{
    if (!output || !left || !right || operation < 0 || operation > 2) return CHALI_C_INVALID_ARGUMENT;
    if (left->sign < 0 || right->sign < 0) return CHALI_C_DOMAIN_ERROR;
    const size_t shorter = left->limbs.length < right->limbs.length ? left->limbs.length : right->limbs.length;
    const size_t longer = left->limbs.length > right->limbs.length ? left->limbs.length : right->limbs.length;
    const size_t count = operation == 0 ? shorter : longer;
    ChaliBigInt temporary;
    cbn_temporary(&temporary, output);
    ChaliCStatus status = chali_c_buffer_resize(&temporary.limbs, count);
    if (status == CHALI_C_OK)
    {
        for (size_t index = 0; index < count; ++index)
        {
            const uint32_t a = index < left->limbs.length ? cbn_const_words(left)[index] : 0;
            const uint32_t b = index < right->limbs.length ? cbn_const_words(right)[index] : 0;
            cbn_words(&temporary)[index] = operation == 0 ? a & b : operation == 1 ? a | b :
                                                                                     a ^ b;
        }
        temporary.sign = 1;
        cbn_normalize(&temporary);
        cbn_move(output, &temporary);
    }
    chali_bigint_clear(&temporary);
    return status;
}
/**
 * @brief 读取幅值的一位。
 * @param value 数值。
 * @param bit 位下标。
 * @return 该位为一时返回一。
 */
int chali_bigint_bit_at(const ChaliBigInt *value, size_t bit)
{
    if (!value) return 0;
    const size_t index = bit / 32;
    return index < value->limbs.length && ((cbn_const_words(value)[index] >> (bit % 32)) & 1);
}
/**
 * @brief 设置、清除或翻转幅值位，并保持原符号规则。
 * @param value 数值。
 * @param bit 位下标。
 * @param operation 零为清除，一为设置，二为翻转。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_change_bit(ChaliBigInt *value, size_t bit, int operation)
{
    if (!value || operation < 0 || operation > 2) return CHALI_C_INVALID_ARGUMENT;
    const size_t index = bit / 32;
    if (index >= value->limbs.length)
    {
        if (!operation) return CHALI_C_OK;
        ChaliCStatus status = chali_c_buffer_resize(&value->limbs, index + 1);
        if (status != CHALI_C_OK) return status;
    }
    const uint32_t mask = UINT32_C(1) << (bit % 32);
    if (operation == 0)
        cbn_words(value)[index] &= ~mask;
    else if (operation == 1)
        cbn_words(value)[index] |= mask;
    else
        cbn_words(value)[index] ^= mask;
    cbn_normalize(value);
    if (value->limbs.length && !value->sign) value->sign = 1;
    return CHALI_C_OK;
}
