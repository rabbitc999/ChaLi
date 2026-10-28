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
 * @brief 查询原长度头、容量描述符或字面量的字符串存储元数据。
 * @param ctx 当前地址空间及诊断。
 * @param operation 零长度、一内容地址、二容量、三描述符标记。
 * @param address 字符串句柄。
 * @return 对应整数，访问失败保存具体诊断。
 */
std::int64_t stringStorageQuery(NativeContext &ctx, std::int64_t operation, std::int64_t address)
{
    const auto source = static_cast<std::size_t>(address);
    const bool heap = ctx.heap && ctx.heap->contains(source);
    std::int64_t marker = 0;
    const bool readable = heap && ctx.heap->readInt(source, marker);
    const bool buffered = readable && marker == -1;
    if (operation == 3) return buffered ? 1 : 0;
    if (operation == 1 && !buffered) return address + (heap ? 8 : 0);
    if (buffered)
    {
        std::int64_t value = 0;
        const auto offset = operation == 0 ? 8u : operation == 1 ? 24u : 16u;
        if (ctx.heap->readInt(source + offset, value) && value >= 0) return value;
    }
    else if (operation == 0 && readable && marker >= 0) return marker;
    else if (operation == 2 && heap)
    {
        std::size_t capacity = 0;
        if (ctx.heap->capacityAt(source, capacity) && capacity >= 8u) return static_cast<std::int64_t>(capacity - 8u);
    }
    else if (!heap)
    {
        Memory::String text;
        if (nativeReadCString(ctx, source, &text)) return static_cast<std::int64_t>(text.size());
    }
    ctx.error = "取字符串存储时读不到它（地址 " + Memory::toString(address) + "）";
    return 0;
}

/**
 * @brief 查询字符串内容区容量。
 * @param ctx 当前地址空间。
 * @param args 唯一参数为字符串句柄。
 * @return 字节容量槽。
 */
Slot strCapacity(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    return Slot::fromInt(stringStorageQuery(ctx, 2, nativeInt(args[0])));
}

/**
 * @brief 判断字符串是否带有可增长描述符。
 * @param ctx 当前地址空间。
 * @param args 唯一参数为字符串句柄。
 * @return 描述符标记槽。
 */
Slot strStorageKind(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    return Slot::fromInt(stringStorageQuery(ctx, 3, nativeInt(args[0])));
}

/**
 * @brief 查询当前字节堆捕获的分配预算，不借用线程局部分配器状态。
 * @param ctx 当前堆及诊断。
 * @param args 无参数。
 * @return 最大单块有效载荷预算。
 */
Slot bufferMaxBytes(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    (void)args;
#if STACKVM_WITH_DEFAULT_HEAP
    if (const auto *heap = dynamic_cast<const Heap *>(ctx.heap))
        return Slot::fromInt(static_cast<std::int64_t>(std::min<std::size_t>(heap->maximumAllocationBytes(), INT64_MAX)));
#endif
    ctx.error = "当前字节堆未提供缓冲区分配上限";
    return Slot::fromInt(0);
}

/**
 * @brief 查询字符串存储布局，不复制或扫描文本内容。
 * @param ctx 当前内存及分配身份上下文。
 * @param args 唯一参数为字符串值的逻辑地址。
 * @return 堆字符串的缓冲区地址，字面量保持原地址。
 */
Slot strData(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    return Slot::fromInt(stringStorageQuery(ctx, 1, nativeInt(args[0])));
}
/**
 * @brief `__std_str_len(地址) -> 长度`。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Slot strLen(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    return Slot::fromInt(stringStorageQuery(ctx, 0, nativeInt(args[0])));
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
