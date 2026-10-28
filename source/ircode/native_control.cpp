/**
 * @file native_control.cpp
 * @brief 程序退出、断言与 panic。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
using namespace runtimeDetail;
namespace runtimeDetail
{
/**
 * @brief 设置程序退出码并终止当前执行。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot programExit(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    if (ctx.exitRequested != nullptr) *ctx.exitRequested = true;
    if (ctx.exitCode != nullptr) *ctx.exitCode = static_cast<int>(nativeInt(args[0]));
    return Slot::fromInt(0);
}

/**
 * @brief 校验运行时断言，失败时报告错误。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot programAssert(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    if (nativeInt(args[0]) == 0) ctx.error = "断言失败：assert(x) 的 x 为假";
    return Slot::fromInt(0);
}

/**
 * @brief 触发程序 panic 并保存诊断信息。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot programPanic(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const std::size_t address = static_cast<std::size_t>(nativeInt(args[0]));
    Memory::String message;
    if (!nativeReadCString(ctx, address, &message))
    {
        ctx.error = "panic 读不到错误信息字符串（地址 " + Memory::toString(address) + "）";
        return Slot::fromInt(0);
    }
    ctx.error = "panic：" + message;
    return Slot::fromInt(0);
}

}  // namespace runtimeDetail
}  // namespace stackvm
