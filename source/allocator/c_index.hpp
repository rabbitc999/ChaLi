/**
 * @file c_index.hpp
 * @brief 预算分配器与 C 槽索引的薄适配。
 */
#pragma once
#include "chali_allocator.hpp"
#include "../runtime_c/chali_index.h"
#include <stdexcept>

namespace Memory
{
/** @brief C 开放寻址索引，复制保留目标所有者，移动转移原所有者。 */
class CIndex
{
    /** @brief 构造失败时也释放 C 存储。 */
    struct Storage
    {
        ChaliCIndex value{};
        /** @brief 释放槽表。 */
        ~Storage() { chali_c_index_destroy(&value); }
    } storage_;
    /**
     * @brief 按捕获的预算所有者分配槽表。
     * @param owner 原所有者。
     * @param bytes 字节数。
     * @return 地址；失败为空。
     */
    static void *allocate(void *owner, std::size_t bytes) noexcept
    {
        try
        {
            return Memory::allocate(static_cast<RuntimeAllocator *>(owner), bytes, alignof(std::max_align_t));
        } catch (...) { return nullptr; }
    }
    /**
     * @brief 将槽表归还原所有者。
     * @param owner 原所有者。
     * @param pointer 地址。
     * @param bytes 字节数。
     */
    static void release(void *owner, void *pointer, std::size_t bytes) noexcept
    {
        (void)bytes;
        Memory::release(static_cast<RuntimeAllocator *>(owner), pointer);
    }
    /**
     * @brief 转换 C 状态为原异常协议。
     * @param status 操作状态。
     */
    static void check(ChaliCStatus status)
    {
        if (status == CHALI_C_OUT_OF_MEMORY) throw std::bad_alloc();
        if (status != CHALI_C_OK) throw std::length_error("C 索引容量超出范围");
    }

public:
    /** @brief 建立空索引，捕获当前预算所有者。 */
    CIndex() noexcept
    {
        ChaliCAllocator owner{ currentRuntimeAllocator(), allocate, release };
        (void)chali_c_index_init(&storage_.value, &owner);
    }
    /**
     * @brief 复制到当前预算所有者。
     * @param source 源索引。
     */
    CIndex(const CIndex &source) : CIndex() { check(chali_c_index_copy(&storage_.value, &source.storage_.value)); }
    /**
     * @brief 转移槽表与其所有者。
     * @param source 源索引。
     */
    CIndex(CIndex &&source) noexcept : CIndex()
    {
        storage_.value = source.storage_.value;
        source.storage_.value.slots.data = nullptr;
        source.storage_.value.slots.length = source.storage_.value.slots.capacity = 0;
        source.storage_.value.length = source.storage_.value.deleted = 0;
    }
    /**
     * @brief 复制到目标原所有者，失败保持原值。
     * @param source 源索引。
     * @return 当前对象。
     */
    CIndex &operator=(const CIndex &source)
    {
        check(chali_c_index_copy(&storage_.value, &source.storage_.value));
        return *this;
    }
    /**
     * @brief 转移槽表与其原所有者。
     * @param source 源索引。
     * @return 当前对象。
     */
    CIndex &operator=(CIndex &&source) noexcept
    {
        if (this != &source)
        {
            CIndex moved(std::move(source));
            std::swap(storage_.value, moved.storage_.value);
        }
        return *this;
    }
    /**
     * @brief 查询键的槽号。
     * @param key 完整键位模式。
     * @param value 接收槽号。
     * @return 找到为真。
     */
    bool get(std::uint64_t key, std::size_t &value) const noexcept { return chali_c_index_get(&storage_.value, key, &value) != 0; }
    /**
     * @brief 插入或更新槽号。
     * @param key 键。
     * @param value 槽号。
     */
    void set(std::uint64_t key, std::size_t value) { check(chali_c_index_set(&storage_.value, key, value)); }
    /**
     * @brief 删除键。
     * @param key 键。
     */
    void remove(std::uint64_t key) noexcept { (void)chali_c_index_remove(&storage_.value, key); }
    /** @brief 清空键但保留槽表。 */
    void clear() noexcept { chali_c_index_clear(&storage_.value); }
};
}  // namespace Memory
