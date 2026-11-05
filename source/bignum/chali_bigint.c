/**
 * @file chali_bigint.c
 * @brief 任意精度整数的 C 存储、符号和基础算术。
 */
#include "chali_bigint_internal.h"
#include <limits.h>

/**
 * @brief 建立具有明确分配所有者的零值。
 * @param value 目标。
 * @param allocator 可选分配器。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_init(ChaliBigInt *value, const ChaliCAllocator *allocator)
{
    if (!value) return CHALI_C_INVALID_ARGUMENT;
    value->sign = 0;
    return chali_c_buffer_init(&value->limbs, sizeof(uint32_t), allocator);
}
/**
 * @brief 回收数值的独占存储。
 * @param value 已初始化整数。
 */
void chali_bigint_clear(ChaliBigInt *value)
{
    if (!value) return;
    chali_c_buffer_clear(&value->limbs);
    value->sign = 0;
}
/**
 * @brief 按记录的容量和分配所有者回收文本。
 * @param text 独占文本。
 */
void chali_big_string_clear(ChaliBigString *text)
{
    if (!text) return;
    if (text->data) text->allocator.release(text->allocator.context, text->data, text->capacity);
    text->data = NULL;
    text->length = 0;
    text->capacity = 0;
}
/**
 * @brief 复制分块，成功后发布符号。
 * @param output 目标。
 * @param input 输入。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_copy(ChaliBigInt *output, const ChaliBigInt *input)
{
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    const ChaliCStatus status = chali_c_buffer_copy(&output->limbs, &input->limbs);
    if (status == CHALI_C_OK) output->sign = input->sign;
    return status;
}
/**
 * @brief 用一至两个分块保存无符号数。
 * @param output 目标。
 * @param value 数值。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_set_u64(ChaliBigInt *output, uint64_t value)
{
    if (!output) return CHALI_C_INVALID_ARGUMENT;
    ChaliBigInt temporary;
    cbn_temporary(&temporary, output);
    const size_t count = value > UINT32_MAX ? 2 : value ? 1 :
                                                          0;
    const ChaliCStatus status = chali_c_buffer_resize(&temporary.limbs, count);
    if (status == CHALI_C_OK)
    {
        if (count) cbn_words(&temporary)[0] = (uint32_t)value;
        if (count == 2) cbn_words(&temporary)[1] = (uint32_t)(value >> 32);
        temporary.sign = count ? 1 : 0;
        cbn_move(output, &temporary);
    }
    chali_bigint_clear(&temporary);
    return status;
}
/**
 * @brief 使用无符号取负避免最小有符号整数溢出。
 * @param output 目标。
 * @param value 数值。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_set_i64(ChaliBigInt *output, int64_t value)
{
    const uint64_t magnitude = value < 0 ? ~(uint64_t)value + 1 : (uint64_t)value;
    const ChaliCStatus status = chali_bigint_set_u64(output, magnitude);
    if (status == CHALI_C_OK && value < 0) output->sign = -1;
    return status;
}
/**
 * @brief 先比较有效长度再从高位比较分块。
 * @param left 左值。
 * @param right 右值。
 * @return 幅值顺序。
 */
int cbn_compare_magnitude(const ChaliBigInt *left, const ChaliBigInt *right)
{
    if (left->limbs.length != right->limbs.length) return left->limbs.length < right->limbs.length ? -1 : 1;
    for (size_t index = left->limbs.length; index-- > 0;)
        if (cbn_const_words(left)[index] != cbn_const_words(right)[index])
            return cbn_const_words(left)[index] < cbn_const_words(right)[index] ? -1 : 1;
    return 0;
}
/**
 * @brief 结合符号比较整数。
 * @param left 左值。
 * @param right 右值。
 * @return 数值顺序。
 */
int chali_bigint_compare(const ChaliBigInt *left, const ChaliBigInt *right)
{
    if (left->sign != right->sign) return left->sign < right->sign ? -1 : 1;
    return left->sign * cbn_compare_magnitude(left, right);
}
/**
 * @brief 将两个幅值相加，输出必须与输入分离。
 * @param output 空临时目标。
 * @param left 左值。
 * @param right 右值。
 * @return 操作状态。
 */
