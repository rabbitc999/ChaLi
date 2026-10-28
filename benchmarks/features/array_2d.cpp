/** @file array_2d.cpp
 * @brief 原生二维数组访问参考，保持与 ChaLi 相同的依赖索引。
 */
#include <cstdint>
static std::int64_t matrix[64][64];
/**
 * @brief 在计时前填写全部二维元素。
 */
extern "C" void feature_setup()
{
    for (std::int64_t row = 0; row < 64; ++row)
        for (std::int64_t column = 0; column < 64; ++column)
            matrix[row][column] = row * 131 + column * 17 + 3;
}
/**
 * @brief 通过依赖前次结果的二维索引读取并归并。
 * @param seed 初始归并值。
 * @return 归并校验值。
 */
extern "C" std::int64_t feature_work(std::int64_t seed)
{
    auto acc = seed;
    for (std::int64_t i = 0; i < 262144; ++i)
        acc = (acc * 33 + matrix[(acc >> 6) & 63][(i + acc) & 63]) & 2147483647;
    return acc;
}
