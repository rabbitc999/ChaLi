#include <cstdint>
#include <string>

namespace {
/** @brief 保存不计时构造的 UTF-8 输入与拼接块。 */
struct Data {
    std::string text, first, second;
    std::size_t width = 0;
};
Data data;
constexpr std::int64_t mask = 2147483647;
/**
 * @brief 按无符号字节归并字符串，防止只使用长度而消除构造内容。
 * @param text 已完成构造的字符串。
 * @param seed 初始归并值。
 * @return 三十一位归并结果。
 */
std::int64_t checksum(const std::string &text, std::int64_t seed) {
    for (const unsigned char value : text) seed = (seed * 33 + value) & mask;
    return seed;
}
}

/** @brief 在计时之前构造二百五十六个完整 UTF-8 单元，不预留被测结果容量。 */
extern "C" void feature_setup() {
    const std::string unit = u8"ChaLi-字符串-🙂-0123456789|";
    data.first = unit + "a"; data.second = unit + "B";
    data.width = data.first.size(); data.text.clear();
    for (int index = 0; index < 256; ++index) {
        data.text += unit;
        data.text.push_back(static_cast<char>('A' + index % 26));
    }
}

/**
 * @brief 以依赖索引读取 UTF-8 的原始字节，不解码码点。
 * @param seed 索引与归并的初值。
 * @return 三十一位归并结果。
 */
extern "C" std::int64_t string_read(std::int64_t seed) {
    const auto &source = data.text;
    const auto length = static_cast<std::int64_t>(source.size());
    for (std::int64_t index = 0; index < 131072; ++index) {
        const auto at = (index * 13 + ((seed >> 7) & 65535)) % length;
        seed = (seed * 33 + static_cast<unsigned char>(source[static_cast<std::size_t>(at)])) & mask;
    }
    return seed;
}

/**
 * @brief 从空字符串逐次追加，不调用 reserve，最终归并全部字节。
 * @param seed 追加块选择及校验初值。
 * @return 三十一位归并结果。
 */
extern "C" std::int64_t string_concat(std::int64_t seed) {
    std::string result;
    for (std::int64_t index = 0; index < 2048; ++index)
        result += ((index + seed) & 1) == 0 ? data.first : data.second;
    return checksum(result, seed);
}

/**
 * @brief 在完整单元边界创建两个拥有型子串并比较，覆盖相同与不同内容。
 * @param seed 端点与归并初值。
 * @return 三十一位归并结果。
 */
extern "C" std::int64_t string_slice(std::int64_t seed) {
    const auto &source = data.text;
    const auto initial = seed;
    for (std::int64_t index = 0; index < 1024; ++index) {
        const auto block = (index * 13 + (seed & 127)) & 127;
        const auto start = static_cast<std::size_t>(block) * data.width;
        const auto width = static_cast<std::size_t>(((index + initial) & 3) + 1) * data.width;
        const auto other = (index & 1) == 0 ? start : static_cast<std::size_t>((block + 13) & 127) * data.width;
        const auto part = source.substr(start, width), compared = source.substr(other, width);
        seed = (seed * 33 + static_cast<unsigned char>(part.front()) +
                static_cast<unsigned char>(part.back()) + (part == compared ? 1 : 0)) & mask;
    }
    return seed;
}

/**
 * @brief 从单元边界查找 UTF-8 和 ASCII 模式，包含缺失模式。
 * @param seed 查找起点、模式选择和归并初值。
 * @return 将未命中统一为负一后的三十一位归并结果。
 */
extern "C" std::int64_t string_find(std::int64_t seed) {
    const char *patterns[] = {u8"字符串", u8"🙂", u8"不存在!", "012345"};
    const auto initial = seed;
    for (std::int64_t index = 0; index < 256; ++index) {
        const auto start = static_cast<std::size_t>(((seed >> 7) + index) & 255) * data.width;
        const auto found = data.text.find(patterns[(index + initial) & 3], start);
        const auto position = found == std::string::npos ? -1 : static_cast<std::int64_t>(found);
        seed = (seed * 33 + position + 1) & mask;
    }
    return seed;
}
