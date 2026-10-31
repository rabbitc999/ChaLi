/** @file c_api_failure_arena.hpp @brief C API 回归共享的可控分配器。 */
#pragma once
#include "allocator/system_arena.hpp"

/** @brief 在每个分配位置注入失败，实际内存仍由系统分配器跟踪。 */
struct FailingArena final : Memory::RuntimeAllocator
{
    Memory::SystemRuntimeArena storage;
    /** @brief 设置测试预算。
     * @param bytes 可同时存活的字节数。
     */
    explicit FailingArena(std::size_t bytes = 4 * 1024 * 1024) : storage(bytes) {}
    int remaining = -1;
    /**
     * @brief 根据剩余额度分配普通存储。
     * @param bytes 字节数量。
     * @return 分配地址，注入点为空。
     */
    void *allocate(std::size_t bytes) override
    {
        if (remaining == 0) return nullptr;
        if (remaining > 0) --remaining;
        return storage.allocate(bytes);
    }
    /**
     * @brief 根据剩余额度分配对齐存储。
     * @param bytes 字节数量。
     * @param alignment 对齐要求。
     * @return 分配地址，注入点为空。
     */
    void *allocateAligned(std::size_t bytes, std::size_t alignment) override
    {
        if (remaining == 0) return nullptr;
        if (remaining > 0) --remaining;
        return storage.allocateAligned(bytes, alignment);
    }
    /**
     * @brief 归还真实分配。
     * @param pointer 分配地址。
     * @return 释放是否成功。
     */
    bool release(void *pointer) override { return storage.release(pointer); }
    /**
     * @brief 查询活跃分配。
     * @param pointer 地址。
     * @return 地址仍存活时为真。
     */
    bool owns(const void *pointer) const override { return storage.owns(pointer); }
    /**
     * @brief 取得实际分配统计。
     * @return 系统分配器统计。
     */
    Stats stats() const override { return storage.stats(); }
    /** @brief 取得预算。
     * @return 字节预算。
     */
    std::size_t capacity() const override { return storage.capacity(); }
    /** @brief 使用系统 malloc 字节堆。
     * @return 恒为真。
     */
    bool usesSystemMalloc() const override { return true; }
};
