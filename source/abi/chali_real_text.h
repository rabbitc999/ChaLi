/**
 * @file chali_real_text.h
 * @brief C 运行库浮点文本转换接口。
 */
#ifndef CHALI_REAL_TEXT_H
#define CHALI_REAL_TEXT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 输出双精度数的最短往返文本，使用小数点和规范指数。
 * @param value IEEE-754 双精度值。
 * @param output 调用方缓冲区。
 * @param capacity 缓冲区字节数，包含结尾零。
 * @return 文本字节数；缓冲区不足时返回零。
 */
size_t chali_format_real(double value, char *output, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
