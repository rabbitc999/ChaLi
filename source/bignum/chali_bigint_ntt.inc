/**
 * @file chali_bigint_ntt.inc
 * @brief 按编译期素数实例化的 C NTT 内核，保持双素数卷积算法。
 */
#define CBN_JOIN_INNER(a, b) a##b
#define CBN_JOIN(a, b)       CBN_JOIN_INNER(a, b)
#define CBN_NTT(name)        CBN_JOIN(CBN_NTT_PREFIX, name)
_Static_assert(CBN_NTT_MODULUS < UINT32_C(0x80000000), "NTT correction masks require a modulus below 2^31");
/**
 * @brief 固定素数下的模幂。
 * @param base 底数。
 * @param exponent 非负指数。
 * @return 模幂结果。
 */
static uint32_t CBN_NTT(power)(uint32_t base, uint64_t exponent)
{
    const uint64_t modulus = CBN_NTT_MODULUS;
    uint64_t result = 1;
    uint64_t value = base % modulus;
    while (exponent)
    {
        if (exponent & 1) result = result * value % modulus;
        value = value * value % modulus;
        exponent >>= 1;
    }
    return (uint32_t)result;
}
/**
 * @brief 进行原地正变换或逆变换。
 * @param values 幂二长度的 32 位系数缓冲区。
 * @param inverse 非零时执行逆变换。
 * @return 操作状态。
 */
static ChaliCStatus CBN_NTT(transform)(ChaliCBuffer *values, int inverse)
{
    const uint64_t modulus = CBN_NTT_MODULUS;
    const size_t count = values->length;
    uint32_t *data = values->data;
    ChaliCBuffer weights;
    chali_c_buffer_init(&weights, sizeof(uint32_t), &values->allocator);
    ChaliCStatus status = chali_c_buffer_resize(&weights, count / 2);
    if (status != CHALI_C_OK) return status;
    uint32_t *roots = weights.data;
    for (size_t index = 0; index < weights.length; ++index) roots[index] = 1;
    for (size_t index = 1, reverse = 0; index < count; ++index)
    {
        size_t bit = count >> 1;
        for (; reverse & bit; bit >>= 1) reverse ^= bit;
        reverse ^= bit;
        if (index < reverse)
        {
            const uint32_t temporary = data[index];
            data[index] = data[reverse];
            data[reverse] = temporary;
        }
    }
    for (size_t length = 2; length <= count; length <<= 1)
    {
        uint32_t root = CBN_NTT(power)(3, (modulus - 1) / length);
        if (inverse) root = CBN_NTT(power)(root, modulus - 2);
        const size_t half = length / 2;
        for (size_t offset = 1; offset < half; ++offset)
            roots[offset] = (uint32_t)((uint64_t)roots[offset - 1] * root % modulus);
        for (size_t start = 0; start < count; start += length)
        {
            for (size_t offset = 0; offset < half; ++offset)
            {
                const uint32_t even = data[start + offset];
                const uint32_t odd = (uint32_t)((uint64_t)data[start + offset + half] * roots[offset] % modulus);
                /* 两个素数均小于 2^31，减模后的最高位可直接形成无分支修正掩码。
                 * 避免旧 GCC 为随机蝶形差值生成不可预测的条件跳转。 */
                uint32_t sum = even + odd - (uint32_t)modulus;
                sum += (0u - (sum >> 31)) & (uint32_t)modulus;
                uint32_t difference = even - odd;
                difference += (0u - (difference >> 31)) & (uint32_t)modulus;
                data[start + offset] = sum;
                data[start + offset + half] = difference;
            }
        }
    }
    if (inverse)
    {
        const uint32_t scale = CBN_NTT(power)((uint32_t)count, modulus - 2);
        for (size_t index = 0; index < count; ++index)
            data[index] = (uint32_t)((uint64_t)data[index] * scale % modulus);
    }
    chali_c_buffer_clear(&weights);
    return CHALI_C_OK;
}
/**
 * @brief 计算一个固定素数下的卷积，相等输入复用正变换。
 * @param output 接收有效卷积系数。
 * @param left 非空左系数。
 * @param right 非空右系数。
 * @return 操作状态。
 */
static ChaliCStatus CBN_NTT(convolution)(ChaliCBuffer *output, const ChaliCBuffer *left, const ChaliCBuffer *right)
{
    const uint64_t modulus = CBN_NTT_MODULUS;
    const size_t used = left->length + right->length - 1;
    size_t count = 1;
    while (count < used) count <<= 1;
    const int square = left->length == right->length && memcmp(left->data, right->data, left->length * sizeof(uint32_t)) == 0;
    ChaliCBuffer a, b;
    chali_c_buffer_init(&a, sizeof(uint32_t), &output->allocator);
    chali_c_buffer_init(&b, sizeof(uint32_t), &output->allocator);
    ChaliCStatus status = chali_c_buffer_resize(&a, count);
    if (status != CHALI_C_OK) goto done;
    status = chali_c_buffer_resize(&b, square ? 0 : count);
    if (status != CHALI_C_OK) goto done;
    uint32_t *first = a.data;
    uint32_t *second = b.data;
    const uint32_t *left_data = left->data;
    const uint32_t *right_data = right->data;
    for (size_t index = 0; index < left->length; ++index)
        first[index] = left_data[index] >= modulus ? left_data[index] - (uint32_t)modulus : left_data[index];
    for (size_t index = 0; !square && index < right->length; ++index)
        second[index] = right_data[index] >= modulus ? right_data[index] - (uint32_t)modulus : right_data[index];
    status = CBN_NTT(transform)(&a, 0);
    if (status != CHALI_C_OK) goto done;
    if (!square)
    {
        status = CBN_NTT(transform)(&b, 0);
        if (status != CHALI_C_OK) goto done;
    }
    for (size_t index = 0; index < count; ++index)
        first[index] = (uint32_t)((uint64_t)first[index] * (square ? first[index] : second[index]) % modulus);
    status = CBN_NTT(transform)(&a, 1);
    if (status != CHALI_C_OK) goto done;
    a.length = used;
    chali_c_buffer_clear(output);
    *output = a;
    a.data = NULL;
    a.length = a.capacity = 0;
done:
    chali_c_buffer_clear(&a);
    chali_c_buffer_clear(&b);
    return status;
}
#undef CBN_NTT
#undef CBN_JOIN
#undef CBN_JOIN_INNER
