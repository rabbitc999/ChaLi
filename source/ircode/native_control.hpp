/**
 * @file native_control.hpp
 * @brief VM 内部 native control 模块的类型与接口。
 */
#pragma once
#include "runtime_objects.hpp"
#include "value_transfer.hpp"

namespace stackvm::runtimeDetail
{
/**
 * @brief 设置程序退出码并终止当前执行。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot programExit(NativeContext &ctx, const Memory::Vector<Slot> &args);

/**
 * @brief 校验运行时断言，失败时报告错误。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot programAssert(NativeContext &ctx, const Memory::Vector<Slot> &args);

/**
 * @brief 触发程序 panic 并保存诊断信息。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot programPanic(NativeContext &ctx, const Memory::Vector<Slot> &args);

}  // namespace stackvm::runtimeDetail
