/**
 * @file chali_heap_address_view.cpp
 * @brief 核对地址视图在扩容、释放、收窄、重置和代次耗尽后的失效规则。
 */
#include "allocator/system_arena.hpp"
#include "ircode/runtime_heap.hpp"
#include <cassert>
#include <limits>

/**
 * @brief 在一个真实分配后端中核对视图映射、边界及失效，不解引用失效物理指针。
 * @param arena 本轮系统分配器或内存池。
 */
void checkViews(Memory::RuntimeAllocator &arena)
{
    Memory::AllocationScope allocation(&arena);
    const std::atomic<std::uint64_t> *survivingRevision = nullptr;
    std::uint64_t beforeDestruction = 0;
    {
        stackvm::runtimeDetail::Heap heap;
        heap.reset(8);
        const auto address = heap.allocate(64);
        assert(address && heap.writeInt(address + 8, 12345));
        stackvm::runtimeDetail::Heap::AddressView view;
        auto *bytes = heap.borrowView(address + 8, 8, view);
        assert(bytes && bytes == heap.borrowBytes(address + 8, 8));
        assert(view.begin == address && view.size == 64 && bytes == view.bytes + 8);
        assert(view.revision == heap.mappingRevision() && view.stamp == view.revision->load());
        assert(!heap.borrowView(address + 60, 8, view));
        assert(!heap.borrowView(address + 64, 0, view));
        const auto firstStamp = view.stamp;
        assert(heap.writeInt(address + 8, 54321) && view.revision->load() == firstStamp);
        assert(heap.allocate(256 * 1024));  // 连续后端可能搬移整个物理缓冲。
        assert(view.revision->load() != firstStamp);
        assert(heap.borrowView(address + 8, 8, view));
        std::int64_t value = 0;
        assert(heap.readInt(address + 8, value) && value == 54321);
        const auto beforeRestrict = view.stamp;
        assert(heap.restrictAllocation(address, 16, 32));
        assert(view.revision->load() != beforeRestrict);
        assert(!heap.borrowView(address + 8, 8, view));
        assert(heap.borrowView(address + 16, 32, view));
        assert(view.begin == address + 16 && view.size == 32);
        assert(!heap.borrowView(address + 16, 33, view));
        const auto beforeFree = view.stamp;
        assert(heap.release(address) && view.revision->load() != beforeFree);
        assert(!heap.borrowView(address + 16, 1, view));
        assert(heap.allocate(64));
        assert(!heap.borrowView(address + 16, 1, view));
        const auto beforeReset = heap.mappingRevision()->load();
        heap.reset(1024);
        assert(heap.mappingRevision()->load() != beforeReset && !heap.borrowView(address, 1, view));
        const auto fresh = heap.allocate(64);
        assert(fresh && heap.borrowView(fresh, 64, view));
        survivingRevision = view.revision;
        beforeDestruction = view.stamp;
    }
    assert(survivingRevision && survivingRevision->load() != beforeDestruction);
    assert(arena.stats().liveBlocks == 0);
}

/**
 * @brief 在 malloc 与 pool 配置中执行相同的失效边界核对。
 * @return 全部断言通过时返回零。
 */
int main()
{
    Memory::SystemRuntimeArena system(4 * 1024 * 1024);
    checkViews(system);
    Memory::DynamicRuntimeArena pool(4 * 1024 * 1024);
    checkViews(pool);
    {
        Memory::AllocationScope allocation(&system);
        stackvm::runtimeDetail::Heap heap;
        heap.reset(8);
        const auto address = heap.allocate(64);
        // 只在所有正常配置验证后注入模块代次边界，回绕后必须永久禁用缓存。
        const_cast<std::atomic<std::uint64_t> *>(heap.mappingRevision())->store(std::numeric_limits<std::uint64_t>::max());
        assert(heap.allocate(16) && heap.mappingRevision()->load() == 0);
        assert(heap.allocate(16) && heap.mappingRevision()->load() == 0);
        stackvm::runtimeDetail::Heap::AddressView view;
        assert(heap.borrowView(address, 64, view) && view.stamp == 0);
    }
}
