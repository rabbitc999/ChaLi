/**
 * @file abi_string.c
 * @brief `std::lang` 字符串构造原语的直通 C 实现。
 *
 * 浮点 → 文本的完整规则（去尾随零、孤立小数点）在 std/lang.chl 的 ChaLi 函数
 * `__std_str_of_flt` 里实现；本模块只保留不可约的格式化核心 `__std_str_fixed`：
 * 按 `%.*f` 输出固定小数位文本。AOT 直连降级时由生成代码直接调用 libc snprintf，
 * 不经过本表。
 */
#include "chali_abi_internal.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief 文本缓冲区容量；与旧实现一致，超长结果按此截断。
 */
#define CHALI_ABI_TEXT_CAPACITY 256

/**
 * @brief 把双精度值按 `%.*f` 格式化成固定小数位文本。
 * @param frame 帧：0 = 值，1 = 小数位数（负数按 3 位）。
 * @return 新 VM 字符串；不含去尾零处理。
 */
static void abiStrFixed(ChaliAbiFrame *frame)
{
    double value = 0.0;
    int64_t precision = 3;
    chali_abi_argument_real(frame, 0, &value);
    chali_abi_argument_integer(frame, 1, &precision);
    if (precision < 0) precision = 3;

    char buffer[CHALI_ABI_TEXT_CAPACITY];
    const int written = snprintf(buffer, sizeof(buffer), "%.*f", (int)precision, value);
    if (written < 0)
    {
        chali_abi_fail(frame, "浮点数格式化失败");
        chali_abi_return_integer(frame, 0);
        return;
    }
    const size_t length = (size_t)written < sizeof(buffer) ? (size_t)written : sizeof(buffer) - 1u;
    chali_abi_return_integer(frame, chali_abi_string(frame, buffer, length));
}

/**
 * @brief 本模块入口表，按名称升序排列。
 */
static const ChaliAbiRecord kRecords[] = {
    { "std::lang::__std_str_fixed", abiStrFixed, 2 },
};

const ChaliAbiRecord *abi_string_records(size_t *count)
{
    if (count != NULL) *count = sizeof(kRecords) / sizeof(kRecords[0]);
    return kRecords;
}
