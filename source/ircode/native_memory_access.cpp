/**
 * @file native_memory_access.cpp
 * @brief 受检查的堆读写、字符串分配与地址范围验证。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
using namespace runtimeDetail;
namespace runtimeDetail
{
/**
 * @brief 解析原长度头或带负一标记的容量描述符，并验证有效分配范围。
 * @param ctx 当前地址空间。
 * @param address 堆字符串句柄。
 * @param out 接收字节视图。
 * @return 元数据及内容完整位于活跃块中时为真。
 */
bool nativeHeapStringStorage(const NativeContext &ctx, std::size_t address, NativeStringStorage &out)
{
    if (!ctx.heap) return false;
    std::size_t block = 0;
    std::int64_t header[4] = {};
    if (!ctx.heap->capacityAt(address, block) || block < 8u || !ctx.heap->readInt(address, header[0])) return false;
    if (header[0] >= 0)
    {
        if (static_cast<std::uint64_t>(header[0]) > block - 8u) return false;
        out = { address + 8u, static_cast<std::size_t>(header[0]), block - 8u, false };
        return true;
    }
    if (header[0] != -1 || block < sizeof(header)
        || !ctx.heap->readBytes(address, reinterpret_cast<char *>(header), sizeof(header))) return false;
    if (header[1] < 0 || header[2] < header[1] || header[2] == INT64_MAX || header[3] <= 0) return false;
    const auto data = static_cast<std::size_t>(header[3]);
    const auto capacity = static_cast<std::size_t>(header[2]);
    if (!allocatedHeapRange(*ctx.heap, data, capacity + 1u)) return false;
    out = { data, static_cast<std::size_t>(header[1]), capacity, true };
    return true;
}

/**
 * @brief 释放字符串的独立缓冲区与描述符，内联内容只随描述符释放。
 * @param ctx 字符串所属地址空间。
 * @param address 堆字符串句柄。
 * @return 描述符释放成功时为真。
 */
bool nativeReleaseString(NativeContext &ctx, std::size_t address)
{
    if (!ctx.heap) return false;
    NativeStringStorage storage;
    if (nativeHeapStringStorage(ctx, address, storage) && storage.buffered && storage.data != address + 32u)
        if (!ctx.heap->release(storage.data)) return false;
    return ctx.heap->release(address);
}

/**
 * @brief 验证访问完全位于同一个活跃分配中，避免宽访问跨过块尾。
 * @param heap 分配器。
 * @param address 地址。
 * @param count 字节数。
 * @return 零长度或有效分配范围返回 true。
 */
bool allocatedHeapRange(const HeapAllocator &heap, std::size_t address, std::size_t count)
{
#if STACKVM_WITH_ALLOCATION_GUARDS
    std::size_t capacity = 0;
    return count == 0 || (heap.capacityAt(address, capacity) && count <= capacity);
#else
    (void)heap;
    (void)address;
    (void)count;
    return true;
#endif
}

/**
 * @brief 从堆或数据区里读一段字节。
 * @param ctx 环境。
 * @param address 地址。
 * @param out 输出缓冲。
 * @param count 字节数。
 * @return 读得到返回 true（已释放 / 越界返回 false）。
 */
bool nativeRead(const NativeContext &ctx, std::size_t address, char *out, std::size_t count)
{
    if (count == 0) return true;
#if STACKVM_WITH_ALLOCATION_GUARDS
    const bool heap = ctx.heap != nullptr && ctx.heap->contains(address);
    const bool valid = heap ? allocatedHeapRange(*ctx.heap, address, count) :
                              ctx.data != nullptr && address <= ctx.data->size() && count <= ctx.data->size() - address;
    if (!valid)
    {
        if (ctx.memoryFault.empty()) ctx.memoryFault = CHALI_TEXT("panic：访问未分配内存（读）：地址 ") + Memory::toString(address) + CHALI_TEXT("，字节数 ") + Memory::toString(count);
        return false;
    }
#endif
    if (ctx.heap != nullptr && ctx.heap->contains(address))
    {
        return ctx.heap->readBytes(address, out, count);
    }
    return (ctx.data != nullptr) && ctx.data->readBytes(address, out, count);
}

/**
 * @brief 往堆或数据区按字节写一段（改字符串里的某个字节用它）。
 * @param ctx 环境。
 * @param address 地址。
 * @param in 输入缓冲。
 * @param count 字节数。
 * @return 成功返回 true。
 */
