/** @file chali_data_cstring.cpp
 * @brief 以旧逐字节协议核对数据区字符串批量扫描、错误边界和真实分配次数。
 */
#include "ircode/runtime_internal.hpp"
#include "allocator/system_arena.hpp"
#include <cassert>
#include <cstdio>

using namespace stackvm;
using namespace stackvm::runtimeDetail;

/**
 * @brief 保留优化前的读取协议，作为独立差分参考。
 * @param context 当前数据区与堆。
 * @param address 逻辑字符串地址。
 * @param out 接收完整或失败前已读取的文本。
 * @return 读取成功时返回 true。
 */
static bool legacyCString(const NativeContext &context, std::size_t address, Memory::String *out)
{
    out->clear();
    if (address == 0) return true;
    if (context.heap && context.heap->contains(address))
    {
        NativeStringStorage storage;
        if (!nativeHeapStringStorage(context, address, storage)) return false;
        out->resize(storage.length);
        return storage.length == 0 || nativeRead(context, storage.data, out->data(), out->size());
    }
    for (std::size_t index = 0; index < kStringScanLimit; ++index)
    {
        char ch = 0;
        if (!nativeRead(context, address + index, &ch, 1)) return false;
        if (ch == 0) return true;
        out->push_back(ch);
    }
    return false;
}

/**
 * @brief 比较成功状态、完整／部分输出以及首次内存错误。
 * @param context 本用例上下文。
 * @param address 待读取的逻辑地址。
 */
static void checkCString(NativeContext &context, std::size_t address)
{
    Memory::String expected = "previous", actual = "previous";
    context.memoryFault.clear();
    const bool oldStatus = legacyCString(context, address, &expected);
    const auto oldFault = context.memoryFault;
    context.memoryFault.clear();
    const bool newStatus = nativeReadCString(context, address, &actual);
    assert(newStatus == oldStatus && actual == expected && context.memoryFault == oldFault);
}

/**
 * @brief 核对哨兵、上限、原地变化、堆显式长度和分配减少。
 * @return 全部检查通过时返回零。
 */
int main()
{
    Memory::SystemRuntimeArena arena(32 * 1024 * 1024);
    Memory::AllocationScope allocation(&arena);
    DataArea data;
    NativeContext context;
    context.data = &data;
    checkCString(context, 0);
    checkCString(context, 1);
    data.binary = { 0, 'a', 'b', 0, 'x', 'y' };
    for (std::size_t i = 0; i <= data.binary.size(); ++i) checkCString(context, i);
    data.binary[2] = 'z';
    checkCString(context, 1);
    checkCString(context, SIZE_MAX);
    const Memory::String utf8 = u8"接口类型／布局";
    data.binary.assign(1, 0);
    data.binary.insert(data.binary.end(), utf8.begin(), utf8.end());
    data.binary.push_back(0);
    checkCString(context, 1);
    Heap heap;
    heap.reset(data.size() + 8);
    context.heap = &heap;
    checkCString(context, 1);
    heap.reset(1);  // 数据区不在声明的堆基址之前，保守回退。
    checkCString(context, 1);
    heap.reset(data.size() + 8);
    const Memory::String embedded("a\0b", 3);
    const auto buffered = nativeAllocString(context, embedded);
    assert(buffered);
    checkCString(context, buffered);
    assert(nativeReleaseString(context, buffered));
    checkCString(context, buffered);
    const auto legacy = heap.allocate(11);
    assert(legacy && heap.writeInt(legacy, 3) && heap.writeBytes(legacy + 8, embedded.data(), 3));
    checkCString(context, legacy);
    assert(heap.release(legacy));
    context.heap = nullptr;
    data.binary.assign(kStringScanLimit + 2, 'x');
    data.binary[kStringScanLimit] = 0;
    checkCString(context, 1);  // 最后一个允许扫描的字节为哨兵。
    data.binary[kStringScanLimit] = 'x';
    data.binary[kStringScanLimit + 1] = 0;
    checkCString(context, 1);  // 超过上限，部分输出及空错误状态保持。
    data.binary.resize(512);
    std::uint32_t state = 23;
    for (auto &byte : data.binary)
    {
        state = state * 1664525u + 1013904223u;
        byte = static_cast<std::uint8_t>((state >> 24) & 31u);
    }
    for (std::size_t i = 0; i <= data.binary.size(); ++i) checkCString(context, i);
    data.binary.assign(4098, 'm');
    data.binary.back() = 0;
    std::size_t oldCount = 0, newCount = 0;
    {
        Memory::String text;
        const auto before = arena.stats().totalBlocks;
        assert(legacyCString(context, 1, &text) && text.size() == 4096);
        oldCount = arena.stats().totalBlocks - before;
    }
    {
        Memory::String text;
        const auto before = arena.stats().totalBlocks;
        assert(nativeReadCString(context, 1, &text) && text.size() == 4096);
        newCount = arena.stats().totalBlocks - before;
    }
    assert(newCount < oldCount);
    context.memoryFault = "earlier";
    Memory::String text;
    assert(nativeReadCString(context, 1, &text) && context.memoryFault == "earlier");
    std::printf("data CString: legacy=%zu bulk=%zu allocations; boundaries, partial errors, UTF-8 and heap layouts OK\n", oldCount, newCount);
}
