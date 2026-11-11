/**
 * @file chali_system_arena.cpp
 * @brief 核对编译器默认分配器及运行时 malloc 后端的预算、对齐、活跃身份与并发释放。
 */
#include "allocator/system_arena.hpp"
#include <cassert>
#include <thread>
#include <vector>

/**
 * @brief 在共享系统分配器中反复分配和释放。
 * @param arena 所有线程共同使用的分配器。
 */
void exercise(Memory::SystemRuntimeArena &arena)
{
    for (int index = 0; index < 2000; ++index)
    {
        void *pointer = arena.allocate(37);
        assert(pointer && arena.owns(pointer));
        assert(arena.release(pointer));
    }
}

/**
 * @brief 核对编译器可分配超过旧预算的内存，以及运行时的预算和释放行为。
 * @return 全部断言通过时返回零。
 */
int main()
{
#ifndef NO_CHALI_DEFAULT_POOL
    auto *fallback = Memory::defaultRuntimeAllocator();
    const auto previousBlocks = fallback->stats().liveBlocks;
    {
        Memory::Vector<unsigned char> early(8u, 7u);
        assert(fallback->owns(early.data()));
        Memory::installCompilerAllocator();
        assert(early.front() == 7u && fallback->owns(early.data()));
    }
    assert(fallback->stats().liveBlocks == previousBlocks);
#else
    Memory::installCompilerAllocator();
#endif
    auto *compiler = Memory::defaultRuntimeAllocator();
    assert(compiler->usesSystemMalloc());
    constexpr std::size_t bytes = Memory::defaultRuntimeCapacity + 1024u * 1024u;
    auto *buffer = static_cast<unsigned char *>(Memory::allocate(nullptr, bytes, 64));
    assert(reinterpret_cast<std::uintptr_t>(buffer) % 64 == 0);
    buffer[0] = 1;
    buffer[bytes - 1] = 2;
    assert(compiler->owns(buffer + bytes - 1));
    assert(compiler->stats().liveBytes == bytes);
    Memory::release(nullptr, buffer);
    assert(compiler->stats().liveBlocks == 0);

    Memory::SystemRuntimeArena arena(4096);
    assert(arena.usesSystemMalloc());
    void *first = arena.allocate(4096);
    assert(first && arena.owns(static_cast<char *>(first) + 4095));
    assert(!arena.owns(static_cast<char *>(first) + 4096));
    assert(!arena.allocate(1));
    assert(!arena.release(static_cast<char *>(first) + 1));
    assert(arena.release(first));
    assert(!arena.release(first));
    assert(!arena.owns(first));
    assert(!arena.allocateAligned(1, 3));
    void *aligned = arena.allocateAligned(65, 4096);
    assert(aligned && reinterpret_cast<std::uintptr_t>(aligned) % 4096 == 0);
    assert(arena.stats().liveBytes == 65 && arena.stats().liveBlocks == 1);
    assert(arena.release(aligned));
    std::vector<std::thread> threads;
    for (int index = 0; index < 4; ++index) threads.emplace_back(exercise, std::ref(arena));
    for (auto &thread : threads) thread.join();
    const auto stats = arena.stats();
    assert(stats.liveBytes == 0 && stats.liveBlocks == 0 && stats.peakBytes == 4096);
    assert(stats.totalBlocks == 8002);
}