bool nativeWrite(NativeContext &ctx, std::size_t address, const char *in, std::size_t count)
{
    if (count == 0) return true;
#if STACKVM_WITH_ALLOCATION_GUARDS
    const bool heap = ctx.heap != nullptr && ctx.heap->contains(address);
    const bool valid = heap ? allocatedHeapRange(*ctx.heap, address, count) :
                              ctx.data != nullptr && address <= ctx.data->size() && count <= ctx.data->size() - address;
    if (!valid)
    {
        if (ctx.memoryFault.empty()) ctx.memoryFault = CHALI_TEXT("panic：访问未分配内存（写）：地址 ") + Memory::toString(address) + CHALI_TEXT("，字节数 ") + Memory::toString(count);
        return false;
    }
#endif
    if (ctx.heap != nullptr && ctx.heap->contains(address))
    {
        return ctx.heap->writeBytes(address, in, count);
    }
    return (ctx.data != nullptr) && const_cast<DataArea *>(ctx.data)->writeBytes(address, in, count);
}

/**
 * @brief 读取显式长度的堆字符串，或读取生成期带哨兵的数据区字面量。
 * @param ctx 环境。
 * @param address 地址（0 = 空指针）。
 * @param out 结果。
 * @return 成功返回 true；长度非法、地址不可读或数据区哨兵缺失返回 false。
 */
bool nativeReadCString(const NativeContext &ctx, std::size_t address, Memory::String *out)
{
    out->clear();
    if (address == 0u) return true;  ///< 空指针当空串
    if (ctx.heap != nullptr && ctx.heap->contains(address))
    {
        NativeStringStorage storage;
        if (!nativeHeapStringStorage(ctx, address, storage)) return false;
        out->resize(storage.length);
        return storage.length == 0 || nativeRead(ctx, storage.data, out->data(), out->size());
    }
    for (std::size_t index = 0; index < kStringScanLimit; ++index)
    {
        char ch = 0;
        if (!nativeRead(ctx, address + index, &ch, 1u)) return false;
        if (ch == '\0') return true;
        out->push_back(ch);
    }
    return false;
}

/**
 * @brief 分配新存储并清除旧地址的销毁标记。
 * @param ctx 运行环境。
 * @param bytes 字节数。
 * @return 新地址，失败返回零。
 */
std::size_t nativeAllocate(NativeContext &ctx, std::size_t bytes);

/**
 * @brief 创建带容量描述符的堆字符串，小文本与描述符放在同一分配中。
 * @param ctx 环境。
 * @param text UTF-8 字节内容。
 * @return 地址；0 = 失败（`ctx.error` 里是原因）。
 */
std::size_t nativeAllocString(NativeContext &ctx, const Memory::String &text)
{
    if (ctx.heap == nullptr) return 0u;
    if (text.size() > static_cast<std::uint64_t>(INT64_MAX) - 33u)
    {
        ctx.error = "String：结果过长";
        return 0u;
    }
    const bool small = text.size() <= 16u;
    const std::size_t bytes = small ? 49u : 32u;
    const std::size_t address = nativeAllocate(ctx, bytes);
    if (address == 0u)
    {
        ctx.error = "堆内存不足：拼接 / 格式化需要 " + Memory::toString(bytes) + " 字节";
        return 0u;
    }
    const auto data = small ? address + 32u : nativeAllocate(ctx, text.size() + 1u);
    if (!data)
    {
        ctx.heap->release(address);
        ctx.error = "堆内存不足：字符串缓冲区需要 " + Memory::toString(text.size() + 1u) + " 字节";
        return 0u;
    }
    const std::int64_t header[4] = { -1, static_cast<std::int64_t>(text.size()),
                                    static_cast<std::int64_t>(small ? 16u : text.size()), static_cast<std::int64_t>(data) };
    const char zero = 0;
    if (!ctx.heap->writeBytes(address, reinterpret_cast<const char *>(header), sizeof(header))
        || (!text.empty() && !ctx.heap->writeBytes(data, text.data(), text.size()))
        || !ctx.heap->writeBytes(data + text.size(), &zero, 1u))
    {
        if (!small) ctx.heap->release(data);
        ctx.heap->release(address);
        ctx.error = "往堆上写字符串失败（地址 " + Memory::toString(address) + "）";
        return 0u;
    }
    return address;
}
}  // namespace runtimeDetail
}  // namespace stackvm
