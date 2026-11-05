/**
 * @file chali_bigint_multiplication.c
 * @brief 学校算法、Karatsuba、Toom-Cook 3 与双素数 CRT+NTT 的 C 乘法分派。
 */
#include "chali_bigint_internal.h"
#include <stdbool.h>
#include <stdint.h>

#define CBN_NTT_MODULUS 998244353u
#define CBN_NTT_PREFIX  cbn_ntt1_
#include "chali_bigint_ntt.inc"
#undef CBN_NTT_MODULUS
#undef CBN_NTT_PREFIX
#define CBN_NTT_MODULUS 1004535809u
#define CBN_NTT_PREFIX  cbn_ntt2_
#include "chali_bigint_ntt.inc"
#undef CBN_NTT_MODULUS
#undef CBN_NTT_PREFIX

#if defined(__SIZEOF_INT128__) && !defined(BIGINT_FORCE_PORTABLE_MULTIPLY)
static size_t schoolbook_threshold = 512;
#define CBN_KARATSUBA_WEIGHT  16
#define CBN_SCHOOLBOOK_DIRECT 64
#else
static size_t schoolbook_threshold = 4096;
#define CBN_KARATSUBA_WEIGHT  36
#define CBN_SCHOOLBOOK_DIRECT 128
#endif
static size_t karatsuba_threshold = 96;
static size_t ntt_threshold = 6144;
static size_t square_threshold = 256;
static size_t toom_threshold = 160;
static size_t toom_square_threshold = 256;
static size_t division_threshold = 256;

/**
 * @brief 饱和乘法，防止规模估计自身溢出。
 * @param left 左估计量。
 * @param right 右估计量。
 * @return 饱和到无符号六十四位上限的乘积。
 */
static uint64_t multiplication_cost_product(uint64_t left, uint64_t right)
{ return right && left > UINT64_MAX / right ? UINT64_MAX : left * right; }
/**
 * @brief 饱和加法，防止规模估计自身溢出。
 * @param left 左估计量。
 * @param right 右估计量。
 * @return 饱和估计。
 */
static uint64_t multiplication_cost_sum(uint64_t left, uint64_t right)
{ return left > UINT64_MAX - right ? UINT64_MAX : left + right; }
/**
 * @brief 按补齐形状估计第一阶段成本，学校算法保留一成、Toom 保留十六分之一选择裕量。
 * @param left_size 左分块数。
 * @param right_size 右分块数。
 * @param square 是否为平方。
 * @param cost 写入估计工作量，不使用浮点幂或系统服务。
 * @return 一学校、二 Karatsuba、三 Toom。
 */
static int first_stage_route(size_t left_size, size_t right_size, int square, uint64_t *cost)
{
    const size_t shorter = left_size < right_size ? left_size : right_size;
    const size_t longer = left_size > right_size ? left_size : right_size;
    uint64_t school, best, candidate, leaves = 1;
    size_t width = 1;
    int route = 2;
    if (!shorter)
    {
        *cost = 0;
        return 2;
    }
    if (shorter == 1)
    {
        *cost = longer;
        return 2;
    }
    school = square ? multiplication_cost_product(multiplication_cost_product(longer, longer == SIZE_MAX ? UINT64_MAX : longer + 1), 11) / 10 :
                      multiplication_cost_product(multiplication_cost_product(left_size, right_size), 2);
#if defined(__SIZEOF_INT128__) && !defined(BIGINT_FORCE_PORTABLE_MULTIPLY)
    if (longer <= 4)
    {
        *cost = school;
        return 2;
    }
#endif
    if ((!square && shorter <= schoolbook_threshold && shorter <= CBN_SCHOOLBOOK_DIRECT) || (square && longer <= square_threshold && longer < toom_square_threshold))
    {
        *cost = school;
        return 1;
    }
    while (width < shorter)
    {
        if (width > SIZE_MAX / 2)
        {
            *cost = UINT64_MAX;
            return 2;
        }
        width <<= 1;
        leaves = multiplication_cost_product(leaves, 3);
    }
    best = multiplication_cost_product(multiplication_cost_product(leaves, longer / width + (longer % width != 0)),
                                       CBN_KARATSUBA_WEIGHT);
    if ((square ? longer <= square_threshold : shorter <= schoolbook_threshold) && school < best - best / 10)
    {
        best = school;
        route = 1;
    }
    if (longer >= (square ? toom_square_threshold : toom_threshold) && shorter >= 8 && longer <= SIZE_MAX - 2)
    {
        uint64_t evaluation_cost, constant_cost, highest_cost;
        const size_t width = (longer + 2) / 3;
        const size_t part = width + 1;
        const size_t right_part = shorter < part ? shorter : part;
        const size_t right_constant = shorter < width ? shorter : width;
        const size_t right_highest = shorter > width * 2 ? shorter - width * 2 : 0;
        first_stage_route(part, right_part, square, &evaluation_cost);
        first_stage_route(width, right_constant, square, &constant_cost);
        first_stage_route(longer - width * 2, right_highest, square, &highest_cost);
        candidate = multiplication_cost_sum(multiplication_cost_product(evaluation_cost, 3),
                                            multiplication_cost_sum(constant_cost, highest_cost));
        candidate = multiplication_cost_sum(candidate,
                                            multiplication_cost_sum(multiplication_cost_product(longer, square ? 64 : 80), 5000));
        if (candidate < best - best / 16)
        {
            best = candidate;
            route = 3;
        }
    }
    *cost = best;
    return route;
}
/**
 * @brief 用补齐后的变换规模比较 CRT+NTT 与第一阶段成本。
 * @param left_size 左块数。
 * @param right_size 右块数。
 * @param square 是否为平方。
 * @param first_stage_cost 学校、Karatsuba 或 Toom 的较优估计。
 * @return 选择 NTT 时返回一。
 */
