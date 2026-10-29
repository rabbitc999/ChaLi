/** @file chali_interface_snapshot.cpp
 * @brief 核对非拥有指针包装的独立句柄、共享载荷、写时分离及实际分配次数。
 */
#include "ircode/runtime_internal.hpp"
#include "allocator/system_arena.hpp"
#include <cassert>
#include <cstdio>

/**
 * @brief 比较共享与原复制路径，并核对分离、释放和非指针回退。
 * @return 全部断言通过时返回零。
 */
int main()
{
    using namespace stackvm::runtimeDetail;
    Memory::SystemRuntimeArena arena(4 * 1024 * 1024);
    Memory::AllocationScope allocation(&arena);
    std::size_t counts[2]{};
    for (unsigned mode = 0; mode < 2; ++mode)
    {
        ObjectStore store;
        store.pointerSnapshots = mode == 0;
        store.inlineHandles = false;
        auto pointer = Memory::makeShared<RuntimeObject>();
        pointer->kind = ObjectKind::Obj;
        pointer->objTag = 17;
        pointer->objValue.integer = 48;
        pointer->objTypeName = "S const&";
        pointer->objTextShape = "R0:0:0:8:0:0:";
        const auto original = store.add(std::move(pointer));
        std::int64_t handles[64]{};
        Memory::String error;
        const auto before = arena.stats().totalBlocks;
        for (auto &handle : handles)
        {
            handle = store.copyHandle(original, &error);
            assert(handle != 0 && handle != original && error.empty());
            assert((store.find(handle) == store.find(original)) == (mode == 0));
            assert(store.find(handle)->identity == store.find(original)->identity);
        }
        counts[mode] = arena.stats().totalBlocks - before;
        auto *changed = store.detachPointerSnapshot(handles[7]);
        assert(changed != store.find(original));
        changed->objValue.integer = 96;
        assert(store.find(original)->objValue.integer == 48);
        assert(store.find(handles[8])->objValue.integer == 48);
        assert(store.release(original));
        assert(store.copyHandle(original, &error) == 0 && !error.empty());
        for (auto handle : handles) assert(store.release(handle));
        assert(store.liveCount() == 0);
        auto scalar = Memory::makeShared<RuntimeObject>();
        scalar->kind = ObjectKind::Obj;
        scalar->objTextShape = "i0:0:0:8:0:0:";
        const auto first = store.add(std::move(scalar));
        error.clear();
        const auto second = store.copyHandle(first, &error);
        assert(second && error.empty() && store.find(first) != store.find(second));
        assert(store.release(first) && store.release(second));
    }
    assert(counts[0] + 64 <= counts[1]);
    std::printf("pointer snapshot allocations: shared=%zu original=%zu; independent handles and detach OK\n", counts[0], counts[1]);
    std::size_t slotCounts[2]{};
    for (unsigned mode = 0; mode < 2; ++mode)
    {
        ObjectStore store;
        store.pointerSnapshots = true;
        store.inlineHandles = mode == 0;
        auto pointer = Memory::makeShared<RuntimeObject>();
        pointer->kind = ObjectKind::Obj;
        pointer->objValue.integer = 48;
        pointer->objTextShape = "R0:0:0:8:0:0:";
        const auto original = store.add(std::move(pointer));
        Memory::String error;
        std::int64_t handles[96]{};
        for (auto &handle : handles) handle = store.copyHandle(original, &error);
        assert(error.empty() && store.liveCount() == 97);
        assert(store.inlineHandleCount == (mode == 0 ? 32u : 0u));
        for (auto handle : handles)
        {
            assert(store.find(handle) != nullptr);
            assert(store.release(handle));
            assert(store.find(handle) == nullptr && !store.release(handle));
        }
        assert(store.liveCount() == 1 && store.find(original));
        const auto before = arena.stats().totalBlocks;
        std::int64_t previous = original;
        for (unsigned i = 0; i < 4096; ++i)
        {
            const auto handle = store.copyHandle(original, &error);
            assert(handle > previous && store.find(handle));
            assert(store.find(handle - 32) == nullptr);
            assert(store.release(handle) && store.find(handle) == nullptr);
            assert(store.find(previous) == (previous == original ? store.find(original) : nullptr));
            previous = handle;
        }
        slotCounts[mode] = arena.stats().totalBlocks - before;
        assert(store.release(original) && store.liveCount() == 0);
        assert(store.find(0) == nullptr && store.detachPointerSnapshot(0) == nullptr);
        auto record = Memory::makeShared<RuntimeObject>();
        record->kind = ObjectKind::NativeRecord;
        const auto recordHandle = store.add(std::move(record));
        assert(store.objects.count(recordHandle) == 1 && store.inlineHandleCount == 0);
        assert(store.release(recordHandle));
    }
    assert(slotCounts[0] * 8 < slotCounts[1]);
    assert(arena.stats().liveBlocks == 0);
    std::printf("object slot allocations: inline=%zu original=%zu; collisions, 4096 lifetimes and cleanup OK\n", slotCounts[0], slotCounts[1]);
}
