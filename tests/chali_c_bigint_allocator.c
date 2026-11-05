/** @file chali_c_bigint_allocator.c @brief 对 C 大数工作区逐次注入分配失败并核对所有权与原子发布。 */
#include "../source/bignum/chali_bigint.h"
#include <assert.h>
#include <stdlib.h>

/** @brief 单一失败点及真实活动分配统计。 */
typedef struct AllocationState
{
    size_t attempts, fail_at, blocks, bytes;
    int failed;
} AllocationState;
/**
 * @brief 在指定调用次序返回空指针。
 * @param context 统计状态。
 * @param bytes 申请字节数。
 * @return C 堆地址或注入的失败。
 */
static void *allocate_test(void *context, size_t bytes)
{
    AllocationState *state = context;
    void *result;
    if (++state->attempts == state->fail_at)
    {
        state->failed = 1;
        return NULL;
    }
    result = malloc(bytes);
    if (result)
    {
        ++state->blocks;
        state->bytes += bytes;
    }
    return result;
}
/**
 * @brief 核对并归还实际申请的字节。
 * @param context 统计状态。
 * @param memory 存储。
 * @param bytes 原申请大小。
 */
static void release_test(void *context, void *memory, size_t bytes)
{
    AllocationState *state = context;
    assert(memory && state->blocks && state->bytes >= bytes);
    --state->blocks;
    state->bytes -= bytes;
    free(memory);
}
/**
 * @brief 执行需要多级临时工作区的数值操作。
 * @param operation 操作编号。
 * @param output 结果。
 * @param left 左值。
 * @param right 右值。
 * @param exponent 大整数指数。
 * @return C 状态码。
 */
static ChaliCStatus calculate(int operation, ChaliBigInt *output, const ChaliBigInt *left,
                              const ChaliBigInt *right, const ChaliBigInt *exponent)
{
    switch (operation)
    {
    case 0: return chali_bigint_multiply(output, left, right);
    case 1: return chali_bigint_divmod(output, NULL, left, right);
    case 2: return chali_bigint_divmod(NULL, output, left, right);
    case 3: return chali_bigint_add(output, left, right);
    case 4: return chali_bigint_subtract(output, left, right);
    case 5: return chali_bigint_pow(output, left, 3);
    case 6: return chali_bigint_gcd(output, left, right);
    case 7: return chali_bigint_lcm(output, left, right);
    case 8: return chali_bigint_mod(output, left, right);
    case 9: return chali_bigint_powmod(output, left, exponent, right);
    case 10: return chali_bigint_inverse(output, left, exponent);
    default:
    {
        ChaliBigInt x, y;
        ChaliCStatus status;
        assert(chali_bigint_init(&x, &output->limbs.allocator) == CHALI_C_OK);
        assert(chali_bigint_init(&y, &output->limbs.allocator) == CHALI_C_OK);
        status = chali_bigint_extended_gcd(output, &x, &y, left, right);
        if (status != CHALI_C_OK) assert(!x.sign && !y.sign);
        chali_bigint_clear(&x);
        chali_bigint_clear(&y);
        return status;
    }
    }
}
#ifdef CHALI_BIGINT_TESTING
/**
 * @brief 对三路插值的普通乘法、平方及输出别名逐点注入失败。
 * @param square 非零时两个输入使用同一对象。
 * @param alias 非零时结果覆盖左输入。
 * @param right_words 右操作数分块数，覆盖最高段为空及单分块的插值路径。
 */
static void check_toom_failures(int square, int alias, size_t right_words)
{
    AllocationState input_state = { 0 }, output_state = { 0 };
    ChaliCAllocator input_owner = { &input_state, allocate_test, release_test };
    ChaliCAllocator output_owner = { &output_state, allocate_test, release_test };
    ChaliBigInt a, b, result, expected, saved;
    size_t i, point, failures = 0;
    assert(chali_bigint_init(&a, &input_owner) == CHALI_C_OK);
    assert(chali_bigint_init(&b, &input_owner) == CHALI_C_OK);
    assert(chali_bigint_init(&saved, &input_owner) == CHALI_C_OK);
    assert(chali_bigint_init(&result, &output_owner) == CHALI_C_OK);
    assert(chali_bigint_init(&expected, &output_owner) == CHALI_C_OK);
    assert(chali_c_buffer_resize(&a.limbs, 769) == CHALI_C_OK);
    assert(chali_c_buffer_resize(&b.limbs, right_words) == CHALI_C_OK);
    for (i = 0; i < a.limbs.length; ++i) ((uint32_t *)a.limbs.data)[i] = UINT32_MAX - (uint32_t)(i * 37);
    for (i = 0; i < b.limbs.length; ++i) ((uint32_t *)b.limbs.data)[i] = UINT32_MAX - (uint32_t)(i * 53);
    a.sign = 1;
    b.sign = -1;
    assert(chali_bigint_copy(&saved, &a) == CHALI_C_OK);
    assert(chali_bigint_test_multiply(&expected, &a, square ? &a : &b, 2) == CHALI_C_OK);
    for (point = 1; point < 10000; ++point)
    {
        size_t input_blocks, output_blocks, input_bytes, output_bytes;
        ChaliCStatus status;
        AllocationState *owner = alias ? &input_state : &output_state;
        ChaliBigInt *destination = alias ? &a : &result;
        assert(chali_bigint_set_u64(&result, 17) == CHALI_C_OK);
        input_blocks = input_state.blocks;
        output_blocks = output_state.blocks;
        input_bytes = input_state.bytes;
        output_bytes = output_state.bytes;
        owner->failed = 0;
        owner->fail_at = owner->attempts + point;
        status = chali_bigint_test_multiply(destination, &a, square ? &a : &b, 3);
        owner->fail_at = 0;
        if (!owner->failed)
        {
            assert(status == CHALI_C_OK && chali_bigint_compare(destination, &expected) == 0);
            break;
        }
        ++failures;
        assert(status == CHALI_C_OUT_OF_MEMORY);
        assert(chali_bigint_compare(&a, &saved) == 0);
        assert(chali_bigint_to_u64(&result) == 17);
        assert(input_blocks == input_state.blocks && output_blocks == output_state.blocks);
        assert(input_bytes == input_state.bytes && output_bytes == output_state.bytes);
    }
    assert(point < 10000 && failures > 0);
    chali_bigint_clear(&a);
    chali_bigint_clear(&b);
    chali_bigint_clear(&saved);
    chali_bigint_clear(&result);
    chali_bigint_clear(&expected);
    assert(!input_state.blocks && !output_state.blocks && !input_state.bytes && !output_state.bytes);
}
#endif
/**
 * @brief 覆盖算法的每个实际分配点，失败后结果与活动量均保持不变。
 * @return 通过返回零。
 */
