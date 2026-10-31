/**
 * @file abi_demo.c
 * @brief 以纯 C 演示名义句柄、聚合槽、接口方法、同步闭包及可空引用。
 */
#include "chali_standard.h"
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
/** @brief 编译器与 AOT 共用的演示操作。 */
enum Operation
{
    CounterFrom,
    CounterTo,
    CounterAdd,
    IdentityI32,
    TextSize,
    VariadicSum,
    Apply,
    IdentityEnum,
    EnumTag,
    EnumPayload,
    PointArea,
    PointScaled,
    SignalCode,
    CubeArea,
    CubeScaled,
    CubeUnit,
    BufferSize,
    BufferIndex,
    BufferMul,
    BufferConvert,
    BufferAdd,
    RectArea,
    RectScaled,
    RectPerimeter,
    PointerBump,
    PointerSum,
    PointerOffset,
    PointerAt,
    ReferenceOptional,
    ReferenceRead,
    ReferenceWrite
};
/**
 * @brief 保留整数返回值的完整位模式，避免无符号高位的有符号溢出。
 * @param value 原始位模式。
 * @return 对应的整数槽。
 */
static int64_t signedBits(uint64_t value)
{
    int64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}
/**
 * @brief 按八字节槽计算逻辑偏移，不将语言地址转换成宿主指针。
 * @param address 逻辑块地址。
 * @param index 槽偏移。
 * @return 逻辑地址位模式；越界由后续受检查读写报告。
 */
static int64_t slotAddress(int64_t address, int64_t index) { return signedBits((uint64_t)address + (uint64_t)index * 8u); }
/**
 * @brief 从存活聚合或引用读取原始整数槽。
 * @param frame 当前帧。
 * @param address 逻辑地址。
 * @param out 接收位模式。
 * @return 完整读取成功为一。
 */
static int readWord(const ChaliAbiFrame *frame, int64_t address, int64_t *out)
{
    if (!address)
    {
        chali_abi_fail(frame, "胶水层读取了空的 struct/enum 值");
        return 0;
    }
    return chali_abi_read(frame, address, out, sizeof(*out));
}
/**
 * @brief 向受检查逻辑槽写入原始位模式。
 * @param frame 当前帧。
 * @param address 逻辑地址。
 * @param value 待写入整数槽。
 * @return 写入成功为一。
 */
static int writeWord(const ChaliAbiFrame *frame, int64_t address, int64_t value) { return chali_abi_write(frame, address, &value, sizeof(value)); }
/**
 * @brief 解码 Option<引用> 的既有标签，不为空值构造无效引用。
 * @param frame 当前帧。
 * @param address Option 的聚合存储。
 * @param out 接收非空引用，None 为零。
 * @return 标签与载荷有效为一。
 */
static int optionalReference(const ChaliAbiFrame *frame, int64_t address, int64_t *out)
{
    int64_t tag = 0;
    if (!address)
    {
        chali_abi_fail(frame, "Option 引用缺少枚举存储");
        return 0;
    }
    if (!readWord(frame, address, &tag)) return 0;
    if (tag == 1)
    {
        *out = 0;
        return 1;
    }
    if (tag != 0)
    {
        chali_abi_fail(frame, "Option 引用标签无效");
        return 0;
    }
    if (!readWord(frame, slotAddress(address, 1), out)) return 0;
    if (!*out)
    {
        chali_abi_fail(frame, "Option::Value 不能包含零地址引用");
        return 0;
    }
    return 1;
}
/**
 * @brief 直接执行一次演示操作，所有语言值均由 C 帧及逻辑内存服务读取。
 * @param frame 已检查数量的调用帧。
 * @param operation 演示操作编号。
 */
