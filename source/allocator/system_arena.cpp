/**
 * @file system_arena.cpp
 * @brief 直接系统分配、对齐、预算与存活统计。
 */
#include "system_arena.hpp"

namespace Memory
{
/**
 * @brief 初始化系统分配预算，不预留连续内存池。
 * @param capacity 有效载荷预算。
 */
SystemRuntimeArena::SystemRuntimeArena(std::size_t capacity) : capacity_(capacity) {}

/**
 * @brief 在运行对象析构后核对没有遗漏的宿主分配。
 */
SystemRuntimeArena::~SystemRuntimeArena()
{
    if (!allocations_.empty())
    {
        std::fputs("运行时分配器：malloc 仍有活动块，不能销毁\n", stderr);
        std::abort();
    }
}

/**
 * @brief 通过系统 malloc 分配普通对齐存储。
 * @param bytes 有效载荷字节数。
 * @return 成功地址；失败为空。
 */
void *SystemRuntimeArena::allocate(std::size_t bytes)
{
    return allocateAligned(bytes, alignof(std::max_align_t));
}

/**
 * @brief 使用 malloc 分配并记录真实存储，必要时在存储内对齐。
 * @param bytes 有效载荷字节数。
 * @param alignment 二的幂对齐要求。
 * @return 成功地址；无效或失败为空。
 */
void *SystemRuntimeArena::allocateAligned(std::size_t bytes, std::size_t alignment)
{
    if (!alignment || (alignment & (alignment - 1u))) return nullptr;
    const auto payload = std::max<std::size_t>(bytes, 1u);
    const auto padding = alignment > alignof(std::max_align_t) ? alignment - 1u : 0u;
    if (payload > std::numeric_limits<std::size_t>::max() - padding) return nullptr;
    std::lock_guard<std::mutex> lock(mutex_);
    if (bytes > capacity_ - stats_.liveBytes) return nullptr;
    void *storage = std::malloc(payload + padding);
    if (!storage) return nullptr;
    const auto raw = reinterpret_cast<std::uintptr_t>(storage);
    const auto address = padding ? (raw + padding) & ~static_cast<std::uintptr_t>(padding) : raw;
    try
    {
        allocations_.emplace(address, Allocation{ storage, bytes });
    } catch (...)
    {
        std::free(storage);
        return nullptr;
    }
    stats_.liveBytes += bytes;
    ++stats_.liveBlocks;
    ++stats_.totalBlocks;
    stats_.peakBytes = std::max(stats_.peakBytes, stats_.liveBytes);
    return reinterpret_cast<void *>(address);
}

/**
 * @brief 撤销精确首地址的活跃身份并调用系统 free。
 * @param pointer 待释放地址。
 * @return 首地址有效或输入为空时为真。
 */
bool SystemRuntimeArena::release(void *pointer)
{
    if (!pointer) return true;
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = allocations_.find(reinterpret_cast<std::uintptr_t>(pointer));
    if (found == allocations_.end()) return false;
    const auto allocation = found->second;
    stats_.liveBytes -= allocation.bytes;
    --stats_.liveBlocks;
    allocations_.erase(found);
    std::free(allocation.storage);
    return true;
}

/**
 * @brief 以有序活跃范围查询地址，不读取未知地址的内容。
 * @param pointer 待查询宿主地址。
 * @return 地址属于活跃分配时为真。
 */
bool SystemRuntimeArena::owns(const void *pointer) const
{
    const auto address = reinterpret_cast<std::uintptr_t>(pointer);
    std::lock_guard<std::mutex> lock(mutex_);
    auto after = allocations_.upper_bound(address);
    if (after == allocations_.begin()) return false;
    --after;
    return address - after->first < std::max<std::size_t>(after->second.bytes, 1u);
}

/**
 * @brief 在锁内获取本轮统计快照。
 * @return 活跃量、峰值与累计分配次数。
 */
RuntimeAllocator::Stats SystemRuntimeArena::stats() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

/**
 * @brief 返回有效载荷预算。
 * @return 初始化指定的预算字节数。
 */
std::size_t SystemRuntimeArena::capacity() const { return capacity_; }

/**
 * @brief 让语言字节堆使用直接 malloc 后端。
 * @return 始终为真。
 */
bool SystemRuntimeArena::usesSystemMalloc() const { return true; }
}  // namespace Memory
