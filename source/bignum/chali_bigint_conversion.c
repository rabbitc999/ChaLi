/**
 * @file chali_bigint_conversion.c
 * @brief 大整数文本、十进制位数与机器数值转换的 C 实现。
 */
#include "chali_bigint_internal.h"
#include <stdio.h>

/**
 * @brief 将 ASCII 进制字符转为数值。
 * @param ch 字符。
 * @return 数字，非法字符返回三十六。
 */
static unsigned digit_value(unsigned char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
    return 36;
}
/**
 * @brief 按原有两种文本入口解析，base 为零时采用严格十进制语法。
 * @param output 已初始化目标。
 * @param text 文本字节。
 * @param size 文本长度。
 * @param base 零为严格十进制，其余允许二至三十六。
 * @return 操作状态，非法文本不修改目标。
 */
ChaliCStatus chali_bigint_parse(ChaliBigInt *output, const char *text, size_t size, unsigned base)
{
    if (!output || (size && !text) || (base && (base < 2 || base > 36))) return CHALI_C_INVALID_ARGUMENT;
    size_t start = 0;
    while (start < size && (text[start] == ' ' || text[start] == '\t')) ++start;
    int sign = 1;
    if (start < size && (text[start] == '+' || text[start] == '-'))
    {
        if (text[start] == '-') sign = -1;
        ++start;
    }
    size_t end = size;
    if (base)
        while (end > start && (text[end - 1] == ' ' || text[end - 1] == '\t')) --end;
    if (start == end) return CHALI_C_INVALID_ARGUMENT;
    const unsigned radix = base ? base : 10;
    for (size_t index = start; index < end; ++index)
        if (digit_value((unsigned char)text[index]) >= radix) return CHALI_C_INVALID_ARGUMENT;
    ChaliBigInt temporary;
    cbn_temporary(&temporary, output);
    ChaliCStatus status = CHALI_C_OK;
    if (!base)
    {
        size_t cursor = start;
        while (cursor < end)
        {
            const size_t remaining = end - cursor;
            const size_t take = remaining < 9 ? remaining : 9;
            uint32_t chunk = 0, scale = 1;
            for (size_t index = 0; index < take; ++index)
            {
                chunk = chunk * 10 + (unsigned)(text[cursor + index] - '0');
                scale *= 10;
            }
            status = cbn_multiply_small_add(&temporary, scale, chunk);
            if (status != CHALI_C_OK) break;
            cursor += take;
        }
    }
    else
    {
        for (size_t index = start; index < end; ++index)
        {
            status = cbn_multiply_small_add(&temporary, base, digit_value((unsigned char)text[index]));
            if (status != CHALI_C_OK) break;
        }
    }
    if (status == CHALI_C_OK)
    {
        if (temporary.sign) temporary.sign = sign;
        cbn_move(output, &temporary);
    }
    chali_bigint_clear(&temporary);
    return status;
}
/**
 * @brief 分配独占的文本存储。
 * @param text 目标记录。
 * @param length 文本长度。
 * @param allocator 分配接口。
 * @return 操作状态。
 */
static ChaliCStatus allocate_text(ChaliBigString *text, size_t length, const ChaliCAllocator *allocator)
{
    if (length == SIZE_MAX) return CHALI_C_RANGE_ERROR;
    text->allocator = *allocator;
    text->length = length;
    text->capacity = length + 1;
    text->data = allocator->allocate(allocator->context, text->capacity);
    if (!text->data) return CHALI_C_OUT_OF_MEMORY;
    text->data[length] = 0;
    return CHALI_C_OK;
}
/**
 * @brief 将幅值按十进制十亿分块输出。
 * @param output 临时文本。
 * @param value 输入。
 * @param allocator 工作区与输出分配器。
 * @return 操作状态。
 */
