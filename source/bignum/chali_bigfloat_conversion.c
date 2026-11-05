/**
 * @file chali_bigfloat_conversion.c
 * @brief 十进制普通/科学记法的 C 文本输出。
 */
#include "chali_bigfloat_internal.h"
#include <string.h>

/**
 * @brief 直接编码科学记数法指数，避免为整数指数牵入可变参数格式化器。
 * @param output 至少三十二字节的调用方缓冲区。
 * @param exponent 有符号十进制指数。
 * @return 有效字节数。
 */
static size_t cbf_exponent_text(char *output, int64_t exponent)
{
    char reverse[24];
    size_t count = 0, length = 0;
    uint64_t magnitude = exponent < 0 ? (uint64_t)-(exponent + 1) + 1 : (uint64_t)exponent;
    output[length++] = 'e';
    output[length++] = exponent < 0 ? '-' : '+';
    do
    {
        reverse[count++] = (char)('0' + magnitude % 10);
        magnitude /= 10;
    } while (magnitude);
    while (count) output[length++] = reverse[--count];
    return length;
}

/**
 * @brief 在字节缓冲区末尾补指定数量的 ASCII 零。
 * @param output 已初始化字节缓冲区。
 * @param count 零的个数。
 * @return 操作状态。
 */
static ChaliCStatus cbf_append_zeros(ChaliCBuffer *output, uint64_t count)
{
    size_t start = output->length;
    ChaliCStatus status;
    if (count > SIZE_MAX - start) return CHALI_C_RANGE_ERROR;
    status = chali_c_buffer_resize(output, start + (size_t)count);
    if (status == CHALI_C_OK && count) memset((char *)output->data + start, '0', (size_t)count);
    return status;
}
/**
 * @brief 按原显示阈值和舍入规则输出十进制文本。
 * @param output 独占文本，空 allocator 时沿用输入分配接口。
 * @param input 输入值。
 * @param digits 有效位数，非正时不舍入。
 * @param plain 非零时恒用普通记法。
 * @return 操作状态，失败不修改已有文本。
 */
ChaliCStatus chali_bigfloat_to_string(ChaliBigString *output, const ChaliBigFloat *input, int digits, int plain)
{
    ChaliBigFloat rounded;
    const ChaliBigFloat *value = input;
    ChaliBigInt magnitude;
    ChaliBigString raw = { 0 };
    ChaliCBuffer text;
    ChaliCStatus status = CHALI_C_OK;
    const ChaliCAllocator *allocator;
    int64_t count, point, exponent;
    int precision = digits > 0 ? digits : 17;
    char exponent_text[32];
    if (!output || !input) return CHALI_C_INVALID_ARGUMENT;
    allocator = chali_c_allocator_valid(&output->allocator) ? &output->allocator : &input->mantissa.limbs.allocator;
    chali_bigfloat_init(&rounded, allocator);
    chali_c_buffer_init(&text, 1, allocator);
    raw.allocator = *allocator;
    if (digits > 0)
    {
        CBF_TRY(chali_bigfloat_round(&rounded, input, digits));
        value = &rounded;
    }
    if (!value->mantissa.sign)
    {
        CBF_TRY(chali_c_buffer_append(&text, "0", 1));
        goto commit;
    }
    magnitude = value->mantissa;
    magnitude.sign = 1;
    CBF_TRY(chali_bigint_to_string(&raw, &magnitude, 0));
    if (raw.length > INT64_MAX - INT_MAX)
    {
        status = CHALI_C_RANGE_ERROR;
        goto cleanup;
    }
    count = (int64_t)raw.length;
    point = count + value->exponent10;
    exponent = point - 1;
    if (value->mantissa.sign < 0) CBF_TRY(chali_c_buffer_append(&text, "-", 1));
    if (!plain && (exponent < -4 || exponent >= precision))
    {
        size_t length;
        CBF_TRY(chali_c_buffer_append(&text, raw.data, 1));
        if (count > 1)
        {
            CBF_TRY(chali_c_buffer_append(&text, ".", 1));
            CBF_TRY(chali_c_buffer_append(&text, raw.data + 1, raw.length - 1));
        }
        length = cbf_exponent_text(exponent_text, exponent);
        CBF_TRY(chali_c_buffer_append(&text, exponent_text, length));
    }
    else if (point <= 0)
    {
        CBF_TRY(chali_c_buffer_append(&text, "0.", 2));
        CBF_TRY(cbf_append_zeros(&text, (uint64_t)-point));
        CBF_TRY(chali_c_buffer_append(&text, raw.data, raw.length));
    }
    else if (point >= count)
    {
        CBF_TRY(chali_c_buffer_append(&text, raw.data, raw.length));
        CBF_TRY(cbf_append_zeros(&text, (uint64_t)(point - count)));
    }
    else
    {
        CBF_TRY(chali_c_buffer_append(&text, raw.data, (size_t)point));
        CBF_TRY(chali_c_buffer_append(&text, ".", 1));
        CBF_TRY(chali_c_buffer_append(&text, raw.data + (size_t)point, raw.length - (size_t)point));
    }
commit:
    CBF_TRY(chali_c_buffer_append(&text, "", 1));
    chali_big_string_clear(output);
    output->data = text.data;
    output->length = text.length - 1;
    output->capacity = text.capacity;
    output->allocator = text.allocator;
    text.data = NULL;
    text.length = text.capacity = 0;
cleanup:
    chali_big_string_clear(&raw);
    chali_bigfloat_clear(&rounded);
    chali_c_buffer_clear(&text);
    return status;
}
