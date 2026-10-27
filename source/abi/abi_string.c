/**
 * @file abi_string.c
 * @brief `std::lang` 字符串构造类胶水的直通 C 实现。
 *
 * 目前只有浮点格式化入口 `__std_str_of_flt`：编译器把 `{}` 里的浮点实参降级成
 * 对这个函数的调用，再由它分配一个新的 VM 字符串返回。数值 → 文本的规则必须与
 * 语言默认完全一致（默认 3 位小数、去掉尾随零、`2.0` 仍保留一位），否则
 * `println` 的输出会整体变化。
 */
#include "chali_abi_internal.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief 文本缓冲区容量；与旧实现一致，超长结果按此截断。
 */
#define CHALI_ABI_TEXT_CAPACITY 256

/**
 * @brief 把双精度值格式化成语言默认的文本。
 * @param frame 帧：0 = 值，1 = 小数位数（负数或缺失按 3 位）。
 * @note 先按 `%.*f` 打印，再在存在小数点时去掉尾随零，最后去掉可能剩下的孤立小数点，
 *       于是 `3.500` → `3.5`、`2.000` → `2.0`。
 */
static void abiStrOfFloat(ChaliAbiFrame *frame)
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
    size_t length = (size_t)written < sizeof(buffer) ? (size_t)written : sizeof(buffer) - 1u;

    if (precision > 0 && memchr(buffer, '.', length) != NULL)
    {
        while (length > 0u && buffer[length - 1u] == '0') --length;
        if (length > 0u && buffer[length - 1u] == '.') --length;
    }
    chali_abi_return_integer(frame, chali_abi_string(frame, buffer, length));
}

/**
 * @brief 本模块入口表，按名称升序排列。
 */
static const ChaliAbiRecord kRecords[] = {
    { "__std_str_of_flt", abiStrOfFloat, 2 },
};

const ChaliAbiRecord *abi_string_records(size_t *count)
{
    if (count != NULL) *count = sizeof(kRecords) / sizeof(kRecords[0]);
    return kRecords;
}
