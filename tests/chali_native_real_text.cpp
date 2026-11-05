/**
 * @file chali_native_real_text.cpp
 * @brief 将轻量浮点文本与标准库最短格式做位模式差分。
 */
#include "../source/abi/chali_real_text.h"
#include <charconv>
#include <cfenv>
#include <clocale>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <random>

/**
 * @brief 比较一个 binary64 值的完整文本并核对舍入方式保持。
 * @param value 输入值。
 * @return 文本和舍入状态一致时返回 true。
 */
bool verify(double value)
{
    char reference[128];
    const auto expected = std::to_chars(reference, reference + sizeof(reference), value);
    const int rounding = std::fegetround();
    char actual[128];
    const auto size = chali_format_real(value, actual, sizeof(actual));
    const bool same = expected.ec == std::errc{} && size == static_cast<std::size_t>(expected.ptr - reference)
                      && std::memcmp(actual, reference, size) == 0 && std::fegetround() == rounding;
    if (!same) std::fprintf(stderr, "float mismatch %.17g expected=%.*s actual=%s\n",
                            value, static_cast<int>(expected.ptr - reference), reference, actual);
    return same;
}

/**
 * @brief 覆盖边界值、随机位模式、舍入方式和当前平台可用的小数逗号区域设置。
 * @return 全部通过时返回零。
 */
int main()
{
    const double limits[] = {0.0, -0.0, 1.0, -1.0, 0.1, 1e-6, 1e-7, 1e20, 1e21,
                             std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
                             std::numeric_limits<double>::denorm_min(), std::numeric_limits<double>::infinity(),
                             -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN(),
                             -std::numeric_limits<double>::quiet_NaN()};
    for (int rounding : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO})
    {
        if (std::fesetround(rounding) != 0) return 1;
        for (double value : limits)
            if (!verify(value)) return 1;
    }
    std::fesetround(FE_TONEAREST);
    std::mt19937_64 random(12345);
    for (int index = 0; index < 1000000; ++index)
    {
        const auto bits = random();
        double value;
        std::memcpy(&value, &bits, sizeof(value));
        if (!verify(value)) return 1;
    }
    bool comma = false;
    for (const char *locale : {"de-DE", "German_Germany.1252", "de_DE.UTF-8", "fr_FR.UTF-8"})
    {
        if (!std::setlocale(LC_NUMERIC, locale)) continue;
        comma = true;
        for (double value : limits)
            if (!verify(value)) return 1;
        break;
    }
    std::setlocale(LC_NUMERIC, "C");
    std::printf("float text: 1000000 bit patterns, boundaries and rounding modes passed; comma locale %s\n",
                comma ? "passed" : "unavailable");
}
