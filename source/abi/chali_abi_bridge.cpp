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

#include "ircode/runtime_internal.hpp"
#include "ircode/native_memory_access.hpp"
#include "ircode/native_memory.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
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
 * @param typeName 静态类型名，不得为空。
 * @param destroy 销毁回调，仅首次登记时使用；为空表示只查找不登记。
 * @return 地址稳定的描述符；参数无效或未登记时返回空指针。
 * @note 描述符只增不删，对象表会长期持有其地址，因此返回指针始终有效。
 */
const void *chali_abi_record_type(const char *typeName, void (*destroy)(void *));

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

const void *chali_abi_record_type(const char *typeName, void (*destroy)(void *))
{
    if (typeName == nullptr || *typeName == '\0') return nullptr;
    for (auto &binding : recordTypes())
        if (binding->name == typeName) return &binding->descriptor;
    if (destroy == nullptr) return nullptr;
    auto binding = std::make_unique<RecordTypeBinding>();
    binding->name = typeName;
    binding->descriptor.name = typeName;
    binding->descriptor.destroy = destroy;
    const NativeRecordType *descriptor = &binding->descriptor;
    recordTypes().push_back(std::move(binding));
    return descriptor;
}

namespace
{
/**
 * @brief 取得当前帧可用的宿主上下文壳。
 * @param frame 当前调用帧。
 * @param host 接收构造结果。
 * @return 上下文壳可用时为 true。
 */
bool prepareHost(const ChaliAbiFrame *frame, std::unique_ptr<NativeCallContextImpl> &host)
{
    auto *context = contextOf(frame);
    if (context == nullptr) return false;
    host = std::make_unique<NativeCallContextImpl>(
        *context, context->invoker != nullptr ? *context->invoker : NativeInvoker{});
    return true;
}
}  // namespace

