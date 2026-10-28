/**
 * @file std_collection_bootstrap.cpp
 * @brief 自举 Map/Set 的单槽与值封送边界；探测、扩容和删除修复由语言执行。
 */
#include "std_collection_state.hpp"
#include <cstring>

namespace collection_glue
{
/**
 * @brief 验证哈希容器的身份及回调重入状态。
 * @param state 本次调用的运行时适配状态。
 * @param handle 容器句柄。
 * @return 有效 Map/Set 记录，失败为空。
 */
static Record *hashedStorage(State &state, const Slot &handle)
{
    auto *record = state.objects->find(nativeInt(handle));
    if (!record || (record->kind != StorageKind::Map && record->kind != StorageKind::Set))
    {
        state.error = "Map/Set 存储句柄无效";
        return nullptr;
    }
    return nativeObject(state, handle, record->kind, "Map/Set 存储");
}

/** @brief 在用户键协议执行期间禁止同一容器重入。 */
struct KeyCallbackGuard
{
    Record &record;
    /**
     * @brief 进入键协议调用。
     * @param value 正在查询的容器。
     */
    explicit KeyCallbackGuard(Record &value) : record(value) { record.mapComparing = true; }
    /** @brief 恢复键协议调用前的状态。 */
    ~KeyCallbackGuard() { record.mapComparing = false; }
};

/**
 * @brief 从参数取得查询键快照，写入事务优先使用复制值之前的快照。
 * @param state 本次运行时状态。
 * @param record 容器记录。
 * @param arguments 原始参数。
 * @param offset 键参数的位置，随后为文本标志和布局。
 * @param key 接收只读键快照。
 * @return 取得快照时为 true。
 */
static bool queryKey(State &state, Record &record, const Arguments &arguments, std::size_t offset, ObjectKey &key)
{
    if (record.pendingWrite)
    {
        key = record.pendingKey;
        return true;
    }
    ObjectValue value;
    if (!nativeValue(state, arguments[offset], nativeInt(arguments[offset + 1]) != 0,
                     nativeInt(arguments[offset + 2]), false, &value)) return false;
    key = valueKey(value);
    return true;
}

/**
 * @brief 执行一个存储或值封送原语，不实现容器查找或扩容算法。
 * @param state 本次运行时状态。
 * @param operation 存储原语编号。
 * @param arguments 按对应入口签名传入的槽。
 * @return 原始结果槽，失败通过 state.error 报告。
 */
static Slot hashedStorageOperation(State &state, int operation, const Arguments &arguments)
{
    if (operation >= 12) return mapStorageMetadata(state, operation, arguments);
    auto *record = hashedStorage(state, arguments[0]);
    if (!record) return Slot::fromInt(0);
    if (operation == 0)
    {
        const auto command = nativeInt(arguments[1]);
        const auto index = nativeInt(arguments[2]);
        const auto value = nativeInt(arguments[3]);
        switch (command)
        {
        case 0: return Slot::fromInt(static_cast<std::int64_t>(record->entries.size()));
        case 1: return Slot::fromInt(static_cast<std::int64_t>(record->languageSlots.size()));
        case 2: return Slot::fromInt(record->entries.languageIndexed());
        case 3:
        case 4:
            if (index < 0 || static_cast<std::size_t>(index) >= record->languageSlots.size()) break;
            if (command == 4) record->languageSlots[static_cast<std::size_t>(index)] = value;
            return Slot::fromInt(record->languageSlots[static_cast<std::size_t>(index)]);
        case 5:
            if (value < 0 || static_cast<std::uint64_t>(value) > record->languageSlots.max_size()) break;
            record->languageSlots.assign(static_cast<std::size_t>(value), 0);
            return Slot::fromInt(0);
        case 6: record->entries.markLanguageIndexed(); return Slot::fromInt(0);
        case 7:
            if (index < 0 || static_cast<std::size_t>(index) >= record->entries.size()) break;
            return Slot::fromInt((record->entries.begin() + index)->first.hash);
        case 8: return Slot::fromInt(record->mapHash != 0);
        default: break;
        }
        state.error = "Map/Set 存储槽或操作编号越界";
        return Slot::fromInt(0);
    }
    if (operation == 1 || operation == 2)
    {
        const std::size_t offset = operation == 1 ? 1u : 2u;
        ObjectKey key;
        if (!queryKey(state, *record, arguments, offset, key)) return Slot::fromInt(0);
        if (operation == 1)
        {
            if (!record->mapHash) return Slot::fromInt(0);
            if (!record->mapEqual) return Slot::fromInt(static_cast<std::int64_t>(intrinsicHash(key)));
            KeyCallbackGuard guard(*record);
            const auto query = arguments[offset];
            NativeValue result;
            if (!state.host.invoke(record->mapHash, { query.isFloat ? NativeValue::fromFloat(query.real) : NativeValue::fromInt(query.integer) }, result))
                return Slot::fromInt(0);
            return Slot::fromInt(result.asInt());
        }
        const auto index = nativeInt(arguments[1]);
        if (index < 0 || static_cast<std::size_t>(index) >= record->entries.size())
        {
            state.error = "Map/Set 键槽越界";
            return Slot::fromInt(0);
        }
        const auto &stored = (record->entries.begin() + index)->first;
        if (!record->mapEqual)
        {
            key.hashed = stored.hashed;
            key.hash = stored.hash;
            return Slot::fromInt(!(key < stored) && !(stored < key));
        }
        KeyCallbackGuard guard(*record);
        Memory::Vector<std::size_t> temporary;
        Slot left;
        bool matched = false;
        if (mapCallbackKey(state, stored, &left, &temporary))
        {
            const auto right = arguments[offset];
            NativeValue result;
            if (state.host.invoke(record->mapEqual,
                                 { left.isFloat ? NativeValue::fromFloat(left.real) : NativeValue::fromInt(left.integer),
                                   right.isFloat ? NativeValue::fromFloat(right.real) : NativeValue::fromInt(right.integer) },
                                 result)) matched = result.asInt() != 0;
        }
        for (auto it = temporary.rbegin(); it != temporary.rend(); ++it) state.heap->release(*it);
        return Slot::fromInt(matched);
    }
    if (operation == 3)
    {
        if (record->pendingWrite)
        {
            state.error = "Map 写入尚未提交";
            return Slot::fromInt(0);
        }
        ObjectValue key, value;
        if (!nativeValue(state, arguments[1], nativeInt(arguments[2]) != 0, nativeInt(arguments[3]), false, &key)
            || !nativeValue(state, arguments[4], nativeInt(arguments[5]) != 0, nativeInt(arguments[6]), true, &value)) return Slot::fromInt(0);
        record->pendingKey = valueKey(key);
        record->pendingValue = std::move(value);
        record->pendingWrite = true;
        return Slot::fromInt(0);
    }
    if (operation == 4)
    {
        const auto index = nativeInt(arguments[1]);
        if (!record->pendingWrite || index < -1 || (index >= 0 && static_cast<std::size_t>(index) >= record->entries.size()))
        {
            state.error = "Map 写入事务或目标槽无效";
            return Slot::fromInt(0);
        }
        auto result = index;
        if (index >= 0)
        {
            auto &value = (record->entries.begin() + index)->second;
            state.objects->releaseValue(value);
            value = std::move(record->pendingValue);
        }
        else
        {
            auto key = record->pendingKey;
            key.hashed = record->mapHash != 0;
            key.hash = nativeInt(arguments[2]);
            if (!state.objects->ownKey(&key, &state.error))
            {
                state.objects->releaseKey(key);
                return Slot::fromInt(0);
            }
            result = static_cast<std::int64_t>(record->entries.size());
            record->entries.appendUnindexed(std::move(key), std::move(record->pendingValue));
        }
        record->pendingKey = {};
        record->pendingValue = {};
        record->pendingWrite = false;
        return Slot::fromInt(result);
    }
    if (operation == 5)
    {
        if (!record->pendingWrite)
        {
            state.error = "Set 插入缺少键快照";
            return Slot::fromInt(0);
        }
        ObjectValue value;
        if (!nativeValue(state, arguments[1], nativeInt(arguments[2]) != 0, nativeInt(arguments[3]), true, &value)) return Slot::fromInt(0);
        ObjectKey key = valueKey(value);
        if (record->mapEqual)
        {
            key = record->pendingKey;
            if (!state.objects->ownKey(&key, &state.error))
            {
                state.objects->releaseKey(key);
                state.objects->releaseValue(value);
                return Slot::fromInt(0);
            }
        }
        key.hashed = record->mapHash != 0;
        key.hash = nativeInt(arguments[4]);
        const auto result = static_cast<std::int64_t>(record->entries.size());
        record->entries.appendUnindexed(std::move(key), std::move(value));
        record->pendingKey = {};
        record->pendingWrite = false;
        return Slot::fromInt(result);
    }
    if (operation == 6 || operation == 7)
    {
        const auto index = nativeInt(arguments[1]);
        if (index < 0 || static_cast<std::size_t>(index) >= record->entries.size())
        {
            state.error = operation == 7 ? "map 里没有这个键" : "Map/Set 清理槽越界";
            return Slot::fromInt(0);
        }
        auto &entry = *(record->entries.begin() + index);
        if (operation == 7)
        {
            Slot result;
            if (!nativeSlot(state, entry.second, &result)) return Slot::fromInt(0);
            return result;
        }
        state.objects->releaseKey(entry.first);
        state.objects->releaseValue(entry.second);
        record->entries.eraseUnindexed(static_cast<std::size_t>(index));
        return Slot::fromInt(0);
    }
    if (operation == 8)
    {
        const auto result = nativeNewObject(state, record->kind);
        if (auto *destination = state.objects->find(nativeInt(result)))
        {
            destination->mapEqual = record->mapEqual;
            destination->mapHash = record->mapHash;
        }
        return result;
    }
    if (operation == 9)
    {
        auto *source = hashedStorage(state, arguments[1]);
        const auto index = nativeInt(arguments[2]);
        if (!source || index < 0 || static_cast<std::size_t>(index) >= source->entries.size())
        {
            if (state.error.empty()) state.error = "Map/Set 复制槽越界";
            return Slot::fromInt(0);
        }
        const auto &entry = *(source->entries.begin() + index);
        auto key = entry.first;
        ObjectValue value = entry.second;
        // ownKey 只克隆未拥有的快照，因此先把借用副本恢复为查询状态。
        std::function<void(ObjectKey &)> borrow = [&](ObjectKey &item) {
            item.ownsObject = false;
            for (auto &field : item.fields) borrow(field);
        };
        borrow(key);
        const bool valueKeyOnly = record->kind == StorageKind::Set && !record->mapEqual;
        if (!valueKeyOnly && !state.objects->ownKey(&key, &state.error))
        {
            state.objects->releaseKey(key);
            return Slot::fromInt(0);
        }
        if (!state.objects->copyValue(&value, &state.error))
        {
            if (!valueKeyOnly) state.objects->releaseKey(key);
            return Slot::fromInt(0);
        }
        if (valueKeyOnly)
        {
            const auto hash = key.hash;
            const auto hashed = key.hashed;
            key = valueKey(value);
            key.hash = hash;
            key.hashed = hashed;
        }
        record->entries.appendUnindexed(std::move(key), std::move(value));
        return Slot::fromInt(0);
    }
    if (operation == 10 || operation == 11)
    {
        if (record->kind != StorageKind::Set || (operation == 10 && record->pendingWrite))
        {
            state.error = "Set 键快照事务无效";
            return Slot::fromInt(0);
        }
        if (operation == 10)
        {
            ObjectValue key;
            if (!nativeValue(state, arguments[1], nativeInt(arguments[2]) != 0, nativeInt(arguments[3]), false, &key)) return Slot::fromInt(0);
            record->pendingKey = valueKey(key);
            record->pendingWrite = true;
        }
        else
        {
            record->pendingKey = {};
            record->pendingWrite = false;
        }
        return Slot::fromInt(0);
    }
    state.error = "Map/Set 存储原语编号无效";
    return Slot::fromInt(0);
}

/**
 * @brief 使用栈上数值槽连接自举哈希容器，避免动态参数数组与函数表查找。
 * @param host 当前运行时及同步回调上下文。
 * @param operation 存储原语编号。
 * @param first 第一个实参位模式。
 * @param second 第二个实参位模式。
 * @param third 第三个实参位模式。
 * @param fourth 第四个实参位模式。
 * @param fifth 第五个实参位模式。
 * @param sixth 第六个实参位模式。
 * @param seventh 第七个实参位模式。
 * @param floating 浮点实参位图。
 * @return 带有正确浮点种类的原始结果。
 */
NativeValue mapStorageOperation(stackvm::NativeCallContext &host, std::int64_t operation,
                                std::int64_t first, std::int64_t second, std::int64_t third,
                                std::int64_t fourth, std::int64_t fifth, std::int64_t sixth,
                                std::int64_t seventh, std::uint64_t floating)
{
    const std::int64_t values[] = { first, second, third, fourth, fifth, sixth, seventh };
    Slot slots[7];
    for (std::size_t index = 0; index < 7; ++index)
    {
        if (floating & (std::uint64_t(1) << index))
        {
            double value;
            std::memcpy(&value, values + index, sizeof(value));
            slots[index] = Slot::fromDouble(value);
        }
        else
            slots[index] = Slot::fromInt(values[index]);
    }
    State state(host);
    const auto result = hashedStorageOperation(state, static_cast<int>(operation), Arguments(slots, 7));
    if (!state.error.empty()) host.fail(state.error);
    return result.isFloat ? NativeValue::fromFloat(result.real) : NativeValue::fromInt(result.integer);
}

/**
 * @brief 注册自举 Map/Set 使用的有类型单槽入口。
 * @param registry 当前宿主函数注册表。
 * @param error 接收绑定失败的诊断。
 * @return 全部绑定成功时为 true。
 */
bool bindHashedStorage(stackvm::NativeRegistry &registry, Memory::String *error)
{
    struct Binding
    {
        const char *name;
        std::size_t arity;
        int operation;
    };
    static const Binding bindings[] = {
        { "std::lang::__mapStorageState", 4, 0 },
        { "std::lang::__mapStorageHashI", 4, 1 },
        { "std::lang::__mapStorageHashF", 4, 1 },
        { "std::lang::__mapStorageMatchI", 5, 2 },
        { "std::lang::__mapStorageMatchF", 5, 2 },
        { "std::lang::__mapStoragePrepareII", 7, 3 },
        { "std::lang::__mapStoragePrepareIF", 7, 3 },
        { "std::lang::__mapStoragePrepareFI", 7, 3 },
        { "std::lang::__mapStoragePrepareFF", 7, 3 },
        { "std::lang::__mapStorageCommit", 3, 4 },
        { "std::lang::__setStorageAppendI", 5, 5 },
        { "std::lang::__setStorageAppendF", 5, 5 },
        { "std::lang::__mapStorageErase", 2, 6 },
        { "std::lang::__mapStorageReadI", 4, 7 },
        { "std::lang::__mapStorageReadF", 4, 7 },
        { "std::lang::__mapStorageEmptyLike", 1, 8 },
        { "std::lang::__mapStorageCloneAppend", 3, 9 },
        { "std::lang::__setStoragePrepareI", 4, 10 },
        { "std::lang::__setStoragePrepareF", 4, 10 },
        { "std::lang::__setStorageCancel", 1, 11 }
    };
    for (const auto &binding : bindings)
        if (!registry.add(binding.name, binding.arity, [operation = binding.operation](stackvm::NativeCallContext &host, const Memory::Vector<NativeValue> &values) {
                State state(host);
                const auto result = hashedStorageOperation(state, operation, Arguments(values));
                if (!state.error.empty()) host.fail(state.error);
                return result.isFloat ? NativeValue::fromFloat(result.real) : NativeValue::fromInt(result.integer); }, error)) return false;
    return true;
}
}  // namespace collection_glue