static int prefer_ntt(size_t left_size, size_t right_size, int square, uint64_t first_stage_cost)
{
    const size_t shorter = left_size < right_size ? left_size : right_size;
    if (shorter <= 1) return 0;
    if (left_size > (SIZE_MAX - 14) / 32 || right_size > (SIZE_MAX - 14) / 32) return 0;
    const size_t first = (left_size * 32 + 14) / 15;
    const size_t second = (right_size * 32 + 14) / 15;
    if (first > SIZE_MAX - second || first + second > ((size_t)1 << 21)) return 0;
    const size_t digits = first + second;
    if (ntt_threshold <= 1) return 1;
    if (left_size > SIZE_MAX - right_size || left_size + right_size < ntt_threshold || shorter <= karatsuba_threshold) return 0;
    size_t transform = 1, levels = 0;
    while (transform < digits)
    {
        transform <<= 1;
        ++levels;
    }
    const uint64_t ntt_work = (uint64_t)transform * levels;
    return first_stage_cost > ntt_work * (square ? 22 : 32);
}
/**
 * @brief 按高位顺序比较等宽分块，再计算绝对差。
 * @param left 左输入。
 * @param right 右输入。
 * @param count 宽度。
 * @param difference 差值输出。
 * @return 左值小于右值时为真。
 */
static bool subtract_blocks(const uint32_t *left, const uint32_t *right, size_t count, uint32_t *difference)
{
    bool negative = false;
    for (size_t index = count; index-- > 0;)
    {
        if (left[index] == right[index]) continue;
        negative = left[index] < right[index];
        break;
    }
    if (negative)
    {
        const uint32_t *temporary = left;
        left = right;
        right = temporary;
    }
    uint64_t borrow = 0;
    for (size_t index = 0; index < count; ++index)
    {
        const uint64_t subtrahend = (uint64_t)right[index] + borrow;
        difference[index] = (uint32_t)(left[index] - subtrahend);
        borrow = left[index] < subtrahend;
    }
    return negative;
}
/**
 * @brief 在原生乘法宽度处终止等宽 Karatsuba，保持原工作区布局与进位顺序。
 * @param left 左输入。
 * @param right 右输入。
 * @param count 幂二宽度。
 * @param output 两倍宽度输出。
 * @param scratch 递归共用工作区。
 */
