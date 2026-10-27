/**
 * @file machine_control.cpp
 * @brief 原生宿主与独立 VM 共用的停止、事件及机器配置。
 */
#include "runtime_internal.hpp"
#include "thread_runtime.hpp"

namespace stackvm
{
using namespace runtimeDetail;
/**
 * @brief 建立虚拟机独立的协作停止标志。
 * @param trace 是否记录逐指令执行轨迹。
 */
Machine::Machine(bool trace) : control_(std::make_shared<ExecutionControl>()), trace_(trace) {}
/**
 * @brief 设置协作停止请求，执行与等待安全点将观察此原子标志。
 */
void Machine::requestStop() { control_->requested.store(true, std::memory_order_release); }
#if STACKVM_WITH_THREADS
/**
 * @brief 返回当前运行的弱引用投递器，不捕获 Machine 裸指针。
 * @return 运行结束后稳定返回关闭状态的投递器。
 */
EventPoster Machine::eventPoster()
{
    const std::weak_ptr<EventQueue> target = events_;
    return [target](RuntimeEvent event) { return postEvent(target, event); };
}

#endif

/**
 * @brief 设置断点集合（指令下标）。
 * @param instructions 断点位置；重复项会被忽略，空表等于清空。
 */
void Machine::setBreakpoints(Memory::Vector<std::size_t> instructions)
{
    breakpoints_.clear();
    for (const std::size_t instruction : instructions) breakpoints_.insert(instruction);
}

}  // namespace stackvm
