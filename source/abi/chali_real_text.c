/**
 * @file chali_real_text.c
 * @brief 不依赖 C++ 标准库的最短往返浮点格式化。
 */
#include "chali_real_text.h"
#include <errno.h>
#include <fenv.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 将当前数值区域设置的小数分隔符归一化为小数点。
 * @param text 可修改的数值缓冲区。
 */
static void decimal_point(char *text)
{
    const char *separator = localeconv()->decimal_point;
    if (separator && *separator && strcmp(separator, ".") != 0)
    {
        char *at = strstr(text, separator);
        if (at)
        {
            const size_t size = strlen(separator);
            *at = '.';
            memmove(at + 1, at + size, strlen(at + size) + 1);
        }
    }
}

/**
 * @brief 有界复制已经格式化的数值。
 * @param text 数值文本。
 * @param output 调用方缓冲区。
 * @param capacity 缓冲区字节数。
 * @return 文本长度；缓冲区不足时返回零。
 */
static size_t copy_text(const char *text, char *output, size_t capacity)
{
    const size_t size = strlen(text);
    if (!output || size >= capacity) return 0;
    memcpy(output, text, size + 1);
    return size;
}

/**
 * @brief 搜索最少有效数字，再选择固定或指数表示。
 * @param value 输入双精度数。
 * @param output 输出缓冲区。
 * @param capacity 缓冲区字节数。
 * @return 文本长度；转换或缓冲区校验失败时返回零。
 */
static size_t format_real(double value, char *output, size_t capacity)
{
    if (isnan(value)) return copy_text(signbit(value) ? "-nan" : "nan", output, capacity);
    if (isinf(value)) return copy_text(signbit(value) ? "-inf" : "inf", output, capacity);
    if (value == 0) return copy_text(signbit(value) ? "-0" : "0", output, capacity);
    char scientific[64];
    for (int precision = 1; precision <= 17; ++precision)
    {
        const int count = snprintf(scientific, sizeof(scientific), "%.*e", precision - 1, value);
        if (count < 0 || (size_t)count >= sizeof(scientific)) return 0;
        if (strtod(scientific, NULL) != value) continue;
        decimal_point(scientific);
        char *exponent_at = strchr(scientific, 'e');
        if (!exponent_at) return 0;
        const int exponent = atoi(exponent_at + 1);
        const size_t remaining = sizeof(scientific) - (size_t)(exponent_at - scientific);
        const int suffix = snprintf(exponent_at, remaining, "e%+03d", exponent);
        if (suffix < 0 || (size_t)suffix >= remaining) return 0;
        /* binary64 的固定表示最多包含 309 位整数或 340 位小数。 */
        char fixed[768];
        const int decimals = precision > exponent + 1 ? precision - exponent - 1 : 0;
        const int size = snprintf(fixed, sizeof(fixed), "%.*f", decimals, value);
        if (size < 0 || (size_t)size >= sizeof(fixed)) return 0;
        decimal_point(fixed);
        return copy_text(strlen(fixed) <= strlen(scientific) ? fixed : scientific, output, capacity);
    }
    return 0;
}

/**
 * @brief 在最近舍入下执行转换，随后恢复调用方的舍入配置和 errno。
 * @param value 输入双精度数。
 * @param output 输出缓冲区。
 * @param capacity 缓冲区字节数。
 * @return 文本长度；转换或缓冲区校验失败时返回零。
 */
size_t chali_format_real(double value, char *output, size_t capacity)
{
    const int error = errno;
    const int rounding = fegetround();
    if (rounding != FE_TONEAREST && rounding != -1) fesetround(FE_TONEAREST);
    const size_t size = format_real(value, output, capacity);
    if (rounding != FE_TONEAREST && rounding != -1) fesetround(rounding);
    errno = error;
    return size;
}