static void karatsuba_blocks(const uint32_t *left, const uint32_t *right,
                             size_t count, uint32_t *output, uint32_t *scratch)
{
#if defined(__SIZEOF_INT128__) && !defined(BIGINT_FORCE_PORTABLE_MULTIPLY)
    if (count == 4u)
    {
        __extension__ typedef unsigned __int128 DoubleWord;
        const uint64_t aLow = (uint64_t)(left[0]) | ((uint64_t)(left[1]) << 32u);
        const uint64_t aHigh = (uint64_t)(left[2]) | ((uint64_t)(left[3]) << 32u);
        const uint64_t bLow = (uint64_t)(right[0]) | ((uint64_t)(right[1]) << 32u);
        const uint64_t bHigh = (uint64_t)(right[2]) | ((uint64_t)(right[3]) << 32u);
        const DoubleWord low = (DoubleWord)(aLow)*bLow;
        const DoubleWord high = (DoubleWord)(aHigh)*bHigh;
        const DoubleWord difference = (DoubleWord)(aHigh < aLow ? aLow - aHigh : aHigh - aLow) * (bHigh < bLow ? bLow - bHigh : bHigh - bLow);
        DoubleWord middle = low + high;
        uint64_t top = middle < low ? 1u : 0u;
        const DoubleWord previous = middle;
        if ((aHigh < aLow) != (bHigh < bLow))
        {
            middle += difference;
            top += middle < previous ? 1u : 0u;
        }
        else
        {
            middle -= difference;
            top -= previous < difference ? 1u : 0u;
        }
        const DoubleWord second = (low >> 64u) + (uint64_t)(middle);
        const DoubleWord third = (uint64_t)(high) + (middle >> 64u) + (second >> 64u);
        const uint64_t words[] = { (uint64_t)(low), (uint64_t)(second),
                                   (uint64_t)(third), (uint64_t)((high >> 64u) + top + (third >> 64u)) };
        for (size_t index = 0u; index < 4u; ++index)
        {
            output[index * 2u] = (uint32_t)(words[index]);
            output[index * 2u + 1u] = (uint32_t)(words[index] >> 32u);
        }
        return;
    }
    if (count == 2u)
    {
        __extension__ typedef unsigned __int128 DoubleWord;
        const uint64_t a = (uint64_t)(left[0]) | ((uint64_t)(left[1]) << 32u);
        const uint64_t b = (uint64_t)(right[0]) | ((uint64_t)(right[1]) << 32u);
        const DoubleWord product = (DoubleWord)(a)*b;
        for (size_t index = 0u; index < 4u; ++index)
            output[index] = (uint32_t)(product >> (index * 32u));
        return;
    }
#endif
    if (count == 1u)
    {
        const uint64_t product = (uint64_t)(*left) * *right;
        output[0] = (uint32_t)(product);
        output[1] = (uint32_t)(product >> 32u);
        return;
    }
    const size_t half = count / 2u;
    karatsuba_blocks(left, right, half, output, scratch);
    karatsuba_blocks(left + half, right + half, half, output + count, scratch);
    uint32_t *leftDifference = scratch;
    uint32_t *rightDifference = scratch + half;
    uint32_t *differenceProduct = scratch + count;
    uint32_t *middle = scratch + count * 2u;
    uint32_t *childScratch = middle + count + 1u;
    const bool leftNegative = subtract_blocks(left + half, left, half, leftDifference);
    const bool rightNegative = subtract_blocks(right + half, right, half, rightDifference);
    karatsuba_blocks(leftDifference, rightDifference, half, differenceProduct, childScratch);
    uint64_t carry = 0u;
    for (size_t index = 0u; index < count; ++index)
    {
        const uint64_t sum = (uint64_t)(output[index]) + output[count + index] + carry;
        middle[index] = (uint32_t)(sum);
        carry = sum >> 32u;
    }
    middle[count] = (uint32_t)(carry);
    carry = 0u;
    for (size_t index = 0u; index < count; ++index)
    {
        if (leftNegative != rightNegative)
        {
            const uint64_t sum = (uint64_t)(middle[index]) + differenceProduct[index] + carry;
            middle[index] = (uint32_t)(sum);
            carry = sum >> 32u;
        }
        else
        {
            const uint64_t subtrahend = (uint64_t)(differenceProduct[index]) + carry;
            carry = middle[index] < subtrahend ? 1u : 0u;
            middle[index] -= (uint32_t)(subtrahend);
        }
    }
    if (leftNegative != rightNegative)
        middle[count] += (uint32_t)(carry);
    else
        middle[count] -= (uint32_t)(carry);
    carry = 0u;
    for (size_t index = 0u; index <= count; ++index)
    {
        const uint64_t sum = (uint64_t)(output[half + index]) + middle[index] + carry;
        output[half + index] = (uint32_t)(sum);
        carry = sum >> 32u;
    }
    for (size_t index = half + count + 1u; carry != 0u && index < count * 2u; ++index)
    {
        const uint64_t sum = (uint64_t)(output[index]) + carry;
        output[index] = (uint32_t)(sum);
        carry = sum >> 32u;
    }
}