static ChaliCStatus decimal_text(ChaliBigString *output, const ChaliBigInt *value, const ChaliCAllocator *allocator)
{
    ChaliBigInt magnitude;
    chali_bigint_init(&magnitude, allocator);
    ChaliCBuffer chunks;
    chali_c_buffer_init(&chunks, sizeof(uint32_t), allocator);
    ChaliCStatus status = chali_bigint_copy(&magnitude, value);
    if (status != CHALI_C_OK) goto done;
    while (magnitude.sign)
    {
        const uint32_t chunk = cbn_divide_small(&magnitude, 1000000000);
        status = chali_c_buffer_append(&chunks, &chunk, 1);
        if (status != CHALI_C_OK) goto done;
    }
    char leading[16];
    const uint32_t *data = chunks.data;
    const int first = snprintf(leading, sizeof(leading), "%u", (unsigned)data[chunks.length - 1]);
    if (first < 1 || (size_t)first >= sizeof(leading))
    {
        status = CHALI_C_RANGE_ERROR;
        goto done;
    }
    const size_t prefix = value->sign < 0;
    if (chunks.length - 1 > (SIZE_MAX - prefix - (size_t)first) / 9)
    {
        status = CHALI_C_RANGE_ERROR;
        goto done;
    }
    status = allocate_text(output, prefix + (size_t)first + (chunks.length - 1) * 9, allocator);
    if (status != CHALI_C_OK) goto done;
    size_t position = 0;
    if (prefix) output->data[position++] = '-';
    memcpy(output->data + position, leading, (size_t)first);
    position += (size_t)first;
    for (size_t index = chunks.length - 1; index-- > 0;)
    {
        snprintf(output->data + position, 10, "%09u", (unsigned)data[index]);
        position += 9;
    }
done:
    chali_bigint_clear(&magnitude);
    chali_c_buffer_clear(&chunks);
    return status;
}
/**
 * @brief 使用原有逐块除法生成任意进制文本。
 * @param output 零初始化或先前转换得到的文本。
 * @param value 输入数值。
 * @param base 零表示十亿分块十进制，其余为二至三十六。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_to_string(ChaliBigString *output, const ChaliBigInt *value, unsigned base)
{
    if (!output || !value || (base && (base < 2 || base > 36))) return CHALI_C_INVALID_ARGUMENT;
    const ChaliCAllocator *allocator = chali_c_allocator_valid(&output->allocator) ? &output->allocator : &value->limbs.allocator;
    ChaliBigString text = { 0 };
    ChaliCStatus status;
    if (!value->sign)
    {
        status = allocate_text(&text, 1, allocator);
        if (status == CHALI_C_OK) text.data[0] = '0';
    }
    else if (!base)
    {
        status = decimal_text(&text, value, allocator);
    }
    else
    {
        static const char alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        ChaliBigInt magnitude;
        chali_bigint_init(&magnitude, allocator);
        ChaliCBuffer reversed;
        chali_c_buffer_init(&reversed, sizeof(char), allocator);
        status = chali_bigint_copy(&magnitude, value);
        while (status == CHALI_C_OK && magnitude.sign)
        {
            const char digit = alphabet[cbn_divide_small(&magnitude, base)];
            status = chali_c_buffer_append(&reversed, &digit, 1);
        }
        if (status == CHALI_C_OK)
        {
            const size_t prefix = value->sign < 0;
            if (reversed.length > SIZE_MAX - prefix)
                status = CHALI_C_RANGE_ERROR;
            else
                status = allocate_text(&text, reversed.length + prefix, allocator);
            if (status == CHALI_C_OK)
            {
                if (prefix) text.data[0] = '-';
                const char *digits = reversed.data;
                for (size_t index = 0; index < reversed.length; ++index)
                    text.data[prefix + index] = digits[reversed.length - index - 1];
            }
        }
        chali_bigint_clear(&magnitude);
        chali_c_buffer_clear(&reversed);
    }
    if (status == CHALI_C_OK)
    {
        chali_big_string_clear(output);
        *output = text;
    }
    else
        chali_big_string_clear(&text);
    return status;
}
/**
 * @brief 保留原十亿分块计数方法计算十进制位数。
 * @param value 数值。
 * @param digits 接收位数。
 * @param allocator 可选工作区分配器。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_decimal_digits(const ChaliBigInt *value, size_t *digits, const ChaliCAllocator *allocator)
{
    if (!value || !digits || (allocator && !chali_c_allocator_valid(allocator))) return CHALI_C_INVALID_ARGUMENT;
    if (!value->sign)
    {
        *digits = 1;
        return CHALI_C_OK;
    }
    ChaliBigInt magnitude;
    chali_bigint_init(&magnitude, allocator ? allocator : &value->limbs.allocator);
    ChaliCStatus status = chali_bigint_copy(&magnitude, value);
    size_t count = 0;
    while (status == CHALI_C_OK && magnitude.sign)
    {
        uint32_t rest = cbn_divide_small(&magnitude, 1000000000);
        if (magnitude.sign)
            count += 9;
        else
        {
            do
            {
                ++count;
                rest /= 10;
            } while (rest);
        }
    }
    chali_bigint_clear(&magnitude);
    if (status == CHALI_C_OK) *digits = count;
    return status;
}
/**
 * @brief 按原高位到低位顺序累积二进制浮点值。
 * @param value 数值。
 * @return 带符号的双精度近似。
 */
double chali_bigint_to_double(const ChaliBigInt *value)
{
    double result = 0;
    for (size_t index = value->limbs.length; index-- > 0;)
        result = result * 4294967296.0 + (double)cbn_const_words(value)[index];
    return value->sign < 0 ? -result : result;
}
