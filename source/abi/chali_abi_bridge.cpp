/**
 * @file chali_abi_bridge.cpp
 * @brief 直通 C 胶水访问运行时服务的桥接实现。
 *
 * C 胶水只能通过 `chali_abi.h` 里的 `extern "C"` 函数触达运行时：数据区、堆、
 * 对象表、错误通道和同步回调。桥接层把 `NativeContext` 收窄成一个不透明句柄，
 * C 侧因此不需要包含任何 C++ 头，也不依赖 STL 容器与异常。
 */
#include "chali_abi_internal.h"
#include "chali_abi_runtime.hpp"
#include "../glue/std_collection.hpp"

#include "ircode/runtime_internal.hpp"
#include "ircode/native_memory_access.hpp"
#include "ircode/native_memory.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <optional>
#include <exception>
#include <mutex>
#include <string>
#include <vector>

namespace
{
using namespace stackvm;
using namespace stackvm::runtimeDetail;

/**
 * @brief 取当前帧的运行时上下文。
 * @param frame 当前调用帧。
 * @return 上下文指针；帧或句柄缺失时返回空指针。
 */
NativeContext *contextOf(const ChaliAbiFrame *frame)
{
    return (frame != nullptr && frame->runtime != nullptr) ? frame->runtime->context : nullptr;
}

/**
 * @brief 按类型名缓存的宿主记录描述符；地址稳定，供对象表长期持有。
 * @note 名称用宿主堆的 `std::string`：本表是进程级静态存储，析构时机晚于运行时
 *       arena 的收尾检查，若用 `Memory::String` 会留下无法回收的活动块。
 */
struct RecordTypeBinding
{
    std::string name;
    NativeRecordType descriptor{};
};

/**
 * @brief 已登记的记录描述符；只增不删，生命周期与进程相同。
 */
std::vector<std::unique_ptr<RecordTypeBinding>> &recordTypes()
{
    static std::vector<std::unique_ptr<RecordTypeBinding>> storage;
    return storage;
}
}  // namespace

/**
 * @brief 取（必要时登记）某个类型名对应的宿主记录描述符。
 * @param typeName 类型名，不得为空，首次登记时复制。
 * @param destroy 销毁回调，仅首次登记时使用；为空表示只查找不登记。
 * @param clone 自动复制回调；首次登记后不得更换。
 * @return 地址稳定的描述符；参数无效或未登记时返回空指针。
 * @note 描述符只增不删，对象表会长期持有其地址，因此返回指针始终有效。
 */
const void *chali_abi_record_type(const char *typeName, void (*destroy)(void *), ChaliAbiClone clone);

namespace
{
/**
 * @brief 把桥接持有的描述符指针还原成运行时类型。
 * @param descriptor 桥接返回的描述符指针。
 * @return 运行时记录描述符指针；入参为空时返回空指针。
 */
const NativeRecordType *asRecordType(const void *descriptor)
{
    return static_cast<const NativeRecordType *>(descriptor);
}
}  // namespace

const void *chali_abi_record_type(const char *typeName, void (*destroy)(void *), ChaliAbiClone clone)
try
{
    if (typeName == nullptr || *typeName == '\0') return nullptr;
    static std::mutex mutex;
    const std::lock_guard<std::mutex> guard(mutex);
    for (auto &binding : recordTypes())
        if (binding->name == typeName)
            return destroy == nullptr || (binding->descriptor.destroy == destroy && binding->descriptor.cloneC == clone) ? &binding->descriptor : nullptr;
    if (destroy == nullptr) return nullptr;
    auto binding = std::make_unique<RecordTypeBinding>();
    binding->name = typeName;
    binding->descriptor.name = binding->name.c_str();
    binding->descriptor.destroy = destroy;
    binding->descriptor.cloneC = clone;
    const NativeRecordType *descriptor = &binding->descriptor;
    recordTypes().push_back(std::move(binding));
    return descriptor;
} catch (...) { return nullptr; }