extern "C" {

int chali_abi_bytes(const ChaliAbiFrame *frame, int64_t address, const unsigned char **data, size_t *length)
{
    if (data == nullptr) return 0;
    *data = nullptr;
    if (length != nullptr) *length = 0;
    auto *context = contextOf(frame);
    if (context == nullptr) return 0;
    if (address <= 0) return 1;
    const auto offset = static_cast<std::size_t>(address);
    if (context->heap != nullptr && context->heap->contains(offset))
    {
        std::int64_t stored = 0;
        if (!nativeRead(*context, offset, reinterpret_cast<char *>(&stored), sizeof(stored)) || stored < 0) return 0;
        std::size_t capacity = 0;
        if (!context->heap->capacityAt(offset, capacity)) return 0;
        const auto header = capacity < sizeof(stored) ? capacity : sizeof(stored);
        const auto total = static_cast<std::size_t>(stored);
        if (total > capacity - header) return 0;
        if (total == 0) return 1;
        auto *heap = dynamic_cast<Heap *>(context->heap);
        if (heap == nullptr) return 0;
        unsigned char *base = heap->borrowBytes(offset + sizeof(stored), total);
        if (base == nullptr) return 0;
        *data = base;
        if (length != nullptr) *length = total;
        return 1;
    }
    if (context->data != nullptr && offset < context->data->size())
    {
        const unsigned char *base = context->data->binary.data() + offset;
        std::size_t total = 0;
        const std::size_t limit = context->data->size() - offset;
        while (total < limit && base[total] != 0u) ++total;
        *data = base;
        if (length != nullptr) *length = total;
        return 1;
    }
    return 0;
}

int64_t chali_abi_string(const ChaliAbiFrame *frame, const char *data, size_t length)
{
    auto *context = contextOf(frame);
    if (context == nullptr) return 0;
    Memory::String text;
    if (length != 0u && data != nullptr) text.assign(data, length);
    return static_cast<int64_t>(nativeAllocString(*context, text));
}

int64_t chali_abi_temporary(const ChaliAbiFrame *frame, size_t bytes)
{
    auto *context = contextOf(frame);
    if (context == nullptr) return 0;
    return static_cast<int64_t>(nativeAllocate(*context, bytes));
}

void chali_abi_emit(const ChaliAbiFrame *frame, const char *data, size_t length)
{
    auto *context = contextOf(frame);
    if (context == nullptr || data == nullptr || length == 0u) return;
    if (context->textSink != nullptr) context->textSink(context->textSinkUser, data, length);
    if (context->text != nullptr && (context->textKeep == nullptr || *context->textKeep))
        context->text->append(data, length);
}

void chali_abi_emit_error(const ChaliAbiFrame *frame, const char *data, size_t length, int newline)
{
    /* 与旧胶水一致：整次写入串行化，避免多线程下标准错误输出交错。 */
    static std::mutex outputMutex;
    const std::lock_guard<std::mutex> guard(outputMutex);
    bool failed = length != 0u && data != nullptr && std::fwrite(data, 1u, length, stderr) != length;
    if (newline != 0 && std::fputc('\n', stderr) == EOF) failed = true;
    if (std::fflush(stderr) != 0) failed = true;
    if (!failed) return;
    auto *context = contextOf(frame);
    if (context != nullptr && context->error.empty()) context->error = "标准错误流写入失败";
}

void chali_abi_fail(const ChaliAbiFrame *frame, const char *message)
{
    auto *context = contextOf(frame);
    if (context == nullptr) return;
    if (context->error.empty()) context->error = message != nullptr ? message : "";
}

int chali_abi_invoke(const ChaliAbiFrame *frame, int64_t function, const int64_t *arguments,
                     const unsigned char *kinds, int64_t count, int64_t *result, int *resultIsFloat)
{
    auto *context = contextOf(frame);
    if (context == nullptr || result == nullptr) return 0;
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
    NativeCallContextImpl host(*context, context->invoker != nullptr ? *context->invoker : NativeInvoker{});
    if (!host.invoke(function, values, returned)) return 0;
    if (resultIsFloat != nullptr) *resultIsFloat = returned.isFloat() ? 1 : 0;
    *result = returned.isFloat() ? chali_abi_slot_from_real(returned.floating) : returned.integer;
    return 1;
}

/**
 * @brief 把 C 胶水创建的宿主记录交给当前运行时托管。
 * @param frame 当前调用帧。
 * @param typeName 记录类型名，用于诊断与借用校验；须为静态字符串。
 * @param record 记录指针，成功时所有权交给运行时。
 * @param destroy 销毁回调，不得为空。
 * @return 新句柄；失败返回零且调用方仍负责销毁记录。
 */
int64_t chali_abi_record_make(const ChaliAbiFrame *frame, const char *typeName, void *record,
                              void (*destroy)(void *))
{
    std::unique_ptr<NativeCallContextImpl> host;
    if (!prepareHost(frame, host) || record == nullptr || destroy == nullptr) return 0;
    const NativeRecordType *descriptor = asRecordType(chali_abi_record_type(typeName, destroy));
    if (descriptor == nullptr) return 0;
    const auto handle = host->makeNativeRecord(*descriptor, record);
    return handle == 0 ? 0 : handle;
}

/**
 * @brief 借用类型匹配的存活宿主记录。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @param typeName 创建时使用的类型名。
 * @return 记录指针；句柄失效或类型不匹配返回空指针。
 */
void *chali_abi_record_borrow(const ChaliAbiFrame *frame, int64_t handle, const char *typeName)
{
    std::unique_ptr<NativeCallContextImpl> host;
    if (!prepareHost(frame, host) || handle == 0) return nullptr;
    const NativeRecordType *descriptor = asRecordType(chali_abi_record_type(typeName, nullptr));
    if (descriptor == nullptr) return nullptr;
    return host->nativeRecord(handle, *descriptor);
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
{
    std::unique_ptr<NativeCallContextImpl> host;
    if (!prepareHost(frame, host) || handle == 0 || clone == nullptr) return 0;
    const NativeRecordType *descriptor = asRecordType(chali_abi_record_type(typeName, nullptr));
    if (descriptor == nullptr) return 0;
    void *source = host->nativeRecord(handle, *descriptor);
    if (source == nullptr) return 0;
    void *copy = clone(source);
    if (copy == nullptr) return 0;
    const auto created = host->makeNativeRecord(*descriptor, copy);
    if (created == 0 && descriptor->destroy != nullptr) descriptor->destroy(copy);
    return created;
}

/**
 * @brief 释放宿主记录及其资源。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @return 接受释放时为 true；句柄无效时返回 false。
 */
int chali_abi_record_release(const ChaliAbiFrame *frame, int64_t handle)
{
    std::unique_ptr<NativeCallContextImpl> host;
    if (!prepareHost(frame, host) || handle == 0) return 0;
    return host->releaseNativeRecord(handle) ? 1 : 0;
}

}  // extern "C"
