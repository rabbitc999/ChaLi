/**
 * @file host_thread.cpp
 * @brief 将 C++ 调用体的所有权交给独立 C 线程服务。
 */
#include "host_thread.hpp"
#include "../allocator/c_heap_allocator.hpp"
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

namespace runtime
{
namespace
{
/**
 * @brief 在线程内回收一次性调用体，并阻止异常跨越平台线程边界。
 * @param argument 被接管的调用体。
 */
void executeThread(void *argument)
{
    auto *task = static_cast<Memory::Function<void()> *>(argument);
    try
    {
        (*task)();
    }
    catch (...)
    {
        std::terminate();
    }
    Memory::destroyCHeap(task);
}
}
/**
 * @brief 创建 C 线程，启动失败时由调用方继续回收调用体。
 * @param task 一次性线程函数。
 */
HostThread::HostThread(Memory::Function<void()> task)
{
    auto payload = Memory::makeCHeapUnique<Memory::Function<void()>>(std::move(task));
    if (chali_thread_create(executeThread, payload.get(), &thread_) != 0)
        throw std::runtime_error("C thread creation failed");
    payload.release();
}
/**
 * @brief 接管唯一线程句柄。
 * @param other 可移动源。
 */
HostThread::HostThread(HostThread &&other) noexcept : thread_(std::exchange(other.thread_, nullptr)) {}
/**
 * @brief 接管源句柄，接收者仍可等待时终止进程以防遗失线程。
 * @param other 可移动源。
 * @return 当前对象。
 */
HostThread &HostThread::operator=(HostThread &&other) noexcept
{
    if (this != &other)
    {
        if (thread_) std::terminate();
        thread_ = std::exchange(other.thread_, nullptr);
    }
    return *this;
}
/** @brief 检查线程已经完成等待，保持与既有线程句柄相同的销毁契约。 */
HostThread::~HostThread()
{
    if (thread_) std::terminate();
}
/** @brief 等待并清空线程句柄，等待失败时保留状态并抛出诊断。 */
void HostThread::join()
{
    if (chali_thread_join(thread_) != 0) throw std::runtime_error("C thread join failed");
    thread_ = nullptr;
}
}
