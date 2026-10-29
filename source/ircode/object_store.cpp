/**
 * @file object_store.cpp
 * @brief 运行时对象句柄、代次与资源清理。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
using namespace runtimeDetail;
namespace runtimeDetail
{
/**
 * @brief 判断包装是否只有非拥有指针和描述元数据，不含需要复制或销毁的状态。
 * @param object 待检查的运行时对象。
 * @return 可以共享不可变包装快照时返回 true。
 */
static bool shareablePointerSnapshot(const RuntimeObject &object)
{
    return object.kind == ObjectKind::Obj && !object.objOwnsObject
           && !object.objValue.ownsObject && !object.objValue.isStruct
           && !object.objValue.isText && !object.objValue.isFloat
           && object.objValue.fields.empty() && !object.objTextShape.empty()
           && object.objTextShape.front() == 'R' && object.objTextCallbacks.empty()
           && !object.iteratorStateBox && !object.closureCopy && !object.closureEnvironment
           && object.items.empty() && object.elementViews.empty() && object.captureDestructors.empty()
           && !object.nativeRecord && !object.rawStorage;
}

/** @brief 读取指针包装共享开关，默认保留独立句柄并共享不可变载荷。 */
ObjectStore::ObjectStore()
{
    const auto *disabled = std::getenv("CHALI_NO_INTERFACE_COPY_COW");
    pointerSnapshots = !(disabled && *disabled);
    const auto *disabledSlots = std::getenv("CHALI_NO_OBJECT_INLINE_SLOTS");
    inlineHandles = !(disabledSlots && *disabledSlots);
}

/**
 * @brief 修改非拥有指针包装前创建私有载荷，保持其他句柄的快照不变。
 * @param handle 已通过类别与存活检查的对象句柄。
 * @return 当前可修改对象；句柄不存在时返回空。
 */
RuntimeObject *ObjectStore::detachPointerSnapshot(std::int64_t handle)
{
    auto *storage = handleStorage(handle);
    if (storage == nullptr) return nullptr;
    if (!storage->unique() && shareablePointerSnapshot(**storage))
        *storage = Memory::makeShared<RuntimeObject>(**storage);
    return storage->get();
}

/**
 * 对象句柄的起点（2^62）：编码堆地址与数据区地址均低于此边界。
 */

/**
 * @brief 种类名（诊断文案里用）。
 * @param kind 待处理对象的类别。
 * @retval 对应对象的指针。
 */
[[nodiscard]] const char *objectKindName(ObjectKind kind)
{
    switch (kind)
    {
    case ObjectKind::NativeRecord: return "native record";
    case ObjectKind::Closure: return "lambda";
    case ObjectKind::Obj: return "obj";
    case ObjectKind::RawStorage: return "raw storage";
    }
    return "?";
}

bool nativeRefreshViews(NativeContext &ctx, RuntimeObject &object);
bool nativeWriteView(NativeContext &ctx, const RuntimeObject::ElementView &view, const ObjectValue &value);

void nativeReleaseViewContents(NativeContext &ctx, const RuntimeObject::ElementView &view);  ///< 还活着的对象
/**
 * @brief 新分配或重新赋值的存储开始生命周期。
 * @param address 地址。
 * @param bytes 字节数。
 * @param fresh 是否为新分配块，需清除所有重叠的旧标记。
 */
void ObjectStore::beginStorage(std::size_t address, std::size_t bytes, bool fresh)

{
    if ((!bytes && !fresh) || movedStorage.empty()) return;
    // 含于新生命周期的标记从本地址开始；重叠查询只需额外考虑最大跨度以内的前驱。
    const auto first = fresh && largestMovedStorage < address ? address - largestMovedStorage : fresh ? 0u : address;
    auto it = movedStorage.lower_bound({ first, std::numeric_limits<std::int64_t>::min() });
    for (; it != movedStorage.end();)
    {
        const auto start = it->first.first;
        if (start >= address && start - address >= bytes) break;
        const bool overlap = start >= address ? start - address < bytes : address - start < it->second;
        const bool contained = start >= address && start - address < bytes && it->second <= bytes - (start - address);
        if (fresh ? overlap : contained)
            it = movedStorage.erase(it);
        else
            ++it;
    }
}

/**
 * @brief 释放对象存储中仍持有的全部资源。
 */
ObjectStore::~ObjectStore()

{
    invalidateNativeRecordViews();
    shuttingDown = true;
    // 固定槽只接收 Obj，宿主记录的 dispose 顺序继续使用原有按句柄排序的表。
    for (const auto &entry : objects)
        if (entry.second->kind == ObjectKind::NativeRecord) releaseContents(*entry.second);
    objects.clear();
    for (auto &slot : handleSlots) slot.object.reset();
}