/**
 * @brief 包含不平衡分块的 Karatsuba 幅值乘法。
 * @param output 临时输出。
 * @param left 左幅值。
 * @param right 右幅值。
 * @return 操作状态。
 */
static ChaliCStatus karatsuba_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    const size_t left_size = left->limbs.length;
    const size_t right_size = right->limbs.length;
    const ChaliBigInt *wide = left_size < right_size ? right : left;
    const ChaliBigInt *narrow = left_size < right_size ? left : right;
    if (!left_size || !right_size) return CHALI_C_OK;
    if (left_size == 1 || right_size == 1)
    {
        const uint32_t scalar = wide == left ? cbn_const_words(right)[0] : cbn_const_words(left)[0];
        ChaliCStatus status = chali_c_buffer_resize(&output->limbs, wide->limbs.length + 1);
        if (status != CHALI_C_OK) return status;
        uint64_t carry = 0;
        for (size_t index = 0; index < wide->limbs.length; ++index)
        {
            const uint64_t product = (uint64_t)cbn_const_words(wide)[index] * scalar + carry;
            cbn_words(output)[index] = (uint32_t)product;
            carry = product >> 32;
        }
        cbn_words(output)[wide->limbs.length] = (uint32_t)carry;
        return CHALI_C_OK;
    }
    if (wide->limbs.length <= 4)
    {
        uint32_t a[4] = { 0 }, b[4] = { 0 }, scratch[88] = { 0 };
        memcpy(a, cbn_const_words(left), left_size * sizeof(uint32_t));
        memcpy(b, cbn_const_words(right), right_size * sizeof(uint32_t));
        const size_t width = wide->limbs.length <= 2 ? 2 : 4;
        ChaliCStatus status = chali_c_buffer_resize(&output->limbs, width * 2);
        if (status == CHALI_C_OK) karatsuba_blocks(a, b, width, cbn_words(output), scratch);
        return status;
    }
    size_t width = 1;
    while (width < narrow->limbs.length)
    {
        if (width > SIZE_MAX / 2) return CHALI_C_RANGE_ERROR;
        width <<= 1;
    }
    if (width > (SIZE_MAX - 64) / 6 || left_size > SIZE_MAX - right_size - 1) return CHALI_C_RANGE_ERROR;
    ChaliCBuffer a, b, product, scratch;
    const ChaliCAllocator *allocator = &output->limbs.allocator;
    chali_c_buffer_init(&a, sizeof(uint32_t), allocator);
    chali_c_buffer_init(&b, sizeof(uint32_t), allocator);
    chali_c_buffer_init(&product, sizeof(uint32_t), allocator);
    chali_c_buffer_init(&scratch, sizeof(uint32_t), allocator);
    ChaliCStatus status = chali_c_buffer_resize(&a, width);
    if (status != CHALI_C_OK) goto done;
    status = chali_c_buffer_resize(&b, width);
    if (status != CHALI_C_OK) goto done;
    status = chali_c_buffer_resize(&product, width * 2);
    if (status != CHALI_C_OK) goto done;
    status = chali_c_buffer_resize(&scratch, width * 6 + 64);
    if (status != CHALI_C_OK) goto done;
    status = chali_c_buffer_resize(&output->limbs, left_size + right_size + 1);
    if (status != CHALI_C_OK) goto done;
    memcpy(a.data, cbn_const_words(narrow), narrow->limbs.length * sizeof(uint32_t));
    for (size_t offset = 0; offset < wide->limbs.length; offset += width)
    {
        memset(b.data, 0, width * sizeof(uint32_t));
        const size_t remaining = wide->limbs.length - offset;
        const size_t count = width < remaining ? width : remaining;
        memcpy(b.data, cbn_const_words(wide) + offset, count * sizeof(uint32_t));
        karatsuba_blocks(a.data, b.data, width, product.data, scratch.data);
        uint64_t carry = 0;
        size_t index = 0;
        const size_t used = narrow->limbs.length + count;
        const uint32_t *part = product.data;
        for (; index < used; ++index)
        {
            const uint64_t sum = (uint64_t)cbn_words(output)[offset + index] + part[index] + carry;
            cbn_words(output)[offset + index] = (uint32_t)sum;
            carry = sum >> 32;
        }
        while (carry)
        {
            const uint64_t sum = (uint64_t)cbn_words(output)[offset + index] + carry;
            cbn_words(output)[offset + index++] = (uint32_t)sum;
            carry = sum >> 32;
        }
    }
