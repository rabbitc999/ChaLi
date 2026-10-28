/** @file array_3d.cpp
 * @brief 原生三维数组的跨层访问参考。
 */
#include <cstdint>
static std::int64_t cube[16][16][16];
/**
 * @brief 在计时前填写全部三维元素。
 */
extern "C" void feature_setup()
{
    for (std::int64_t x = 0; x < 16; ++x)
        for (std::int64_t y = 0; y < 16; ++y)
            for (std::int64_t z = 0; z < 16; ++z)
                cube[x][y][z] = x * 257 + y * 31 + z * 7 + 3;
}
/**
 * @brief 通过依赖索引读取三维数组并归并。
 * @param seed 初始归并值。
 * @return 归并校验值。
 */
extern "C" std::int64_t feature_work(std::int64_t seed)
{
    auto acc = seed;
    for (std::int64_t i = 0; i < 262144; ++i)
        acc = (acc * 33 + cube[(acc >> 4) & 15][(acc >> 8) & 15][(i + seed) & 15]) & 2147483647;
    return acc;
}
