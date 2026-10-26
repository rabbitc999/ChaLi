/**
 * @file native_runtime.cpp
 * @brief LLVM 原生程序复用共享堆、对象表和标准库胶水，不解释语言指令。
 */
#include "stackvm/runtime_internal.hpp"
#include "chali_glue.hpp"
#include "glue/std_registry.hpp"
#include "console/utf8.hpp"
#include <memory>
#include <cstring>
#include <limits>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__linux__)
#include <execinfo.h>
#endif

extern "C" {
extern const unsigned char chali_data_image[];
extern const char chali_data_layout[];
extern const std::size_t chali_data_size, chali_heap_size;
extern const std::size_t chali_function_count;
extern const std::int64_t chali_function_addresses[];
extern const int chali_threads_enabled;
void chali_panic(const char *message);
std::int64_t chali_invoke(std::int64_t function, std::int64_t count, const std::int64_t *arguments);
std::int64_t chali_function_returns_real(std::int64_t function);
unsigned char chali_result_float();
void chali_reset_slots();
void chali_host_tick(std::uint64_t steps);
bool chali_program_abi_bind(chali_glue::Registry *registry, Memory::String *error);
void chali_location(const char **file, std::int64_t *line, const void **function);
extern const std::size_t chali_trace_count;
extern const void *const chali_trace_functions[];
extern const char *const chali_trace_names[];
#if defined(__linux__)
extern const char __start_chali_code[], __stop_chali_code[];
#endif
}

namespace
{
using namespace stackvm;
using namespace stackvm::runtimeDetail;
thread_local NativeContext *activeContext = nullptr;
thread_local execution_detail::DataCapacityIndex *activeDataCapacity = nullptr;
thread_local std::size_t callbackDepth = 0;
thread_local unsigned char nativeResultFloat = 0;
thread_local std::uint64_t logicalSteps = 0;
thread_local bool nativeCleanup = false;

/**
 * @brief 在隔离宿主线程内建立上下文并调用已经生成的机器码。
 * @param machine 共享线程服务配置的子线程宿主。
 * @param data 从父线程复制的数据映像。
 * @param label 已验证的无捕获任务入口。
 * @return 原生任务的完成、取消或错误结果。
 */
RunResult runChild(Machine &machine, DataArea &data, const Memory::String &label);

/**
 * @brief 将共享原生服务的输出直接写入宿主标准输出。
 * @param user 未使用的输出上下文。
 * @param bytes 本次输出的字节。
 * @param count 输出长度。
 */
void writeOutput(void *user, const char *bytes, std::size_t count)
{
    (void)user;
    std::fwrite(bytes, 1, count, stdout);
}

/**
 * @brief 单个原生模块拥有的共享服务状态，析构顺序保证对象先于堆释放。
 */
struct Runtime
{
    Memory::DynamicRuntimeArena arena;
    Memory::RuntimeSession session;
    DataArea data;
    Memory::UniquePtr<execution_detail::DataCapacityIndex> dataCapacity;
    Heap heap;
    NativeInvoker invoker;
    chali_glue::Registry registry;
    Machine machine;
    Program program;
    Memory::Vector<Memory::String> arguments;
    NativeContext context;
    WatchdogRuntime watchdog;
    bool keepText = false;
    bool exiting = false;
    int exitCode = 0;
    ObjectStore objects;