int main(void)
{
#ifdef CHALI_BIGINT_TESTING
    check_toom_failures(0, 0, 768);
    check_toom_failures(1, 0, 768);
    check_toom_failures(0, 1, 768);
    check_toom_failures(1, 1, 768);
    check_toom_failures(0, 0, 193);
    check_toom_failures(0, 1, 193);
    check_toom_failures(0, 0, 514);
    check_toom_failures(0, 1, 515);
#endif
    AllocationState state = { 0 };
    ChaliCAllocator allocator = { &state, allocate_test, release_test };
    ChaliBigInt left, right, exponent, expected, output, seventeen;
    size_t offset;
    int operation, strategy;
    assert(chali_bigint_init(&left, &allocator) == CHALI_C_OK);
    assert(chali_bigint_init(&right, &allocator) == CHALI_C_OK);
    assert(chali_bigint_init(&exponent, &allocator) == CHALI_C_OK);
    assert(chali_bigint_init(&expected, &allocator) == CHALI_C_OK);
    assert(chali_bigint_init(&output, &allocator) == CHALI_C_OK);
    assert(chali_bigint_init(&seventeen, &allocator) == CHALI_C_OK);
    assert(chali_bigint_set_u64(&seventeen, 17) == CHALI_C_OK);
    assert(chali_bigint_set_u64(&left, 123) == CHALI_C_OK);
    assert(chali_bigint_shift_left(&left, 4096) == CHALI_C_OK);
    assert(chali_bigint_add(&left, &left, &seventeen) == CHALI_C_OK);
    assert(chali_bigint_set_u64(&right, 321) == CHALI_C_OK);
    assert(chali_bigint_shift_left(&right, 2048) == CHALI_C_OK);
    assert(chali_bigint_add(&right, &right, &seventeen) == CHALI_C_OK);
    assert(chali_bigint_set_u64(&exponent, 13) == CHALI_C_OK);
    for (strategy = 0; strategy < 2; ++strategy)
    {
        chali_bigint_set_multiplication_threshold(strategy ? 1 : SIZE_MAX);
        chali_bigint_set_division_threshold(strategy ? 32 : SIZE_MAX);
        for (operation = 0; operation < 12; ++operation)
        {
            assert(calculate(operation, &expected, &left, &right, &exponent) == CHALI_C_OK);
            for (offset = 1; offset < 10000; ++offset)
            {
                size_t blocks, bytes;
                ChaliCStatus status;
                assert(chali_bigint_copy(&output, &seventeen) == CHALI_C_OK);
                blocks = state.blocks;
                bytes = state.bytes;
                state.failed = 0;
                state.fail_at = state.attempts + offset;
                status = calculate(operation, &output, &left, &right, &exponent);
                state.fail_at = 0;
                if (!state.failed)
                {
                    assert(status == CHALI_C_OK && chali_bigint_compare(&output, &expected) == 0);
                    break;
                }
                assert(status == CHALI_C_OUT_OF_MEMORY);
                assert(chali_bigint_compare(&output, &seventeen) == 0);
                assert(state.blocks == blocks && state.bytes == bytes);
            }
            assert(offset < 10000);
        }
    }
    chali_bigint_clear(&left);
    chali_bigint_clear(&right);
    chali_bigint_clear(&exponent);
    chali_bigint_clear(&expected);
    chali_bigint_clear(&output);
    chali_bigint_clear(&seventeen);
    assert(state.blocks == 0 && state.bytes == 0);
    return 0;
}
