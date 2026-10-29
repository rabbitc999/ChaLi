/**
 * @file chali_numeric_list_storage.cpp
 * @brief 核对数值列表槽布局、读取、兼容物化、容量和最终释放。
 */
#include "ircode/runtime_internal.hpp"
#include "glue/std_collection_state.hpp"
#include "allocator/system_arena.hpp"
#include <cassert>
#include <cstdio>

/**
 * @brief 比较紧凑布局与通用回退的位模式及生命周期。
 * @return 全部断言通过时返回零。
 */
int main()
{
    using namespace stackvm::runtimeDetail;
    Memory::SystemRuntimeArena arena(16 * 1024 * 1024);
    Memory::AllocationScope allocation(&arena);
    {
        ObjectStore objects;
        NativeContext context;
        context.objects = &objects;
        NativeCallContextImpl host(context);
        const auto handle = collection_glue::listStorageScalarOperation(host, 9, 0, 0, 0, 0, 0, 0).bits;
        auto *record = static_cast<collection_glue::Record *>(host.nativeRecord(handle, collection_glue::descriptor(collection_glue::StorageKind::Vector)));
        assert(record);
        const auto *disabled = std::getenv("CHALI_NO_NUMERIC_LIST_STORAGE");
        const bool compact = !(disabled && *disabled);
        for (std::int64_t index = 0; index < 257; ++index)
            collection_glue::listStorageScalarOperation(host, 18, handle, index - 128, 0, 0, 0, 0);
        assert(context.error.empty());
        assert(record->scalarItemsActive == compact);
        assert((compact ? record->scalarItems.size() : record->items.size()) == 257);
        assert((compact ? record->scalarItems.capacity() : record->items.capacity()) == 512);
        if (compact) assert(record->items.empty() && record->items.capacity() == 0);
        for (std::int64_t index = 0; index < 257; ++index)
        {
            const auto value = collection_glue::listStorageScalarOperation(host, 11, handle, index, 0, 0, 0, 0);
            assert(!value.floating && value.bits == index - 128);
        }
        assert(record->scalarItemsActive == compact);
        const std::uint64_t patterns[] = { 0, 0x8000000000000000ULL, 0x7ff8000000000042ULL, 1 };
        for (auto pattern : patterns)
        {
            std::int64_t bits = 0;
            std::memcpy(&bits, &pattern, sizeof(bits));
            collection_glue::listStorageScalarOperation(host, 18, handle, bits, 0, 0, 0, 2);
            const auto index = collection_glue::listStorageScalarOperation(host, 10, handle, 0, 0, 0, 0, 0).bits - 1;
            const auto value = collection_glue::listStorageScalarOperation(host, 11, handle, index, 0, 0, 0, 0);
            assert(value.floating && value.bits == bits);
        }
        // 通用写入触发布局回退，已有整数及浮点位模式、容量保持不变。
        collection_glue::listStorageScalarOperation(host, 12, handle, 0, 999, 0, 0, 0);
        assert(context.error.empty() && !record->scalarItemsActive);
        assert(record->items.size() == 261 && record->items.capacity() == 512);
        if (compact) assert(record->scalarItems.empty() && record->scalarItems.capacity() == 512);
        assert(record->items[0].integer == 999 && record->items[1].integer == -127);
        for (std::size_t index = 0; index < 4; ++index)
        {
            std::uint64_t bits = 0;
            std::memcpy(&bits, &record->items[257 + index].real, sizeof(bits));
            assert(record->items[257 + index].isFloat && bits == patterns[index]);
        }
        assert(host.releaseNativeRecord(handle));
        assert(!host.nativeRecord(handle, collection_glue::descriptor(collection_glue::StorageKind::Vector)));
        assert(objects.liveCount() == 0);
    }
    assert(arena.stats().liveBlocks == 0);
    std::printf("numeric List storage: scalar slot=%zu, generic slot=%zu; values, fallback, capacity and drop OK\n",
                sizeof(collection_glue::ScalarResult), sizeof(stackvm::NativeStoredValue));
}
