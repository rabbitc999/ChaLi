/**
 * @file native_arena.cpp
 * @brief 运行时内存分配统计。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
using namespace runtimeDetail;
namespace runtimeDetail
{
/**
 * @brief 查询当前存活分配占用的字节数。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @retval 封装为原生值的运行时结果。
 */
Slot memoryLiveBytes(NativeContext &ctx, const Memory::Vector<Slot> &)
{
    return Slot::fromInt(ctx.heap == nullptr ? 0 : static_cast<std::int64_t>(ctx.heap->liveBytes()));
}

/**
 * @brief 查询当前存活分配的块数。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @retval 封装为原生值的运行时结果。
 */
Slot memoryLiveBlocks(NativeContext &ctx, const Memory::Vector<Slot> &)
{
    return Slot::fromInt(ctx.heap == nullptr ? 0 : static_cast<std::int64_t>(ctx.heap->liveBlocks()));
}

/**
 * @brief 查询内存分配的峰值字节数。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @retval 封装为原生值的运行时结果。
 */
Slot memoryPeakBytes(NativeContext &ctx, const Memory::Vector<Slot> &)
{
    return Slot::fromInt(ctx.heap == nullptr ? 0 : static_cast<std::int64_t>(ctx.heap->peakBytes()));
}

}  // namespace runtimeDetail
}  // namespace stackvm
