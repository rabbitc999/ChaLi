/**
 * @file c_heap_allocator.hpp
 * @brief 仅使用 C 堆接口的宿主元数据分配适配器。
 */
#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <new>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Memory
{
/** @brief 不依赖运行时 arena、通过 malloc/free 管理元数据的容器适配器。 */
template <class T>
struct CHeapAllocator
{
    using value_type = T;
    using is_always_equal = std::true_type;
    /** @brief 创建不持有状态的 C 堆适配器。 */
    CHeapAllocator() noexcept = default;
    /**
     * @brief 在容器内部节点类型间转换同一分配策略。
     * @param other 来源适配器。
     */
    template <class U>
    CHeapAllocator(const CHeapAllocator<U> &other) noexcept
    { (void)other; }
    /**
     * @brief 分配满足元素对齐的存储，不计入被记录对象的预算。
     * @param count 元素数量。
     * @return 分配地址；失败时抛出 bad_alloc。
     */
    T *allocate(std::size_t count)
    {
        if constexpr (alignof(T) <= alignof(std::max_align_t))
        {
            if (count > std::numeric_limits<std::size_t>::max() / sizeof(T)) throw std::bad_alloc();
            const auto bytes = count * sizeof(T);
            void *storage = std::malloc(bytes ? bytes : 1);
            if (!storage) throw std::bad_alloc();
            return static_cast<T *>(storage);
        }
        else
        {
            constexpr auto alignment = alignof(T) > alignof(void *) ? alignof(T) : alignof(void *);
            constexpr auto overhead = sizeof(void *) + alignment - 1u;
            if (count > (std::numeric_limits<std::size_t>::max() - overhead) / sizeof(T))
                throw std::bad_alloc();
            const auto bytes = count * sizeof(T);
            void *storage = std::malloc(bytes + overhead);
            if (!storage) throw std::bad_alloc();
            const auto start = reinterpret_cast<std::uintptr_t>(storage) + sizeof(void *);
            const auto address = (start + alignment - 1u) & ~static_cast<std::uintptr_t>(alignment - 1u);
            auto *result = reinterpret_cast<T *>(address);
            reinterpret_cast<void **>(result)[-1] = storage;
            return result;
        }
    }
    /**
     * @brief 使用配对的 C free 释放存储。
     * @param pointer 本适配器分配的地址或空指针。
     * @param count 原元素数量。
     */
    void deallocate(T *pointer, std::size_t count) noexcept
    {
        (void)count;
        if constexpr (alignof(T) <= alignof(std::max_align_t))
            std::free(pointer);
        else if (pointer)
            std::free(reinterpret_cast<void **>(pointer)[-1]);
    }
    /**
     * @brief 查询两个无状态适配器是否能互相释放存储。
     * @param other 待比较适配器。
     * @return 始终为真。
     */
    template <class U>
    bool operator==(const CHeapAllocator<U> &other) const noexcept
    {
        (void)other;
        return true;
    }
    /**
     * @brief 查询两个适配器是否使用不同的堆。
     * @param other 待比较适配器。
     * @return 始终为假。
     */
    template <class U>
    bool operator!=(const CHeapAllocator<U> &other) const noexcept
    {
        (void)other;
        return false;
    }
};
/** @brief 适合进程级缓存、不会借用执行 arena 的 C 堆字符串。 */
using CHeapString = std::basic_string<char, std::char_traits<char>, CHeapAllocator<char>>;
/** @brief 使用 C 堆的 UTF-16 字符串。 */
using CHeapU16String = std::basic_string<char16_t, std::char_traits<char16_t>, CHeapAllocator<char16_t>>;
/** @brief 使用 C 堆的连续存储，生命周期可独立于执行 arena。 */
template <class T>
using CHeapVector = std::vector<T, CHeapAllocator<T>>;
/** @brief 保存完整对象地址和析构入口，支持拥有型指针的基类转换。 */
template <class T>
struct CHeapDeleter
{
    void *storage = nullptr;
    void (*dispose)(void *) = nullptr;
    /** @brief 创建空对象的删除策略。 */
    CHeapDeleter() noexcept = default;
    /**
     * @brief 记录具体对象的释放入口。
     * @param allocation 完整对象地址。
     * @param callback 析构与 C 堆释放入口。
     */
    CHeapDeleter(void *allocation, void (*callback)(void *)) noexcept : storage(allocation), dispose(callback) {}
    /**
     * @brief 转换指针类型时保留原始释放信息。
     * @param other 来源策略。
     */
    template <class U>
    CHeapDeleter(const CHeapDeleter<U> &other) noexcept : storage(other.storage), dispose(other.dispose)
    {
    }
    /**
     * @brief 析构原始具体对象并释放 C 堆存储。
     * @param pointer 拥有的对象地址，允许基类子对象地址。
     */
    void operator()(T *pointer) const noexcept
    {
        if (pointer) dispose(storage);
    }
};
/** @brief 使用 C 堆存储的唯一拥有型指针。 */
template <class T>
using CHeapUniquePtr = std::unique_ptr<T, CHeapDeleter<T>>;
/**
 * @brief 析构独立 C 堆上的具体对象。
 * @param pointer 完整对象地址。
 */
template <class T>
void destroyCHeap(T *pointer) noexcept
{
    if (!pointer) return;
    pointer->~T();
    CHeapAllocator<T>().deallocate(pointer, 1);
}
/**
 * @brief 在 C 堆上构造引导对象，构造失败自动归还存储。
 * @param args 构造参数。
 * @return 记录具体析构入口的拥有型指针。
 */
template <class T, class... Args>
CHeapUniquePtr<T> makeCHeapUnique(Args &&...args)
{
    T *storage = CHeapAllocator<T>().allocate(1);
    try
    {
        new (storage) T(std::forward<Args>(args)...);
        return CHeapUniquePtr<T>(storage, CHeapDeleter<T>(storage, [](void *pointer) { destroyCHeap(static_cast<T *>(pointer)); }));
    } catch (...)
    {
        CHeapAllocator<T>().deallocate(storage, 1);
        throw;
    }
}
/** @brief 保存数组长度以调用逐元素析构和配对的 C free。 */
template <class T>
struct CHeapArrayDeleter
{
    std::size_t count = 0;
    /**
     * @brief 结束所有数组元素的生命周期并释放原始存储。
     * @param pointer 首元素地址。
     */
    void operator()(T *pointer) const noexcept
    {
        if (!pointer) return;
        std::destroy_n(pointer, count);
        CHeapAllocator<T>().deallocate(pointer, count);
    }
};
/** @brief 使用 C 堆并保留真实元素指针的数组，bool 也逐字节存储。 */
template <class T>
using CHeapArrayPtr = std::unique_ptr<T[], CHeapArrayDeleter<T>>;
/**
 * @brief 值初始化 C 堆数组，元素构造失败时归还已构造对象与存储。
 * @param count 元素数量。
 * @return 数组所有者；零长度返回空指针。
 */
template <class T>
CHeapArrayPtr<T> makeCHeapArray(std::size_t count)
{
    if (!count) return CHeapArrayPtr<T>();
    T *storage = CHeapAllocator<T>().allocate(count);
    try
    {
        std::uninitialized_value_construct_n(storage, count);
        return CHeapArrayPtr<T>(storage, CHeapArrayDeleter<T>{ count });
    } catch (...)
    {
        CHeapAllocator<T>().deallocate(storage, count);
        throw;
    }
}
}  // namespace Memory