namespace
{
/**
 * @brief 共用文本和原始字节字符串的分配路径，先复制输入再分配运行时存储。
 * @param frame 当前帧。
 * @param data 借用的输入字节。
 * @param length 字节数。
 * @param allowZero 是否允许内容中的零字节。
 * @return 拥有型字符串句柄，失败为零。
 */
int64_t createRuntimeString(const ChaliAbiFrame *frame, const char *data, size_t length, bool allowZero)
{
    auto *context = contextOf(frame);
    if (context == nullptr)
    {
        chali_abi_mark_failure(frame, "missing runtime context");
        return 0;
    }
    if ((length && !data) || (!allowZero && length && std::memchr(data, 0, length)))
    {
        chali_abi_mark_failure(frame, "invalid String bytes");
        return 0;
    }
    Memory::String text;
    if (length != 0u && data != nullptr) text.assign(data, length);
    const auto created = nativeAllocString(*context, text);
    if (!created) chali_abi_mark_failure(frame, "String allocation failed");
    return static_cast<int64_t>(created);
}

/**
 * @brief 取得当前帧可用的宿主上下文壳。
 * @param frame 当前调用帧。
 * @param host 在调用栈中持有宿主壳，不拥有运行时或调用器。
 * @return 上下文壳可用时为 true。
 */
bool prepareHost(const ChaliAbiFrame *frame, std::optional<NativeCallContextImpl> &host)
{
    auto *context = contextOf(frame);
    if (context == nullptr)
    {
        chali_abi_mark_failure(frame, "missing runtime context");
        return false;
    }
    host.emplace(*context, context->invoker);
    return true;
}
}  // namespace

/**
 * @brief 保存首个 C 服务错误；内存不足时也不让异常穿过调用边界。
 * @param frame 运行时句柄所在帧。
 * @param message 错误文本。
 */
void chali_abi_mark_failure(const ChaliAbiFrame *frame, const char *message) noexcept
{
    if (!frame || !frame->runtime) return;
    auto &runtime = *frame->runtime;
    if (runtime.status != ChaliAbiOk) return;
    runtime.status = ChaliAbiHostFailure;
    runtime.explicitError = false;
    runtime.inheritedError = runtime.context && (!runtime.context->memoryFault.empty() || !runtime.context->error.empty());
    runtime.fallback = "host service failed (diagnostic allocation failed)";
    const auto *text = message && *message ? message : "host service failed";
    try
    {
        if (runtime.context && !runtime.context->memoryFault.empty())
            text = runtime.context->memoryFault.c_str();
        else if (runtime.context && !runtime.context->error.empty())
            text = runtime.context->error.c_str();
        runtime.diagnostic.assign(text);
        runtime.fallback = nullptr;
    } catch (...) {}
    try
    {
        if (runtime.context && runtime.context->error.empty()) runtime.context->error = text;
    } catch (...) {}
}

