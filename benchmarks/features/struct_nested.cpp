/** @file struct_nested.cpp
 * @brief 嵌套结构体、字段数组与跨层访问的 C++ 参考。
 */
#include <cstdint>
/** @brief 三分量坐标或速度。 */
struct Vector3 { std::int64_t x, y, z; };
/** @brief 含两层坐标及定长权重的变换。 */
struct Transform { Vector3 position, velocity; std::int64_t weights[4]; };
/** @brief 含嵌套变换、载荷数组及标志的记录。 */
struct Record { Transform transform; std::int64_t payload[8], flags; };
static Record records[256];
/**
 * @brief 在计时前构造全部记录及字段数组。
 */
extern "C" void feature_setup()
{
    for (std::int64_t i = 0; i < 256; ++i)
    {
        auto &record = records[i];
        record.transform.position = { i * 3 + 1, i * 5 + 2, i * 7 + 3 };
        record.transform.velocity = { i + 4, i + 5, i + 6 };
        record.flags = i * 11 + 9;
        for (std::int64_t k = 0; k < 4; ++k) record.transform.weights[k] = i * 13 + k;
        for (std::int64_t k = 0; k < 8; ++k) record.payload[k] = i * 17 + k * 3;
    }
}
/**
 * @brief 读取嵌套字段及两个不同长度的数组字段。
 * @param seed 初始归并值。
 * @return 归并校验值。
 */
extern "C" std::int64_t feature_work(std::int64_t seed)
{
    auto acc = seed;
    for (std::int64_t i = 0; i < 262144; ++i)
    {
        const auto &record = records[(i * 13 + (acc & 255)) & 255];
        const auto value = record.transform.position.x + record.transform.position.z
                           + record.transform.velocity.y + record.transform.weights[i & 3]
                           + record.payload[(i + seed) & 7] + record.flags;
        acc = (acc * 33 + value) & 2147483647;
    }
    return acc;
}
