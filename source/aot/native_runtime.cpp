/**
 * @file native_runtime.cpp
 * @brief LLVM 原生程序复用共享堆、对象表和标准库胶水，不解释语言指令。
 */
#include "ircode/runtime_internal.hpp"
#include "chali_glue.hpp"
#include "abi/chali_abi_runtime.hpp"
#include "allocator/system_arena.hpp"
#include "glue/std_registry.hpp"
#include "glue/std_collection.hpp"
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
/**
 * @brief 登记当前程序实际可达的标准库模块。
 * @param registry 注册表。
 * @param error 诊断。
 * @return 绑定是否成功。
 */
bool chali_program_std_bind(chali_glue::Registry *registry, Memory::String *error);

extern const unsigned char chali_data_image[];
/**
 * @brief 稀疏初始化片段，三个字段与生成的 LLVM i64 记录保持一致。
 */
struct ChaliDataSegment
{
    std::size_t destination, source, size;
};
extern const ChaliDataSegment chali_data_segments[];
extern const std::size_t chali_data_segment_count;
extern const char chali_data_layout[];
extern const std::size_t chali_data_layout_size;
extern const std::size_t chali_data_size, chali_heap_size;
extern const int chali_allocator_kind;
extern const std::size_t chali_step_limit;
extern const std::size_t chali_function_count;
extern const std::int64_t chali_function_addresses[];
extern const int chali_threads_enabled;
void chali_panic(const char *message);
std::int64_t chali_invoke(std::int64_t function, std::int64_t count, const std::int64_t *arguments);
std::int64_t chali_function_returns_real(std::int64_t function);
unsigned char chali_result_float();
void chali_reset_slots();
void chali_reset_address_view();
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
/**
 * @brief 同一原生线程的短期服务状态，共用一个 TLS 槽以减少重复寻址。
 * @note 各线程仍独立保存上下文、浮点返回标记、回调深度及清理状态。
 */
struct NativeThreadState
{
    NativeContext *activeContext;
    execution_detail::DataCapacityIndex *activeDataCapacity;
    std::size_t callbackDepth;
    std::uint64_t logicalSteps;
    unsigned char nativeResultFloat;
    bool nativeCleanup;
};
thread_local NativeThreadState nativeThreadState{};

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
 * @brief 根据模块配置创建宿主对象和语言字节堆共用的分配后端。
 * @return 系统 malloc 分配器或原有连续内存池，所有者由模块持有。
 */
std::unique_ptr<Memory::RuntimeAllocator> createRuntimeArena()
{
    if (chali_allocator_kind == 1) return std::make_unique<Memory::SystemRuntimeArena>(chali_heap_size);
    return std::make_unique<Memory::DynamicRuntimeArena>(chali_heap_size);
}

/**
 * @brief 单个原生模块拥有的共享服务状态，析构顺序保证对象先于堆释放。
 */
struct Runtime
{
    std::unique_ptr<Memory::RuntimeAllocator> arena;
    Memory::AllocationScope session;
    DataArea data;
    Memory::UniquePtr<execution_detail::DataCapacityIndex> dataCapacity;
    Heap heap;
    NativeInvoker invoker;
    chali_glue::Registry registry;
    Machine machine;
    Program program;
    Memory::Vector<Memory::String> arguments;
    NativeContext context;
    bool keepText = false;
    bool exiting = false;
    int exitCode = 0;
    ObjectStore objects;

