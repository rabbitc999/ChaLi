/**
 * @file chali_malloc_heap.cpp
 * @brief 系统 malloc 字节堆的逻辑身份、范围、初始化与清理验证。
 */
#include "allocator/system_arena.hpp"
#include "ircode/runtime_heap.hpp"
#include <cassert>

/**
 * @brief 对两个分配后端执行相同的存储与身份检查。
 * @param allocator 本轮宿主分配后端。
 */
void checkHeap(Memory::RuntimeAllocator &allocator)
{
    Memory::AllocationScope allocation(&allocator);
    {
        stackvm::runtimeDetail::Heap heap;
        heap.reset(8);
        std::size_t stale = 0;
        for (int index = 0; index < 1000; ++index)
        {
            const auto address = heap.allocate(32);
            assert(address && address != stale);
            std::int64_t value = -1;
            assert(heap.readInt(address, value) && value == 0);
            assert(heap.writeInt(address + 8, index));
            assert(heap.readInt(address + 8, value) && value == index);
            assert(!heap.readInt(address + 25, value));
            assert(heap.borrowBytes(address, 32) && !heap.borrowBytes(address, 33));
            assert(heap.liveBytes() == 32 && heap.liveBlocks() == 1);
            assert(heap.release(address));
            assert(!heap.release(address));
            assert(!heap.borrowBytes(address, 1));
            assert(heap.liveBytes() == 0 && heap.liveBlocks() == 0);
            if (stale) assert(!heap.borrowBytes(stale, 1));
            stale = address;
        }
        const auto narrowed = heap.allocate(128);
        assert(narrowed && heap.restrictAllocation(narrowed, 16, 64));
        assert(!heap.borrowBytes(narrowed, 1));
        assert(heap.borrowBytes(narrowed + 16, 64));
        assert(!heap.borrowBytes(narrowed + 16, 65));
        assert(heap.release(narrowed));
        assert(heap.allocate(100));
        heap.reset(16);
        assert(heap.liveBytes() == 0 && heap.liveBlocks() == 0);
        assert(heap.allocate(200));
    }
    assert(allocator.stats().liveBlocks == 0);
}

/**
 * @brief 验证切换分配器不会改变字节堆的逻辑内存契约。
 * @return 全部检查通过时返回零。
 */
int main()
{
    Memory::SystemRuntimeArena system(4 * 1024 * 1024);
    checkHeap(system);
    Memory::DynamicRuntimeArena pool(4 * 1024 * 1024);
    checkHeap(pool);
}
