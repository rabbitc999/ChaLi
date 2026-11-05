/** @file chali_c_bigfloat_allocator.c @brief C 大浮点逐分配失败、原所有者及结果回滚验证。 */
#include "chali_bigfloat_math.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/** @brief 一个隔离分配所有者的活动量与失败点。 */
typedef struct Owner
{
    size_t calls, fail_at, live, blocks;
} Owner;
/** @brief 测试分配头，用于验证释放所有者及对齐。 */
typedef struct Allocation
{
    Owner *owner;
    size_t bytes;
    max_align_t alignment;
} Allocation;
/**
 * @brief 分配并在指定调用点返回空，地址保持最大基本对齐。
 * @param context 所有者。
 * @param bytes 请求大小。
 * @return 独占地址或空。
 */
static void *allocate(void *context, size_t bytes)
{
    Owner *owner = context;
    Allocation *header;
    ++owner->calls;
    if (owner->fail_at && owner->calls == owner->fail_at) return NULL;
    if (bytes > SIZE_MAX - sizeof(Allocation)) return NULL;
    header = malloc(sizeof(Allocation) + bytes);
    if (!header) return NULL;
    header->owner = owner;
    header->bytes = bytes;
    owner->live += bytes;
    ++owner->blocks;
    return header + 1;
}
/**
 * @brief 检查地址确属原所有者后释放。
 * @param context 原所有者。
 * @param pointer 独占地址。
 * @param bytes 原容量。
 */
static void release(void *context, void *pointer, size_t bytes)
{
    Owner *owner = context;
    Allocation *header;
    if (!pointer) return;
    header = (Allocation *)pointer - 1;
    assert(header->owner == owner && header->bytes == bytes && owner->blocks && owner->live >= bytes);
    owner->live -= bytes;
    --owner->blocks;
    free(header);
}
/**
 * @brief 按指定所有者构造非零十进制值。
 * @param value 目标。
 * @param owner 分配所有者。
 * @param text 十进制输入。
 */
static void initialize(ChaliBigFloat *value, Owner *owner, const char *text)
{
    ChaliCAllocator allocator = { owner, allocate, release };
    int valid;
    assert(chali_bigfloat_init(value, &allocator) == CHALI_C_OK);
    assert(chali_bigfloat_parse(value, text, strlen(text), &valid) == CHALI_C_OK && valid);
}
/**
 * @brief 使用独立默认所有者检查规范化文本，不影响被测预算。
 * @param value 待检查值。
 * @param expected 预期精确文本。
 */
static void expect(const ChaliBigFloat *value, const char *expected)
{
    ChaliBigString text = { 0 };
    text.allocator = chali_c_default_allocator();
    assert(chali_bigfloat_to_string(&text, value, 0, 0) == CHALI_C_OK);
    assert(strcmp(text.data, expected) == 0);
    chali_big_string_clear(&text);
}
/**
 * @brief 调用一项代表性运算，覆盖基础、格式化、方根和超越函数。
 * @param operation 操作编号。
 * @param output 结果工作值。
 * @param a 左输入。
 * @param b 右输入。
 * @return 操作状态。
 */
