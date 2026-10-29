/**
 * @file std_numeric_list_storage.hpp
 * @brief 为 LLVM 数值操作提供稳定头部，缓冲区由捕获的宿主分配器拥有。
 */
#pragma once
#include "std_collection.hpp"

namespace collection_glue
{
/** @brief LLVM 可直接访问的数值数组头部，长度与容量均以槽为单位。 */
struct ScalarListHeader
{
    ScalarResult *data = nullptr;
    std::uint64_t length = 0;
    std::uint64_t capacity = 0;
};

/** @brief 保留容量的数值数组，物理槽由标准容器负责分配及释放。 */
class ScalarListStorage
{
    Memory::Vector<ScalarResult> backing_;

public:
    ScalarListHeader header;
    /** @brief 创建空的数值数组。 */
    ScalarListStorage() = default;
    /**
     * @brief 复制有效元素，新数组容量沿原标准容器复制规则取有效长度。
     * @param source 源数组。
     */
    ScalarListStorage(const ScalarListStorage &source) : backing_(source.backing_.get_allocator())
    {
        reserve(source.size());
        for (std::size_t index = 0; index < source.size(); ++index) header.data[index] = source[index];
        header.length = source.size();
    }
    /**
     * @brief 将另一数组复制到当前分配器，重新绑定头部。
     * @param source 源数组。
     * @return 当前数组。
     */
    ScalarListStorage &operator=(const ScalarListStorage &source)
    {
        if (this != &source)
        {
            reserve(source.size());
            for (std::size_t index = 0; index < source.size(); ++index) header.data[index] = source[index];
            header.length = source.size();
        }
        return *this;
    }
    /**
     * @brief 查询有效元素数。
     * @return 有效长度。
     */
    std::size_t size() const { return static_cast<std::size_t>(header.length); }
    /**
     * @brief 查询可写槽数。
     * @return 已分配容量。
     */
    std::size_t capacity() const { return static_cast<std::size_t>(header.capacity); }
    /**
     * @brief 查询是否为空。
     * @return 没有有效元素时为真。
     */
    bool empty() const { return size() == 0; }
    /**
     * @brief 为连续槽预留容量，成功后更新稳定头部。
     * @param count 至少需要的槽数。
     */
    void reserve(std::size_t count)
    {
        if (count <= capacity()) return;
        backing_.reserve(count);
        backing_.resize(count);
        header.data = backing_.data();
        header.capacity = count;
    }
    /**
     * @brief 追加一个数值槽，调用者通常已按统一策略预留容量。
     * @param value 完整数值位模式与种类。
     */
    void push_back(ScalarResult value)
    {
        if (size() == capacity()) reserve(size() + 1);
        header.data[header.length++] = value;
    }
    /** @brief 移除最后一个元素，保留缓冲区。 */
    void pop_back() { --header.length; }
    /** @brief 清空有效元素，保留缓冲区。 */
    void clear() { header.length = 0; }
    /**
     * @brief 访问可写数值槽，边界由集合入口验证。
     * @param index 槽下标。
     * @return 对应槽的引用。
     */
    ScalarResult &operator[](std::size_t index) { return header.data[index]; }
    /**
     * @brief 访问只读数值槽，边界由集合入口验证。
     * @param index 槽下标。
     * @return 对应槽的引用。
     */
    const ScalarResult &operator[](std::size_t index) const { return header.data[index]; }
    /**
     * @brief 取得有效序列起点。
     * @return 首槽地址。
     */
    const ScalarResult *begin() const { return header.data; }
    /**
     * @brief 取得有效序列终点。
     * @return 尾后地址；空数组允许为空。
     */
    const ScalarResult *end() const { return size() ? header.data + size() : header.data; }
};
static_assert(sizeof(void *) != 8 || sizeof(ScalarResult) == 16, "LLVM scalar slot stride");
static_assert(offsetof(ScalarResult, floating) == 8, "LLVM scalar kind offset");
static_assert(sizeof(void *) != 8 || offsetof(ScalarListHeader, length) == 8, "LLVM list length offset");
static_assert(sizeof(void *) != 8 || offsetof(ScalarListHeader, capacity) == 16, "LLVM list capacity offset");
static_assert(sizeof(void *) != 8 || offsetof(NumericListView, revision) == 16, "LLVM list revision offset");
static_assert(sizeof(void *) != 8 || sizeof(NumericListView) == 32, "LLVM list view size");
}  // namespace collection_glue
