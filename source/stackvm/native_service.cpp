/**
 * @file native_service.cpp
 * @brief 原生后端复用共享的并发服务，不解释语言指令。
 */
#include "concurrent_call_state.hpp"

namespace stackvm
{
using namespace runtimeDetail;
/**
 * @brief 将创建线程时捕获的参数还原到本线程堆，沿用 VM 传值规则。
 * @param context 本线程隔离的堆、对象表和诊断上下文。
 * @return 初始参数值；无参数或还原失败时返回空值。
 */
std::optional<NativeValue> Machine::nativeThreadArgument(NativeContext &context)
{
#if STACKVM_WITH_THREADS
    if (threadRuntime_)
    {
        const auto packet = threadRuntime_->workers.at(threadId_)->initial;
        if (packet)
        {
            Slot value;
            if (!restoreTransfer(context, *packet, &value)) return std::nullopt;
            return value.isFloat ? NativeValue::fromFloat(value.real) : NativeValue::fromInt(value.integer);
        }
    }
#else
    (void)context;
#endif
    return std::nullopt;
}

/**
 * @brief 检查原生执行的协作取消及并发失败状态。
 * @param context 接收诊断的运行时上下文。
 */
void Machine::checkNativeExecution(NativeContext &context)
{
    if (control_->requested.load(std::memory_order_acquire) && context.error.empty()) context.error = "执行已取消";
#if STACKVM_WITH_THREADS
    if (threadRuntime_ && threadRuntime_->panicked.load(std::memory_order_acquire) && context.error.empty())
        context.error = threadRuntime_->panicError;
#endif
}

/**
 * @brief 将原生槽实参交给与 VM 一致的并发服务并返回单槽结果。
 * @param program 函数标签和参数元数据。
 * @param context 当前线程的共享堆、对象表和诊断。
 * @param name 目标服务全名。
 * @param arguments 实参数组。
 * @param callbackDepth 同步回调深度。
 * @return 已识别服务的结果，未识别服务返回空值。
 */
std::optional<NativeValue> Machine::invokeNativeService(const Program &program, NativeContext &context,
                                                        const Memory::String &name, const Memory::Vector<NativeValue> &arguments, std::size_t callbackDepth)
{
    data_ = const_cast<DataArea *>(context.data);
    context.machine = this;
    context.threadId = threadId_;
#if STACKVM_WITH_THREADS
    if (!events_) events_ = std::make_shared<EventQueue>();
    if (!sharedRuntime_) sharedRuntime_ = Memory::makeShared<SharedRuntime>();
    if (threadsEnabled_ && !threadRuntime_)
    {
        threadRuntime_ = Memory::makeShared<ThreadRuntime>();
        threadRuntime_->workers.emplace(0u, Memory::makeShared<ThreadRuntime::Worker>());
        threadRuntime_->workers.at(0u)->control = control_;
    }
#endif
    Memory::Vector<PendingArg> pending;
    for (const auto &value : arguments)
        pending.push_back({ {}, value.isFloat() ? Slot::fromDouble(value.floating) : Slot::fromInt(value.integer) });
    RunResult result;
    Slot returned;
    Memory::Function<void()> check = [&] {
        checkNativeExecution(context);
        if (!context.error.empty()) result.error = context.error;
    };
    const auto fail = [&](const Memory::String &error) { if (result.error.empty()) result.error = error; };
    const auto emit = [&](const char *bytes, std::size_t size) {
        if (context.textSink) context.textSink(context.textSinkUser, bytes, size);
        if (context.text && (!context.textKeep || *context.textKeep)) context.text->append(bytes, size);
    };
#if STACKVM_WITH_THREADS
    const auto wait = [&](std::unique_lock<std::mutex> &guard, const Memory::Function<bool()> &ready,
                          std::chrono::steady_clock::time_point deadline) {
        while (!ready() && std::chrono::steady_clock::now() < deadline)
        {
            guard.unlock();
            check();
            guard.lock();
            if (!result.error.empty()) break;
            threadRuntime_->changed.wait_until(guard, std::min(deadline, std::chrono::steady_clock::now() + std::chrono::milliseconds(1)));
        }
    };
#endif
    ConcurrentCallState state{ program, pending, *context.heap, *context.objects, result, callbackDepth, 0,
                               [&](const Memory::String &, const Slot &value) { returned = value; }, fail, emit, check
#if STACKVM_WITH_THREADS
                               ,
                               nativeReceivedEvent_, [&](const Memory::String &error, const RunResult &) { fail(error); }, wait
#endif
    };
    std::shared_ptr<TransferPacket> packet;
    const bool create = name == "__std_thread_create";
    auto handled = callTransferService(state, name, "__native_result", packet, create);
    if (!handled) handled = callMailboxService(state, name, "__native_result", packet, create);
    if (!handled) handled = callThreadService(state, name, "__native_result", packet, create);
    if (!handled) return std::nullopt;
    if (!result.error.empty()) context.error = result.error;
    return returned.isFloat ? NativeValue::fromFloat(returned.real) : NativeValue::fromInt(returned.integer);
}

/**
 * @brief 在原生模块生命周期结束时停止并回收子线程与事件队列。
 * @param context 输出与错误接收上下文。
 */
void Machine::finishNativeServices(NativeContext &context)
{
#if STACKVM_WITH_THREADS
    if (threadId_ == 0 && threadRuntime_)
    {
        {
            std::lock_guard<std::mutex> guard(threadRuntime_->mutex);
            threadRuntime_->closing = true;
            for (const auto &entry : threadRuntime_->workers)
                if (entry.first != 0 && !entry.second->done) entry.second->control->requested.store(true, std::memory_order_release);
        }
        threadRuntime_->changed.notify_all();
        for (const auto &entry : threadRuntime_->workers)
        {
            if (entry.first == 0) continue;
            auto &worker = *entry.second;
            if (worker.host.joinable()) worker.host.join();
            if (!worker.reported)
            {
                if (context.textSink) context.textSink(context.textSinkUser, worker.result.text.data(), worker.result.text.size());
                if (!worker.result.ok && !worker.result.cancelled && context.error.empty()) context.error = worker.result.error;
            }
        }
        threadRuntime_.reset();
    }
    closeEventQueue(events_);
    events_.reset();
#else
    (void)context;
#endif
}
}  // namespace stackvm
