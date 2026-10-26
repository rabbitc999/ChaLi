/**
 * @file std_collection_state.hpp
 * @brief 集合存储私有状态与单值封送边界，供自举存储原语共享。
 */
#pragma once
#include "std_collection.hpp"
#include "std_collection_storage.hpp"
#include <functional>
#include <memory>

namespace collection_glue
{
using NativeValue = stackvm::NativeValue;
using ObjectValue = stackvm::NativeStoredValue;

/**
 * @brief 胶水内部的数值槽，保持浮点位模式与整数句柄分离。
 */
struct Slot
{
    bool isFloat = false;
    std::int64_t integer = 0;
    double real = 0;
    /**
     * @brief 保存整数、地址或对象句柄。
     * @param value 原始整数槽。
     * @return 对应的内部槽。
     */
    static Slot fromInt(std::int64_t value) { return { false, value, 0 }; }
    /**
     * @brief 保存浮点数并保留其数值种类。
     * @param value 双精度数值。
     * @return 浮点槽。
     */
    static Slot fromDouble(double value) { return { true, 0, value }; }
};

/**
 * @brief 借用本次调用的原生实参或内部栈数组，按需读取数值槽。
 * @note 不拥有实参，也不跨越宿主回调的生命周期保存引用。
 */
class Arguments
{
public:
    /**
     * @brief 直接借用统一原生调用协议提供的只读实参。
     * @param values 当前宿主回调期间保持有效的原生值数组。
     */
    explicit Arguments(const Memory::Vector<NativeValue> &values) : native_(values.data()), count_(values.size()) {}
    /**
     * @brief 借用集合内部调用使用的栈上数值槽。
     * @param values 保持到内部调用返回的槽数组。
     * @param count 数组内有效槽的数量。
     */
    Arguments(const Slot *values, std::size_t count) : local_(values), count_(count) {}
    /**
     * @brief 取得已经由宿主注册表检查数量的一个实参。
     * @param index 调用协议规定的实参位置。
     * @return 保留浮点标记和整数位模式的数值槽。
     */
    Slot operator[](std::size_t index) const
    {
        if (!native_) return local_[index];
        const auto &value = native_[index];
        return value.isFloat() ? Slot::fromDouble(value.asFloat()) : Slot::fromInt(value.asInt());
    }
    /**
     * @brief 查询本次调用的实参数量。
     * @return 借用数组内有效实参的数量。
     */
    std::size_t size() const { return count_; }
    /**
     * @brief 判断本次调用是否没有实参。
     * @return 实参数量为零时返回 true。
     */
    bool empty() const { return count_ == 0; }

private:
    const NativeValue *native_ = nullptr;
    const Slot *local_ = nullptr;
    std::size_t count_ = 0;
};

/**
 * @brief 仅供标准集合胶水内部区分记录布局，VM 不解释此枚举。
 */
enum class StorageKind
{
    Vector,
    Set,
    Map,
    IteratorLease
};
struct State;
struct Record;
/**
 * @brief 借用由标准集合胶水维护的静态描述符身份。
 * @param kind 胶水内部的存储类别。
 * @return 对应的不透明记录描述符。
 */
const stackvm::NativeRecordType &descriptor(StorageKind kind);
/**
 * @brief 将元素引用单元中的当前值同步到记录快照。
 * @param ctx 本次调用的公开运行时适配状态。
 * @param object 待同步的记录。
 * @return 所有活跃单元均同步成功时返回 true。
 */
bool nativeRefreshViews(State &ctx, Record &object);

/**
 * @brief 释放记录的拥有值，记录本体由 destroyRecord 销毁。
 * @param host 当前运行时上下文。
 * @param record 待清理的记录。
 */
void disposeRecord(stackvm::NativeCallContext &host, void *record);

/**
 * @brief 多个游标副本共享一次来源租约与稳定元素单元。
 */
struct Lease
{
    std::int64_t source = 0;
    Memory::Map<std::size_t, std::size_t> cells;
};

/**
 * @brief ArrayList、集合及映射独占的宿主数据布局。
 */
struct Record
{
    StorageKind kind = StorageKind::Vector;
    Memory::Vector<ObjectValue> items;
    Entries entries;
    /**
     * @brief 借用元素的稳定内存槽与嵌套借用次数。
     */
    struct ElementView
    {
        std::size_t address = 0;
        std::size_t users = 1;
        bool isText = false;
        bool isFloat = false;
        std::int64_t objectFlag = 0;
    };
    Memory::Map<std::size_t, ElementView> elementViews;
    bool refreshingViews = false;
    std::size_t iteratorLeases = 0;
    bool pendingRelease = false;
    std::shared_ptr<Lease> iteratorLease;
    std::int64_t mapEqual = 0, mapHash = -1;
    bool mapComparing = false;
    Memory::Vector<std::int64_t> languageSlots;  ///< 语言层开放寻址槽，零表示无值，其余是密集槽号加一。
    ObjectKey pendingKey;                        ///< 写入事务在 Hash/Equal 回调前取得的键快照。
    ObjectValue pendingValue;                    ///< 写入事务已复制、尚未提交的拥有值。
    bool pendingWrite = false;                   ///< 挂起值由记录负责失败清理。
};

/**
 * @brief 通过公开运行时接口读写与释放胶水分配的内存。
 */
struct Heap
{
    stackvm::NativeCallContext &host;
    /**
     * @brief 为稳定引用或临时聚合申请受运行时追踪的内存。
     * @param bytes 请求字节数。
     * @return 块首地址，失败返回零。
     */
    std::size_t allocate(std::size_t bytes) { return static_cast<std::size_t>(host.makeTemporary(bytes)); }
    /**
     * @brief 释放拥有的运行时内存，已释放地址可重复清理。
     * @param address 块首地址。
     * @return 释放或跳过成功时返回 true。
     */
    bool release(std::size_t address) { return host.releaseMemory(static_cast<std::int64_t>(address), true); }
    /**
     * @brief 在运行时边界检查后写入原始槽字节。
     * @param address 写入起点。
     * @param bytes 原始数据。
     * @param count 数据长度。
     * @return 完整写入时返回 true。
     */
    bool writeBytes(std::size_t address, const char *bytes, std::size_t count) { return host.writeBytes(static_cast<std::int64_t>(address), bytes, count); }
};

/**
 * @brief 为转换产生的临时值登记最终清理。
 */
struct Temporaries
{
    stackvm::NativeCallContext &host;
    bool object;
    /**
     * @brief 将临时资源加入运行时清理队列。
     * @param handle 对象句柄或内存地址。
     */
    void push_back(std::int64_t handle) { host.trackTemporary(handle, object); }
};

/**
 * @brief 通过通用记录协议维护集合值的递归所有权。
 */
struct Store
{
    stackvm::NativeCallContext &host;
    Temporaries materializedHeap{ host, false }, materializedObjects{ host, true };
    /**
     * @brief 将集合记录交给运行时通用壳管理。
     * @param record 待移交的独立记录。
     * @return 新句柄，失败返回零。
     */
    std::int64_t add(std::shared_ptr<Record> record)
    {
        auto *owned = new Record(std::move(*record));
        const auto handle = host.makeNativeRecord(descriptor(owned->kind), owned);
        if (!handle) delete owned;
        return handle;
    }
    /**
     * @brief 查找任意标准集合胶水记录。
     * @param handle 当前运行时的句柄。
     * @return 匹配的记录；其他对象或失效句柄返回空。
     */
    Record *find(std::int64_t handle)
    {
        for (auto kind : { StorageKind::Vector, StorageKind::Set, StorageKind::Map, StorageKind::IteratorLease })
            if (auto *record = host.nativeRecord(handle, descriptor(kind))) return static_cast<Record *>(record);
        return nullptr;
    }
    /**
     * @brief 复制任意拥有型句柄，具体类型由描述符处理。
     * @param handle 源对象句柄。
     * @param error 可选的本地诊断输出。
     * @return 独立副本句柄，失败返回零并保留运行时诊断。
     */
    std::int64_t copyHandle(std::int64_t handle, Memory::String *error)
    {
        (void)error;
        return host.cloneNativeRecord(handle);
    }
    /**
     * @brief 释放句柄所拥有的记录及嵌套资源。
     * @param handle 待释放对象。
     * @return 接受立即或延期释放时返回 true。
     */
    bool release(std::int64_t handle) { return host.releaseNativeRecord(handle); }
    /**
     * @brief 递归释放槽快照拥有的子对象。
     * @param value 拥有型值快照。
     */
    void releaseValue(const ObjectValue &value)
    {
        for (const auto &field : value.fields) releaseValue(field);
        if (value.ownsObject) release(value.integer);
    }
    /**
     * @brief 深复制槽快照及其嵌套拥有值。
     * @param value 接收独立副本的值。
     * @param error 可选的诊断输出。
     * @return 所有子值复制成功时返回 true。
     */
    bool copyValue(ObjectValue *value, Memory::String *error)
    {
        ObjectValue copy = *value;
        copy.fields.clear();
        copy.ownsObject = false;
        for (auto field : value->fields)
        {
            if (!copyValue(&field, error))
            {
                releaseValue(copy);
                return false;
            }
            copy.fields.push_back(std::move(field));
        }
        if (value->ownsObject)
        {
            copy.integer = copyHandle(value->integer, error);
            if (!copy.integer)
            {
                releaseValue(copy);
                return false;
            }
            copy.ownsObject = true;
        }
        *value = std::move(copy);
        return true;
    }
    /**
     * @brief 释放键快照独占的嵌套对象。
     * @param key 拥有型键快照。
     */
    void releaseKey(const ObjectKey &key)
    {
        for (const auto &field : key.fields) releaseKey(field);
        if (key.ownsObject) release(key.ownedInteger);
    }
    /**
     * @brief 将查询键变为独立拥有的稳定键。
     * @param key 待复制的键及其嵌套字段。
     * @param error 可选的诊断输出。
     * @return 所有对象复制成功时返回 true。
     */
    bool ownKey(ObjectKey *key, Memory::String *error)
    {
        for (auto &field : key->fields)
            if (!ownKey(&field, error)) return false;
        if (!key->isObject || key->ownsObject) return true;
        key->ownedInteger = copyHandle(key->ownedInteger, error);
        key->ownsObject = key->ownedInteger != 0;
        return key->ownsObject;
    }
};

/**
 * @brief 单次集合胶水调用的适配状态，不持有运行时内部地址。
 */
struct State
{
    stackvm::NativeCallContext &host;
    Memory::String error;
    Heap memory{ host };
    Store storage{ host };
    Heap *heap = &memory;
    Store *objects = &storage;
    using Invoker = Memory::Function<bool(std::int64_t, const Memory::Vector<NativeValue> &, NativeValue &)>;
    Invoker invoke;
    Invoker *invoker = &invoke;
    /**
     * @brief 将同步回调接到当前 VM 调用帧。
     * @param context 仅本次胶水调用有效的公开上下文。
     */
    explicit State(stackvm::NativeCallContext &context) : host(context), invoke([&context](std::int64_t function, const Memory::Vector<NativeValue> &arguments, NativeValue &result) { return context.invoke(function, arguments, result); }) {}
};

/**
 * @brief 读取数值槽的整数值。
 * @param value 原始槽。
 * @return 整数结果。
 */
std::int64_t nativeInt(const Slot &value);
/**
 * @brief 创建一个空的集合存储壳。
 * @param ctx 当前宿主上下文。
 * @param kind 存储类别。
 * @return 新句柄。
 */
Slot nativeNewObject(State &ctx, StorageKind kind);
/**
 * @brief 校验存储身份及同步回调的重入权限。
 * @param ctx 当前宿主上下文。
 * @param handle 容器句柄。
 * @param kind 预期类别。
 * @param who 诊断入口名。
 * @return 有效记录，失败时为空。
 */
Record *nativeObject(State &ctx, const Slot &handle, StorageKind kind, const char *who);
/**
 * @brief 将一个有类型槽转换为拥有或借用的原生值快照。
 * @param ctx 当前宿主上下文。
 * @param value 原始数值槽。
 * @param text 是否为字符串。
 * @param layout 对象标记或结构体布局。
 * @param copy 是否取得独立拥有值。
 * @param out 接收快照。
 * @return 转换成功时为 true。
 */
bool nativeValue(State &ctx, const Slot &value, bool text, std::int64_t layout, bool copy, ObjectValue *out);
/**
 * @brief 物化一个可交给语言调用者拥有的值。
 * @param ctx 当前宿主上下文。
 * @param value 存储快照。
 * @param out 接收原始槽。
 * @return 物化成功时为 true。
 */
bool nativeSlot(State &ctx, const ObjectValue &value, Slot *out);
/**
 * @brief 为键协议回调建立短期、只读的键视图。
 * @param ctx 当前宿主上下文。
 * @param key 存储键。
 * @param out 接收回调槽。
 * @param temporary 接收需回收的临时地址。
 * @return 建立成功时为 true。
 */
bool mapCallbackKey(State &ctx, const ObjectKey &key, Slot *out, Memory::Vector<std::size_t> *temporary);

/**
 * @brief 注册自举 Map/Set 使用的单槽与封送边界。
 * @param registry 当前宿主函数注册表。
 * @param error 接收绑定失败的诊断。
 * @return 全部绑定成功时为 true。
 */
bool bindHashedStorage(stackvm::NativeRegistry &registry, Memory::String *error);

/**
 * @brief 复用 Map/Set 的创建、长度及按槽读取边界，供 AOT 标量桥接。
 * @param state 当前封送及所有权上下文。
 * @param operation 元数据原语编号，十二至二十。
 * @param arguments 原语实参。
 * @return 对应的原始结果槽。
 */
Slot mapStorageMetadata(State &state, std::int64_t operation, const Arguments &arguments);

}  // namespace collection_glue