    /**
     * @brief 装入静态数据并注册 VM 同源的原生函数与同步回调。
     */
    Runtime() : arena(createRuntimeArena()), session(arena.get())
    {
        const unsigned char zero = 0;
        if (!data.loadFromBytes(&zero, 1u, Memory::String(chali_data_layout, chali_data_layout_size))) chali_panic("静态数据布局无效");
        data.binary.assign(chali_data_size, 0u);
        for (std::size_t index = 0; index < chali_data_segment_count; ++index)
        {
            const auto &segment = chali_data_segments[index];
            std::memcpy(data.binary.data() + segment.destination, chali_data_image + segment.source, segment.size);
        }
        dataCapacity = Memory::makeUnique<execution_detail::DataCapacityIndex>(&data);
        heap.reset(chali_data_size);
        context.heap = &heap;
        context.data = &data;
        context.objects = &objects;
        context.textSink = writeOutput;
        context.textKeep = &keepText;
        context.programArguments = &arguments;
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
                DepthGuard() { ++nativeThreadState.callbackDepth; }
                /**
                 * @brief 回调返回或异常时恢复嵌套深度。
                 */
                ~DepthGuard() { --nativeThreadState.callbackDepth; }
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
        if (!chali_program_std_bind(&registry, &error)) chali_panic(error.c_str());
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
    return nativeThreadState.activeContext ? *nativeThreadState.activeContext : current().context;
}

/**
 * @brief 子线程安全点使用的协作取消信号。
 */
struct NativeCancelled
{
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
 * @param machine 子线程的共享服务宿主。
 * @param data 父线程数据快照。
 * @param label 无捕获任务标签。
 * @return 任务完成状态和诊断。
 */
RunResult runChild(Machine &machine, DataArea &data, const Memory::String &label)
{
    auto &root = current();
    Memory::AllocationScope allocation(root.arena.get());
    NativeContext context = root.context;
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
}  // namespace

extern "C" {
/**
 * @brief 为原生模块建立独立的共享运行时服务。
 */
void chali_host_init()
{
    if (runtime) chali_panic("原生模块已经初始化");
    chali_reset_address_view();
    nativeThreadState.logicalSteps = 0;
    nativeThreadState.nativeCleanup = false;
    nativeThreadState.nativeResultFloat = 0;
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
    chali_reset_address_view();
    delete runtime;
    runtime = nullptr;
    chali_reset_address_view();
}

/**
 * @brief 原生循环安全点观察共享线程服务的取消与失败状态。
 */
void chali_host_poll()
{
    if (nativeThreadState.nativeCleanup) return;
    auto &context = currentContext();
    context.machine->checkNativeExecution(context);
    if (!context.error.empty())
    {
        if (context.threadId != 0) throw NativeCancelled{};
        chali_panic(context.error.c_str());
    }
}

/**
 * @brief 检查显式步数上限和分配失败，缺省路径不查询线程局部计数。
 * @param steps 本次执行推进的逻辑操作数，零仅检查已发生的错误与显式上限。
 */
void chali_host_tick(std::uint64_t steps)
{
    auto &failure = Memory::allocationFailureSignal();
    if (steps == 0 && chali_step_limit == 0 && !failure.pending()) return;
    if (nativeThreadState.nativeCleanup) return;
    if (failure.pending())
    {
        if (currentContext().threadId != 0) throw std::bad_alloc();
        chali_panic("运行时内存分配失败或预算不足");
    }
    if (chali_step_limit && nativeThreadState.logicalSteps >= chali_step_limit)
        chali_panic("执行步数超过上限");
    nativeThreadState.logicalSteps = steps > UINT64_MAX - nativeThreadState.logicalSteps ? UINT64_MAX : nativeThreadState.logicalSteps + steps;
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
        found = (nativeThreadState.activeDataCapacity ? nativeThreadState.activeDataCapacity : current().dataCapacity.get())->capacityAt(at, capacity);
    if (!found) chali_panic("数组下标检查失败：地址不在任何一块已分配内存里");
    if (capacity < static_cast<std::size_t>(stride) || static_cast<std::uint64_t>(index) >= capacity / static_cast<std::size_t>(stride))
        chali_panic("数组下标越界");
}

/**
 * @brief 验证数据区或活跃堆区间，借用一次同步操作所需的物理存储。
 * @param address 语言逻辑地址。
 * @param count 必须可访问的字节数。
 * @return 当前操作结束前有效的物理地址。
 * @note 根上下文和隔离线程上下文的堆均由本执行器建立为 Heap，直接调用具体实现避免逐次 RTTI。
 */
unsigned char *chali_host_address(std::int64_t address, std::size_t count)
{
    auto &context = currentContext();
    const auto at = static_cast<std::size_t>(address);
    if (context.data && address >= 0 && at < context.data->binary.size() && count <= context.data->binary.size() - at)
        return const_cast<unsigned char *>(context.data->binary.data()) + at;
    if (address >= 0 && context.heap)
        if (auto *bytes = static_cast<Heap *>(context.heap)->borrowBytes(at, count)) return bytes;
    chali_panic("访问未分配或已释放的内存");
    return nullptr;
}

/**
 * @brief 为 LLVM 地址原语建立已验证的连续视图，缓存失效时使用现有分配索引。
 * @param address 本次逻辑地址。
 * @param count 必须可访问的完整字节数。
 * @param view 当前线程的非空 LLVM 视图存储，只在本次调用写入，不保留指针。
 * @return 验证成功后的物理地址，失效时按原诊断终止。
 */
unsigned char *chali_host_address_view(std::int64_t address, std::size_t count, Heap::AddressView *view)
{
    static_assert(sizeof(Heap::AddressView) == 40 && offsetof(Heap::AddressView, stamp) == 32,
                  "AOT 地址视图必须与 64 位 LLVM 布局一致");
    static_assert(std::atomic<std::uint64_t>::is_always_lock_free && sizeof(std::atomic<std::uint64_t>) == 8,
                  "LLVM 地址原语要求原生无锁的 64 位映射代次");
    auto &context = currentContext();
    auto *heap = static_cast<Heap *>(context.heap);
    const auto at = static_cast<std::size_t>(address);
    if (context.data && address >= 0 && at < context.data->binary.size() && count <= context.data->binary.size() - at)
    {
        auto *bytes = const_cast<unsigned char *>(context.data->binary.data());
        const auto *revision = heap ? heap->mappingRevision() : nullptr;
        *view = { 0, context.data->binary.size(), bytes, revision,
                  revision ? revision->load(std::memory_order_acquire) : 0 };
        return bytes + at;
    }
    if (address >= 0 && heap)
        if (auto *bytes = heap->borrowView(at, count, *view)) return bytes;
    chali_panic("访问未分配或已释放的内存");
    return nullptr;
}

/**
 * @brief 在现有原生上下文中执行类型化导出封送，保留重入和原始错误传播。
 * @param marshal 由 C++ 编译器生成的同步封送入口，不是语言函数值。
 * @param frame 本次原生调用独立的参数和返回值帧。
 */
void chali_host_export_apply(void (*marshal)(stackvm::NativeCallContext *, void *), void *frame)
{
    auto &context = currentContext();
    if (!marshal || !context.objects || !context.heap) chali_panic("导出调用上下文无效");
    auto &temporaries = context.objects->materializedHeap;
    const auto begin = temporaries.size();
    try
    {
        NativeCallContextImpl call(context, context.invoker ? context.invoker : &current().invoker);
        marshal(&call, frame);
    } catch (const std::exception &error)
    {
        context.error = Memory::String("C ABI 导出失败：") + error.what();
    }
    // 参数封送已释放的临时块从登记表移除；内部导入产生的仍活跃借用保留既有生命周期。
    if (begin <= temporaries.size())
        temporaries.erase(std::remove_if(temporaries.begin() + static_cast<std::ptrdiff_t>(begin), temporaries.end(), [&](std::size_t address) {
                              std::size_t capacity = 0;
                              return !context.heap->capacityAt(address, capacity);
                          }),
                          temporaries.end());
    if (!context.memoryFault.empty()) context.error = context.memoryFault;
    if (!context.error.empty()) chali_panic(context.error.c_str());
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
    NativeContext &context = currentContext();
    NativeValue result;
    const auto makeArguments = [&] {
        Memory::Vector<NativeValue> arguments;
        if (count > 0) arguments.reserve(static_cast<std::size_t>(count));
        for (std::int64_t at = 0; at < count; ++at)
        {
            double real = 0;
            std::memcpy(&real, bits + at, sizeof(real));
            arguments.push_back(floating[at] ? NativeValue::fromFloat(real) : NativeValue::fromInt(bits[at]));
        }
        return arguments;
    };
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
            if (entry != nullptr && entry->direct != nullptr)
            {
                /* 直通：实参直接是生成代码写好的槽数组，不构造 NativeValue 向量。 */
                ChaliAbiRuntime runtime{ &context };
                ChaliAbiFrame frame{ &runtime, bits, floating, count, 0, 0 };
                entry->direct(&frame);
                if (frame.resultIsFloat != 0)
                {
                    double value = 0.0;
                    std::memcpy(&value, &frame.result, sizeof(value));
                    result = NativeValue::fromFloat(value);
                }
                else
                {
                    result = NativeValue::fromInt(frame.result);
                }
            }
            else if (entry != nullptr)
            {
                Memory::Vector<NativeValue> arguments = makeArguments();
                NativeCallContextImpl call(context, &state.invoker);
                result = entry->callback(call, arguments);
            }
            else
            {
                const auto handled = context.machine->invokeNativeService(state.program, context, name, makeArguments(), nativeThreadState.callbackDepth);
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
    nativeThreadState.nativeResultFloat = result.isFloat() ? 1 : 0;
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
 * @brief 直接查询盒装对象的类型号或地址，沿用对象查找和视图刷新协议。
 * @param operation 零读取类型号，一读取引用地址；由生成代码传入固定值。
 * @param handle 对象或接口描述符句柄。
 * @return 整数类型号或地址；失败时按通用原生入口的错误优先级终止。
 */
std::int64_t chali_host_object_scalar(std::int64_t operation, std::int64_t handle)
{
    NativeContext &context = currentContext();
    const char *name = operation == 0 ? "__std_obj_name" : "__std_obj_addr";
    std::int64_t result = 0;
    try
    {
        RuntimeObject *object = nativeObject(context, Slot::fromInt(handle), ObjectKind::Obj,
                                             operation == 0 ? "obj.name" : "接口指针");
        if (object) result = operation == 0 ? object->objTag : object->objValue.integer;
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
    nativeThreadState.nativeResultFloat = 0;
    return result;
}

/**
 * @brief 为 LLVM 字节 intrinsic 验证范围并返回物理地址，不复制或填充字节。
 * @param operation 7 为复制，8 为填充。
 * @param destination 目标逻辑地址。
 * @param sourceOrByte 源逻辑地址或填充值。
 * @param bytes 字节数。
 * @param output 接收目标物理地址。
 * @param input 接收源物理地址。
 */
void chali_host_memory_prepare(std::int64_t operation, std::int64_t destination, std::int64_t sourceOrByte,
                               std::int64_t bytes, unsigned char **output, unsigned char **input)
{
    NativeContext &context = currentContext();
    if (!rawMemoryView(context, operation, destination, sourceOrByte, bytes, *output, *input))
        chali_panic(context.error.c_str());
}

/**
 * @brief 直接执行原始存储标量内核，保留诊断与安全点，省去通用原生调用分派。
 * @param operation 操作编号。
 * @param handle 分配句柄或目标地址。
 * @param index 槽位下标、源地址或填充值。
 * @param value 操作值或字节数。
 * @param alignment 分配对齐。
 * @param tag 受检槽位类型身份。
 * @return 整数结果。
 */
std::int64_t chali_host_raw_storage(std::int64_t operation, std::int64_t handle, std::int64_t index,
                                    std::int64_t value, std::int64_t alignment, std::int64_t tag)
{
    NativeContext &context = currentContext();
    Slot result;
    try
    {
        result = rawStorageOperation(context, operation, handle, index, value, alignment, tag);
    } catch (const std::exception &error)
    {
        context.error = Memory::String("宿主函数 std::lang::__rawStorage 抛出异常：") + error.what();
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
    nativeThreadState.nativeResultFloat = 0;
    return result.integer;
}

/**
 * @brief 返回最近一次同线程宿主调用的原始槽类型，避免将浮点位模式当成整数。
 * @return 浮点返回一，其余原生结果返回零。
 */
unsigned char chali_host_result_float()
{
    return nativeThreadState.nativeResultFloat;
}

/**
 * @brief 以标量 ABI 查询字符串长度或内容地址，不经过函数表及动态实参数组。
 * @param operation 零读取长度，一取得首个内容字节地址。
 * @param address 字符串值的逻辑地址，零表示空值。
 * @return 对应长度或内容地址；访问失败时保留当前源码位置并终止。
 */
std::int64_t chali_host_string_storage(std::int64_t operation, std::int64_t address)
{
    NativeContext &context = currentContext();
    const auto source = static_cast<std::size_t>(address);
    const bool heap = context.heap != nullptr && context.heap->contains(source);
    if (operation == 1) return address + (heap ? 8 : 0);
    std::int64_t length = 0;
    if (heap && context.heap->readInt(source, length) && length >= 0) return length;
    try
    {
        Memory::String text;
        if (nativeReadCString(context, source, &text)) return static_cast<std::int64_t>(text.size());
        context.error = "取字符串长度时读不到它";
    } catch (const std::exception &error)
    {
        context.error = Memory::String("String 存储查询抛出异常：") + error.what();
    }
    if (!context.memoryFault.empty()) context.error = context.memoryFault;
    chali_panic(context.error.c_str());
    return 0;
}

/**
 * @brief 以标量 ABI 连接 List 单槽内核，保留异常、存活检查和同步回调协议。
 * @param operation 存储原语编号。
 * @param first 第一个实参位模式。
 * @param second 第二个实参位模式。
 * @param third 第三个实参位模式。
 * @param fourth 第四个实参位模式。
 * @param fifth 第五个实参位模式。
 * @param floating 浮点实参位图。
 * @return 返回槽的原始位模式。
 */
std::int64_t chali_host_list_storage(std::int64_t operation, std::int64_t first, std::int64_t second,
                                     std::int64_t third, std::int64_t fourth, std::int64_t fifth,
                                     std::int64_t floating)
{
    auto &state = current();
    NativeContext &context = currentContext();
    NativeValue result;
    try
    {
        NativeCallContextImpl bridge(context, &state.invoker);
        result = collection_glue::listStorageOperation(bridge, operation, first, second, third, fourth, fifth,
                                                       static_cast<std::uint64_t>(floating));
    } catch (const std::exception &error)
    {
        context.error = Memory::String("List 存储原语抛出异常：") + error.what();
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
    nativeThreadState.nativeResultFloat = result.isFloat();
    std::int64_t bits = 0;
    if (nativeThreadState.nativeResultFloat)
    {
        const double real = result.asFloat();
        std::memcpy(&bits, &real, sizeof(bits));
    }
    else
        bits = result.asInt();
    return bits;
}
/**
 * @brief 以标量 ABI 连接 Map/Set 单槽内核，保留异常、存活检查和同步回调协议。
 * @param operation 存储原语编号。
 * @param first 第一个实参位模式。
 * @param second 第二个实参位模式。
 * @param third 第三个实参位模式。
 * @param fourth 第四个实参位模式。
 * @param fifth 第五个实参位模式。
 * @param sixth 第六个实参位模式。
 * @param seventh 第七个实参位模式。
 * @param floating 浮点实参位图。
 * @return 返回槽的原始位模式。
 */
std::int64_t chali_host_map_storage(std::int64_t operation, std::int64_t first, std::int64_t second,
                                    std::int64_t third, std::int64_t fourth, std::int64_t fifth,
                                    std::int64_t sixth, std::int64_t seventh, std::int64_t floating)
{
    auto &state = current();
    NativeContext &context = currentContext();
    NativeValue result;
    try
    {
        NativeCallContextImpl bridge(context, &state.invoker);
        result = collection_glue::mapStorageOperation(bridge, operation, first, second, third, fourth, fifth, sixth, seventh,
                                                      static_cast<std::uint64_t>(floating));
    } catch (const std::exception &error)
    {
        context.error = Memory::String("Map/Set 存储原语抛出异常：") + error.what();
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
    nativeThreadState.nativeResultFloat = result.isFloat();
    std::int64_t bits = 0;
    if (nativeThreadState.nativeResultFloat)
    {
        const double real = result.asFloat();
        std::memcpy(&bits, &real, sizeof(bits));
    }
    else
        bits = result.asInt();
    return bits;
}
/**
 * @brief 直接返回数值与种类：以标量 ABI 连接 List 单槽内核，保留异常、存活检查和同步回调协议。
 * @param operation 存储原语编号。
 * @param first 第一个实参位模式。
 * @param second 第二个实参位模式。
 * @param third 第三个实参位模式。
 * @param fourth 第四个实参位模式。
 * @param fifth 第五个实参位模式。
 * @param floating 浮点实参位图。
 * @param resultFloating 接收结果种类，调用者提供当前栈帧内的有效字节。
 * @return 返回槽的原始位模式。
 */
std::int64_t chali_host_list_storage_scalar(std::int64_t operation, std::int64_t first, std::int64_t second,
                                     std::int64_t third, std::int64_t fourth, std::int64_t fifth,
                                     std::int64_t floating, unsigned char *resultFloating)
{
    auto &state = current();
    NativeContext &context = currentContext();
    collection_glue::ScalarResult result;
    try
    {
        NativeCallContextImpl bridge(context, &state.invoker);
        result = collection_glue::listStorageScalarOperation(bridge, operation, first, second, third, fourth, fifth,
                                                       static_cast<std::uint64_t>(floating));
    } catch (const std::exception &error)
    {
        context.error = Memory::String("List 存储原语抛出异常：") + error.what();
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
    *resultFloating = result.floating;
    return result.bits;
}

/**
 * @brief 直接返回数值与种类：以标量 ABI 连接 Map/Set 单槽内核，保留异常、存活检查和同步回调协议。
 * @param operation 存储原语编号。
 * @param first 第一个实参位模式。
 * @param second 第二个实参位模式。
 * @param third 第三个实参位模式。
 * @param fourth 第四个实参位模式。
 * @param fifth 第五个实参位模式。
 * @param sixth 第六个实参位模式。
 * @param seventh 第七个实参位模式。
 * @param floating 浮点实参位图。
 * @param resultFloating 接收结果种类，调用者提供当前栈帧内的有效字节。
 * @return 返回槽的原始位模式。
 */
std::int64_t chali_host_map_storage_scalar(std::int64_t operation, std::int64_t first, std::int64_t second,
                                    std::int64_t third, std::int64_t fourth, std::int64_t fifth,
                                    std::int64_t sixth, std::int64_t seventh, std::int64_t floating, unsigned char *resultFloating)
{
    auto &state = current();
    NativeContext &context = currentContext();
    collection_glue::ScalarResult result;
    try
    {
        NativeCallContextImpl bridge(context, &state.invoker);
        result = collection_glue::mapStorageScalarOperation(bridge, operation, first, second, third, fourth, fifth, sixth, seventh,
                                                      static_cast<std::uint64_t>(floating));
    } catch (const std::exception &error)
    {
        context.error = Memory::String("Map/Set 存储原语抛出异常：") + error.what();
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
    *resultFloating = result.floating;
    return result.bits;
}
}  // extern "C"
