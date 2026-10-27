/**
 * @file native_text.cpp
 * @brief 字节文本操作与内存复制服务。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
using namespace runtimeDetail;
namespace runtimeDetail
{
/**
 * @brief 查询字符串存储布局，不复制或扫描文本内容。
 * @param ctx 当前内存及分配身份上下文。
 * @param args 唯一参数为字符串值的逻辑地址。
 * @return 堆字符串跳过八字节长度头，字面量保持原地址。
 */
Slot strData(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const auto address = nativeInt(args[0]);
    const bool heap = ctx.heap != nullptr && ctx.heap->contains(static_cast<std::size_t>(address));
    return Slot::fromInt(address + (heap ? 8 : 0));
}
/**
 * @brief `__std_str_len(地址) -> 长度`。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot strLen(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const std::size_t address = static_cast<std::size_t>(nativeInt(args[0]));
    std::int64_t length = 0;
    if (ctx.heap != nullptr && ctx.heap->contains(address) && ctx.heap->readInt(address, length) && length >= 0)
        return Slot::fromInt(length);
    Memory::String text;
    if (!nativeReadCString(ctx, address, &text))
    {
        ctx.error = "取字符串长度时读不到它（地址 " + Memory::toString(nativeInt(args[0])) + " 不在任何一块已分配内存里，或者没有结尾的 \\0）";
        return Slot::fromInt(0);
    }
    return Slot::fromInt(static_cast<std::int64_t>(text.size()));
}

/**
 * @brief `__std_str_fmt(格式串, 实参…) -> 新串地址`。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot strFormat(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    if (args.empty())
    {
        ctx.error = "format 至少要有格式串";
        return Slot::fromInt(0);
    }
    Memory::String format;
    if (!nativeReadCString(ctx, static_cast<std::size_t>(nativeInt(args[0])), &format))
    {
        ctx.error = "format 的格式串读不到（地址 " + Memory::toString(nativeInt(args[0])) + "）";
        return Slot::fromInt(0);
    }
    Memory::String text;
    if (!nativeFormatInto(ctx, format, args, args.size() - 1u, &text)) return Slot::fromInt(0);
    return Slot::fromInt(static_cast<std::int64_t>(nativeAllocString(ctx, text)));
}

/**
 * @brief 输出一段文本：挂了下沉就先实时推给宿主，`keepText=false` 时不再进文本桶。
 * @param ctx 环境。
 * @param bytes 字节。
 * @param count 字节数。
 */
void nativeEmitText(NativeContext &ctx, const char *bytes, std::size_t count)
{
    if (bytes == nullptr || count == 0u) return;
    if (ctx.textSink != nullptr) ctx.textSink(ctx.textSinkUser, bytes, count);
    if (ctx.text != nullptr && (ctx.textKeep == nullptr || *ctx.textKeep))
        ctx.text->append(bytes, count);
}

/**
 * @brief `__std_print(格式串, 实参…) -> 0`：把格式化结果追加到文本输出桶（`printf` 用它）。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot strPrint(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    if (args.empty())
    {
        ctx.error = "printf 至少要有格式串";
        return Slot::fromInt(0);
    }
    Memory::String format;
    if (!nativeReadCString(ctx, static_cast<std::size_t>(nativeInt(args[0])), &format))
    {
        ctx.error = "printf 的格式串读不到（地址 " + Memory::toString(nativeInt(args[0])) + "）";
        return Slot::fromInt(0);
    }
    Memory::String text;
    if (!nativeFormatInto(ctx, format, args, args.size() - 1u, &text)) return Slot::fromInt(0);
    nativeEmitText(ctx, text.data(), text.size());
    return Slot::fromInt(0);
}

/**
 * @brief 输出一个字符。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot putChar(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const char byte = static_cast<char>(nativeInt(args[0]) & 0xff);
    nativeEmitText(ctx, &byte, 1u);
    return Slot::fromInt(0);
}

}  // namespace runtimeDetail
}  // namespace stackvm