done:
    chali_c_buffer_clear(&a);
    chali_c_buffer_clear(&b);
    chali_c_buffer_clear(&product);
    chali_c_buffer_clear(&scratch);
    return status;
}
/**
 * @brief 将幅值分成 15 位小端系数。
 * @param output 32 位系数缓冲区。
 * @param value 幅值。
 * @return 操作状态。
 */
static ChaliCStatus to_base15(ChaliCBuffer *output, const ChaliBigInt *value)
{
    const size_t bits = chali_bigint_bit_length(value);
    const size_t count = (bits + 14) / 15;
    ChaliCStatus status = chali_c_buffer_resize(output, count);
    if (status != CHALI_C_OK) return status;
    uint32_t *digits = output->data;
    for (size_t index = 0; index < count; ++index)
    {
        const size_t bit = index * 15;
        const size_t limb = bit / 32;
        const unsigned shift = (unsigned)(bit % 32);
        uint64_t window = cbn_const_words(value)[limb];
        if (limb + 1 < value->limbs.length) window |= (uint64_t)cbn_const_words(value)[limb + 1] << 32;
        digits[index] = (uint32_t)((window >> shift) & 0x7fff);
    }
    return CHALI_C_OK;
}
/**
 * @brief 将完成进位的 15 位系数打包为 32 位分块。
 * @param output 幅值输出。
 * @param digits 64 位存储的 15 位系数。
 * @return 操作状态。
 */
static ChaliCStatus pack_base15(ChaliBigInt *output, const ChaliCBuffer *digits)
{
    const uint64_t *data = digits->data;
    size_t bits = 0;
    for (size_t index = digits->length; index-- > 0;)
    {
        if (!data[index]) continue;
        bits = index * 15;
        for (uint64_t top = data[index]; top; top >>= 1) ++bits;
        break;
    }
    ChaliCStatus status = chali_c_buffer_resize(&output->limbs, (bits + 31) / 32);
    if (status != CHALI_C_OK) return status;
    size_t limb = 0, buffered = 0;
    uint64_t buffer = 0;
    for (size_t index = 0; index < digits->length; ++index)
    {
        buffer |= (data[index] & 0x7fff) << buffered;
        buffered += 15;
        while (buffered >= 32)
        {
            if (limb < output->limbs.length) cbn_words(output)[limb] = (uint32_t)buffer;
            ++limb;
            buffer >>= 32;
            buffered -= 32;
        }
    }
    if (buffered && limb < output->limbs.length) cbn_words(output)[limb] = (uint32_t)buffer;
    return CHALI_C_OK;
}
/**
 * @brief 双素数卷积、CRT 重建与 15 位进位。
 * @param output 幅值输出。
 * @param left 左幅值。
 * @param right 右幅值。
 * @return 操作状态。
 */