static void execute(ChaliAbiFrame *frame, enum Operation operation)
{
    int64_t a = 0, b = 0, c = 0, x = 0, y = 0, result = 0;
    frame->result = 0;
    frame->resultIsFloat = 0;
    if (frame->count > 0 && !chali_abi_argument_integer(frame, 0, &a)) return;
    if (frame->count > 1 && !chali_abi_argument_integer(frame, 1, &b)) return;
    if (frame->count > 2 && !chali_abi_argument_integer(frame, 2, &c)) return;
    if (operation >= PointerBump && operation <= PointerAt && !a)
    {
        chali_abi_fail(frame, "胶水层收到无效的零地址引用");
        return;
    }
    switch (operation)
    {
    case CounterFrom:
    case CounterTo:
    case IdentityEnum: result = a; break;
    case CounterAdd: result = signedBits((uint64_t)a + (uint64_t)b); break;
    case IdentityI32: result = (int32_t)a; break;
    case TextSize:
    {
        const unsigned char *data = NULL;
        size_t length = 0;
        if (!chali_abi_bytes(frame, a, &data, &length)) return;
        result = (int32_t)(uint32_t)length;
        break;
    }
    case VariadicSum:
    {
        size_t count = 0;
        if (!chali_abi_list_numbers(frame, b, NULL, NULL, 0, &count)) return;
        if (count > SIZE_MAX / (sizeof(int64_t) + sizeof(unsigned char)))
        {
            chali_abi_fail(frame, "变长参数列表过大");
            return;
        }
        int64_t *bits = count ? (int64_t *)malloc(count * (sizeof(int64_t) + sizeof(unsigned char))) : NULL;
        if (count && !bits)
        {
            chali_abi_fail(frame, "变长参数缓冲区分配失败");
            return;
        }
        unsigned char *kinds = count ? (unsigned char *)(bits + count) : NULL;
        size_t required = 0;
        if (!chali_abi_list_numbers(frame, b, bits, kinds, count, &required))
        {
            free(bits);
            return;
        }
        result = a;
        for (size_t i = 0; i < required; ++i)
        {
            x = bits[i];
            if (kinds[i] == ChaliAbiSlotFloat)
            {
                double real;
                memcpy(&real, &bits[i], sizeof(real));
                x = (int64_t)real;
            }
            result = signedBits((uint64_t)result + (uint64_t)x);
        }
        free(bits);
        break;
    }
    case Apply:
    {
        unsigned char kind = ChaliAbiSlotInteger;
        int floating = 0;
        if (!chali_abi_invoke(frame, a, &b, &kind, 1, &result, &floating)) return;
        if (floating)
        {
            double real;
            memcpy(&real, &result, sizeof(real));
            result = (int64_t)real;
        }
        break;
    }
    case EnumTag:
    case SignalCode:
        if (!readWord(frame, a, &x)) return;
        result = (int32_t)x;
        break;
    case EnumPayload:
        if (!readWord(frame, slotAddress(a, 1), &result)) return;
        break;
    case PointArea:
    case PointScaled:
    case RectArea:
    case RectScaled:
        if (!readWord(frame, a, &x) || !readWord(frame, slotAddress(a, 1), &y)) return;
        result = signedBits((uint64_t)x * (uint64_t)y);
        if (operation == PointScaled || operation == RectScaled) result = signedBits((uint64_t)result * (uint64_t)b);
        break;
    case CubeArea:
    case CubeScaled:
        if (!readWord(frame, a, &x)) return;
        result = signedBits((uint64_t)x * (uint64_t)x);
        if (operation == CubeScaled) result = signedBits((uint64_t)result * (uint64_t)b);
        break;
    case CubeUnit:
        if (!readWord(frame, slotAddress(a, 1), &result)) return;
        break;
    case BufferSize:
    case BufferConvert:
        if (!readWord(frame, a, &result)) return;
        break;
    case BufferIndex:
    case BufferAdd:
        if (!readWord(frame, a, &x)) return;
        result = signedBits((uint64_t)x + (uint64_t)b);
        break;
    case BufferMul:
        if (!readWord(frame, a, &x)) return;
        result = signedBits((uint64_t)x * (uint64_t)b);
        break;
    case RectPerimeter:
        if (!readWord(frame, a, &x) || !readWord(frame, slotAddress(a, 1), &y)) return;
        result = signedBits(2u * ((uint64_t)x + (uint64_t)y));
        break;
    case PointerBump:
        if (!readWord(frame, a, &result) || !writeWord(frame, a, signedBits((uint64_t)result + 1u))) return;
        break;
    case PointerSum:
        if (!readWord(frame, a, &x) || !readWord(frame, slotAddress(a, 1), &y)) return;
        result = signedBits((uint64_t)x + (uint64_t)y);
        break;
    case PointerOffset:
        if (!readWord(frame, a, &x) || !writeWord(frame, a, signedBits((uint64_t)x + (uint64_t)b))) return;
        if (!readWord(frame, slotAddress(a, 1), &y) || !writeWord(frame, slotAddress(a, 1), signedBits((uint64_t)y + (uint64_t)c))) return;
        break;
    case PointerAt:
        if (!readWord(frame, slotAddress(a, b), &result)) return;
        break;
    case ReferenceOptional:
    {
        if (!b)
        {
            chali_abi_fail(frame, "胶水层收到无效的零地址引用");
            return;
        }
        const int64_t words[] = { a ? 0 : 1, a ? b : 0 };
        result = chali_abi_tracked_temporary(frame, sizeof(words));
        if (!result) return;
        if (!chali_abi_write(frame, result, words, sizeof(words)))
        {
            chali_abi_temporary_release(frame, result);
            return;
        }
        break;
    }
    case ReferenceRead:
    case ReferenceWrite:
        if (!optionalReference(frame, a, &x)) return;
        if (!x)
        {
            result = operation == ReferenceRead ? -1 : 0;
            break;
        }
        if (!readWord(frame, x, &y)) return;
        if (operation == ReferenceRead)
            result = y;
        else if (!writeWord(frame, x, signedBits((uint64_t)y + 1u)))
            return;
        break;
    }
    chali_abi_return_integer(frame, result);
}
/**
 * @brief 验证 demo_counter_from_uint 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry0(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, CounterFrom);
}
/**
 * @brief 验证 demo_counter_to_uint 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry1(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, CounterTo);
}
/**
 * @brief 验证 demo_cpp_add 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry2(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, CounterAdd);
}
/**
 * @brief 验证 demo_cpp_identity__int32 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry3(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, IdentityI32);
}
/**
 * @brief 验证 demo_cpp_text_size 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry4(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, TextSize);
}
/**
 * @brief 验证 demo_cpp_variadic_sum 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry5(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, VariadicSum);
}
/**
 * @brief 验证 demo_cpp_apply 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry6(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, Apply);
}
/**
 * @brief 验证 demo_cpp_identity____chl_4aa9f607508db74b_program__Signal 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry7(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, IdentityEnum);
}
/**
 * @brief 验证 demo_cpp_enum_tag____chl_4aa9f607508db74b_program__Signal 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry8(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, EnumTag);
}
/**
 * @brief 验证 demo_cpp_enum_payload____chl_4aa9f607508db74b_program__Signal 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry9(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, EnumPayload);
}
/**
 * @brief 验证 glue_demo_inspect.area 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry10(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, PointArea);
}
/**
 * @brief 验证 glue_demo_inspect.scaled 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry11(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, PointScaled);
}
/**
 * @brief 验证 glue_demo_signal 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry12(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, SignalCode);
}
/**
 * @brief 验证 glue_demo_cube.area 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry13(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, CubeArea);
}
/**
 * @brief 验证 glue_demo_cube.scaled 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry14(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, CubeScaled);
}
/**
 * @brief 验证 glue_demo_cube.unit 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry15(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, CubeUnit);
}
/**
 * @brief 验证 glue_demo_buffer.size 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry16(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, BufferSize);
}
/**
 * @brief 验证 glue_demo_buffer.index 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry17(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, BufferIndex);
}
/**
 * @brief 验证 glue_demo_buffer.leftMul 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry18(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, BufferMul);
}
/**
 * @brief 验证 glue_demo_buffer.explicitConvert 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry19(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, BufferConvert);
}
/**
 * @brief 验证 glue_demo_buffer_add 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry20(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, BufferAdd);
}
/**
 * @brief 验证 glue_demo_rect_area 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry21(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, RectArea);
}
/**
 * @brief 验证 glue_demo_rect_area_scaled 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry22(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, RectScaled);
}
/**
 * @brief 验证 glue_demo_rect_perimeter 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry23(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, RectPerimeter);
}
/**
 * @brief 验证 demo_pointer_bump 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry24(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, PointerBump);
}
/**
 * @brief 验证 demo_pointer_sum 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry25(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, PointerSum);
}
/**
 * @brief 验证 demo_pointer_offset 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry26(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 3, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, PointerOffset);
}
/**
 * @brief 验证 demo_pointer_at 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry27(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, PointerAt);
}
/**
 * @brief 验证 demo_reference_optional 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry28(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 2, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, ReferenceOptional);
}
/**
 * @brief 验证 demo_reference_read 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry29(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, ReferenceRead);
}
/**
 * @brief 验证 demo_reference_write 的 C 帧并执行对应演示操作。
 * @param frame 参数与结果帧。
 */