/** @brief 递增宿主记录代次，耗尽后不重新启用旧视图。 */
void ObjectStore::invalidateNativeRecordViews()
{
    if (nativeRecordRevision) ++nativeRecordRevision;
}

/**
 * @brief 收下一个对象，返回它的句柄。
 * @param object 待访问的对象。
 * @retval 收下一个对象，返回它的句柄。
 */
std::int64_t ObjectStore::add(std::shared_ptr<RuntimeObject> object)

{
    const std::int64_t handle = next++;
    if (object->identity == 0) object->identity = handle;
    auto &slot = handleSlots[static_cast<std::uint64_t>(handle) % handleSlots.size()];
    if (inlineHandles && object->kind == ObjectKind::Obj && !slot.object)
    {
        slot.handle = handle;
        slot.object = std::move(object);
        ++inlineHandleCount;
    }
    else
        objects[handle] = std::move(object);
    return handle;
}

/**
 * @brief 根据名称或键查找匹配项。
 *
 * 按句柄找对象；已经释放 / 不认识返回 nullptr。
 * @param handle 待操作的运行时句柄。
 * @retval 匹配项的指针，未找到时为空指针。
 */
RuntimeObject *ObjectStore::find(std::int64_t handle)

{
    const auto *storage = handleStorage(handle);
    return storage == nullptr ? nullptr : storage->get();
}

/**
 * @brief 比较固定槽中的完整身份，未命中时查询原有稳定节点表。
 * @param handle 待查找的完整对象句柄。
 * @return 活跃共享指针槽，或不存在时的空指针。
 */
std::shared_ptr<RuntimeObject> *ObjectStore::handleStorage(std::int64_t handle)
{
    auto &slot = handleSlots[static_cast<std::uint64_t>(handle) % handleSlots.size()];
    if (slot.object && slot.handle == handle) return &slot.object;
    const auto found = objects.find(handle);
    return found == objects.end() ? nullptr : &found->second;
}

/**
 * @brief 复制对象值并维护其引用或所有权。
 * @param value 待转换、保存或参与计算的值。
 * @param error 接收失败原因的错误信息。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool ObjectStore::copyValue(ObjectValue *value, Memory::String *error)

{
    if (value == nullptr) return true;
    for (ObjectValue &field : value->fields)
        if (!copyValue(&field, error)) return false;
    if (!value->ownsObject) return true;
    const std::int64_t nestedCopy = copyHandle(value->integer, error);
    if (nestedCopy == 0) return false;
    value->integer = nestedCopy;
    return true;
}

/**
 * @brief 释放对象值持有的资源。
 * @param value 待转换、保存或参与计算的值。
 */
void ObjectStore::releaseValue(const ObjectValue &value)

{
    for (const ObjectValue &field : value.fields) releaseValue(field);
    if (value.ownsObject) release(value.integer);
}

/**
 * @brief 释放通用对象拥有的槽值与宿主记录资源。
 * @param object 待访问的对象。
 */
void ObjectStore::releaseContents(const RuntimeObject &object)

{
    invalidateNativeRecordViews();
    if (object.kind == ObjectKind::NativeRecord && object.nativeType && object.nativeType->dispose)
    {
        NativeCallContextImpl host(recordContext);
        object.nativeType->dispose(host, object.nativeRecord.get());
    }
    if (object.iteratorStateBox || object.closureCopy)
    {
        auto context = object.iteratorContext;
        for (const auto &entry : object.elementViews)
        {
            const auto destructor = object.captureDestructors.find(entry.first);
            if (destructor != object.captureDestructors.end() && context.invoker)
            {
                NativeValue result;
                (*context.invoker)(destructor->second, { NativeValue::fromInt(entry.second.address) }, result);
            }
            nativeReleaseViewContents(context, entry.second);
            context.heap->release(entry.second.address);
        }
    }
    for (const ObjectValue &value : object.items) releaseValue(value);
    if (object.kind == ObjectKind::Obj && object.objOwnsObject)
    {
        release(object.objValue.integer);
    }
}

/**
 * @brief 复制运行时对象并调用宿主记录的复制协议。
 * @param object 待访问的对象。
 * @param error 接收失败原因的错误信息。
 * @retval 对应对象的指针。
 */
std::shared_ptr<RuntimeObject> ObjectStore::copyObject(const RuntimeObject &object, Memory::String *error)