static ChaliCStatus add_magnitude(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    const size_t count = left->limbs.length > right->limbs.length ? left->limbs.length : right->limbs.length;
    if (count == SIZE_MAX) return CHALI_C_RANGE_ERROR;
    const ChaliCStatus status = chali_c_buffer_resize(&output->limbs, count + 1);
    if (status != CHALI_C_OK) return status;
    uint64_t carry = 0;
    for (size_t index = 0; index < count; ++index)
    {
        const uint64_t a = index < left->limbs.length ? cbn_const_words(left)[index] : 0;
        const uint64_t b = index < right->limbs.length ? cbn_const_words(right)[index] : 0;
        const uint64_t sum = a + b + carry;
        cbn_words(output)[index] = (uint32_t)sum;
        carry = sum >> 32;
    }
    cbn_words(output)[count] = (uint32_t)carry;
    output->sign = 1;
    cbn_normalize(output);
    return CHALI_C_OK;
}
/**
 * @brief 从较大幅值减去较小幅值。
 * @param output 空临时目标。
 * @param left 较大幅值。
 * @param right 较小幅值。
 * @return 操作状态。
 */
static ChaliCStatus subtract_magnitude(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    const ChaliCStatus status = chali_c_buffer_resize(&output->limbs, left->limbs.length);
    if (status != CHALI_C_OK) return status;
    uint64_t borrow = 0;
    for (size_t index = 0; index < left->limbs.length; ++index)
    {
        const uint64_t a = cbn_const_words(left)[index];
        const uint64_t b = (index < right->limbs.length ? cbn_const_words(right)[index] : 0) + borrow;
        cbn_words(output)[index] = (uint32_t)(a - b);
        borrow = a < b;
    }
    output->sign = 1;
    cbn_normalize(output);
    return CHALI_C_OK;
}
/**
 * @brief 按符号选择幅值加减，并在完成后原子替换目标。
 * @param output 目标。
 * @param left 左值。
 * @param right 右值。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_add(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    if (!output || !left || !right) return CHALI_C_INVALID_ARGUMENT;
    if (!left->sign) return chali_bigint_copy(output, right);
    if (!right->sign) return chali_bigint_copy(output, left);
    ChaliBigInt temporary;
    cbn_temporary(&temporary, output);
    ChaliCStatus status;
    if (left->sign == right->sign)
    {
        status = add_magnitude(&temporary, left, right);
        if (status == CHALI_C_OK && temporary.sign) temporary.sign = left->sign;
    }
    else
    {
        const int order = cbn_compare_magnitude(left, right);
        status = order >= 0 ? subtract_magnitude(&temporary, left, right) : subtract_magnitude(&temporary, right, left);
        if (status == CHALI_C_OK && temporary.sign) temporary.sign = order >= 0 ? left->sign : right->sign;
    }
    if (status == CHALI_C_OK) cbn_move(output, &temporary);
    chali_bigint_clear(&temporary);
    return status;
}
/**
 * @brief 使用借用的反符号视图执行减法。
 * @param output 目标。
 * @param left 左值。
 * @param right 右值。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_subtract(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    if (!right) return CHALI_C_INVALID_ARGUMENT;
    ChaliBigInt view = *right;
    view.sign = -view.sign;
    return chali_bigint_add(output, left, &view);
}
/**
 * @brief 计算幅值的有效位数。
 * @param value 输入。
 * @return 位数。
 */
size_t chali_bigint_bit_length(const ChaliBigInt *value)
{
    if (!value->limbs.length) return 0;
    uint32_t top = cbn_const_words(value)[value->limbs.length - 1];
    size_t bits = (value->limbs.length - 1) * 32;
    while (top)
    {
        ++bits;
        top >>= 1;
    }
    return bits;
}
/**
 * @brief 有界读取幅值的低两块，保持原有忽略符号的约定。
 * @param value 输入。
 * @return 低 64 位。
 */