static void entry30(ChaliAbiFrame *frame)
{
    if (chali_abi_check_frame(frame, 1, NULL) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid demo call frame");
        return;
    }
    execute(frame, ReferenceWrite);
}
/**
 * @brief 取得纯 C 演示服务的直接入口表。
 * @param count 接收入口数量，可为空。
 * @return 进程期有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_demo_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "demo_counter_from_uint", entry0, 1 },
        { "demo_counter_to_uint", entry1, 1 },
        { "demo_cpp_add", entry2, 2 },
        { "demo_cpp_identity__int32", entry3, 1 },
        { "demo_cpp_text_size", entry4, 1 },
        { "demo_cpp_variadic_sum", entry5, 2 },
        { "demo_cpp_apply", entry6, 2 },
        { "demo_cpp_identity____chl_4aa9f607508db74b_program__Signal", entry7, 1 },
        { "demo_cpp_enum_tag____chl_4aa9f607508db74b_program__Signal", entry8, 1 },
        { "demo_cpp_enum_payload____chl_4aa9f607508db74b_program__Signal", entry9, 1 },
        { "glue_demo_inspect.area", entry10, 1 },
        { "glue_demo_inspect.scaled", entry11, 2 },
        { "glue_demo_signal", entry12, 1 },
        { "glue_demo_cube.area", entry13, 1 },
        { "glue_demo_cube.scaled", entry14, 2 },
        { "glue_demo_cube.unit", entry15, 1 },
        { "glue_demo_buffer.size", entry16, 1 },
        { "glue_demo_buffer.index", entry17, 2 },
        { "glue_demo_buffer.leftMul", entry18, 2 },
        { "glue_demo_buffer.explicitConvert", entry19, 1 },
        { "glue_demo_buffer_add", entry20, 2 },
        { "glue_demo_rect_area", entry21, 1 },
        { "glue_demo_rect_area_scaled", entry22, 2 },
        { "glue_demo_rect_perimeter", entry23, 1 },
        { "demo_pointer_bump", entry24, 1 },
        { "demo_pointer_sum", entry25, 1 },
        { "demo_pointer_offset", entry26, 3 },
        { "demo_pointer_at", entry27, 2 },
        { "demo_reference_optional", entry28, 2 },
        { "demo_reference_read", entry29, 1 },
        { "demo_reference_write", entry30, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
/** @brief 以普通语言文本提供的既有演示声明，名称及声明顺序保持不变。 */
static const char declarations[] =
    "struct GlueDemoPoint { int64 x; int64 y; };\n"
    "enum GlueDemoSignal { Idle, Tone(int64 hz) };\n"
    "interface GlueDemoInspect { int64 area(); int64 scaled(int64 factor); };\n"
    "impl GlueDemoPoint : GlueDemoInspect = glue_demo_inspect;\n"
    "interface GlueDemoCoded { int64 code(); };\n"
    "impl GlueDemoSignal : GlueDemoCoded = glue_demo_signal;\n"
    "interface GlueDemoMeasured : GlueDemoInspect { int64 unit(); };\n"
    "interface GlueDemoTagged : GlueDemoMeasured { };\n"
    "struct GlueDemoCube { int64 edge; int64 unitScale; };\n"
    "impl GlueDemoCube : GlueDemoTagged = glue_demo_cube;\n"
    "struct GlueDemoBuffer { int64 base; };\n"
    "interface GlueDemoBuffered : Index<int64, int64>, Multiply<int64, int64>, ExplicitConvert<int64> { int64 size(); };\n"
    "impl GlueDemoBuffer : GlueDemoBuffered = glue_demo_buffer;\n"
    "interface GlueDemoAddable : Add<int64, int64> { };\n"
    "impl GlueDemoBuffer : GlueDemoAddable = glue_demo_buffer_add;\n"
    "struct GlueDemoRect { int64 width; int64 height; };\n"
    "interface GlueDemoRect__glue__area { int64 area(); };\n"
    "impl GlueDemoRect : GlueDemoRect__glue__area = glue_demo_rect_area;\n"
    "interface GlueDemoRect__glue__area__int64 { int64 area(int64 factor); };\n"
    "impl GlueDemoRect : GlueDemoRect__glue__area__int64 = glue_demo_rect_area_scaled;\n"
    "interface GlueDemoRect__glue__perimeter { int64 perimeter(); };\n"
    "impl GlueDemoRect : GlueDemoRect__glue__perimeter = glue_demo_rect_perimeter;\n"
    "interface GlueDemoRect__glue__missing { int64 missing(); };\n"
    "impl GlueDemoRect : GlueDemoRect__glue__missing = glue_demo_rect_missing;\n";