extern "C" {

/**
 * @brief 读取受检查的逻辑范围，拒绝宿主空缓冲。
 * @param frame 当前调用帧。
 * @param address 逻辑地址。
 * @param out 可写宿主缓冲。
 * @param bytes 字节数量。
 * @return 成功为一，失败记录首个错误。
 */
int chali_abi_read(const ChaliAbiFrame *frame, int64_t address, void *out, size_t bytes)
try
{
    if (address >= 0 && !bytes) return 1;
    auto *context = contextOf(frame);
    if (!context || address <= 0 || !out || !nativeRead(*context, static_cast<std::size_t>(address), static_cast<char *>(out), bytes))
    {
        chali_abi_mark_failure(frame, "invalid logical read range");
        return 0;
    }
    return 1;
} catch (...)
{
    chali_abi_mark_failure(frame, "logical read failed");
    return 0;
}

/**
 * @brief 写入受检查的逻辑范围，保留存活和边界验证。
 * @param frame 当前调用帧。
 * @param address 逻辑地址。
 * @param data 宿主字节缓冲。
 * @param bytes 字节数量。
 * @return 成功为一，失败记录首个错误。
 */
int chali_abi_write(const ChaliAbiFrame *frame, int64_t address, const void *data, size_t bytes)
try
{
    if (address >= 0 && !bytes) return 1;
    auto *context = contextOf(frame);
    if (!context || address <= 0 || !data || !nativeWrite(*context, static_cast<std::size_t>(address), static_cast<const char *>(data), bytes))
    {
        chali_abi_mark_failure(frame, "invalid logical write range");
        return 0;
    }
    return 1;
} catch (...)
{
    chali_abi_mark_failure(frame, "logical write failed");
    return 0;
}

/**
 * @brief 成对释放 String 的描述符和外置缓冲。
 * @param frame 当前调用帧。
 * @param address 拥有的 String 地址。
 * @return 成功为一，重复释放或非法地址为零。
 */
int chali_abi_string_release(const ChaliAbiFrame *frame, int64_t address)
try
{
    if (!address) return 1;
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    if (address < 0 || !host->releaseString(address, false))
    {
        chali_abi_mark_failure(frame, "invalid owned String release");
        return 0;
    }
    return 1;
} catch (...)
{
    chali_abi_mark_failure(frame, "String release failed");
    return 0;
}

/**
 * @brief 严格释放 temporary 创建的原始块。
 * @param frame 当前调用帧。
 * @param address 拥有的原始块地址。
 * @return 成功为一，非法或已释放地址为零。
 */
int chali_abi_temporary_release(const ChaliAbiFrame *frame, int64_t address)
try
{
    if (!address) return 1;
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    if (address < 0 || !host->releaseMemory(address, false))
    {
        chali_abi_mark_failure(frame, "invalid temporary release");
        return 0;
    }
    return 1;
} catch (...)
{
    chali_abi_mark_failure(frame, "temporary release failed");
    return 0;
}

/**
 * @brief 解析长度头或容量描述符，借用有效物理字节视图。
 * @param frame 当前调用帧。
 * @param address 逻辑字符串或字节块地址。
 * @param data 非空的视图接收器。
 * @param length 可选的字节数接收器。
 * @return 成功为一，非法存储记录错误并返回零。
 */
int chali_abi_bytes(const ChaliAbiFrame *frame, int64_t address, const unsigned char **data, size_t *length)
try
{
    if (data == nullptr)
    {
        chali_abi_mark_failure(frame, "missing byte view output");
        return 0;
    }
    *data = nullptr;
    if (length != nullptr) *length = 0;
    auto *context = contextOf(frame);
    if (context == nullptr)
    {
        chali_abi_mark_failure(frame, "missing runtime context");
        return 0;
    }
    if (address < 0)
    {
        chali_abi_mark_failure(frame, "invalid byte view address");
        return 0;
    }
    if (address == 0) return 1;
    const auto offset = static_cast<std::size_t>(address);
    if (context->heap != nullptr && context->heap->contains(offset))
    {
        NativeStringStorage storage;
        if (!nativeHeapStringStorage(*context, offset, storage))
        {
            chali_abi_mark_failure(frame, "invalid string storage");
            return 0;
        }
        if (storage.length == 0) return 1;
        auto *heap = dynamic_cast<Heap *>(context->heap);
        if (heap == nullptr)
        {
            chali_abi_mark_failure(frame, "byte view unavailable");
            return 0;
        }
        unsigned char *base = heap->borrowBytes(storage.data, storage.length);
        if (base == nullptr)
        {
            chali_abi_mark_failure(frame, "invalid byte view range");
            return 0;
        }
        *data = base;
        if (length != nullptr) *length = storage.length;
        return 1;
    }
    if (context->data != nullptr && offset < context->data->binary.size() && (!context->heap || context->data->binary.size() <= context->heap->base()))
    {
        const unsigned char *base = context->data->binary.data() + offset;
        const std::size_t limit = std::min(kStringScanLimit, context->data->binary.size() - offset);
        const auto *end = static_cast<const unsigned char *>(std::memchr(base, 0, limit));
        if (!end)
        {
            chali_abi_mark_failure(frame, "unterminated data string");
            return 0;
        }
        const auto total = static_cast<std::size_t>(end - base);
        *data = base;
        if (length != nullptr) *length = total;
        return 1;
    }
    chali_abi_mark_failure(frame, "invalid byte view address");
    return 0;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 复制字节到当前语言堆，创建拥有的容量字符串。
 * @param frame 当前调用帧。
 * @param data 输入字节，非空文本不得为空或含零字节。
 * @param length 字节数量。
 * @return 拥有的逻辑地址，失败为零。
 */
int64_t chali_abi_string(const ChaliAbiFrame *frame, const char *data, size_t length)
try
{
    return createRuntimeString(frame, data, length, false);
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 按显式长度构造原始字符串，保持运行时已有的二进制内容行为。
 * @param frame 当前帧。
 * @param data 原始内容。
 * @param length 字节数。
 * @return 拥有型字符串句柄，失败为零。
 */
int64_t chali_abi_string_bytes(const ChaliAbiFrame *frame, const char *data, size_t length)
try
{
    return createRuntimeString(frame, data, length, true);
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 通过集合原语验证并读取完整字节跨度。
 * @param frame 当前帧。
 * @param handle 字节列表句柄。
 * @param data 目标缓冲区或查询用空指针。
 * @param capacity 目标容量。
 * @param required 接收实际长度。
 * @return 成功为一，失败为零并保留运行时诊断。
 */
int chali_abi_list_bytes(const ChaliAbiFrame *frame, int64_t handle, unsigned char *data,
                         size_t capacity, size_t *required)
try
{
    if (!required || (!data && capacity))
    {
        chali_abi_mark_failure(frame, "invalid byte list output");
        return 0;
    }
    *required = 0;
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    if (!collection_glue::copyListBytes(*host, handle, data, capacity, *required))
    {
        chali_abi_mark_failure(frame, "byte list read failed");
        return 0;
    }
    return 1;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 完成字节列表后才发布句柄，失败前不转移任何输入所有权。
 * @param frame 当前帧。
 * @param data 借用的输入字节。
 * @param length 字节数。
 * @return 独立列表句柄，失败为零。
 */
int64_t chali_abi_list_from_bytes(const ChaliAbiFrame *frame, const unsigned char *data, size_t length)
try
{
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    const auto handle = collection_glue::makeByteList(*host, data, length);
    if (!handle) chali_abi_mark_failure(frame, "byte list allocation failed");
    return handle;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 在最终列表元素中复制文本，省去通用原生值中间封装。
 * @param frame 当前调用帧。
 * @param handle 文本列表句柄。
 * @param data 借用文本，长度为零时可为空。
 * @param length 字节数。
 * @return 成功为一，失败为零并记录错误。
 */
int chali_abi_list_append_text(const ChaliAbiFrame *frame, int64_t handle, const char *data, size_t length)
try
{
    if ((!data && length) || length > uint64_t(INT64_MAX))
    {
        chali_abi_mark_failure(frame, "invalid text list input");
        return 0;
    }
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    if (!collection_glue::listPushText(*host, handle, data, length))
    {
        chali_abi_mark_failure(frame, "text list append failed");
        return 0;
    }
    return 1;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 创建可提前释放的运行时原始块。
 * @param frame 当前调用帧。
 * @param bytes 分配字节数。
 * @return 逻辑块地址，失败为零。
 */
int64_t chali_abi_temporary(const ChaliAbiFrame *frame, size_t bytes)
try
{
    auto *context = contextOf(frame);
    if (context == nullptr)
    {
        chali_abi_mark_failure(frame, "missing runtime context");
        return 0;
    }
    if (bytes > static_cast<uint64_t>(INT64_MAX))
    {
        chali_abi_mark_failure(frame, "temporary size overflow");
        return 0;
    }
    const auto created = nativeAllocate(*context, bytes);
    if (!created) chali_abi_mark_failure(frame, "temporary allocation failed");
    return static_cast<int64_t>(created);
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 把字节发送到当前文本接收器及保留输出桶。
 * @param frame 当前调用帧。
 * @param data 输入缓冲。
 * @param length 字节数量。
 */
void chali_abi_emit(const ChaliAbiFrame *frame, const char *data, size_t length)
try
{
    if (!length) return;
    auto *context = contextOf(frame);
    if (!context || !data)
    {
        chali_abi_mark_failure(frame, "invalid output buffer or runtime");
        return;
    }
    if (context->textSink != nullptr) context->textSink(context->textSinkUser, data, length);
    if (context->text != nullptr && (context->textKeep == nullptr || *context->textKeep))
        context->text->append(data, length);
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return;
}

/**
 * @brief 串行写入并刷新标准错误流，失败保留诊断。
 * @param frame 当前调用帧。
 * @param data 输入缓冲。
 * @param length 字节数量。
 * @param newline 非零时追加换行。
 */
void chali_abi_emit_error(const ChaliAbiFrame *frame, const char *data, size_t length, int newline)
try
{
    if (length && !data)
    {
        chali_abi_mark_failure(frame, "invalid error output buffer");
        return;
    }
    /* 与旧胶水一致：整次写入串行化，避免多线程下标准错误输出交错。 */
    static std::mutex outputMutex;
    const std::lock_guard<std::mutex> guard(outputMutex);
    bool failed = length != 0u && data != nullptr && std::fwrite(data, 1u, length, stderr) != length;
    if (newline != 0 && std::fputc('\n', stderr) == EOF) failed = true;
    if (std::fflush(stderr) != 0) failed = true;
    if (!failed) return;
    chali_abi_mark_failure(frame, "标准错误流写入失败");
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return;
}

/**
 * @brief 保留首条显式模块诊断，细化通用失败且不覆盖已有运行期或内存故障。
 * @param frame 当前调用帧。
 * @param message 模块失败消息。
 */
void chali_abi_fail(const ChaliAbiFrame *frame, const char *message)
{
    if (frame && frame->runtime)
    {
        auto &runtime = *frame->runtime;
        if (runtime.status != ChaliAbiOk && !runtime.explicitError && !runtime.inheritedError && (!runtime.context || runtime.context->memoryFault.empty()))
        {
            if (runtime.context) runtime.context->error.clear();
            runtime.status = ChaliAbiOk;
            runtime.diagnostic.clear();
            runtime.fallback = nullptr;
        }
    }
    chali_abi_mark_failure(frame, message);
    if (frame && frame->runtime) frame->runtime->explicitError = true;
}

/**
 * @brief 借用受检查调用器，同步调用语言函数值或闭包。
 * @param frame 当前调用帧。
 * @param function 函数标签或闭包句柄，不是机器函数指针。
 * @param arguments 参数位模式数组。
 * @param kinds 类别数组，为空时所有参数按整数处理。
 * @param count 参数数量。
 * @param result 非空的返回位模式接收器。
 * @param resultIsFloat 可选的返回类别接收器。
 * @return 正常返回为一，错误或异常为零并清空返回输出。
 */
int chali_abi_invoke(const ChaliAbiFrame *frame, int64_t function, const int64_t *arguments,
                     const unsigned char *kinds, int64_t count, int64_t *result, int *resultIsFloat)
try
{
    auto *context = contextOf(frame);
    if (result) *result = 0;
    if (resultIsFloat) *resultIsFloat = 0;
    if (context == nullptr || result == nullptr || function == 0 || count < 0 || static_cast<uint64_t>(count) > SIZE_MAX / sizeof(NativeValue) || (count && !arguments))
    {
        chali_abi_mark_failure(frame, "invalid callback arguments");
        return 0;
    }
    for (int64_t at = 0; kinds && at < count; ++at)
        if (kinds[at] != ChaliAbiSlotInteger && kinds[at] != ChaliAbiSlotFloat)
        {
            chali_abi_mark_failure(frame, "invalid callback slot kind");
            return 0;
        }
    Memory::Vector<NativeValue> values;
    if (count > 0) values.reserve(static_cast<std::size_t>(count));
    for (int64_t at = 0; at < count; ++at)
    {
        if (kinds != nullptr && kinds[at] != 0u)
        {
            double value = 0.0;
            std::memcpy(&value, &arguments[at], sizeof(value));
            values.push_back(NativeValue::fromFloat(value));
        }
        else
        {
            values.push_back(NativeValue::fromInt(arguments[at]));
        }
    }
    NativeValue returned;
    NativeCallContextImpl host(*context, context->invoker);
    if (!host.invoke(function, values, returned))
    {
        chali_abi_mark_failure(frame, "callback invocation failed");
        return 0;
    }
    if (returned.isText())
    {
        chali_abi_mark_failure(frame, "callback returned an unencoded text value");
        return 0;
    }
    if (resultIsFloat != nullptr) *resultIsFloat = returned.isFloat() ? 1 : 0;
    *result = returned.isFloat() ? chali_abi_slot_from_real(returned.floating) : returned.integer;
    return 1;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 创建不提供自动复制回调的 C 宿主记录。
 * @param frame 当前调用帧。
 * @param typeName 类型身份名称。
 * @param record 成功后转移的记录。
 * @param destroy 非抛出销毁回调。
 * @return 新句柄；失败时不接管记录。
 */
int64_t chali_abi_record_make(const ChaliAbiFrame *frame, const char *typeName, void *record,
                            ChaliAbiDestroy destroy)
{
    return chali_abi_record_make_clonable(frame, typeName, record, destroy, nullptr);
}

/**
 * @brief 把 C 胶水创建的宿主记录交给当前运行时托管。
 * @param frame 当前调用帧。
 * @param typeName 记录类型名，用于诊断与借用校验；首次登记时复制。
 * @param record 记录指针，成功时所有权交给运行时。
 * @param destroy 销毁回调，不得为空。
 * @param clone 可选的独立副本回调，不得抛出异常。
 * @return 新句柄；失败返回零且调用方仍负责销毁记录。
 */
int64_t chali_abi_record_make_clonable(const ChaliAbiFrame *frame, const char *typeName, void *record,
                              ChaliAbiDestroy destroy, ChaliAbiClone clone)
try
{
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    if (!record || !destroy)
    {
        chali_abi_mark_failure(frame, "invalid native record arguments");
        return 0;
    }
    const NativeRecordType *descriptor = asRecordType(chali_abi_record_type(typeName, destroy, clone));
    if (descriptor == nullptr)
    {
        chali_abi_mark_failure(frame, "native record type conflict");
        return 0;
    }
    const auto handle = host->makeNativeRecord(*descriptor, record);
    if (!handle) chali_abi_mark_failure(frame, "native record allocation failed");
    return handle == 0 ? 0 : handle;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 探测存活记录；普通未找到不写入错误通道。
 * @param frame 当前调用帧。
 * @param handle 待检查的记录句柄。
 * @param typeName 类型身份名称。
 * @return 匹配的借用指针，未找到为空。
 */
void *chali_abi_record_probe(const ChaliAbiFrame *frame, int64_t handle, const char *typeName)
try
{
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host) || !handle) return nullptr;
    const auto *descriptor = asRecordType(chali_abi_record_type(typeName, nullptr, nullptr));
    return descriptor ? host->nativeRecord(handle, *descriptor) : nullptr;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return nullptr;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return nullptr;
}

/**
 * @brief 借用类型匹配的存活宿主记录。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @param typeName 创建时使用的类型名。
 * @return 记录指针；句柄失效或类型不匹配返回空指针。
 */
void *chali_abi_record_borrow(const ChaliAbiFrame *frame, int64_t handle, const char *typeName)
try
{
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return nullptr;
    if (!handle)
    {
        chali_abi_mark_failure(frame, "invalid native record handle");
        return nullptr;
    }
    const NativeRecordType *descriptor = asRecordType(chali_abi_record_type(typeName, nullptr, nullptr));
    if (descriptor == nullptr)
    {
        chali_abi_mark_failure(frame, "unknown native record type");
        return nullptr;
    }
    auto *record = host->nativeRecord(handle, *descriptor);
    if (!record) chali_abi_mark_failure(frame, "invalid native record handle or type");
    return record;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return nullptr;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return nullptr;
}

/**
 * @brief 用 C 侧复制回调生成独立记录副本。
 * @param frame 当前调用帧。
 * @param handle 待复制的存活句柄。
 * @param typeName 创建时使用的类型名。
 * @param clone 复制回调，返回新记录指针。
 * @return 新句柄；失败返回零。
 */
int64_t chali_abi_record_clone(const ChaliAbiFrame *frame, int64_t handle, const char *typeName,
                               void *(*clone)(const void *))
try
{
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    if (!handle || !clone)
    {
        chali_abi_mark_failure(frame, "invalid native clone arguments");
        return 0;
    }
    const NativeRecordType *descriptor = asRecordType(chali_abi_record_type(typeName, nullptr, nullptr));
    if (descriptor == nullptr)
    {
        chali_abi_mark_failure(frame, "unknown native record type");
        return 0;
    }
    void *source = host->nativeRecord(handle, *descriptor);
    if (source == nullptr)
    {
        chali_abi_mark_failure(frame, "invalid native record handle or type");
        return 0;
    }
    void *copy = clone(source);
    if (copy == nullptr)
    {
        chali_abi_mark_failure(frame, "native record clone failed");
        return 0;
    }
    std::int64_t created = 0;
    try
    {
        created = host->makeNativeRecord(*descriptor, copy);
    } catch (...)
    {
        try
        {
            if (descriptor->destroy) descriptor->destroy(copy);
        } catch (...) {}
        throw;
    }
    if (created == 0)
    {
        if (descriptor->destroy) descriptor->destroy(copy);
        chali_abi_mark_failure(frame, "native record clone allocation failed");
    }
    return created;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

/**
 * @brief 释放宿主记录及其资源。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @return 接受释放时为 true；句柄无效时返回 false。
 */
int chali_abi_record_release(const ChaliAbiFrame *frame, int64_t handle)
try
{
    std::optional<NativeCallContextImpl> host;
    if (!prepareHost(frame, host)) return 0;
    if (!handle || !host->releaseNativeRecord(handle))
    {
        chali_abi_mark_failure(frame, "invalid native record release");
        return 0;
    }
    return 1;
} catch (const std::exception &error)
{
    chali_abi_mark_failure(frame, error.what());
    return 0;
} catch (...)
{
    chali_abi_mark_failure(frame, "C runtime service failed");
    return 0;
}

}  // extern "C"