uint64_t chali_bigint_to_u64(const ChaliBigInt *value)
{
    uint64_t result = value->limbs.length ? cbn_const_words(value)[0] : 0;
    if (value->limbs.length > 1) result |= (uint64_t)cbn_const_words(value)[1] << 32;
    return result;
}
/**
 * @brief 先扩容再从高位移动原分块，失败时不改动数值。
 * @param value 数值。
 * @param bits 位移。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_shift_left(ChaliBigInt *value, size_t bits)
{
    if (!value) return CHALI_C_INVALID_ARGUMENT;
    if (!value->sign || !bits) return CHALI_C_OK;
    const size_t words = bits / 32;
    const unsigned shift = (unsigned)(bits % 32);
    const size_t old = value->limbs.length;
    if (words > SIZE_MAX - old - 1) return CHALI_C_RANGE_ERROR;
    const ChaliCStatus status = chali_c_buffer_resize(&value->limbs, old + words + 1);
    if (status != CHALI_C_OK) return status;
    uint32_t *data = cbn_words(value);
    memmove(data + words, data, old * sizeof(uint32_t));
    if (words) memset(data, 0, words * sizeof(uint32_t));
    uint64_t carry = 0;
    for (size_t index = words; index < old + words; ++index)
    {
        const uint64_t next = ((uint64_t)data[index] << shift) | carry;
        data[index] = (uint32_t)next;
        carry = next >> 32;
    }
    data[old + words] = (uint32_t)carry;
    cbn_normalize(value);
    return CHALI_C_OK;
}
/**
 * @brief 低位方向移动分块并丢弃移出的幅值位。
 * @param value 数值。
 * @param bits 位移。
 */
void chali_bigint_shift_right(ChaliBigInt *value, size_t bits)
{
    if (!value || !value->sign || !bits) return;
    const size_t words = bits / 32;
    const unsigned shift = (unsigned)(bits % 32);
    if (words >= value->limbs.length)
    {
        value->limbs.length = 0;
        value->sign = 0;
        return;
    }
    const size_t count = value->limbs.length - words;
    uint32_t *data = cbn_words(value);
    for (size_t index = 0; index < count; ++index)
    {
        uint64_t window = data[index + words];
        if (shift && index + words + 1 < value->limbs.length) window |= (uint64_t)data[index + words + 1] << 32;
        data[index] = (uint32_t)(window >> shift);
    }
    value->limbs.length = count;
    cbn_normalize(value);
}
/**
 * @brief 幅值乘加一个原生分块。
 * @param value 数值。
 * @param multiplier 乘数。
 * @param addend 加数。
 * @return 操作状态。
 */
ChaliCStatus cbn_multiply_small_add(ChaliBigInt *value, uint32_t multiplier, uint32_t addend)
{
    const size_t count = value->limbs.length;
    if (count == SIZE_MAX) return CHALI_C_RANGE_ERROR;
    const ChaliCStatus status = chali_c_buffer_resize(&value->limbs, count + 1);
    if (status != CHALI_C_OK) return status;
    uint64_t carry = addend;
    for (size_t index = 0; index < count; ++index)
    {
        const uint64_t product = (uint64_t)cbn_words(value)[index] * multiplier + carry;
        cbn_words(value)[index] = (uint32_t)product;
        carry = product >> 32;
    }
    cbn_words(value)[count] = (uint32_t)carry;
    if (value->limbs.length) value->sign = 1;
    cbn_normalize(value);
    return CHALI_C_OK;
}
/**
 * @brief 从高位执行单块除法。
 * @param value 幅值。
 * @param divisor 非零除数。
 * @return 余数。
 */
uint32_t cbn_divide_small(ChaliBigInt *value, uint32_t divisor)
{
    uint64_t remainder = 0;
    for (size_t index = value->limbs.length; index-- > 0;)
    {
        const uint64_t current = (remainder << 32) | cbn_words(value)[index];
        cbn_words(value)[index] = (uint32_t)(current / divisor);
        remainder = current % divisor;
    }
    cbn_normalize(value);
    return (uint32_t)remainder;
}