/**
 * @brief 取得演示类型、接口和外部实现的普通 ChaLi 声明。
 * @param length 接收字节数，可为空。
 * @return 进程期有效的只读文本。
 */
const char *CHALI_ABI_CALL abi_demo_declarations(size_t *length)
{
    if (length) *length = sizeof(declarations) - 1;
    return declarations;
}
/**
 * @brief 按原有完整标识符规则检测是否需要附加演示声明，避免改变无关程序类型号。
 * @param source 源码文本。
 * @param length 源码字节数。
 * @return 命中至少一个演示声明名为一。
 */
int CHALI_ABI_CALL abi_demo_declarations_used(const char *source, size_t length)
{
    static const char *names[] = {
        "GlueDemoPoint",
        "GlueDemoSignal",
        "GlueDemoInspect",
        "GlueDemoCoded",
        "GlueDemoMeasured",
        "GlueDemoTagged",
        "GlueDemoCube",
        "GlueDemoBuffer",
        "GlueDemoBuffered",
        "GlueDemoAddable",
        "GlueDemoRect",
        "GlueDemoRect__glue__area",
        "GlueDemoRect__glue__area__int64",
        "GlueDemoRect__glue__perimeter",
        "GlueDemoRect__glue__missing",
    };
    if (!source) return 0;
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
    {
        const size_t size = strlen(names[i]);
        if (size > length) continue;
        for (size_t at = 0; at <= length - size; ++at)
        {
            if (memcmp(source + at, names[i], size)) continue;
            const size_t after = at + size;
            if ((at == 0 || (!isalnum((unsigned char)source[at - 1]) && source[at - 1] != '_')) && (after == length || (!isalnum((unsigned char)source[after]) && source[after] != '_'))) return 1;
        }
    }
    return 0;
}