static ChaliCStatus ntt_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{
    ChaliCBuffer a, b, first, second, coefficients;
    const ChaliCAllocator *allocator = &output->limbs.allocator;
    chali_c_buffer_init(&a, sizeof(uint32_t), allocator);
    chali_c_buffer_init(&b, sizeof(uint32_t), allocator);
    chali_c_buffer_init(&first, sizeof(uint32_t), allocator);
    chali_c_buffer_init(&second, sizeof(uint32_t), allocator);
    chali_c_buffer_init(&coefficients, sizeof(uint64_t), allocator);
    ChaliCStatus status = to_base15(&a, left);
    if (status != CHALI_C_OK) goto done;
    status = to_base15(&b, right);
    if (status != CHALI_C_OK) goto done;
    status = cbn_ntt1_convolution(&first, &a, &b);
    if (status != CHALI_C_OK) goto done;
    status = cbn_ntt2_convolution(&second, &a, &b);
    if (status != CHALI_C_OK) goto done;
    const size_t count = first.length > second.length ? first.length : second.length;
    status = chali_c_buffer_resize(&coefficients, count);
    if (status != CHALI_C_OK) goto done;
    const uint64_t prime1 = 998244353, prime2 = 1004535809;
    const uint64_t inverse = cbn_ntt2_power((uint32_t)(prime1 % prime2), prime2 - 2);
    uint64_t carry = 0;
    for (size_t index = 0; index < count; ++index)
    {
        const uint64_t value1 = index < first.length ? ((uint32_t *)first.data)[index] : 0;
        const uint64_t value2 = index < second.length ? ((uint32_t *)second.data)[index] : 0;
        const uint64_t difference = (value2 + prime2 - value1 % prime2) % prime2;
        const uint64_t factor = difference * inverse % prime2;
        const uint64_t current = value1 + prime1 * factor + carry;
        ((uint64_t *)coefficients.data)[index] = current & 0x7fff;
        carry = current >> 15;
    }
    while (carry)
    {
        const uint64_t digit = carry & 0x7fff;
        status = chali_c_buffer_append(&coefficients, &digit, 1);
        if (status != CHALI_C_OK) goto done;
        carry >>= 15;
    }
    status = pack_base15(output, &coefficients);
done:
    chali_c_buffer_clear(&a);
    chali_c_buffer_clear(&b);
    chali_c_buffer_clear(&first);
    chali_c_buffer_clear(&second);
    chali_c_buffer_clear(&coefficients);
    return status;
}
/**
 * @brief 根据输入规模和平方特征选择当前分层算法。
 * @param left_size 左分块数。
 * @param right_size 右分块数。
 * @param square 是否为相同幅值。
 * @return 零表示零输入，一学校、二 Karatsuba、三 Toom、四 NTT。
 */
static int multiplication_route(size_t left_size, size_t right_size, int square)
{
    const size_t shorter = left_size < right_size ? left_size : right_size;
    uint64_t cost;
    int route;
    if (!shorter) return 0;
    if (ntt_threshold <= 1 && prefer_ntt(left_size, right_size, square, 0)) return 4;
    route = first_stage_route(left_size, right_size, square, &cost);
    if (prefer_ntt(left_size, right_size, square, cost)) return 4;
    return route;
}
/**
 * @brief 在隔离临时值上计算指定顶层算法，成功后提交。
 * @param output 目标。
 * @param left 左值。
 * @param right 右值。
 * @param algorithm 零自动、一学校、二 Karatsuba、三 Toom、四 NTT。
 * @return 操作状态。
 */
static ChaliCStatus multiply_using(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right, int algorithm)
{
    if (!output || !left || !right) return CHALI_C_INVALID_ARGUMENT;
    ChaliBigInt temporary;
    cbn_temporary(&temporary, output);
    ChaliCStatus status = CHALI_C_OK;
    if (left->sign && right->sign)
    {
        const int square = left == right || cbn_compare_magnitude(left, right) == 0;
        const int route = algorithm ? algorithm : multiplication_route(left->limbs.length, right->limbs.length, square);
        if (route == 1)
            status = square ? cbn_schoolbook_square(&temporary, left) : cbn_schoolbook_multiply(&temporary, left, right);
        else if (route == 2)
            status = karatsuba_multiply(&temporary, left, right);
        else if (route == 3)
            status = cbn_toom3_multiply(&temporary, left, right, square);
        else if (route == 4)
            status = ntt_multiply(&temporary, left, right);
        else
            status = CHALI_C_INVALID_ARGUMENT;
        if (status == CHALI_C_OK)
        {
            temporary.sign = left->sign * right->sign;
            cbn_normalize(&temporary);
        }
    }
    if (status == CHALI_C_OK) cbn_move(output, &temporary);
    chali_bigint_clear(&temporary);
    return status;
}
/**
 * @brief 按分层策略计算乘法，支持结果与任一输入别名。
 * @param output 目标。
 * @param left 左值。
 * @param right 右值。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right)
{ return multiply_using(output, left, right, 0); }
#ifdef CHALI_BIGINT_TESTING
/**
 * @brief 测试入口强制顶层算法，保留递归层生产策略。
 * @param output 目标。
 * @param left 左值。
 * @param right 右值。
 * @param algorithm 零自动、一学校、二 Karatsuba、三 Toom、四 NTT。
 * @return 操作状态。
 */
