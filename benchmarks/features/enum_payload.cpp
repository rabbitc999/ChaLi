/** @file enum_payload.cpp
 * @brief 载荷枚举的 C++17 std::variant 借用匹配参考。
 */
#include <array>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <variant>
/** @brief 枚举中的嵌套双字段载荷。 */
struct Payload { std::int64_t x, y; };
using Event = std::variant<std::monostate, std::int64_t, std::pair<std::int64_t, std::int64_t>, Payload>;
static std::array<Event, 256> events;
/**
 * @brief 在计时前构造四种变体，不在匹配循环中复制整个枚举。
 */
extern "C" void feature_setup()
{
    for (std::int64_t i = 0; i < 256; ++i)
    {
        if (i % 4 == 0) events[i] = std::monostate{};
        else if (i % 4 == 1) events[i] = i * 3 + 7;
        else if (i % 4 == 2) events[i] = std::make_pair(i * 5, i + 11);
        else events[i] = Payload{ i * 7, i + 13 };
    }
}
/**
 * @brief 借用访问四种载荷并执行与 ChaLi／Rust 相同的归并。
 * @param seed 初始归并值。
 * @return 归并校验值。
 */
extern "C" std::int64_t feature_work(std::int64_t seed)
{
    auto acc = seed;
    for (std::int64_t i = 0; i < 262144; ++i)
    {
        const auto &event = events[(i * 13 + ((acc >> 7) & 255)) & 255];
        const auto value = std::visit([](const auto &payload) -> std::int64_t {
            using T = std::decay_t<decltype(payload)>;
            if constexpr (std::is_same_v<T, std::monostate>) return 17;
            else if constexpr (std::is_same_v<T, std::int64_t>) return payload;
            else if constexpr (std::is_same_v<T, Payload>) return payload.x + payload.y;
            else return payload.first + payload.second;
        }, event);
        acc = (acc * 33 + value) & 2147483647;
    }
    return acc;
}
