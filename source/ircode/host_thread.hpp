/**
 * @file host_thread.hpp
 * @brief C 线程服务的独占生命周期适配。
 */
#pragma once
#include "../abi/chali_thread.h"
#include "vm_config.hpp"

namespace stackvm
{
/**
 * @brief 持有一个可等待的 C 线程句柄，供隔离任务复用。
 */
class HostThread
{
    ChaliThread *thread_ = nullptr;
public:
    /** @brief 建立空线程句柄。 */
    HostThread() = default;
    /**
     * @brief 启动一次可移动任务。
     * @param task 线程执行体。
     */
    explicit HostThread(Memory::Function<void()> task);
    /**
     * @brief 接管源句柄并清空源。
     * @param other 可移动线程句柄。
     */
    HostThread(HostThread &&other) noexcept;
    /**
     * @brief 将未持有运行线程的接收者替换为源句柄。
     * @param other 可移动线程句柄。
     * @return 当前句柄。
     */
    HostThread &operator=(HostThread &&other) noexcept;
    HostThread(const HostThread &) = delete;
    HostThread &operator=(const HostThread &) = delete;
    /** @brief 拒绝遗弃尚未等待的线程，保持独占等待契约。 */
    ~HostThread();
    /** @brief 判断是否仍持有待等待的线程。 */
    bool joinable() const noexcept { return thread_ != nullptr; }
    /** @brief 等待线程退出并清空本句柄，失败时抛出宿主诊断。 */
    void join();
};
}
