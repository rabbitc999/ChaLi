/**
 * @file value_transfer.cpp
 * @brief 跨线程值树快照、验证与恢复。
 */
#include "runtime_internal.hpp"

namespace runtime
{
using namespace runtimeDetail;
namespace runtimeDetail
{
bool captureTransferObject(NativeContext &ctx, std::int64_t handle, TransferPacket &packet,
                           Memory::Set<std::int64_t> &active);

/**
 * @brief 捕获可跨运行时传递的对象值。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param value 待转换、保存或参与计算的值。
 * @param packet 待处理的 JSON-RPC 消息对象。
 * @param active 当前正在处理的节点集合。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool captureTransferValue(NativeContext &ctx, const ObjectValue &value,
                          TransferPacket &packet, Memory::Set<std::int64_t> &active)
{
    for (const ObjectValue &field : value.fields)
        if (!captureTransferValue(ctx, field, packet, active)) return false;
    return value.objectIdentity == 0 || captureTransferObject(ctx, value.integer, packet, active);
}

/**
 * @brief 捕获运行时对象的可传输快照。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param handle 待操作的运行时句柄。
 * @param packet 待处理的 JSON-RPC 消息对象。
 * @param active 当前正在处理的节点集合。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool captureTransferObject(NativeContext &ctx, std::int64_t handle, TransferPacket &packet,
                           Memory::Set<std::int64_t> &active)
{
    if (active.count(handle) != 0u)
    {
        ctx.error = "消息中的容器对象形成循环；请先拆开循环引用";
        return false;
    }
    if (packet.objects.count(handle) != 0u) return true;
    RuntimeObject *source = ctx.objects == nullptr ? nullptr : ctx.objects->find(handle);
    if (source == nullptr)
    {
        ctx.error = "消息包含已释放的对象句柄：" + Memory::toString(handle);
        return false;
    }
    if (source->kind == ObjectKind::Obj || (source->kind == ObjectKind::NativeRecord && (!source->nativeType || (!source->nativeType->clone && !source->nativeType->cloneC))))
    {
        ctx.error = Memory::String("消息不能复制运行期资源或 std::core::Any：") + objectKindName(source->kind);
        return false;
    }
    active.insert(handle);
    if (source->kind == ObjectKind::NativeRecord)
    {
        NativeCallContextImpl host(ctx);
        if (source->nativeType->prepare && !source->nativeType->prepare(host, source->nativeRecord.get())) return false;
        RuntimeObject copy;
        copy.kind = ObjectKind::NativeRecord;
        copy.nativeType = source->nativeType;
        void *record = source->nativeType->clone ? source->nativeType->clone(source->nativeRecord.get(), ctx.error) :
            source->nativeType->cloneC(source->nativeRecord.get());
        if (!record || record == source->nativeRecord.get())
        {
            if (ctx.error.empty()) ctx.error = "宿主复制回调没有返回独立记录";
            return false;
        }
        copy.nativeRecord = std::shared_ptr<void>(record, source->nativeType->destroy);
        packet.objects.emplace(handle, std::move(copy));
    }
    else
        packet.objects.emplace(handle, *source);
    for (const ObjectValue &item : source->items)
        if (!captureTransferValue(ctx, item, packet, active)) return false;
    if (source->kind == ObjectKind::NativeRecord && source->nativeType->visit)
        if (!source->nativeType->visit(packet.objects[handle].nativeRecord.get(), [&](NativeStoredValue &value) { return captureTransferValue(ctx, value, packet, active); })) return false;
    active.erase(handle);
    return true;
}

/**
 * @brief 将原生参数捕获为可传输的值。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param slot 待转换或传递的运行时值槽。
 * @param kind 待处理对象的类别。
 * @param layout 传输值的布局描述。
 * @param type 类型描述。
 * @param out 接收处理结果的输出对象。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool captureTransfer(NativeContext &ctx, const Slot &slot, int kind,
                     std::int64_t layout, const Memory::String &type, TransferPacket *out)
{
    if (kind == 0 && type.find("fn(") != Memory::String::npos && isObjectHandle(slot.integer))
    {
        ctx.error = "跨线程传值不支持带捕获的函数值";
        return false;
    }
    out->type = type;
    const std::int64_t objectFlag = kind == 4 ? layout : kind == 3 ? 1 :
                                                                     0;
    if (!nativeValue(ctx, slot, kind == 2, objectFlag, false, &out->value)) return false;
    Memory::Set<std::int64_t> active;
    return captureTransferValue(ctx, out->value, *out, active);
}

bool restoreTransferObject(NativeContext &ctx, const TransferPacket &packet,
                           std::int64_t oldHandle, Memory::Set<std::int64_t> &active,
                           Memory::Vector<std::int64_t> &created, std::int64_t *newHandle);

/**
 * @brief 在目标运行时恢复传输的对象值。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param packet 待处理的 JSON-RPC 消息对象。
 * @param value 待转换、保存或参与计算的值。
 * @param active 当前正在处理的节点集合。
 * @param created 已恢复对象的句柄映射，避免重复恢复。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool restoreTransferValue(NativeContext &ctx, const TransferPacket &packet,
                          ObjectValue *value, Memory::Set<std::int64_t> &active,
                          Memory::Vector<std::int64_t> &created)
{
    for (ObjectValue &field : value->fields)
        if (!restoreTransferValue(ctx, packet, &field, active, created)) return false;
    if (value->objectIdentity == 0) return true;
    std::int64_t handle = 0;
    if (!restoreTransferObject(ctx, packet, value->integer, active, created, &handle)) return false;
    value->integer = handle;
    value->objectIdentity = ctx.objects->find(handle)->identity;
    value->ownsObject = true;
    return true;
}

/**
 * @brief 在目标运行时恢复传输的对象快照。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param packet 待处理的 JSON-RPC 消息对象。
 * @param oldHandle 原运行时中的对象句柄。
 * @param active 当前正在处理的节点集合。
 * @param created 已恢复对象的句柄映射，避免重复恢复。
 * @param newHandle 目标运行时中恢复的对象句柄。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool restoreTransferObject(NativeContext &ctx, const TransferPacket &packet,
                           std::int64_t oldHandle, Memory::Set<std::int64_t> &active,
                           Memory::Vector<std::int64_t> &created, std::int64_t *newHandle)
{
    const auto found = packet.objects.find(oldHandle);
    if (found == packet.objects.end() || active.count(oldHandle) != 0u)
    {
        ctx.error = "消息对象快照无效或包含循环";
        return false;
    }
    active.insert(oldHandle);
    auto object = Memory::makeShared<RuntimeObject>(found->second);
    if (object->kind == ObjectKind::NativeRecord)
    {
        void *record = object->nativeType->clone ? object->nativeType->clone(object->nativeRecord.get(), ctx.error) :
            (object->nativeType->cloneC ? object->nativeType->cloneC(object->nativeRecord.get()) : nullptr);
        if (!record || record == object->nativeRecord.get())
        {
            if (ctx.error.empty()) ctx.error = "宿主复制回调没有返回独立记录";
            return false;
        }
        object->nativeRecord = std::shared_ptr<void>(record, object->nativeType->destroy);
        ctx.objects->recordContext.heap = ctx.heap;
        ctx.objects->recordContext.data = ctx.data;
        ctx.objects->recordContext.objects = ctx.objects;
    }
    object->identity = 0;
    for (ObjectValue &item : object->items)
        if (!restoreTransferValue(ctx, packet, &item, active, created)) return false;
    if (object->kind == ObjectKind::NativeRecord && object->nativeType->visit)
        if (!object->nativeType->visit(object->nativeRecord.get(), [&](NativeStoredValue &value) { return restoreTransferValue(ctx, packet, &value, active, created); })) return false;
    *newHandle = ctx.objects->add(std::move(object));
    created.push_back(*newHandle);
    active.erase(oldHandle);
    return true;
}

/**
 * @brief 将传输值恢复为目标运行时的原生值。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param packet 待处理的 JSON-RPC 消息对象。
 * @param out 接收处理结果的输出对象。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool restoreTransfer(NativeContext &ctx, const TransferPacket &packet, Slot *out)
{
    ObjectValue value = packet.value;
    Memory::Set<std::int64_t> active;
    Memory::Vector<std::int64_t> created;
    if (!restoreTransferValue(ctx, packet, &value, active, created))
    {
        for (auto it = created.rbegin(); it != created.rend(); ++it) ctx.objects->release(*it);
        return false;
    }
    if (value.objectIdentity != 0 && !value.isStruct)
    {
        *out = Slot::fromInt(value.integer);
        return true;
    }
    const bool okay = nativeSlot(ctx, value, out);
    ctx.objects->releaseValue(value);
    return okay;
}
namespace
{
/** @brief 同一对象表中的壳独立，跨线程只共享不可变的载荷和标准引用计数。 */
struct ArcRecord
{
    std::shared_ptr<const TransferPacket> strong;
    std::weak_ptr<const TransferPacket> weak;
};

/**
 * @brief 复制一个强或弱句柄，保持相同的引用计数身份。
 * @param value 来源记录。
 * @param error 接收分配失败诊断。
 * @return 独立记录，分配失败时为空。
 */
void *cloneArcRecord(const void *value, Memory::String &error)
{
    auto *copy = new (std::nothrow) ArcRecord(*static_cast<const ArcRecord *>(value));
    if (!copy) error = "Arc 句柄分配失败";
    return copy;
}

/** @brief 释放记录持有的一份强引用或弱引用。 */
void destroyArcRecord(void *value) { delete static_cast<ArcRecord *>(value); }

const NativeRecordType arcRecordType{ "Arc", cloneArcRecord, destroyArcRecord };

/**
 * @brief 将一份强引用或弱引用登记为当前线程的独立对象句柄。
 * @param ctx 当前线程的对象表。
 * @param record 待转移的引用。
 * @return 新对象句柄，失败时记录诊断。
 */
Slot makeArcRecord(NativeContext &ctx, ArcRecord record)
{
    auto owned = std::unique_ptr<ArcRecord>(new (std::nothrow) ArcRecord(std::move(record)));
    if (!owned)
    {
        ctx.error = "Arc 句柄分配失败";
        return Slot::fromInt(0);
    }
    NativeCallContextImpl host(ctx);
    const auto handle = host.makeNativeRecord(arcRecordType, owned.get());
    if (handle) owned.release();
    return Slot::fromInt(handle);
}

/**
 * @brief 验证本线程中的 Arc 句柄，允许零表示空。
 * @param ctx 当前线程的对象表。
 * @param handle 待查询的对象句柄。
 * @return 匹配的记录，空或失效时返回空指针。
 */
const ArcRecord *findArcRecord(NativeContext &ctx, std::int64_t handle)
{
    if (!handle) return nullptr;
    NativeCallContextImpl host(ctx);
    const auto *record = static_cast<const ArcRecord *>(host.nativeRecord(handle, arcRecordType));
    if (!record) ctx.error = "Arc 句柄无效";
    return record;
}
}  // namespace

/** @brief 构造空句柄，零值无需分配。 */
Slot arcEmpty(NativeContext &, const Memory::Vector<Slot> &) { return Slot::fromInt(0); }

/** @brief 复制当前句柄，空句柄仍为空。 */
Slot arcCopy(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    return nativeInt(args[0]) == 0 ? Slot::fromInt(0) : objectCopy(ctx, args);
}

/** @brief 以线程传值协议捕获不可变快照并返回第一份强引用。 */
Slot arcCreate(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    Memory::String type;
    if (!nativeReadCString(ctx, static_cast<std::size_t>(nativeInt(args[2])), &type))
        return Slot::fromInt(0);
    auto packet = Memory::makeShared<TransferPacket>();
    if (!captureTransfer(ctx, args[3], static_cast<int>(nativeInt(args[0])), nativeInt(args[1]), type, packet.get()))
        return Slot::fromInt(0);
    return makeArcRecord(ctx, { std::move(packet), {} });
}

/** @brief 在调用线程中恢复强引用载荷的独立副本，并核对类型身份。 */
Slot arcGet(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const auto *record = findArcRecord(ctx, nativeInt(args[0]));
    if (!record || !record->strong)
    {
        if (ctx.error.empty()) ctx.error = "Arc 为空";
        return Slot::fromInt(0);
    }
    // 持有独立引用，恢复宿主记录时即使发生重入，载荷仍然存活。
    const auto packet = record->strong;
    Memory::String type;
    if (!nativeReadCString(ctx, static_cast<std::size_t>(nativeInt(args[3])), &type))
        return Slot::fromInt(0);
    if (type != packet->type)
    {
        ctx.error = "Arc 载荷类型不匹配：" + type;
        return Slot::fromInt(0);
    }
    Slot value;
    return restoreTransfer(ctx, *packet, &value) ? value : Slot::fromInt(0);
}

/** @brief 创建与强引用共享控制块的弱句柄。 */
Slot arcDowngrade(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const auto *record = findArcRecord(ctx, nativeInt(args[0]));
    return record && record->strong ? makeArcRecord(ctx, { {}, record->strong }) : Slot::fromInt(0);
}

/** @brief 在载荷仍然存活时原子取得一份强引用。 */
Slot arcUpgrade(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const auto *record = findArcRecord(ctx, nativeInt(args[0]));
    auto strong = record ? record->weak.lock() : nullptr;
    return strong ? makeArcRecord(ctx, { std::move(strong), {} }) : Slot::fromInt(0);
}

/** @brief 读取弱引用当前是否失效。 */
Slot arcExpired(NativeContext &ctx, const Memory::Vector<Slot> &args)
{
    const auto *record = findArcRecord(ctx, nativeInt(args[0]));
    return Slot::fromInt(!record || record->weak.expired());
}

}  // namespace runtimeDetail
}  // namespace runtime