{
    if (object.kind == ObjectKind::NativeRecord)
    {
        const auto *type = object.nativeType;
        const auto original = object.nativeRecord;
        const auto identity = object.identity;
        if (!type || (!type->clone && !type->copy))
        {
            if (error) *error = Memory::String(type && type->name ? type->name : "宿主记录") + " 不能复制";
            return nullptr;
        }
        Memory::String reason;
        recordContext.error.clear();
        recordContext.memoryFault.clear();
        NativeCallContextImpl host(recordContext);
        if (type->prepare && !type->prepare(host, original.get()))
        {
            if (error) *error = recordContext.memoryFault.empty() ? recordContext.error : recordContext.memoryFault;
            return nullptr;
        }
        void *record = type->copy ? type->copy(host, original.get()) : type->clone(original.get(), reason);
        if (reason.empty()) reason = recordContext.memoryFault.empty() ? recordContext.error : recordContext.memoryFault;
        if (!record || record == original.get())
        {
            if (error) *error = reason.empty() ? "宿主复制回调没有返回独立记录" : reason;
            return nullptr;
        }
        auto copy = Memory::makeShared<RuntimeObject>();
        copy->kind = ObjectKind::NativeRecord;
        copy->nativeType = type;
        copy->nativeRecord = std::shared_ptr<void>(record, type->destroy);
        copy->identity = identity;
        return copy;
    }
    if (object.kind == ObjectKind::RawStorage)
    {
        if (error != nullptr)
        {
            *error = Memory::String(objectKindName(object.kind)) + " 句柄不能复制；请把它当作资源句柄传递并显式 close/delete";
        }
        return nullptr;
    }
    auto copy = Memory::makeShared<RuntimeObject>(object);
    copy->elementViews.clear();
    copy->refreshingViews = false;
    for (ObjectValue &value : copy->items)
    {
        if (!copyValue(&value, error)) return nullptr;
    }
    if (copy->kind == ObjectKind::Obj && copy->objOwnsObject)
    {
        if (!copyValue(&copy->objValue, error)) return nullptr;
    }
    if (object.closureCopy)
    {
        for (const auto &entry : object.elementViews)
        {
            auto view = entry.second;
            view.address = nativeAllocate(copy->iteratorContext, std::max<std::size_t>(8,
                                                                                       copy->items[entry.first].isStruct ? copy->items[entry.first].fields.size() * 8 : 8));
            if (!view.address || !nativeWriteView(copy->iteratorContext, view, copy->items[entry.first]))
            {
                if (error) *error = "闭包副本捕获存储分配失败";
                return nullptr;
            }
            copy->elementViews[entry.first] = view;
        }
    }
    return copy;
}

/**
 * @brief 复制运行时句柄并维护其对象引用。
 * @param handle 待操作的运行时句柄。
 * @param error 接收失败原因的错误信息。
 * @retval 复制运行时句柄并维护其对象引用。
 */
std::int64_t ObjectStore::copyHandle(std::int64_t handle, Memory::String *error)

{
    if (handle < kObjectHandleBase)
    {
        if (error != nullptr) *error = "不是一个能复制的对象句柄：" + Memory::toString(handle);
        return 0;
    }
    auto *storage = handleStorage(handle);
    RuntimeObject *object = storage == nullptr ? nullptr : storage->get();
    if (object == nullptr)
    {
        if (error != nullptr) *error = "这个对象已经释放过了（句柄 " + Memory::toString(handle) + "）";
        return 0;
    }
    if ((object->iteratorStateBox || object->closureCopy) && !nativeRefreshViews(object->iteratorContext, *object))
    {
        if (error) *error = object->iteratorContext.error;
        return 0;
    }
    if (pointerSnapshots && shareablePointerSnapshot(*object)) return add(*storage);
    std::shared_ptr<RuntimeObject> copy = copyObject(*object, error);
    if (!copy) return 0;
    return add(std::move(copy));
}

/**
 * @brief 释放指定地址对应的运行时资源。
 *
 * 释放一个对象；确实释放了返回 true。
 * @param handle 待操作的运行时句柄。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool ObjectStore::release(std::int64_t handle)

{
    auto *storage = handleStorage(handle);
    if (storage == nullptr) return false;
    if ((*storage)->kind == ObjectKind::NativeRecord && (*storage)->nativeType && (*storage)->nativeType->releaseDecision)
    {
        const auto decision = (*storage)->nativeType->releaseDecision((*storage)->nativeRecord.get());
        if (decision == NativeRecordRelease::Deferred) return true;
        if (decision == NativeRecordRelease::Rejected) return false;
    }
    std::shared_ptr<RuntimeObject> object = std::move(*storage);
    auto &slot = handleSlots[static_cast<std::uint64_t>(handle) % handleSlots.size()];
    if (slot.handle == handle)
    {
        slot.handle = 0;
        --inlineHandleCount;
    }
    else
        objects.erase(handle);
    releaseContents(*object);
    return true;
}

/**
 * @brief 还活着的对象个数（停机时 != 0 就是漏了）。
 * @retval 还活着的对象个数（停机时 != 0 就是漏了）。
 */
[[nodiscard]] std::size_t ObjectStore::liveCount() const
{ return objects.size() + inlineHandleCount; }
}  // namespace runtimeDetail
}  // namespace stackvm
