/**
 * @file native_runtime.cpp
 * @brief LLVM 原生程序复用共享堆、对象表和标准库胶水，不解释语言指令。
 */
#include "ircode/runtime_internal.hpp"
#include "ircode/aot_thread_services.hpp"
#include "abi/chali_native_library.hpp"
#include "ircode/runtime_i18n.hpp"
#include "abi/chali_abi_runtime.hpp"
#include "abi/chali_abi_internal.h"
#include "allocator/system_arena.hpp"
#include "abi/chali_abi_registry.hpp"
#include "abi/std_collection.hpp"
#include "console/utf8.hpp"
#include <memory>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <map>
#include <mutex>
#include <stdexcept>
#include <optional>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#elif defined(__linux__)
#include <execinfo.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

namespace
{
bool debugArgumentsEnabled = false;
bool (*debugArgumentReader)() = nullptr;
static_assert(sizeof(ChaliDebugAllocation) == 24 && sizeof(ChaliDebugMemoryThread) == 232
                  && sizeof(ChaliDebugObject) == 32 && sizeof(ChaliDebugValue) == 48
                  && sizeof(ChaliDebugMoved) == 24 && sizeof(ChaliDebugFrameContext) == 24
                  && sizeof(ChaliDebugMemoryIndex) == 24,
              "调试内存目录需要固定的 64 位布局");
}
extern "C" {
/**
 * @brief 安装调试参数读取器并启用调试目录发布。
 * @param reader 仅在调试产物中链接的读取器。
 */
void chali_host_enable_debug(bool (*reader)())
{
    debugArgumentsEnabled = true;
    debugArgumentReader = reader;
}
/** @brief 只读调试目录的稳定根，具体区间在调试源码停点前发布。 */
ChaliDebugMemoryIndex chali_debug_memory{ 5, 8, 0 };
/**
 * @brief 登记当前程序实际可达的标准库模块。
 * @param registry 注册表。
 * @param error 诊断。
 * @return 稳定 C ABI 状态码，成功为零。
 */
int32_t chali_program_std_bind(ChaliAbiRegistry *registry, ChaliAbiError *error);

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
int32_t chali_program_abi_bind(ChaliAbiRegistry *registry, ChaliAbiError *error);
void chali_location(const char **file, std::int64_t *line, const void **function);
extern const std::size_t chali_trace_count;
extern const void *const chali_trace_functions[];
extern const char *const chali_trace_names[];
#if defined(__linux__)
extern const char __start_chali_code[], __stop_chali_code[];
#endif
}

thread_local stackvm::aot::NativeThreadState stackvm::aot::nativeThreadState{};

namespace
{
using namespace stackvm;
using namespace stackvm::runtimeDetail;
using namespace stackvm::aot;

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
Memory::CHeapUniquePtr<Memory::RuntimeAllocator> createRuntimeArena()
{
    if (chali_allocator_kind == 1) return Memory::makeCHeapUnique<Memory::SystemRuntimeArena>(chali_heap_size);
    return Memory::makeCHeapUnique<Memory::DynamicRuntimeArena>(chali_heap_size);
}

/**
 * @brief 单个原生模块拥有的共享服务状态，析构顺序保证对象先于堆释放。
 */
struct Runtime
{
    Memory::CHeapUniquePtr<Memory::RuntimeAllocator> arena;
    Memory::AllocationScope session;
    DataArea data;
    Memory::UniquePtr<execution_detail::DataCapacityIndex> dataCapacity;
    Heap heap;
    NativeInvoker invoker;
    NativeRegistry registry;
    ChaliAbiRegistry registryHandle{{}, &registry};
    const ChaliThreadServices *threadServices = nullptr;
    std::unique_ptr<void, void (*)(void *)> machine{nullptr, nullptr};
    Program program;
    Memory::Vector<Memory::String> arguments;
    NativeContext context;
    bool keepText = false;
    bool exiting = false;
    int exitCode = 0;
    NativeServices services;
    ObjectStore objects;

