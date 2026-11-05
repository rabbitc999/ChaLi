// SPDX-License-Identifier: MIT
// Copyright (c) 2026 小兔糖糖_OvO

#ifndef CAS_MEMORY_H
#define CAS_MEMORY_H

/**
 * @file memory.h
 * @brief 把标准容器接到 CAS 池上的适配层，对应参考工程的 containerAllocator.hpp。
 *
 * 表达式树的所有容器（Add/Mul/Pow 的操作数数组、符号名、输出缓冲区）都使用这里的别名，
 * 因此 memoryStats() 统计到的就是整个引擎的真实占用。
 */

#include "allocator.h"
#include "detail/config.h"
#include "detail/memory_pool.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <deque>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Cas::Memory
{

/** @brief 记录分配归属的容器分配器；对象存活期间即使切换池也能正确归还。 */
template <class T>
class PoolAllocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_swap = std::true_type;
    using is_always_equal = std::true_type;

    PoolAllocator() noexcept = default;
    template <class U>
    PoolAllocator(const PoolAllocator<U> &) noexcept
    {
    }

    /**
     * @brief 从 CAS 池分配；后备存储使用 C malloc 并保持元素对齐。
     * @param count 元素数量。
     * @return 存储地址；无法分配时抛出 bad_alloc。
     */
    T *allocate(std::size_t count)
    {
        if (count > static_cast<std::size_t>(-1) / sizeof(T)) throw std::bad_alloc();
        void *raw = Allocator::ready() ? Allocator::allocate(count * sizeof(T), alignof(T)) : nullptr;
        if (!raw)
        {
            if constexpr (alignof(T) <= alignof(std::max_align_t))
            {
                const auto bytes = count * sizeof(T);
                raw = std::malloc(bytes ? bytes : 1);
                if (!raw) throw std::bad_alloc();
            }
            else
            {
                constexpr auto alignment = alignof(T) > alignof(void *) ? alignof(T) : alignof(void *);
                constexpr auto overhead = sizeof(void *) + alignment - 1u;
                const auto bytes = count * sizeof(T);
                if (bytes > std::numeric_limits<std::size_t>::max() - overhead) throw std::bad_alloc();
                void *storage = std::malloc(bytes + overhead);
                if (!storage) throw std::bad_alloc();
                const auto start = reinterpret_cast<std::uintptr_t>(storage) + sizeof(void *);
                const auto address = (start + alignment - 1u) & ~static_cast<std::uintptr_t>(alignment - 1u);
                raw = reinterpret_cast<void *>(address);
                reinterpret_cast<void **>(raw)[-1] = storage;
            }
        }
        return static_cast<T *>(raw);
    }
    /**
     * @brief 池内地址回池，后备存储用配对的 C free 释放。
     * @param pointer 待释放地址。
     * @param count 原元素数量。
     */
    void deallocate(T *pointer, std::size_t count) noexcept
    {
        (void)count;
        if (!pointer) return;
        if (!Allocator::release(pointer))
        {
            if constexpr (alignof(T) <= alignof(std::max_align_t)) std::free(pointer);
            else std::free(reinterpret_cast<void **>(pointer)[-1]);
        }
    }

    template <class U>
    bool operator==(const PoolAllocator<U> &) const noexcept
    { return true; }
    template <class U>
    bool operator!=(const PoolAllocator<U> &) const noexcept
    { return false; }
};

template <class T>
using Vector = std::vector<T, PoolAllocator<T>>;
template <class T>
using List = std::list<T, PoolAllocator<T>>;
template <class T>
using Deque = std::deque<T, PoolAllocator<T>>;
template <class K, class V, class C = std::less<K>>
using Map = std::map<K, V, C, PoolAllocator<std::pair<const K, V>>>;
template <class K, class C = std::less<K>>
using Set = std::set<K, C, PoolAllocator<K>>;
template <class K, class V, class H = std::hash<K>, class E = std::equal_to<K>>
using UnorderedMap = std::unordered_map<K, V, H, E, PoolAllocator<std::pair<const K, V>>>;
template <class K, class H = std::hash<K>, class E = std::equal_to<K>>
using UnorderedSet = std::unordered_set<K, H, E, PoolAllocator<K>>;

/** @brief 池内字符串。 */
using String = std::basic_string<char, std::char_traits<char>, PoolAllocator<char>>;

/** @brief 池内字符串流。 */
using StringStream = std::basic_stringstream<char, std::char_traits<char>, PoolAllocator<char>>;

/** @brief 由 std::string 转池内字符串。 */
inline String toPoolString(const std::string &text) { return String(text.begin(), text.end()); }

/** @brief 由池内字符串转 std::string。 */
inline std::string toStdString(const String &text) { return std::string(text.begin(), text.end()); }

/** @brief 以池分配构造共享对象。 */
template <class T, class... Args>
std::shared_ptr<T> makeShared(Args &&...args)
{
    return std::allocate_shared<T>(PoolAllocator<T>(), std::forward<Args>(args)...);
}

}  // namespace Cas::Memory

#endif  // CAS_MEMORY_H
