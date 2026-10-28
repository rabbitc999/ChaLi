/**
 * @file native_arena.hpp
 * @brief 运行时内存统计接口。
 */
#pragma once
#include "runtime_objects.hpp"
#include "value_transfer.hpp"

namespace stackvm::runtimeDetail
{
/**
 * @brief 查询当前存活分配占用的字节数。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @retval 封装为原生值的运行时结果。
 */
Slot memoryLiveBytes(NativeContext &ctx, const Memory::Vector<Slot> &);

/**
 * @brief 查询当前存活分配的块数。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @retval 封装为原生值的运行时结果。
 */
Slot memoryLiveBlocks(NativeContext &ctx, const Memory::Vector<Slot> &);

/**
 * @brief 查询内存分配的峰值字节数。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @retval 封装为原生值的运行时结果。
 */
Slot memoryPeakBytes(NativeContext &ctx, const Memory::Vector<Slot> &);

}  // namespace stackvm::runtimeDetail