    /**
     * @brief 装入静态数据并注册 VM 同源的原生函数与同步回调。
     */
    Runtime() : arena(chali_heap_size), session(arena)
    {
        if (!data.loadFromBytes(chali_data_image, chali_data_size, chali_data_layout)) chali_panic("静态数据布局无效");
        dataCapacity = Memory::makeUnique<execution_detail::DataCapacityIndex>(&data);
        heap.reset(chali_data_size);
        context.heap = &heap;
        context.data = &data;
        context.objects = &objects;
        context.textSink = writeOutput;
        context.textKeep = &keepText;
        context.programArguments = &arguments;
        context.watchdog = &watchdog;
        context.exitRequested = &exiting;
        context.exitCode = &exitCode;
        context.invoker = &invoker;
        context.machine = &machine;
        machine.setData(&data);
        machine.setThreadsEnabled(chali_threads_enabled != 0);
        machine.setNativeThreadEntry(runChild);
        for (std::size_t at = 0; at < chali_function_count; ++at)
            program.labels.emplace(data.readCString(static_cast<std::size_t>(chali_function_addresses[at])), at);
        invoker = [](std::int64_t function, const Memory::Vector<NativeValue> &values, NativeValue &result) {
            Memory::Vector<std::int64_t> bits;
            bits.reserve(values.size());
            for (const auto &value : values)
            {
                std::int64_t raw = value.integer;
                if (value.isFloat()) std::memcpy(&raw, &value.floating, sizeof(raw));
                bits.push_back(raw);
            }
            struct DepthGuard
            {
                /**
                 * @brief 标记同步胶水回调，保持 VM 的线程与消息操作限制。
                 */
                DepthGuard() { ++callbackDepth; }
                /**
                 * @brief 回调返回或异常时恢复嵌套深度。
                 */
                ~DepthGuard() { --callbackDepth; }
            } guard;
            const auto raw = chali_invoke(function, static_cast<std::int64_t>(bits.size()), bits.data());
            if (chali_result_float())
            {
                double real = 0;
                std::memcpy(&real, &raw, sizeof(real));
                result = NativeValue::fromFloat(real);
            }
            else
                result = NativeValue::fromInt(raw);
            return true;
        };
        objects.recordContext = context;
        Memory::String error;
        if (!std_registry::bindStandardGlue(registry, &error)) chali_panic(error.c_str());
        if (!chali_program_abi_bind(&registry, &error)) chali_panic(error.c_str());
    }

