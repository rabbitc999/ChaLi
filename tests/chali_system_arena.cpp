/**
 * @file chali_system_arena.cpp
 * @brief 核对系统 malloc 后端的预算、对齐、活跃身份与并发释放。
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
 * @brief 核对失败路径不漏字节，普通和过度对齐分配均由系统释放。
 * @return 全部断言通过时返回零。
 */
int main()
{
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