    /**
     * @brief 装入静态数据并注册 运行时 同源的原生函数与同步回调。
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
        services.emplace("native.rev.v1", &objects.nativeRecordRevision);
        context.hostServices = &services;
        context.textSink = writeOutput;
        context.textKeep = &keepText;
        context.programArguments = &arguments;
        context.exitRequested = &exiting;
        context.exitCode = &exitCode;
        context.invoker = &invoker;
        threadServices = chali_program_thread_services();
        if (threadServices)
        {
            machine = {threadServices->create(&context, &program, arena.get(), chali_threads_enabled != 0), threadServices->destroy};
            context.machine = static_cast<Machine *>(machine.get());
        }
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
                 * @brief 标记同步胶水回调，保持 运行时 的线程与消息操作限制。
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
        char message[1024];
        ChaliAbiError error{message, sizeof(message), 0, ChaliAbiOk};
        if (chali_program_std_bind(&registryHandle, &error) != ChaliAbiOk) chali_panic(message);
        if (chali_program_abi_bind(&registryHandle, &error) != ChaliAbiOk) chali_panic(message);
        if (chali_abi_registry_seal(&registryHandle, &error) != ChaliAbiOk) chali_panic(message);
    }

    /**
     * @brief 清理胶水登记的临时值，其余资源由对象表和堆按顺序收尾。
     */
    ~Runtime()
    {
        if (threadServices) threadServices->finish(&context);
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

std::mutex debugMemoryMutex;             ///< 只保护调试目录节点登记，不参与语言堆分配。
std::size_t debugMemoryThreadCount = 0;  ///< 进程目录节点总数，包含已退出线程。
/** @brief 每个调试线程的宿主观察缓冲，不计入语言对象或堆分配。 */
struct DebugMemoryState
{
    std::vector<ChaliDebugObject, Memory::CHeapAllocator<ChaliDebugObject>> objects;
    std::vector<ChaliDebugValue, Memory::CHeapAllocator<ChaliDebugValue>> values;
    std::vector<char, Memory::CHeapAllocator<char>> text;
    std::vector<ChaliDebugMoved, Memory::CHeapAllocator<ChaliDebugMoved>> moved;
    ChaliDebugMemoryThread *record = nullptr;
    ChaliDebugAllocation *allocations = nullptr;
    std::size_t capacity = 0;
    bool disabled = false;
    /**
     * @brief 核对所有宿主快照缓冲的总预算。
     * @param extra 即将增加的字节数。
     * @return 预算允许时为真。
     */
    bool room(std::size_t extra) const
    {
        const auto used = objects.size() * sizeof(ChaliDebugObject) + values.size() * sizeof(ChaliDebugValue) + text.size()
                          + moved.size() * sizeof(ChaliDebugMoved);
        return used <= 2 * 1024 * 1024 && extra <= 2 * 1024 * 1024 - used;
    }
    /**
     * @brief 把借用值复制到固定宽度节点，索引不受缓冲扩容影响。
     * @param source 当前正常执行期间有效的借用值。
     * @param index 已预留的目标节点。
     * @param depth 聚合递归深度。
     * @return 所有字段均在预算内时为真。
     */
    bool copyValue(const collection_glue::DebugValueView &source, std::size_t index, std::size_t depth)
    {
        if (depth > 32 || source.length > 2 * 1024 * 1024 || source.fields > 32768
            || !room(source.length + source.fields * sizeof(ChaliDebugValue))) return false;
        ChaliDebugValue value{};
        value.flags = (source.aggregate ? 1u : 0u) | (source.textValue ? 2u : 0u) | (source.floating ? 4u : 0u);
        value.bits = source.bits;
        if (source.textValue)
        {
            value.text = text.size();
            value.length = source.length;
            if (source.length) text.insert(text.end(), source.text, source.text + source.length);
        }
        if (source.aggregate)
        {
            value.first = values.size();
            value.count = source.fields;
            if (value.first + value.count > 32768) return false;
            values.resize(values.size() + source.fields);
            for (std::size_t field = 0; field < source.fields; ++field)
            {
                collection_glue::DebugValueView child;
                if (!collection_glue::debugCollectionField(source, field, &child) || !copyValue(child, value.first + field, depth + 1)) return false;
            }
        }
        values[index] = value;
        return true;
    }
    /**
     * @brief 复制已有动态值的存储，不执行复制、析构或格式化回调。
     * @param source 暂停点前实际存在的值。
     * @param index 已预留的观察节点。
     * @param depth 当前递归深度。
     * @return 所有内容均在观察预算内时为真。
     */
    bool copyStored(const stackvm::NativeStoredValue &source, std::size_t index, std::size_t depth)
    {
        if (depth > 32 || source.fields.size() > 32768 || source.text.size() > 2 * 1024 * 1024
            || !room(source.text.size() + source.fields.size() * sizeof(ChaliDebugValue))) return false;
        ChaliDebugValue value{};
        value.flags = source.isStruct ? 1u : source.isText ? 2u :
                                         source.isFloat    ? 4u :
                                                             0u;
        value.bits = static_cast<std::uint64_t>(source.integer);
        if (source.isFloat) std::memcpy(&value.bits, &source.real, sizeof(value.bits));
        if (source.isText)
        {
            value.text = text.size();
            value.length = source.text.size();
            text.insert(text.end(), source.text.begin(), source.text.end());
        }
        if (source.isStruct)
        {
            value.first = values.size();
            value.count = source.fields.size();
            if (value.first + value.count > 32768) return false;
            values.resize(value.first + value.count);
            for (std::size_t field = 0; field < source.fields.size(); ++field)
                if (!copyStored(source.fields[field], value.first + field, depth + 1)) return false;
        }
        values[index] = value;
        return true;
    }
    /**
     * @brief 复制活跃集合、动态装箱或闭包，不调用宿主记录的方法或同步回调。
     * @param handle 原运行时句柄。
     * @param object 当前存活对象。
     * @return 对象观察成功或该对象无需观察时为真，预算耗尽为假。
     */
    bool copyObject(std::int64_t handle, const RuntimeObject &object)
    {
        if (object.kind == ObjectKind::Closure)
        {
            if (object.items.size() >= 32768) return false;
            const auto count = object.items.size() + 1;
            if (objects.size() >= 4096 || values.size() > 32768 - count
                || !room(sizeof(ChaliDebugObject) + count * sizeof(ChaliDebugValue))) return false;
            const auto first = values.size();
            objects.push_back({ static_cast<std::uint64_t>(handle), 'F', count, first });
            values.resize(first + count);
            values[first].bits = static_cast<std::uint64_t>(object.closureTarget);
            for (std::size_t index = 0; index < object.items.size(); ++index)
            {
                const auto view = object.elementViews.find(index);
                if (view != object.elementViews.end())
                {
                    values[first + index + 1].flags = 8;
                    values[first + index + 1].bits = view->second.address;
                }
                else if (!copyStored(object.items[index], first + index + 1, 0))
                    return false;
            }
            return true;
        }
        if (object.kind == ObjectKind::Obj)
        {
            if (objects.size() >= 4096 || values.size() > 32766
                || !room(sizeof(ChaliDebugObject) + 2 * sizeof(ChaliDebugValue))) return false;
            const auto first = values.size();
            objects.push_back({ static_cast<std::uint64_t>(handle), 'O', 2, first });
            values.resize(first + 2);
            values[first].bits = static_cast<std::uint64_t>(object.objTag);
            return copyStored(object.objValue, first + 1, 0);
        }
        if (object.kind != ObjectKind::NativeRecord) return copyOpaque(handle);
        collection_glue::DebugCollectionView view;
        if (!collection_glue::debugCollection(object.nativeType, object.nativeRecord.get(), &view)) return copyOpaque(handle);
        const std::size_t stride = view.kind == 'M' ? 2 : 1;
        if (objects.size() >= 4096 || view.count > 32768 / stride || values.size() > 32768 - view.count * stride
            || !room(sizeof(ChaliDebugObject) + view.count * stride * sizeof(ChaliDebugValue))) return false;
        const auto first = values.size();
        objects.push_back({ static_cast<std::uint64_t>(handle), static_cast<std::uint64_t>(view.kind), view.count, first });
        values.resize(first + view.count * stride);
        for (std::size_t index = 0; index < view.count; ++index)
            for (std::size_t part = 0; part < stride; ++part)
            {
                collection_glue::DebugValueView value;
                if (!collection_glue::debugCollectionItem(view, index, stride == 2 && part == 0, &value)
                    || !copyValue(value, first + index * stride + part, 0)) return false;
            }
        return true;
    }
    /**
     * @brief 保留无法展开的活跃宿主对象身份，不把缺少字段的对象误报为已释放。
     * @param handle 实际对象表句柄。
     * @return 观察预算足够时为真。
     */
    bool copyOpaque(std::int64_t handle)
    {
        if (objects.size() >= 4096 || !room(sizeof(ChaliDebugObject))) return false;
        objects.push_back({ static_cast<std::uint64_t>(handle), 'X', 0, 0 });
        return true;
    }
    /**
     * @brief 发布当前对象表的有界只读快照，失败只撤销本次对象查看。
     * @param context 当前线程的实际原生上下文。
     * @return 无返回值。
     */
    void publishObjects(NativeContext &context)
    {
        objects.clear();
        values.clear();
        text.clear();
        auto *store = context.objects;
        record->objects_complete = 0;
        record->object_revision_address = store ? reinterpret_cast<std::uintptr_t>(&store->debugSnapshotRevision) : 0;
        record->object_revision = store ? store->debugSnapshotRevision : 0;
        try
        {
            if (!store || !store->debugSnapshotRevision) throw std::runtime_error("对象视图失效");
            for (const auto &slot : store->handleSlots)
                if (slot.handle && slot.object && !copyObject(slot.handle, *slot.object)) throw std::runtime_error("对象视图超过预算");
            for (const auto &entry : store->objects)
                if (entry.second && !copyObject(entry.first, *entry.second)) throw std::runtime_error("对象视图超过预算");
            if (record->object_revision != store->debugSnapshotRevision) throw std::runtime_error("对象视图代次变化");
            record->objects_complete = 1;
        } catch (...)
        {
            objects.clear();
            values.clear();
            text.clear();
        }
        record->objects = reinterpret_cast<std::uintptr_t>(objects.data());
        record->object_count = objects.size();
        record->values = reinterpret_cast<std::uintptr_t>(values.data());
        record->value_count = values.size();
        record->text = reinterpret_cast<std::uintptr_t>(text.data());
        record->text_size = text.size();
    }
    /**
     * @brief 仅复制仍处于当前活跃分配或数据区内的销毁标记，共享观察预算。
     * @param context 当前正常执行路径的实际上下文。
     * @return 无；预算不足时撤销本轮完整内存观察。
     */
    void publishMoved(NativeContext &context)
    {
        if (!context.objects || !context.data || !context.heap)
        {
            record->complete = 0;
            record->moved = record->moved_count = 0;
            return;
        }
        std::size_t inspected = 0;
        for (const auto *entries : { &context.objects->movedStorage, &context.objects->debugMovedStorage })
        {
            for (const auto &entry : *entries)
            {
                if (inspected++ >= 65536) { record->complete = 0; break; }
                const auto address = entry.first.first, size = entry.second;
                if (!address || !size) continue;
                const bool inData = address < context.data->size() && size <= context.data->size() - address;
                std::size_t capacity = 0;
                if (!inData && (!context.heap->capacityAt(address, capacity) || size > capacity)) continue;
                if (moved.size() >= 65536 || !room(sizeof(ChaliDebugMoved))) { record->complete = 0; break; }
                moved.push_back({ address, size, entry.first.second });
            }
            if (!record->complete) break;
        }
        record->moved = reinterpret_cast<std::uintptr_t>(moved.data());
        record->moved_count = moved.size();
    }
    /** @brief 线程结束时撤销目录并释放观察缓冲，目录节点保留进程寿命。 */
    ~DebugMemoryState()
    {
        if (record) record->state = 0;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        std::free(allocations);
    }
    /**
     * @brief 发布当前堆的有界公开区间，观察失败不改变程序运行结果。
     * @return 无返回值。
     */
    void publish()
    {
        if (disabled) return;
        if (!record)
        {
            std::lock_guard<std::mutex> guard(debugMemoryMutex);
            if (debugMemoryThreadCount >= 4096)
            {
                disabled = true;
                return;
            }
            record = static_cast<ChaliDebugMemoryThread *>(std::calloc(1, sizeof(ChaliDebugMemoryThread)));
            if (!record)
            {
                disabled = true;
                return;
            }
#ifdef _WIN32
            record->thread = GetCurrentThreadId();
#elif defined(__linux__)
            record->thread = static_cast<std::uint64_t>(syscall(SYS_gettid));
#endif
            record->state = 1;
            record->next = chali_debug_memory.first;
            std::atomic_thread_fence(std::memory_order_release);
            chali_debug_memory.first = reinterpret_cast<std::uintptr_t>(record);
            ++debugMemoryThreadCount;
        }
        const auto previous = record->state;
        if (previous > std::numeric_limits<std::uint64_t>::max() - 2)
        {
            record->state = 0;
            disabled = true;
            return;
        }
        record->state = previous | 1u;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        record->complete = 0;
        auto &context = currentContext();
        moved.clear();
        record->context_address = reinterpret_cast<std::uintptr_t>(&nativeThreadState.activeContext);
        record->context = reinterpret_cast<std::uintptr_t>(nativeThreadState.activeContext);
        record->heap_address = reinterpret_cast<std::uintptr_t>(&context.heap);
        record->heap_identity = reinterpret_cast<std::uintptr_t>(context.heap);
        record->data_address = reinterpret_cast<std::uintptr_t>(&context.data);
        record->data_identity = reinterpret_cast<std::uintptr_t>(context.data);
        record->objects_address = reinterpret_cast<std::uintptr_t>(&context.objects);
        record->objects_identity = reinterpret_cast<std::uintptr_t>(context.objects);
        auto *heap = static_cast<Heap *>(context.heap);
        record->data = reinterpret_cast<std::uintptr_t>(context.data->binary.data());
        record->data_size = context.data->binary.size();
        record->revision_address = reinterpret_cast<std::uintptr_t>(heap->mappingRevision());
        std::size_t count = 0;
        std::uint64_t revision = 0;
        for (unsigned attempt = 0; attempt < 3; ++attempt)
        {
            if (heap->debugAllocations(allocations, capacity, count, revision))
            {
                record->complete = 1;
                break;
            }
            if (count > 65536) break;
            const auto wanted = std::min<std::size_t>(65536, std::max(count, capacity ? capacity * 2 : 16));
            auto *replacement = static_cast<ChaliDebugAllocation *>(std::realloc(allocations, wanted * sizeof(ChaliDebugAllocation)));
            if (!replacement) break;
            allocations = replacement;
            capacity = wanted;
        }
        record->allocations = reinterpret_cast<std::uintptr_t>(allocations);
        record->count = record->complete ? count : 0;
        record->revision = revision;
        publishObjects(context);
        publishMoved(context);
        std::atomic_thread_fence(std::memory_order_release);
        record->state = (previous | 1u) + 1;
    }
};
#ifdef _WIN32
/**
 * @brief 在线程退出或模块卸载时回收独立观察状态，不访问编译器仿真 TLS 存储。
 * @param value 当前 FLS 槽拥有的观察状态，可为空。
 * @return 无返回值。
 */
void NTAPI releaseDebugMemoryState(void *value) noexcept
{
    Memory::destroyCHeap(static_cast<DebugMemoryState *>(value));
}
/** @brief 用 Windows 生命周期回调管理观察状态，避开旧 MinGW 仿真 TLS 的析构顺序。 */
struct DebugMemorySlot
{
    DWORD index;
    /** @brief 分配单个模块级观察槽，资源不足时仅使观察发布失败。 */
    DebugMemorySlot() : index(FlsAlloc(releaseDebugMemoryState))
    {
        if (index == FLS_OUT_OF_INDEXES) throw std::runtime_error("无法创建调试观察槽");
    }
    /** @brief 模块卸载前撤销槽及尚存状态，避免留下指向已卸载代码的回调。 */
    ~DebugMemorySlot() { FlsFree(index); }
};
#endif
/**
 * @brief 仅在调试发布首次使用时建立线程观察状态，避免发布程序初始化额外 TLS 析构。
 * @return 当前线程的调试观察状态。
 */
DebugMemoryState &debugMemoryState()
{
#ifdef _WIN32
    static DebugMemorySlot slot;
    auto *state = static_cast<DebugMemoryState *>(FlsGetValue(slot.index));
    if (!state)
    {
        auto created = Memory::makeCHeapUnique<DebugMemoryState>();
        if (!FlsSetValue(slot.index, created.get())) throw std::runtime_error("无法登记调试观察状态");
        state = created.release();
    }
    return *state;
#else
    static thread_local DebugMemoryState state;
    return state;
#endif
}

}  // namespace

extern "C" {
/**
 * @brief 在正常源码函数序言中捕获不透明的上下文身份，供暂停后核对。
 * @param output 编译器生成的三个观察槽，空指针不写入。
 * @return 无，不调用用户代码或创建语言对象。
 */
void chali_debug_capture_context(ChaliDebugFrameContext *output)
{
    if (!output) return;
    auto &context = currentContext();
    *output = { reinterpret_cast<std::uintptr_t>(context.heap), reinterpret_cast<std::uintptr_t>(context.data),
                reinterpret_cast<std::uintptr_t>(context.objects) };
}
/**
 * @brief 在程序正常执行期间更新独立的调试存储状态。
 * @param operation 零为真实写入，一为移动。
 * @param address 逻辑存储地址。
 * @param bytes 存储宽度。
 * @param type 实际类型编号。
 * @return 无；状态更新失败只撤销观察。
 */
void chali_debug_storage_lifetime(std::uint64_t operation, std::uint64_t address, std::uint64_t bytes, std::int64_t type)
{
    if (!debugArgumentsEnabled) return;
    auto &context = currentContext();
    if (!context.objects) return;
    if (operation == 0) context.objects->beginDebugStorage(address, bytes);
    else if (operation == 1) context.objects->markDebugStorage(address, bytes, type);
}
/**
 * @brief 在调试停点之前复制当前线程的内存目录，不在变量查看请求期间执行。
 * @return 无返回值，目录复制或预算失败时保持目标程序继续运行。
 */
void chali_debug_publish_memory()
{
    if (!debugArgumentsEnabled) return;
    DebugMemoryState *state = nullptr;
    try
    {
        state = &debugMemoryState();
        state->publish();
    } catch (...)
    {
        if (state && state->record) state->record->state = 0;
    }
}
/**
 * @brief 同步语言回调失败时退回受检查的调用器，让 C 调用完成资源释放。
 * @param message 原始运行期错误。
 * @note 无回调时正常返回，随后沿用终止进程的诊断；异常由回调跳板截获。
 */
void chali_host_raise_callback_error(const char *message)
{
    if (nativeThreadState.callbackDepth == 0) return;
    auto &context = currentContext();
    if (context.error.empty()) context.error = message ? message : "同步回调失败";
    throw std::runtime_error(context.error.c_str());
}

/**
 * @brief 按声明路径加载动态库并解析原始 C 函数地址。
 * @param path UTF-8 动态库路径。
 * @param name 目标 C 符号。
 * @param failure 有界错误输出，可为空。
 * @return 解析后的机器地址，失败返回空指针。
 * @note 库引用保留到进程结束，使跨模块重新初始化和持续宿主线程的缓存始终有效。
 */
void *chali_host_abi_lookup(const char *path, const char *name, ChaliAbiError *failure) try
{
    if (!path || !name)
    {
        chali_abi_status(failure, ChaliAbiInvalidArgument, "missing library path or symbol");
        return nullptr;
    }
    static std::mutex mutex;
    using LibraryPath = std::basic_string<char, std::char_traits<char>, Memory::CHeapAllocator<char>>;
    using LibraryReference = std::shared_ptr<chali_host_detail::NativeLibrary>;
    static std::map<LibraryPath, LibraryReference, std::less<>,
                    Memory::CHeapAllocator<std::pair<const LibraryPath, LibraryReference>>> libraries;
    std::shared_ptr<chali_host_detail::NativeLibrary> library;
    {
        std::lock_guard<std::mutex> guard(mutex);
        const auto found = libraries.find(path);
        if (found != libraries.end()) library = found->second;
    }
    Memory::String error;
    if (!library)
    {
        auto opened = std::allocate_shared<chali_host_detail::NativeLibrary>(Memory::CHeapAllocator<chali_host_detail::NativeLibrary>());
        if (!opened->open(path, &error))
        {
            chali_abi_status(failure, ChaliAbiHostFailure, error.c_str());
            return nullptr;
        }
        std::lock_guard<std::mutex> guard(mutex);
        library = libraries.emplace(path, std::move(opened)).first->second;
    }
    void *address = library->resolve(name, &error);
    chali_abi_status(failure, address ? ChaliAbiOk : ChaliAbiHostFailure, address ? nullptr : error.c_str());
    return address;
}
catch (const std::exception &error)
{
    chali_abi_status(failure, ChaliAbiHostFailure, error.what());
    return nullptr;
}
catch (...)
{
    chali_abi_status(failure, ChaliAbiHostFailure, "native library lookup failed");
    return nullptr;
}
/**
 * @brief 解析原生符号并在失败时进入原有运行时错误路径。
 * @param path UTF-8 库路径。
 * @param name 精确 C 符号名。
 * @return 原生机器地址。
 */
void *chali_host_abi_symbol(const char *path, const char *name)
{
    char message[1024];
    ChaliAbiError error{message, sizeof(message), 0, ChaliAbiOk};
    void *address = chali_host_abi_lookup(path, name, &error);
    if (!address) chali_panic(message);
    return address;
}

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
    runtime = Memory::makeCHeapUnique<Runtime>().release();
}

/**
 * @brief 输出错误位置与实际原生调用链，不分配语言堆内存。
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
        std::fprintf(stderr, CHALI_RUNTIME_TEXT("出错位置：%s:%lld%s%s%s\n"), file, static_cast<long long>(line), name ? "（" : "", name ? name : "", name ? "）" : "");
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
        std::fputs(CHALI_RUNTIME_TEXT("调用栈（内 → 外）：\n"), stderr);
        std::fprintf(stderr, "  %s\n", name);
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
            if (!printed) std::fputs(CHALI_RUNTIME_TEXT("调用栈（内 → 外）：\n"), stderr);
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
    Memory::destroyCHeap(runtime);
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
    if (const auto *services = current().threadServices) services->poll(&context);
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
 * @brief 接收调试服务解析的有界 UTF-8 字节串集合。
 * @param values 已验证参数，允许空串及内嵌零。
 */
void chali_host_set_debug_arguments(const Memory::Vector<Memory::String> *values)
{
    current().arguments = *values;
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
    if (debugArgumentReader && debugArgumentReader()) return;
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
 * @param stride 元素步长；非正值沿用 运行时 的八字节默认步长。
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
 * @brief 在共享对象表中开始普通聚合存储的新生命周期。
 * @param address 已分配存储的逻辑地址。
 * @param bytes 聚合存储的字节数。
 * @return 无。
 */
void chali_host_storage_begin(std::int64_t address, std::int64_t bytes)
{
    if (address < 0 || bytes < 0) chali_panic("聚合生命周期地址或长度无效");
    currentContext().objects->beginStorage(static_cast<std::size_t>(address), static_cast<std::size_t>(bytes));
}

/**
 * @brief 在共享对象表中查询、核对或结束已统一类型编号的聚合生命周期。
 * @param operation 零查询、一核对、二结束。
 * @param value 聚合逻辑地址。
 * @param count 聚合存储字节数。
 * @param type 最终程序登记的类型编号。
 * @return 查询返回存活标志，其余成功返回零；失效核对报告原运行错误。
 */
std::int64_t chali_host_storage_lifetime(std::int64_t operation, std::int64_t value, std::int64_t count, std::int64_t type)
{
    auto &context = currentContext();
    const auto address = static_cast<std::size_t>(value);
    const auto bytes = static_cast<std::size_t>(count);
    auto &ranges = context.objects->movedStorage;
    const auto key = std::make_pair(address, type);
    std::size_t capacity = 0;
    const bool allocationLive = !context.heap->contains(address) || context.heap->capacityAt(address, capacity);
    const bool live = !bytes || (allocationLive && ranges.find(key) == ranges.end());
    if (operation == 0) return live;
    if (operation == 1 && !live) chali_panic("move 的源对象已经销毁，必须先重新赋值");
    if (operation == 2)
    {
        context.objects->invalidateDebugSnapshots();
        ranges[key] = bytes;
        context.objects->largestMovedStorage = std::max(context.objects->largestMovedStorage, bytes);
    }
    if (operation < 0 || operation > 2) chali_panic("聚合生命周期操作无效");
    return 0;
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
 * @param marshal 接收 C 帧的同步封送入口，不是语言函数值。
 * @param frame 本次原生调用独立的参数和返回值帧。
 */
void chali_host_export_apply(void (*marshal)(ChaliAbiFrame *, void *), void *frame)
{
    auto &context = currentContext();
    if (!marshal || !context.objects || !context.heap) chali_panic("导出调用上下文无效");
    auto &temporaries = context.objects->materializedHeap;
    const auto begin = temporaries.size();
    try
    {
        ChaliAbiRuntime services{&context};
        ChaliAbiFrame call{&services, nullptr, nullptr, 0, 0, 0};
        marshal(&call, frame);
        if (services.status != ChaliAbiOk)
            context.error = services.fallback ? services.fallback : services.diagnostic.c_str();
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
 * @brief 使用与 运行时 相同的逻辑分配身份创建清零内存。
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
 * @brief 按 运行时 闭包协议解析函数标签或隐藏环境。
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
    std::optional<NativeValue> result;
    std::int64_t directBits = 0;
    int directKind = -1;
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
            result.emplace(value.isFloat ? NativeValue::fromFloat(value.real) : value.isText ? NativeValue::fromText(value.text) :
                                                                                                NativeValue::fromInt(value.integer));
        }
        else
        {
            const auto *entry = state.registry.find(name);
            if (entry && entry->arity != static_cast<std::size_t>(-1) && static_cast<std::size_t>(count) != entry->arity) chali_panic("宿主函数实参数量不匹配");
            if (entry != nullptr && entry->direct != nullptr)
            {
                /* 直通：实参直接是生成代码写好的槽数组，不构造 NativeValue 向量。 */
                ChaliAbiRuntime runtime{ &context };
                ChaliAbiFrame frame{ &runtime, bits, floating, count, 0, 0 };
                char diagnostic[128];
                ChaliAbiError abiError{diagnostic, sizeof(diagnostic), 0, ChaliAbiOk};
                if (chali_abi_check_frame(&frame, entry->directArity, &abiError) != ChaliAbiOk)
                    chali_panic(diagnostic);
                entry->direct(&frame);
                if (runtime.status != ChaliAbiOk)
                    chali_panic(!context.memoryFault.empty() ? context.memoryFault.c_str() :
                                runtime.fallback ? runtime.fallback : runtime.diagnostic.c_str());
                if (chali_abi_check_result(&frame, &abiError) != ChaliAbiOk)
                    chali_panic(diagnostic);
                directBits = frame.result;
                directKind = frame.resultIsFloat;
            }
            else if (entry != nullptr)
            {
                Memory::Vector<NativeValue> arguments = makeArguments();
                NativeCallContextImpl call(context, &state.invoker);
                result.emplace(entry->callback(call, arguments));
            }
            else
            {
                std::optional<NativeValue> handled;
                if (state.threadServices)
                {
                    state.threadServices->invoke(&state.program, &context, name, std::strlen(name),
                                                  static_cast<std::size_t>(count), bits, floating, nativeThreadState.callbackDepth,
                        [](void *target, int kind, std::int64_t raw, const char *text, std::size_t size) {
                            auto &value = *static_cast<std::optional<NativeValue> *>(target);
                            if (kind == 2) value.emplace(NativeValue::fromText(Memory::String(text, size)));
                            else if (kind == 1)
                            {
                                double real = 0;
                                std::memcpy(&real, &raw, sizeof(real));
                                value.emplace(NativeValue::fromFloat(real));
                            }
                            else value.emplace(NativeValue::fromInt(raw));
                        }, &handled);
                }
                if (!handled) chali_panic((Memory::String("找不到原生服务：") + name).c_str());
                result.emplace(*handled);
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
    if (directKind >= 0)
    {
        nativeThreadState.nativeResultFloat = directKind;
        return directBits;
    }
    const auto &value = *result;
    nativeThreadState.nativeResultFloat = value.isFloat() ? 1 : 0;
    if (value.isText()) return static_cast<std::int64_t>(nativeAllocString(context, value.text));
    if (value.isFloat())
    {
        std::int64_t raw = 0;
        std::memcpy(&raw, &value.floating, sizeof(raw));
        return raw;
    }
    return value.integer;
}

/**
 * @brief 直接查询盒装对象或复制句柄，沿用对象查找、视图刷新和复制协议。
 * @param operation 零类型号，一引用地址，二整数句柄复制，三按原浮点槽转换后复制。
 * @param handle 查询时为对象或接口描述符，复制时为原始可复制对象句柄。
 * @return 整数查询结果或独立句柄；失败时按通用原生入口的错误优先级终止。
 */
std::int64_t chali_host_object_scalar(std::int64_t operation, std::int64_t handle)
{
    NativeContext &context = currentContext();
    const bool copying = operation == 2 || operation == 3;
    const char *name = operation == 0 ? "__std_obj_name" : copying ? "__std_object_copy" : "__std_obj_addr";
    std::int64_t result = 0;
    try
    {
        if (copying)
        {
            if (operation == 3)
            {
                double real = 0.0;
                std::memcpy(&real, &handle, sizeof(real));
                handle = nativeInt(Slot::fromDouble(real));
            }
            result = copyObjectHandle(context, handle);
        }
        else
        {
            RuntimeObject *object = nativeObject(context, Slot::fromInt(handle), ObjectKind::Obj,
                                                 operation == 0 ? "obj.name" : "接口指针");
            if (object) result = operation == 0 ? object->objTag : object->objValue.integer;
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
    nativeThreadState.nativeResultFloat = 0;
    return result;
}

/**
 * @brief 取得当前上下文的数据区视图；供无不透明调用的函数在当前帧内使用。
 * @param size 接收当前数据区的实际字节数；缺少上下文时写零。
 * @return 当前数据区首地址；模块尚未初始化时返回空，不提前改变错误行为。
 * @note 不分配、不调整数据区，也不将视图保存在跨调用缓存中。
 */
unsigned char *chali_host_data_snapshot(std::size_t *size)
{
    *size = 0;
    NativeContext *context = nativeThreadState.activeContext ? nativeThreadState.activeContext :
                             runtime ? &runtime->context : nullptr;
    if (!context || !context->data) return nullptr;
    *size = context->data->binary.size();
    return const_cast<unsigned char *>(context->data->binary.data());
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
        context.error = Memory::String("宿主函数 std::core::__rawStorage 抛出异常：") + error.what();
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
 * @param operation 零读取长度，一读取内容地址，二查询容量，三查询描述符标记。
 * @param address 字符串值的逻辑地址，零表示空值。
 * @return 对应长度或内容地址；访问失败时保留当前源码位置并终止。
 */
std::int64_t chali_host_string_storage(std::int64_t operation, std::int64_t address)
{
    NativeContext &context = currentContext();
    std::int64_t result = 0;
    try
    {
        result = stringStorageQuery(context, operation, address);
    } catch (const std::exception &error)
    {
        context.error = Memory::String("String 存储查询抛出异常：") + error.what();
    }
    if (!context.memoryFault.empty()) context.error = context.memoryFault;
    if (!context.error.empty()) chali_panic(context.error.c_str());
    return result;
}

/**
 * @brief 保留原字符串查询及错误优先级，并为后续 LLVM 读取缓存完整堆描述符。
 * @param operation 零长度、一内容地址、二容量、三描述符标记。
 * @param address 字符串的逻辑地址。
 * @param view 可写视图；仅在完整三十二字节缓冲描述符有效时更新。
 * @return 原查询结果，字面量和旧布局不要求额外的可访问字节。
 */
std::int64_t chali_host_string_storage_view(std::int64_t operation, std::int64_t address, Heap::AddressView *view)
{
    const auto result = chali_host_string_storage(operation, address);
    auto &context = currentContext();
    if (view && address >= 0 && context.heap)
    {
        auto *heap = static_cast<Heap *>(context.heap);
        std::int64_t marker = 0;
        const auto at = static_cast<std::size_t>(address);
        if (heap->readInt(at, marker) && marker == -1) heap->borrowView(at, 32u, *view);
    }
    return result;
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
    nativeThreadState.nativeResultFloat = result.floating != 0;
    return result.bits;
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
    collection_glue::ScalarResult result;
    try
    {
        NativeCallContextImpl bridge(context, &state.invoker);
        result = collection_glue::mapStorageScalarOperation(bridge, operation, first, second, third, fourth, fifth, sixth, seventh,
                                                      static_cast<std::uint64_t>(floating));
    } catch (const std::exception &error)
    {
        // 同步语言回调已保存原始错误，外层存储入口只补充未报告的宿主异常。
        if (context.error.empty())
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
    nativeThreadState.nativeResultFloat = result.floating != 0;
    return result.bits;
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
 * @brief 在已完成的存储调用之后取得数值数组视图，不调用用户代码。
 * @param handle 已检查的集合句柄。
 * @param view 接收稳定头部与失效代次。
 */
void chali_host_list_scalar_view(std::int64_t handle, collection_glue::NumericListView *view)
{
    NativeCallContextImpl bridge(currentContext(), &current().invoker);
    collection_glue::numericListView(bridge, handle, view);
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
        // 同步语言回调已保存原始错误，外层存储入口只补充未报告的宿主异常。
        if (context.error.empty())
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
