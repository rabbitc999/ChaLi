/**
 * @file system_arena.hpp
 * @brief 保留预算和分配身份统计的系统 malloc 宿主分配器。
 */
#pragma once
#include "chali_allocator.hpp"
#include <map>

namespace Memory
{
/**
 * @brief 每次分配直接调用系统 malloc，独立跟踪活跃范围与运行预算。
 */
class SystemRuntimeArena final : public RuntimeAllocator
{
    struct Allocation
    {
        void *storage;
        std::size_t bytes;
    };
    std::size_t capacity_;
    mutable std::mutex mutex_;
    std::map<std::uintptr_t, Allocation> allocations_;
    Stats stats_{};

public:
    /**
     * @brief 设置本轮宿主分配的有效载荷预算。
     * @param capacity 允许同时存活的字节数。
     */
    explicit SystemRuntimeArena(std::size_t capacity);
    /**
     * @brief 核对所有运行对象均已释放，拒绝带着活动分配销毁。
     */
    ~SystemRuntimeArena() override;
    /**
     * @brief 使用系统 malloc 分配普通对齐存储。
     * @param bytes 有效载荷字节数。
     * @return 分配地址；预算不足或系统失败时为空。
     */
    void *allocate(std::size_t bytes) override;
    /**
     * @brief 分配满足指定对齐的系统存储。
     * @param bytes 有效载荷字节数。
     * @param alignment 必须为二的幂。
     * @return 分配地址；参数无效、预算不足或分配失败时为空。
     */
    void *allocateAligned(std::size_t bytes, std::size_t alignment) override;
    /**
     * @brief 只释放仍然活跃的精确分配首地址。
     * @param pointer 待释放地址，空地址允许重复释放。
     * @return 已释放或输入为空时为真。
     */
    bool release(void *pointer) override;
    /**
     * @brief 检查地址是否属于本分配器的活跃有效载荷。
     * @param pointer 待查询的宿主地址。
     * @return 落在活跃范围内时为真。
     */
    bool owns(const void *pointer) const override;
    /**
     * @brief 查询真实的活跃分配、峰值与累计分配次数。
     * @return 在同一互斥区获取的统计快照。
     */
    Stats stats() const override;
    /**
     * @brief 查询当前运行的有效载荷预算。
     * @return 预算字节数。
     */
    std::size_t capacity() const override;
    /**
     * @brief 标明语言字节堆也应使用独立的系统 malloc 分配。
     * @return 始终为真。
     */
    bool usesSystemMalloc() const override;
};
}  // namespace Memory
