/** @file chali_bigint_c_ownership.cpp @brief 核对 C 大数适配层跨上下文的预算、复制和移动所有权。 */
#include "bigint.hpp"
#include "../source/allocator/system_arena.hpp"
#include <cassert>
#include <optional>

/**
 * @brief 核对当前工作区预算和目标所有者不会混用，所有活动分配最终归零。
 * @return 通过返回零。
 */
int main()
{
    auto *defaultBefore = Memory::defaultRuntimeAllocatorIfCreated();
    Memory::SystemRuntimeArena first(128 * 1024), second(128 * 1024);
    {
        std::optional<bigint::BigInt> source, copied, quotient, remainder, divisor;
        {
            Memory::AllocationScope scope(&first);
            source.emplace(Memory::String("123456789012345678901234567890"));
            source->shiftLeft(512);
            quotient.emplace(13);
            remainder.emplace(17);
        }
        const auto firstBytes = first.stats().liveBytes;
        {
            Memory::AllocationScope scope(&second);
            copied.emplace(*source);
            divisor.emplace(7);
            assert(first.stats().liveBytes == firstBytes);
            void *filler = second.allocate(second.capacity() - second.stats().liveBytes);
            assert(filler);
            bool rejected = false;
            try { source->divmod(*divisor, &*quotient, &*remainder); }
            catch (const std::bad_alloc &) { rejected = true; }
            assert(rejected);
            assert(quotient->toU64() == 13 && remainder->toU64() == 17);
            assert(first.stats().liveBytes == firstBytes);
            assert(second.stats().liveBytes == second.capacity());
            assert(second.release(filler));
            Memory::allocationFailureSignal().clear();
            assert(source->divmod(*divisor, &*quotient, &*remainder));
            assert(*quotient * *divisor + *remainder == *source);
            *copied = *source;
        }
        {
            Memory::AllocationScope scope(&second);
            source->shiftLeft(1);
        }
        const auto secondBytes = second.stats().liveBytes;
        {
            Memory::AllocationScope scope(&first);
            source->shiftRight(1);
        }
        assert(second.stats().liveBytes < secondBytes);
        {
            Memory::AllocationScope scope(&first);
            assert(*source == *copied);
        }
    }
    assert(first.stats().liveBlocks == 0 && second.stats().liveBlocks == 0);
    assert(Memory::defaultRuntimeAllocatorIfCreated() == defaultBefore);
}
