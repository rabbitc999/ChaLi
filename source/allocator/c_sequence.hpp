/**
 * @file c_sequence.hpp
 * @brief C 对象序列的类型生命周期薄适配。
 */
#pragma once
#include "chali_allocator.hpp"
#include "../runtime_c/chali_sequence.h"
#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace Memory
{
/** @brief 容量、增长与槽移动由 C 服务负责，C++ 只适配对象构造和异常。 */
template <class T>
class CSequence
{
    /** @brief 确保构造期间抛出异常时也能回收已建立的对象。 */
    struct Storage
    {
        ChaliCSequence value{};
        /** @brief 向捕获的分配器归还元素与容量。 */
        ~Storage() { chali_c_sequence_destroy(&value); }
    } storage_;
    /**
     * @brief 从捕获的预算所有者分配存储，不跨越 C 边界抛出异常。
     * @param context 原分配器。
     * @param bytes 字节数。
     * @return 地址；失败返回空。
     */
    static void *allocate(void *context, std::size_t bytes) noexcept
    {
        try
        {
            return Memory::allocate(static_cast<RuntimeAllocator *>(context), bytes, alignof(T));
        } catch (...) { return nullptr; }
    }
    /**
     * @brief 向原所有者归还存储。
     * @param context 原分配器。
     * @param pointer 地址。
     * @param bytes 原字节数。
     */
    static void release(void *context, void *pointer, std::size_t bytes) noexcept
    {
        (void)bytes;
        Memory::release(static_cast<RuntimeAllocator *>(context), pointer);
    }
    /**
     * @brief 在未构造槽中执行不抛异常的类型移动构造。
     * @param target 空槽。
     * @param source 有效源对象。
     */
    static void moveElement(void *target, void *source) noexcept
    {
        static_assert(std::is_nothrow_move_constructible_v<T>, "C sequence requires a nonthrowing move constructor");
        new (target) T(std::move(*static_cast<T *>(source)));
    }
    /**
     * @brief 销毁一个仍有效的类型对象。
     * @param element 元素。
     */
    static void destroyElement(void *element) noexcept { static_cast<T *>(element)->~T(); }
    /**
     * @brief 将 C 状态转换为既有 C++ 异常协议。
     * @param status C 操作状态。
     */
    static void check(ChaliCStatus status)
    {
        if (status == CHALI_C_OUT_OF_MEMORY) throw std::bad_alloc();
        if (status != CHALI_C_OK) throw std::length_error("C 对象序列容量或范围无效");
    }

public:
    using value_type = T;
    using iterator = T *;
    using const_iterator = const T *;
    using allocator_type = Allocator<T>;
    /**
     * @brief 创建空序列，捕获显式或当前分配器。
     * @param allocator 存储所有者。
     */
    explicit CSequence(allocator_type allocator) noexcept
    {
        ChaliCAllocator owner{ allocator.owner, allocate, release };
        (void)chali_c_sequence_init(&storage_.value, sizeof(T), &owner, { moveElement, destroyElement });
    }
    /** @brief 创建序列并捕获当前预算所有者。 */
    CSequence() noexcept : CSequence(allocator_type{}) {}
    /**
     * @brief 复制有效元素，与原容器一样选择当前分配器。
     * @param source 原序列。
     */
    CSequence(const CSequence &source) : CSequence()
    {
        reserve(source.size());
        for (const auto &value : source) emplace_back(value);
    }
    /**
     * @brief 转移存储及其所有者。
     * @param source 原序列，转移后为空。
     */
    CSequence(CSequence &&source) noexcept : CSequence(source.get_allocator()) { swap(source); }
    /**
     * @brief 使用目标原有分配器复制元素，失败保留原值。
     * @param source 源序列。
     * @return 当前对象。
     */
    CSequence &operator=(const CSequence &source)
    {
        if (this != &source)
        {
            if constexpr (std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>)
            {
                reserve(source.size());
                while (size() > source.size()) pop_back();
                const auto previous = size();
                for (std::size_t i = 0; i < previous; ++i) (*this)[i] = source[i];
                for (std::size_t i = previous; i < source.size(); ++i) emplace_back(source[i]);
            }
            else
            {
                CSequence copy(get_allocator());
                copy.reserve(std::max(capacity(), source.size()));
                for (const auto &value : source) copy.emplace_back(value);
                swap(copy);
            }
        }
        return *this;
    }
    /**
     * @brief 按原容器传播规则转移存储与分配器。
     * @param source 源序列。
     * @return 当前对象。
     */
    CSequence &operator=(CSequence &&source) noexcept
    {
        if (this != &source)
        {
            CSequence moved(std::move(source));
            swap(moved);
        }
        return *this;
    }
    /**
     * @brief 查询捕获的分配器。
     * @return 存储所有者。
     */
    allocator_type get_allocator() const noexcept { return allocator_type(static_cast<RuntimeAllocator *>(storage_.value.allocator.context)); }
    /**
     * @brief 查询有效长度。
     * @return 元素数。
     */
    std::size_t size() const noexcept { return storage_.value.length; }
    /**
     * @brief 查询预留容量。
     * @return 槽数。
     */
    std::size_t capacity() const noexcept { return storage_.value.capacity; }
    /**
     * @brief 查询表示允许的最大容量。
     * @return 槽数。
     */
    std::size_t max_size() const noexcept { return static_cast<std::size_t>(PTRDIFF_MAX) / sizeof(T); }
    /**
     * @brief 查询是否为空。
     * @return 无有效元素时为真。
     */
    bool empty() const noexcept { return size() == 0; }
    /**
     * @brief 取得可写首槽。
     * @return 可空地址。
     */
    T *data() noexcept { return static_cast<T *>(storage_.value.data); }
    /**
     * @brief 取得只读首槽。
     * @return 可空地址。
     */
    const T *data() const noexcept { return static_cast<const T *>(storage_.value.data); }
    /**
     * @brief 取得可写起点。
     * @return 首槽。
     */
    iterator begin() noexcept { return data(); }
    /**
     * @brief 取得只读起点。
     * @return 首槽。
     */
    const_iterator begin() const noexcept { return data(); }
    /**
     * @brief 取得可写终点。
     * @return 尾后槽。
     */
    iterator end() noexcept { return empty() ? data() : data() + size(); }
    /**
     * @brief 取得只读终点。
     * @return 尾后槽。
     */
    const_iterator end() const noexcept { return empty() ? data() : data() + size(); }
    /**
     * @brief 访问可写槽。
     * @param index 已验证下标。
     * @return 元素。
     */
    T &operator[](std::size_t index) noexcept { return data()[index]; }
    /**
     * @brief 访问只读槽。
     * @param index 已验证下标。
     * @return 元素。
     */
    const T &operator[](std::size_t index) const noexcept { return data()[index]; }
    /**
     * @brief 访问末元素。
     * @return 非空序列末元素。
     */
    T &back() noexcept { return (*this)[size() - 1]; }
    /**
     * @brief 访问只读末元素。
     * @return 非空序列末元素。
     */
    const T &back() const noexcept { return (*this)[size() - 1]; }
    /**
     * @brief 预留未构造容量。
     * @param count 所需槽数。
     */
    void reserve(std::size_t count)
    {
        if (count > max_size()) throw std::length_error("C 对象序列容量超出范围");
        check(chali_c_sequence_reserve(&storage_.value, count));
    }
    /**
     * @brief 先预留容量再构造元素，分配失败不移动调用方的拥有值。
     * @param arguments 类型构造参数。
     * @return 新元素。
     * @note 构造参数不得借用本序列的槽；追加已有元素使用 push_back。
     */
    template <class... Args>
    T &emplace_back(Args &&...arguments)
    {
        if (size() == max_size()) throw std::length_error("C 对象序列长度超出范围");
        if (size() == capacity()) reserve(size() > max_size() / 2 ? max_size() : std::max<std::size_t>(1, size() * 2));
        T value(std::forward<Args>(arguments)...);
        check(chali_c_sequence_insert_move(&storage_.value, size(), &value));
        return back();
    }
    /**
     * @brief 追加副本。
     * @param value 源元素。
     */
    void push_back(const T &value)
    {
        T copy(value);
        emplace_back(std::move(copy));
    }
    /**
     * @brief 追加移动值。
     * @param value 源元素。
     */
    void push_back(T &&value)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(&value);
        const auto base = reinterpret_cast<std::uintptr_t>(data());
        const bool internal = data() && address >= base && address - base < size() * sizeof(T);
        const auto index = internal ? static_cast<std::size_t>((address - base) / sizeof(T)) : 0;
        if (size() == max_size()) throw std::length_error("C 对象序列长度超出范围");
        if (size() == capacity()) reserve(size() > max_size() / 2 ? max_size() : std::max<std::size_t>(1, size() * 2));
        emplace_back(internal ? std::move((*this)[index]) : std::move(value));
    }
    /** @brief 销毁末元素，保留容量。 */
    void pop_back() noexcept { (void)chali_c_sequence_erase(&storage_.value, size() - 1, 1); }
    /** @brief 清空元素，保留容量。 */
    void clear() noexcept { chali_c_sequence_clear(&storage_.value); }
    /**
     * @brief 删除一个元素并保持后续元素顺序。
     * @param position 已验证的有效元素位置。
     * @return 删除位置的下一元素。
     */
    iterator erase(const_iterator position)
    {
        const auto index = static_cast<std::size_t>(position - begin());
        check(chali_c_sequence_erase(&storage_.value, index, 1));
        return index == size() ? end() : begin() + index;
    }
    /**
     * @brief 用重复值替换序列，失败保持原内容。
     * @param count 元素数。
     * @param value 复制源。
     */
    void assign(std::size_t count, const T &value)
    {
        if constexpr (std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>)
        {
            T snapshot(value);
            reserve(count);
            while (size() > count) pop_back();
            for (auto &item : *this) item = snapshot;
            while (size() < count) emplace_back(snapshot);
        }
        else
        {
            CSequence copy(get_allocator());
            copy.reserve(std::max(capacity(), count));
            for (std::size_t i = 0; i < count; ++i) copy.emplace_back(value);
            swap(copy);
        }
    }
    /**
     * @brief 交换存储及分配器，保持原所有者释放。
     * @param other 另一序列。
     */
    void swap(CSequence &other) noexcept { std::swap(storage_.value, other.storage_.value); }
    /**
     * @brief 按元素字典序比较。
     * @param other 另一序列。
     * @return 当前序列小于另一序列时为真。
     */
    bool operator<(const CSequence &other) const
    {
        const auto count = std::min(size(), other.size());
        for (std::size_t i = 0; i < count; ++i)
        {
            if ((*this)[i] < other[i]) return true;
            if (other[i] < (*this)[i]) return false;
        }
        return size() < other.size();
    }
};
}  // namespace Memory