static ChaliCStatus execute(int operation, ChaliBigFloat *output, const ChaliBigFloat *a, const ChaliBigFloat *b)
{
    ChaliBigFloatError error = { 0 };
    ChaliBigString text = { 0 };
    ChaliBigInt integer;
    ChaliCStatus status;
    int valid;
    switch (operation)
    {
    case 0: return chali_bigfloat_parse(output, "123.456000e-12", 14, &valid);
    case 1: return chali_bigfloat_from_double(output, 0.12345678912345678, 17);
    case 2: return chali_bigfloat_from_integer(output, &a->mantissa);
    case 3: return chali_bigfloat_normalized(output, a);
    case 4: return chali_bigfloat_round(output, a, 5);
    case 5: return chali_bigfloat_add(output, a, b, 0);
    case 6: return chali_bigfloat_add(output, a, b, 1);
    case 7: return chali_bigfloat_multiply(output, a, b);
    case 8: return chali_bigfloat_divide(output, a, b, 20);
    case 9: return chali_bigfloat_sign_change(output, a, 0);
    case 10: return chali_bigfloat_sqrt(output, a, 20);
    case 11: return chali_bigfloat_pow_integer(output, a, 7);
    case 12: return chali_bigfloat_root(output, a, 3, 12);
    case 13:
        text.allocator = output->mantissa.limbs.allocator;
        status = chali_bigfloat_to_string(&text, a, 7, 1);
        chali_big_string_clear(&text);
        return status;
    case 14: return chali_bigfloat_compare(a, b, &valid, &output->mantissa.limbs.allocator);
    case 15:
        assert(chali_bigint_init(&integer, &output->mantissa.limbs.allocator) == CHALI_C_OK);
        status = chali_bigfloat_to_integer(&integer, a, -1);
        chali_bigint_clear(&integer);
        return status;
    case 16: return chali_bigfloat_is_integer(a, &valid, &output->mantissa.limbs.allocator);
    case 17: return chali_bigfloat_pi(output, 7, &error);
    case 18: return chali_bigfloat_exp(output, b, 7, &error);
    case 19: return chali_bigfloat_ln(output, a, 7, &error);
    case 20: return chali_bigfloat_log10(output, a, 7, &error);
    case 21: return chali_bigfloat_pow(output, b, b, 7, &error);
    case 22: return chali_bigfloat_fmod(output, a, b, &error);
    case 23: return chali_bigfloat_sin(output, a, 7, &error);
    case 24: return chali_bigfloat_cos(output, a, 7, &error);
    case 25: return chali_bigfloat_tan(output, a, 7, &error);
    case 26: return chali_bigfloat_atan(output, a, 7, &error);
    case 27: return chali_bigfloat_asin(output, b, 7, &error);
    case 28: return chali_bigfloat_acos(output, b, 7, &error);
    default: return CHALI_C_INVALID_ARGUMENT;
    }
}
/**
 * @brief 执行一次失败点，验证输入身份、预算与失败时的原结果。
 * @param operation 操作编号。
 * @param fail_at 被拒绝的分配调用，零为成功基线。
 * @return 本次操作的分配次数。
 */
static size_t check_failure(int operation, size_t fail_at)
{
    Owner input_owner = { 0 }, result_owner = { 0 };
    ChaliBigFloat a, b, result;
    size_t baseline_input_calls, attempts;
    void *a_data, *b_data;
    ChaliCStatus status;
    initialize(&a, &input_owner, "12345.6789");
    initialize(&b, &input_owner, "0.325");
    initialize(&result, &result_owner, "99.9");
    baseline_input_calls = input_owner.calls;
    a_data = a.mantissa.limbs.data;
    b_data = b.mantissa.limbs.data;
    result_owner.calls = 0;
    result_owner.fail_at = fail_at;
    status = execute(operation, &result, &a, &b);
    attempts = result_owner.calls;
    result_owner.fail_at = 0;
    assert(input_owner.calls == baseline_input_calls);
    assert(a.mantissa.limbs.data == a_data && b.mantissa.limbs.data == b_data);
    assert(result.mantissa.limbs.allocator.context == &result_owner);
    expect(&a, "12345.6789");
    expect(&b, "0.325");
    if (fail_at)
    {
        assert(status == CHALI_C_OUT_OF_MEMORY);
        expect(&result, "99.9");
    }
    else
        assert(status == CHALI_C_OK);
    chali_bigfloat_clear(&a);
    chali_bigfloat_clear(&b);
    chali_bigfloat_clear(&result);
    assert(!input_owner.blocks && !input_owner.live && !result_owner.blocks && !result_owner.live);
    return attempts;
}
/**
 * @brief 枚举所有成功执行中的分配点并逐一注入失败。
 * @return 所有所有者和回滚检查通过时返回零。
 */
int main(void)
{
    int operation;
    size_t injected = 0;
    for (operation = 0; operation < 29; ++operation)
    {
        const size_t allocations = check_failure(operation, 0);
        size_t point;
        printf("C bigfloat: operation %d, checking %zu allocation points\n", operation, allocations);
        fflush(stdout);
        for (point = 1; point <= allocations; ++point)
        {
            check_failure(operation, point);
            ++injected;
        }
    }
    printf("C bigfloat: 29 operations, %zu allocation failures, owners and rollback passed\n", injected);
    return 0;
}