    /**
     * @brief 清理胶水登记的临时值，其余资源由对象表和堆按顺序收尾。
     */
    ~Runtime()
    {
        machine.finishNativeServices(context);
        if (!context.error.empty()) chali_panic(context.error.c_str());
        for (const auto handle : objects.materializedObjects) objects.release(handle);
        for (const auto address : objects.materializedHeap)
        {
            std::size_t capacity = 0;
            if (heap.capacityAt(address, capacity)) heap.release(address);
        }
    }
};

Runtime *runtime = nullptr;

/**
 * @brief 确认模块生命周期已开始并取得当前服务状态。
 * @return 当前模块的原生服务状态。
 */
Runtime &current()
{
    if (!runtime) chali_panic("必须先初始化原生模块");
    return *runtime;
}

/**
 * @brief 取得当前隔离线程或根模块的原生上下文。
 * @return 生命周期覆盖本次同步调用的上下文。
 */
NativeContext &currentContext()
{
    return activeContext ? *activeContext : current().context;
}

/**
 * @brief 子线程安全点使用的协作取消信号。
 */
struct NativeCancelled
{
};

/**
 * @brief 原生子线程的可恢复终止信号。
 */
struct NativeRestart
{
    int reason;
};

/**
 * @brief 原生子线程主动正常退出时保留的状态码。
 */
struct NativeExit
{
    int code;
};

/**
 * @brief 让对象析构及其同步回调始终使用所属隔离线程的上下文。
 */
struct ContextScope
{
    NativeContext *previous = activeContext;
    execution_detail::DataCapacityIndex *previousCapacity = activeDataCapacity;
    /**
     * @brief 进入子线程上下文并借用预建的数据容量索引。
     * @param context 本线程上下文，生命周期必须覆盖本作用域。
     * @param capacity 本线程数据快照的容量索引。
     */
    ContextScope(NativeContext &context, execution_detail::DataCapacityIndex &capacity)
    {
        activeContext = &context;
        activeDataCapacity = &capacity;
    }
    /**
     * @brief 所属对象全部销毁后恢复外层上下文。
     */
    ~ContextScope()
    {
        activeContext = previous;
        activeDataCapacity = previousCapacity;
    }
};

/**
 * @brief 在隔离数据、堆与对象表中调用原生任务，不建立语言解释循环。
 * @param machine 子线程的共享服务宿主。
 * @param data 父线程数据快照。
 * @param label 无捕获任务标签。
 * @return 任务完成状态和诊断。
 */
RunResult runChild(Machine &machine, DataArea &data, const Memory::String &label)
{
    auto &root = current();
    NativeContext context = root.context;
    WatchdogRuntime watchdog;
    execution_detail::DataCapacityIndex capacityIndex(&data);
    ContextScope scope(context, capacityIndex);
    Heap heap;
    heap.reset(data.size());
    Memory::Vector<Memory::String> arguments;
    ObjectStore objects;
    bool exiting = false;
    int exitCode = 0;
    context.heap = &heap;
    context.data = &data;
    context.objects = &objects;
    context.watchdog = &watchdog;
    context.machine = &machine;
    context.programArguments = &arguments;
    context.exitRequested = &exiting;
    context.exitCode = &exitCode;
    context.error.clear();
    context.memoryFault.clear();
    machine.setHeapAllocator(&heap);
    machine.setThreadHeapFactory([] { return Memory::makeUnique<Heap>(); });
    machine.invokeNativeService(root.program, context, "std::thread::current", {}, 0);
    objects.recordContext = context;
    RunResult result;
    logicalSteps = 0;
    nativeCleanup = false;
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
    } catch (const NativeRestart &restart)
    {
        result.recoverable = true;
        result.recoverableReason = restart.reason;
        watchdog.period = 0;
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
    nativeCleanup = true;
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
}  // namespace

extern "C" {
/**
 * @brief 为原生模块建立独立的共享运行时服务。
 */
void chali_host_init()
{
    if (runtime) chali_panic("原生模块已经初始化");
    logicalSteps = 0;
    nativeCleanup = false;
    nativeResultFloat = 0;
    chali_reset_slots();
    runtime = new Runtime;
}

/**
 * @brief 输出错误位置与实际原生调用链，不执行虚拟机指令或分配语言堆内存。
 * @note 内联和尾调用之后的调用链按实际机器栈展示，当前位置保留四元式源码身份。
 */
void chali_host_trace()
{
    const char *file = nullptr;
    std::int64_t line = 0;
    const void *function = nullptr;
    chali_location(&file, &line, &function);
    const char *name = nullptr;
    for (std::size_t at = 0; at < chali_trace_count; ++at)
        if (chali_trace_functions[at] == function) name = chali_trace_names[at];
    if (file && *file && line > 0)
        std::fprintf(stderr, "出错位置：%s:%lld%s%s%s\n", file, static_cast<long long>(line), name ? "（" : "", name ? name : "", name ? "）" : "");
    void *frames[96];
    std::size_t count = 0;
#ifdef _WIN32
    const auto module = GetModuleHandleW(L"ntdll.dll");
    using Capture = USHORT(WINAPI *)(ULONG, ULONG, PVOID *, PULONG);
    using Lookup = PRUNTIME_FUNCTION(WINAPI *)(DWORD64, PDWORD64, PUNWIND_HISTORY_TABLE);
    const auto capture = module ? reinterpret_cast<Capture>(GetProcAddress(module, "RtlCaptureStackBackTrace")) : nullptr;
    const auto lookup = module ? reinterpret_cast<Lookup>(GetProcAddress(module, "RtlLookupFunctionEntry")) : nullptr;
    if (capture && lookup) count = capture(0, 96, frames, nullptr);
#elif defined(__linux__)
    count = static_cast<std::size_t>(backtrace(frames, 96));
#endif
    bool printed = false;
    if (name && *name)
    {
        std::fprintf(stderr, "调用栈（内 → 外）：\n  %s\n", name);
        printed = true;
    }
    for (std::size_t frame = 0; frame < count; ++frame)
    {
        const auto pc = reinterpret_cast<std::uintptr_t>(frames[frame]) - 1;
        const void *start = nullptr;
#ifdef _WIN32
        DWORD64 base = 0;
        if (const auto *entry = lookup(static_cast<DWORD64>(pc), &base, nullptr))
            start = reinterpret_cast<const void *>(base + entry->BeginAddress);
#elif defined(__linux__)
        if (pc < reinterpret_cast<std::uintptr_t>(__start_chali_code) || pc >= reinterpret_cast<std::uintptr_t>(__stop_chali_code)) continue;
        std::uintptr_t nearest = 0;
        for (std::size_t at = 0; at < chali_trace_count; ++at)
        {
            const auto candidate = reinterpret_cast<std::uintptr_t>(chali_trace_functions[at]);
            if (candidate <= pc && candidate > nearest) nearest = candidate;
        }
        start = reinterpret_cast<const void *>(nearest);
#endif
        for (std::size_t at = 0; start && at < chali_trace_count; ++at)
        {
            if (chali_trace_functions[at] != start || !*chali_trace_names[at]) continue;
            if (name && std::strcmp(name, chali_trace_names[at]) == 0) continue;
            if (!printed) std::fputs("调用栈（内 → 外）：\n", stderr);
            std::fprintf(stderr, "  %s\n", chali_trace_names[at]);
            printed = true;
            break;
        }
    }
}

/**
 * @brief 销毁本模块的原生服务并允许之后重新初始化。
 */
void chali_host_finish()
{
    delete runtime;
    runtime = nullptr;
}

/**
 * @brief 原生循环安全点观察共享线程服务的取消与失败状态。
 */
void chali_host_poll()
{
    if (nativeCleanup) return;
    auto &context = currentContext();
    context.machine->checkNativeExecution(context);
    if (!context.error.empty())
    {
        if (context.threadId != 0) throw NativeCancelled{};
        chali_panic(context.error.c_str());
    }
}

/**
 * @brief 在使用看门狗的程序中按当前函数的逻辑指令预算推进安全点。
 * @param steps 当前四元式对应的逻辑指令数；零只观察显式重启请求。
 * @note 未使用看门狗的程序不生成逐四元式调用。
 */
void chali_host_tick(std::uint64_t steps)
{
    if (nativeCleanup) return;
    auto &context = currentContext();
    auto *watchdog = context.watchdog;
    if (watchdog && watchdog->period)
    {
        watchdog->now = logicalSteps;
        if (!watchdog->deadline)
            watchdog->deadline = watchdog->period > UINT64_MAX - logicalSteps ? UINT64_MAX : logicalSteps + watchdog->period;
        if (logicalSteps >= watchdog->deadline) Memory::restartSignal().requestRestart();
    }
    if (Memory::restartSignal().pending())
    {
        const auto reason = Memory::restartSignal().reason();
        if (context.threadId != 0) throw NativeRestart{ static_cast<int>(reason) };
        chali_panic(reason == Memory::WatchdogReason::ExplicitRequest ? "软重启：程序显式请求重启（可恢复终止）" : "软重启：ChaLi 运行内存池用尽（可恢复终止）");
    }
    logicalSteps = steps > UINT64_MAX - logicalSteps ? UINT64_MAX : logicalSteps + steps;
}

/**
 * @brief 将原生入口收到的用户参数交给标准库，排除可执行文件名。
 * @param count 用户实参数量。
 * @param values UTF-8 参数字符串数组。
 */
void chali_host_arguments(int count, const char *const *values)
{
    auto &arguments = current().arguments;
    arguments.clear();
    for (int at = 0; at < count; ++at) arguments.emplace_back(values[at]);
}

/**
 * @brief 从进程入口取得 UTF-8 参数，Windows 使用 CRT 宽字符参数保留中文与非 BMP 字符。
 * @param argc 入口收到的参数数量，包含可执行文件名。
 * @param argv 入口收到的原始参数数组。
 */
void chali_host_process_arguments(int argc, char **argv)
{
    const auto converted = tool_console::arguments(argc, argv);
    auto &arguments = current().arguments;
    arguments.clear();
    for (std::size_t at = 1; at < converted.size(); ++at)
        arguments.emplace_back(converted[at].data(), converted[at].size());
}

/**
 * @brief 使用共享堆和静态符号布局检查完整数组元素是否可访问。
 * @param index 元素下标。
 * @param address 数组逻辑基址。
 * @param stride 元素步长；非正值沿用 VM 的八字节默认步长。
 */
void chali_host_check_pointer(std::int64_t index, std::int64_t address, std::int64_t stride)
{
    auto &context = currentContext();
    if (index < 0) chali_panic("数组下标越界：下标不能为负");
    if (stride <= 0) stride = 8;
    std::size_t capacity = 0;
    const auto at = static_cast<std::size_t>(address);
    bool found = address >= 0 && context.heap->capacityAt(at, capacity);
    if (!found && address >= 0)
        found = (activeDataCapacity ? activeDataCapacity : current().dataCapacity.get())->capacityAt(at, capacity);
    if (!found) chali_panic("数组下标检查失败：地址不在任何一块已分配内存里");
    if (capacity < static_cast<std::size_t>(stride) || static_cast<std::uint64_t>(index) >= capacity / static_cast<std::size_t>(stride))
        chali_panic("数组下标越界");
}

/**
 * @brief 验证数据区或活跃堆区间，借用一次同步操作所需的物理存储。
 * @param address 语言逻辑地址。
 * @param count 必须可访问的字节数。
 * @return 当前操作结束前有效的物理地址。
 */
unsigned char *chali_host_address(std::int64_t address, std::size_t count)
{
    auto &context = currentContext();
    const auto at = static_cast<std::size_t>(address);
    if (context.data && address >= 0 && at < context.data->binary.size() && count <= context.data->binary.size() - at)
        return const_cast<unsigned char *>(context.data->binary.data()) + at;
    if (address >= 0)
        if (auto *heap = dynamic_cast<Heap *>(context.heap))
            if (auto *bytes = heap->borrowBytes(at, count)) return bytes;
    chali_panic("访问未分配或已释放的内存");
    return nullptr;
}

/**
 * @brief 按原聚合助手的前向八字节槽语义复制，完整活跃且不重叠时批量搬运。
 * @param destination 目标逻辑地址。
 * @param source 源逻辑地址。
 * @param slots 八字节槽数；非正值不访问存储。
 * @return 与原内部助手一致，返回零。
 * @note 范围证明失败或存在重叠时逐槽先读后写，保留原陷阱位置及部分写入顺序。
 */
std::int64_t chali_host_copy_words(std::int64_t destination, std::int64_t source, std::int64_t slots)
{
    if (slots <= 0) return 0;
    auto &context = currentContext();
    const auto borrow = [&](std::int64_t address, std::size_t bytes) -> unsigned char * {
        if (address < 0) return nullptr;
        const auto at = static_cast<std::size_t>(address);
        if (context.data && at < context.data->binary.size() && bytes <= context.data->binary.size() - at)
            return const_cast<unsigned char *>(context.data->binary.data()) + at;
        if (auto *heap = dynamic_cast<Heap *>(context.heap)) return heap->borrowBytes(at, bytes);
        return nullptr;
    };
    if (static_cast<std::uint64_t>(slots) <= std::numeric_limits<std::size_t>::max() / 8u)
    {
        const auto bytes = static_cast<std::size_t>(slots) * 8u;
        const auto from = static_cast<std::uint64_t>(source), to = static_cast<std::uint64_t>(destination);
        const bool disjoint = from <= to ? to - from >= bytes : from - to >= bytes;
        if (disjoint || source == destination)
        {
            auto *input = borrow(source, bytes);
            auto *output = borrow(destination, bytes);
            if (input && output)
            {
                if (input != output) std::memcpy(output, input, bytes);
                return 0;
            }
        }
    }
    for (std::int64_t index = 0; index < slots; ++index)
    {
        std::uint64_t value = 0;
        const auto offset = static_cast<std::uint64_t>(index) * 8u;
        std::memcpy(&value, chali_host_address(static_cast<std::int64_t>(static_cast<std::uint64_t>(source) + offset), 8), 8);
        std::memcpy(chali_host_address(static_cast<std::int64_t>(static_cast<std::uint64_t>(destination) + offset), 8), &value, 8);
    }
    return 0;
}

/**
 * @brief 使用与 VM 相同的逻辑分配身份创建清零内存。
 * @param bytes 需要分配的字节数。
 * @return 新的活跃逻辑地址。
 */
std::int64_t chali_host_allocate(std::size_t bytes)
{
    auto &context = currentContext();
    const auto address = nativeAllocate(context, bytes);
    if (!address) chali_panic("堆内存不足");
    return static_cast<std::int64_t>(address);
}

/**
 * @brief 严格释放拥有型对象或活跃堆块，静态数据与空值不需要释放。
 * @param address 对象句柄、块首地址或静态数据地址。
 */
void chali_host_drop(std::int64_t address)
{
    auto &context = currentContext();
    if (!address) return;
    if (isObjectHandle(address))
    {
        if (!context.objects->release(address)) chali_panic("对象句柄已经释放");
        return;
    }
    if (context.data && address > 0 && static_cast<std::size_t>(address) < context.data->binary.size()) return;
    Memory::String error;
    if (!context.heap->release(static_cast<std::size_t>(address), &error)) chali_panic(error.c_str());
}

/**
 * @brief 按 VM 闭包协议解析函数标签或隐藏环境。
 * @param function 函数值或闭包句柄。
 * @param environment 非零时返回环境，零时返回标签地址。
 * @return 解析后的环境句柄或函数标签地址。
 */
std::int64_t chali_host_function(std::int64_t function, int environment)
{
    std::int64_t captured = 0;
    const auto target = closureTarget(*currentContext().objects, function, captured);
    if (!target) chali_panic("调用了空函数指针或无效闭包");
    return environment ? captured : target;
}

/**
 * @brief 通过统一槽协议调用共享原生服务，保留参数检查和原始诊断。
 * @param name 原生函数的完整名称。
 * @param count 实参数量。
 * @param bits 实参的原始位模式。
 * @param floating 每个实参是否为浮点数。
 * @return 原生返回值的原始位模式。
 */
std::int64_t chali_host_call(const char *name, std::int64_t count, const std::int64_t *bits, const unsigned char *floating)
{
    auto &state = current();
    NativeContext context = currentContext();
    NativeValue result;
    try
    {
        const auto *internal = kSlotNatives.find(name);
        if (internal)
        {
            if (internal->second.arity >= 0 && count != internal->second.arity) chali_panic("原生函数实参数量不匹配");
            Memory::Vector<Slot> arguments;
            if (count > 0) arguments.reserve(static_cast<std::size_t>(count));
            for (std::int64_t at = 0; at < count; ++at)
            {
                double real = 0;
                std::memcpy(&real, bits + at, sizeof(real));
                arguments.push_back(floating[at] ? Slot::fromDouble(real) : Slot::fromInt(bits[at]));
            }
            const auto value = internal->second.apply(context, arguments);
            result = value.isFloat ? NativeValue::fromFloat(value.real) : value.isText ? NativeValue::fromText(value.text) :
                                                                                         NativeValue::fromInt(value.integer);
        }
        else
        {
            const auto *entry = state.registry.native().find(name);
            if (entry && entry->arity != static_cast<std::size_t>(-1) && static_cast<std::size_t>(count) != entry->arity) chali_panic("宿主函数实参数量不匹配");
            Memory::Vector<NativeValue> arguments;
            if (count > 0) arguments.reserve(static_cast<std::size_t>(count));
            for (std::int64_t at = 0; at < count; ++at)
            {
                double real = 0;
                std::memcpy(&real, bits + at, sizeof(real));
                arguments.push_back(floating[at] ? NativeValue::fromFloat(real) : NativeValue::fromInt(bits[at]));
            }
            if (entry)
            {
                NativeCallContextImpl call(context, state.invoker);
                result = entry->callback(call, arguments);
            }
            else
            {
                const auto handled = context.machine->invokeNativeService(state.program, context, name, arguments, callbackDepth);
                if (!handled) chali_panic((Memory::String("找不到原生服务：") + name).c_str());
                result = *handled;
            }
        }
    } catch (const std::exception &error)
    {
        context.error = Memory::String("宿主函数 ") + name + " 抛出异常：" + error.what();
    }
    if (!context.memoryFault.empty()) context.error = context.memoryFault;
    if (!context.error.empty()) chali_panic(context.error.c_str());
    chali_host_tick(0);
    if (context.exitRequested && *context.exitRequested)
    {
        const auto code = context.exitCode ? *context.exitCode : 0;
        if (context.threadId != 0) throw NativeExit{ code };
        std::exit(code);
    }
    nativeResultFloat = result.isFloat() ? 1 : 0;
    if (result.isText()) return static_cast<std::int64_t>(nativeAllocString(context, result.text));
    if (result.isFloat())
    {
        std::int64_t raw = 0;
        std::memcpy(&raw, &result.floating, sizeof(raw));
        return raw;
    }
    return result.integer;
}

/**
 * @brief 返回最近一次同线程宿主调用的原始槽类型，避免将浮点位模式当成整数。
 * @return 浮点返回一，其余原生结果返回零。
 */
unsigned char chali_host_result_float()
{
    return nativeResultFloat;
}
}  // extern "C"