ChaliCStatus chali_bigint_test_multiply(ChaliBigInt *output, const ChaliBigInt *left, const ChaliBigInt *right, int algorithm)
{
    if (algorithm < 0 || algorithm > 4) return CHALI_C_INVALID_ARGUMENT;
    if (algorithm == 4 && left && right)
    {
        const size_t a = left->limbs.length, b = right->limbs.length;
        if (a > (SIZE_MAX - 14) / 32 || b > (SIZE_MAX - 14) / 32 || (a * 32 + 14) / 15 + (b * 32 + 14) / 15 > ((size_t)1 << 21)) return CHALI_C_RANGE_ERROR;
    }
    return multiply_using(output, left, right, algorithm);
}
/**
 * @brief 查询测试输入的自动算法编号。
 * @param left 左值。
 * @param right 右值。
 * @return 算法编号，非法或零输入返回零。
 */
int chali_bigint_test_multiplication_route(const ChaliBigInt *left, const ChaliBigInt *right)
{
    if (!left || !right || !left->sign || !right->sign) return 0;
    return multiplication_route(left->limbs.length, right->limbs.length,
                                left == right || cbn_compare_magnitude(left, right) == 0);
}
#endif
/**
 * @brief 设置学校乘法候选的短侧分块上限。
 * @param limbs 阈值，零关闭该基线。
 */
void chali_bigint_set_schoolbook_threshold(size_t limbs) { schoolbook_threshold = limbs; }
/**
 * @brief 读取学校乘法阈值。
 * @return 分块数。
 */
size_t chali_bigint_schoolbook_threshold(void) { return schoolbook_threshold; }
/**
 * @brief 设置学校平方阈值。
 * @param limbs 阈值，零关闭该基线。
 */
void chali_bigint_set_square_threshold(size_t limbs) { square_threshold = limbs; }
/**
 * @brief 读取学校平方阈值。
 * @return 分块数。
 */
size_t chali_bigint_square_threshold(void) { return square_threshold; }
/**
 * @brief 设置 Toom-Cook 3 乘法阈值。
 * @param limbs 阈值，最小为八。
 */
void chali_bigint_set_toom_threshold(size_t limbs) { toom_threshold = limbs < 8 ? 8 : limbs; }
/**
 * @brief 读取 Toom-Cook 3 乘法阈值。
 * @return 分块数。
 */
size_t chali_bigint_toom_threshold(void) { return toom_threshold; }
/**
 * @brief 设置 Toom-Cook 3 平方阈值。
 * @param limbs 阈值，最小为八。
 */
void chali_bigint_set_toom_square_threshold(size_t limbs) { toom_square_threshold = limbs < 8 ? 8 : limbs; }
/**
 * @brief 读取 Toom-Cook 3 平方阈值。
 * @return 分块数。
 */
size_t chali_bigint_toom_square_threshold(void) { return toom_square_threshold; }
/**
 * @brief 设置 NTT 候选阈值，保留原有强制值一。
 * @param limbs 总分块阈值。
 */
void chali_bigint_set_multiplication_threshold(size_t limbs) { ntt_threshold = limbs; }
/**
 * @brief 读取 NTT 阈值。
 * @return 阈值。
 */
size_t chali_bigint_multiplication_threshold(void) { return ntt_threshold; }
/**
 * @brief 设置短侧 Karatsuba 保护阈值。
 * @param limbs 阈值，零按一处理。
 */
void chali_bigint_set_karatsuba_threshold(size_t limbs) { karatsuba_threshold = limbs ? limbs : 1; }
/**
 * @brief 读取 Karatsuba 保护阈值。
 * @return 阈值。
 */
size_t chali_bigint_karatsuba_threshold(void) { return karatsuba_threshold; }
/**
 * @brief 设置 Burnikel-Ziegler 的 Knuth 递归基线。
 * @param limbs 阈值，最小为三十二。
 */
void chali_bigint_set_division_threshold(size_t limbs) { division_threshold = limbs < 32 ? 32 : limbs; }
/**
 * @brief 读取除法递归阈值。
 * @return 阈值。
 */
size_t chali_bigint_division_threshold(void) { return division_threshold; }
