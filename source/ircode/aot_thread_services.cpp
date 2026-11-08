/**
 * @file aot_thread_services.cpp
 * @brief 独立的跨平台 BUILD 线程服务，复用标准 C++ 线程、消息与事件实现。
 */
#include "aot_thread_services.hpp"
#include "../allocator/c_heap_allocator.hpp"

extern "C" {
void chali_reset_address_view();
std::int64_t chali_invoke(std::int64_t, std::int64_t, const std::int64_t *);
}
namespace runtime::build
{
using namespace runtimeDetail;
namespace
{
/**
 * @brief 让对象析构及其同步回调始终使用所属隔离线程的上下文。
 */
struct ContextScope
{
    NativeContext *previous = nativeThreadState.activeContext;
    execution_detail::DataCapacityIndex *previousCapacity = nativeThreadState.activeDataCapacity;
    /**
     * @brief 进入子线程上下文并借用预建的数据容量索引。
     * @param context 本线程上下文，生命周期必须覆盖本作用域。
     * @param capacity 本线程数据快照的容量索引。
     */
    ContextScope(NativeContext &context, execution_detail::DataCapacityIndex &capacity)
    {
        chali_reset_address_view();
        nativeThreadState.activeContext = &context;
        nativeThreadState.activeDataCapacity = &capacity;
    }
    /**
     * @brief 所属对象全部销毁后恢复外层上下文。
     */
    ~ContextScope()
    {
        chali_reset_address_view();
        nativeThreadState.activeContext = previous;
        nativeThreadState.activeDataCapacity = previousCapacity;
    }
};

/**
 * @brief 在隔离数据、堆与对象表中调用原生任务，不建立语言解释循环。
 * @param rootContext 根模块的服务上下文。
 * @param program 函数标签与元数据。
 * @param arena 模块分配所有者。
 * @param machine 子线程的共享服务宿主。
 * @param data 父线程数据快照。
 * @param label 无捕获任务标签。
 * @return 任务完成状态和诊断。
 */
RunResult runChild(const NativeContext &rootContext, const Program &program, Memory::RuntimeAllocator *arena,
                   Machine &machine, DataArea &data, const Memory::String &label)
{
    Memory::AllocationScope allocation(arena);
    NativeContext context = rootContext;
    execution_detail::DataCapacityIndex capacityIndex(&data);
    ContextScope scope(context, capacityIndex);
    Heap heap;
    heap.reset(data.size());
    Memory::Vector<Memory::String> arguments;
    NativeServices services = *rootContext.hostServices;
    ObjectStore objects;
    bool exiting = false;
    int exitCode = 0;
    context.heap = &heap;
    context.data = &data;
    context.objects = &objects;
    services["native.rev.v1"] = &objects.nativeRecordRevision;
    context.hostServices = &services;
    context.machine = &machine;
    context.programArguments = &arguments;
    context.exitRequested = &exiting;
    context.exitCode = &exitCode;
    context.error.clear();
    context.memoryFault.clear();
    machine.setHeapAllocator(&heap);
    machine.setThreadHeapFactory([] { return Memory::makeUnique<Heap>(); });
    machine.invokeNativeService(program, context, "std::thread::current", {}, 0);
    objects.recordContext = context;
    RunResult result;
    nativeThreadState.logicalSteps = 0;
    nativeThreadState.nativeCleanup = false;
    try
    {
        const auto address = data.binary.empty() ? Memory::String::npos :
                                                   Memory::String(reinterpret_cast<const char *>(data.binary.data()), data.binary.size()).find(label + Memory::String(1, '\0'));
        if (address == Memory::String::npos)
            context.error = "原生任务入口没有函数地址";
        else
        {
            const auto initial = machine.nativeThreadArgument(context);
            std::int64_t bits = initial ? initial->integer : 0;
            if (initial && initial->isFloat()) std::memcpy(&bits, &initial->floating, sizeof(bits));
            if (context.error.empty()) chali_invoke(static_cast<std::int64_t>(address), initial ? 1 : 0, initial ? &bits : nullptr);
        }
    } catch (const NativeExit &exit)
    {
        result.exitCode = exit.code;
        exiting = false;
    } catch (const NativeCancelled &)
    {
        result.cancelled = true;
    } catch (const std::exception &error)
    {
        context.error = error.what();
    }
    result.error = std::move(context.error);
    context.error.clear();
    nativeThreadState.nativeCleanup = true;
    machine.finishNativeServices(context);
    for (const auto handle : objects.materializedObjects) objects.release(handle);
    for (const auto address : objects.materializedHeap)
    {
        std::size_t capacity = 0;
        if (heap.capacityAt(address, capacity)) heap.release(address);
    }
    if (result.error.empty()) result.error = std::move(context.error);
    context.error.clear();
    result.ok = result.error.empty() && !result.recoverable && !result.cancelled;
    return result;
}

/**
 * @brief 创建拥有独立并发控制状态的服务宿主。
 * @param contextPointer 根模块上下文。
 * @param programPointer 程序函数元数据，生命周期覆盖服务。
 * @param allocator 模块分配所有者。
 * @param enabled 是否允许创建线程。
 * @return 服务宿主，由 destroyMachine 回收。
 */
void *createMachine(const void *contextPointer, const void *programPointer, void *allocator, int enabled)
{
    const auto &context = *static_cast<const NativeContext *>(contextPointer);
    const auto &program = *static_cast<const Program *>(programPointer);
    auto *arena = static_cast<Memory::RuntimeAllocator *>(allocator);
    auto machine = Memory::makeCHeapUnique<Machine>();
    machine->setData(const_cast<DataArea *>(context.data));
    machine->setThreadsEnabled(enabled);
    machine->setNativeThreadEntry([context, &program, arena](Machine &child, DataArea &data, const Memory::String &label) {
        return runChild(context, program, arena, child, data, label);
    });
    return machine.release();
}
/**
 * @brief 回收已经结束所有线程的服务宿主。
 * @param machine 先前创建的服务。
 */
void destroyMachine(void *machine) { Memory::destroyCHeap(static_cast<Machine *>(machine)); }
/**
 * @brief 等待线程退出并关闭事件，保留对象析构所需的宿主状态。
 * @param pointer 当前模块上下文。
 */
void finishMachine(void *pointer)
{
    auto &context = *static_cast<NativeContext *>(pointer);
    context.machine->finishNativeServices(context);
}
/**
 * @brief 在安全点观察取消与其他线程的失败。
 * @param pointer 当前线程上下文。
 */
void pollMachine(void *pointer)
{
    auto &context = *static_cast<NativeContext *>(pointer);
    context.machine->checkNativeExecution(context);
}
/**
 * @brief 将 C 槽数组转换为现有对象传值协议，并同步交付结果。
 * @param programPointer 不透明程序元数据。
 * @param contextPointer 不透明当前线程上下文。
 * @param name 服务名称字节。
 * @param nameSize 名称长度。
 * @param count 参数个数。
 * @param bits 参数位模式。
 * @param kinds 零为整数，一为浮点。
 * @param callbackDepth 同步回调深度。
 * @param receive 接收结果的同步回调。
 * @param user 回调状态。
 * @return 服务已识别时返回一，否则返回零。
 */
int invokeMachine(const void *programPointer, void *contextPointer, const char *name, std::size_t nameSize,
                  std::size_t count, const std::int64_t *bits, const unsigned char *kinds, std::size_t callbackDepth,
                  ChaliServiceResult receive, void *user)
{
    const auto &program = *static_cast<const Program *>(programPointer);
    auto &context = *static_cast<NativeContext *>(contextPointer);
    Memory::Vector<NativeValue> arguments;
    arguments.reserve(count);
    for (std::size_t index = 0; index < count; ++index)
    {
        double real = 0;
        std::memcpy(&real, bits + index, sizeof(real));
        arguments.push_back(kinds[index] ? NativeValue::fromFloat(real) : NativeValue::fromInt(bits[index]));
    }
    const auto value = context.machine->invokeNativeService(program, context, Memory::String(name, nameSize), arguments, callbackDepth);
    if (!value) return 0;
    std::int64_t raw = value->integer;
    if (value->isFloat()) std::memcpy(&raw, &value->floating, sizeof(raw));
    receive(user, value->isText() ? 2 : value->isFloat() ? 1 : 0, raw, value->text.data(), value->text.size());
    return 1;
}
}
const ChaliThreadServices threadServices{createMachine, destroyMachine, finishMachine, pollMachine, invokeMachine};
}
/**
 * @brief 返回跨平台线程服务，生成模块仅在需要时引用此入口。
 * @return 进程内只读服务表。
 */
extern "C" const ChaliThreadServices *chali_build_thread_services()
{
    return &runtime::build::threadServices;
}
/**
 * @brief 为尚未生成服务选择函数的旧对象提供完整线程服务。
 * @return 完整服务表；新模块以强符号覆盖此兼容选择。
 */
extern "C" __attribute__((weak)) CHALI_BUILD_SERVICE_LOCAL const ChaliThreadServices *chali_program_thread_services()
{
    return chali_build_thread_services();
}
