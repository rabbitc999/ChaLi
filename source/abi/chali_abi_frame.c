/**
 * @file chali_abi_frame.c
 * @brief ABI 槽位读写，独立于全量模块入口表。
 */
#include "chali_abi_internal.h"

#include <string.h>

/**
 * @brief 读取整数参数槽。
 * @param frame 调用帧。
 * @param index 参数索引。
 * @param out 接收整数的地址。
 * @return 读取成功时返回 1，否则返回 0。
 */
int chali_abi_argument_integer(const ChaliAbiFrame *frame, int64_t index, int64_t *out)
{
    if (out == NULL)
    {
        chali_abi_fail(frame, "missing integer argument output");
        return 0;
    }
    *out = 0;
    if (frame == NULL || frame->slots == NULL || frame->kinds == NULL || index < 0 || index >= frame->count || frame->kinds[index] != ChaliAbiSlotInteger)
    {
        chali_abi_fail(frame, "invalid integer argument slot");
        return 0;
    }
    *out = frame->slots[index];
    return 1;
}

/**
 * @brief 读取浮点参数槽。
 * @param frame 调用帧。
 * @param index 参数索引。
 * @param out 接收浮点值的地址。
 * @return 读取成功时返回 1，否则返回 0。
 */
int chali_abi_argument_real(const ChaliAbiFrame *frame, int64_t index, double *out)
{
    if (out == NULL)
    {
        chali_abi_fail(frame, "missing floating argument output");
        return 0;
    }
    *out = 0.0;
    if (frame == NULL || frame->slots == NULL || frame->kinds == NULL || index < 0 || index >= frame->count || frame->kinds[index] != ChaliAbiSlotFloat)
    {
        chali_abi_fail(frame, "invalid floating argument slot");
        return 0;
    }
    const int64_t bits = frame->slots[index];
    double value = 0.0;
    memcpy(&value, &bits, sizeof(value));
    *out = value;
    return 1;
}

/**
 * @brief 读取参数槽的类型。
 * @param frame 调用帧。
 * @param index 参数索引。
 * @return 参数槽类型，读取失败时登记错误并返回整数类型。
 */
int chali_abi_argument_kind(const ChaliAbiFrame *frame, int64_t index)
{
    if (frame == NULL || frame->kinds == NULL || index < 0 || index >= frame->count || frame->kinds[index] > ChaliAbiSlotFloat)
    {
        chali_abi_fail(frame, "invalid argument slot kind");
        return ChaliAbiSlotInteger;
    }
    return frame->kinds[index];
}

/**
 * @brief 写入整数返回值。
 * @param frame 调用帧。
 * @param value 返回值。
 */
void chali_abi_return_integer(ChaliAbiFrame *frame, int64_t value)
{
    if (frame == NULL) return;
    frame->result = value;
    frame->resultIsFloat = 0;
}

/**
 * @brief 取得浮点值的槽位表示。
 * @param value 待转换的浮点值。
 * @return 保存原始位模式的整数槽。
 */
int64_t chali_abi_slot_from_real(double value)
{
    int64_t bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

/**
 * @brief 写入浮点返回值。
 * @param frame 调用帧。
 * @param value 返回值。
 */
void chali_abi_return_real(ChaliAbiFrame *frame, double value)
{
    if (frame == NULL) return;
    frame->result = chali_abi_slot_from_real(value);
    frame->resultIsFloat = 1;
}

/* ---------------------------------------------------------------------------
 *  帧基础访问
 * ------------------------------------------------------------------------- */

/**
 * @brief 读取整数参数，失败时登记错误。
 * @param frame 调用帧。
 * @param index 参数索引。
 * @return 整数参数值，读取失败时返回零。
 */
int64_t chali_abi_integer(const ChaliAbiFrame *frame, int64_t index)
{
    int64_t value = 0;
    chali_abi_argument_integer(frame, index, &value);
    return value;
}

/**
 * @brief 读取浮点参数，失败时登记错误。
 * @param frame 调用帧。
 * @param index 参数索引。
 * @return 浮点参数值，读取失败时返回零。
 */
double chali_abi_real(const ChaliAbiFrame *frame, int64_t index)
{
    double value = 0.0;
    chali_abi_argument_real(frame, index, &value);
    return value;
}
