/** @file chali_collection_storage.cpp @brief C 存储支持的实际集合键值与候选桶回归。 */
#include "../source/abi/std_collection_storage.hpp"
#include "../source/allocator/system_arena.hpp"
#include <cassert>
#include <cstdio>

/**
 * @brief 构造故意制造大量哈希冲突的数值键。
 * @param value 数值。
 * @return 独立键快照。
 */
static collection_glue::ObjectKey keyFor(std::int64_t value)
{
    collection_glue::ObjectKey key;
    key.integer = value;
    key.hashed = true;
    key.hash = value % 7;
    return key;
}
/**
 * @brief 构造原生数值快照。
 * @param value 数值。
 * @return 快照。
 */
static stackvm::NativeStoredValue valueFor(std::int64_t value)
{
    stackvm::NativeStoredValue result;
    result.integer = value;
    return result;
}
/**
 * @brief 检查补洞、复制、延迟重建和各预算失败后的存储一致性。
 * @return 全部断言通过返回零。
 */
int main()
{
    Memory::SystemRuntimeArena owner(16 * 1024 * 1024);
    {
        Memory::AllocationScope scope(&owner);
        collection_glue::Entries entries;
        for (int i = -512; i < 512; ++i) entries.emplace(keyFor(i), valueFor(i * 31));
        for (int i = -512; i < 512; ++i)
        {
            auto position = entries.find(keyFor(i));
            assert(position != entries.end() && position->second.integer == i * 31);
            if (i % 3 == 0) entries.erase(position);
        }
        for (int i = -512; i < 512; ++i)
        {
            auto position = entries.find(keyFor(i));
            assert((position != entries.end()) == (i % 3 != 0));
            if (position != entries.end()) assert(position->second.integer == i * 31);
        }
        collection_glue::Entries copy(entries);
        entries.clear();
        for (int i = -512; i < 512; ++i) assert((copy.find(keyFor(i)) != copy.end()) == (i % 3 != 0));
        for (int i = 0; i < 1000; ++i) entries.appendUnindexed(keyFor(i), valueFor(i * 7));
        for (int i = 0; i < 1000; ++i) assert(entries.find(keyFor(i))->second.integer == i * 7);
        for (int i = 0; i < 200; ++i) entries.eraseUnindexed(0);
        for (const auto &pair : entries) assert(entries.find(pair.first)->second.integer == pair.second.integer);
        collection_glue::ObjectKey aggregate;
        aggregate.isStruct = true;
        aggregate.hashed = true;
        aggregate.hash = INT64_MIN;
        aggregate.fields.push_back(keyFor(-17));
        aggregate.fields.emplace_back();
        aggregate.fields.back().isText = true;
        aggregate.fields.back().text = "nested key with owned text";
        entries.emplace(aggregate, valueFor(87));
        auto found = entries.find(aggregate);
        assert(found != entries.end() && found->second.integer == 87);
        copy = entries;
        assert(copy.find(aggregate)->second.integer == 87);
    }
    assert(owner.stats().liveBlocks == 0);
    for (std::size_t budget = 128; budget < 20000; budget += 137)
    {
        Memory::SystemRuntimeArena limited(budget);
        {
            Memory::AllocationScope scope(&limited);
            collection_glue::Entries entries;
            int inserted = 0;
            try
            {
                for (; inserted < 40; ++inserted)
                {
                    auto key = keyFor(inserted);
                    key.hash = inserted;
                    entries.emplace(std::move(key), valueFor(inserted));
                }
            }
            catch (const std::bad_alloc &) {}
            assert(entries.size() == static_cast<std::size_t>(inserted));
            for (int i = 0; i < 40; ++i)
            {
                auto key = keyFor(i);
                key.hash = i;
                auto found = entries.find(key);
                assert((found != entries.end()) == (i < inserted));
                if (found != entries.end()) assert(found->second.integer == i);
            }
        }
        assert(limited.stats().liveBlocks == 0);
    }
    puts("C collection storage: collisions, swap erase, rebuild, nested keys, copies and 146 budgets passed");
}
