/**
 * @file move_storage_ranges.cpp
 * @brief 核对生命周期范围索引与完整扫描具有相同的重叠及包含语义。
 */
#include "source/ircode/runtime_internal.hpp"
#include <cassert>
#include <limits>
#include <random>

/**
 * @brief 比较随机生命周期范围复位与朴素参考实现，并覆盖同址子对象。
 * @return 全部断言通过时返回零。
 */
int main()
{
    stackvm::runtimeDetail::ObjectStore store;
    std::mt19937_64 random(20261026);
    for (int round = 0; round < 100; ++round)
    {
        store.movedStorage.clear();
        store.largestMovedStorage = 0;
        for (int i = 0; i < 500; ++i)
        {
            const std::size_t address = random() % 100000;
            const std::size_t bytes = 1 + random() % 300;
            store.movedStorage[{address, static_cast<std::int64_t>(random() % 5)}] = bytes;
            store.largestMovedStorage = std::max(store.largestMovedStorage, bytes);
        }
        for (int step = 0; step < 50; ++step)
        {
            const std::size_t address = random() % 100000;
            const std::size_t bytes = random() % 500;
            const bool fresh = random() % 2;
            auto reference = store.movedStorage;
            for (auto it = reference.begin(); it != reference.end();)
            {
                const auto start = it->first.first;
                const bool overlap = start >= address ? start - address < bytes : address - start < it->second;
                const bool contained = start >= address && start - address < bytes && it->second <= bytes - (start - address);
                if (fresh ? overlap : contained) it = reference.erase(it);
                else ++it;
            }
            store.beginStorage(address, bytes, fresh);
            assert(store.movedStorage == reference);
        }
    }
    store.movedStorage = {{{100, 1}, 64}, {{108, 2}, 8}, {{108, 3}, 8}, {{200, 4}, 8}};
    store.largestMovedStorage = 64;
    store.beginStorage(108, 8);
    assert(store.movedStorage.count({100, 1}) == 1 && store.movedStorage.size() == 2);
    store.beginStorage(120, 8, true);
    assert(store.movedStorage.size() == 1 && store.movedStorage.count({200, 4}) == 1);
    return 0;
}
