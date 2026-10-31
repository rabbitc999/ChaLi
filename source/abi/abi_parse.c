/**
 * @file abi_parse.c
 * @brief 浮点文本转换的窄 C 原语；整数和布尔算法由 ChaLi 实现。
 */
#include "chali_abi.h"
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 以标准库原有 strtod 规则完整转换有限浮点数。
 * @param frame 包含一个 String 逻辑句柄的受检查调用帧。
 * @param value 成功时接收浮点值。
 * @return 0 成功，1 空文本，2 范围错误，3 格式错误；服务失败另记入帧。
 */
static int64_t parseFloat(ChaliAbiFrame *frame, double *value)
{
    int64_t handle;
    size_t length = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return 3;
    const unsigned char *bytes = NULL;
    if (!chali_abi_bytes(frame, handle, &bytes, &length)) return 3;
    if (!length) return 1;
    if (length == SIZE_MAX)
    {
        chali_abi_fail(frame, "浮点解析输入过长");
        return 3;
    }
    char *text = (char *)malloc(length + 1);
    if (!text)
    {
        chali_abi_fail(frame, "浮点解析缓冲区分配失败");
        return 3;
    }
    memcpy(text, bytes, length);
    text[length] = '\0';
    errno = 0;
    char *end = NULL;
    const double parsed = strtod(text, &end);
    const int rangeError = errno == ERANGE || !isfinite(parsed);
    const int complete = end == text + length;
    free(text);
    if (rangeError) return 2;
    if (!complete) return 3;
    *value = parsed;
    return 0;
}

/**
 * @brief 返回完整浮点转换状态，不转移任何临时缓冲区。
 * @param frame 单个 String 输入和整数状态输出。
 */
static void parseFloatError(ChaliAbiFrame *frame)
{
    double value = 0.0;
    chali_abi_return_integer(frame, parseFloat(frame, &value));
}

/**
 * @brief 返回浮点转换值，保留旧值入口的防御性错误诊断。
 * @param frame 单个 String 输入和 float64 输出。
 */
static void parseFloatValue(ChaliAbiFrame *frame)
{
    double value = 0.0;
    if (parseFloat(frame, &value) != 0)
        chali_abi_fail(frame, "std::lang::Parse.parseFloat64 输入无效");
    chali_abi_return_real(frame, value);
}

/**
 * @brief 取得浮点解析原语的只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_parse_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::parse::__parseFloat64Error", parseFloatError, 1 },
        { "std::parse::__parseFloat64Value", parseFloatValue, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
