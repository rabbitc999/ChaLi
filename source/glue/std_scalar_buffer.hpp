/** @file std_scalar_buffer.hpp @brief 既有数值内核读取 C 服务存储的私有数据接口，不含注册和 Codec。 */
#pragma once
#include "../abi/scalar_storage.hpp"
#include "stackvm.hpp"
#include <memory>

namespace scalar_buffer
{
/**
 * @brief 通过同一个记录描述符检查句柄，不维护第二份存储或登记表。
 * @param context 既有内核的运行时上下文。
 * @param handle 数值记录句柄。
 * @return 匹配记录，失败保存原诊断并返回空。
 */
inline Storage *get(stackvm::NativeCallContext &context, std::int64_t handle)
{
    const auto *type = static_cast<const stackvm::NativeRecordType *>(descriptor());
    auto *storage = type ? static_cast<Storage *>(context.nativeRecord(handle, *type)) : nullptr;
    if (!storage) context.fail("线性代数存储 opcode 已释放或类型不匹配");
    return storage;
}
/**
 * @brief 为尚未迁移的数值内核复制指定精度的数据。
 * @tparam Scalar 基础数值类型。
 * @param context 当前运行时上下文。
 * @param handle 记录句柄，零表示空序列。
 * @return 独立序列。
 */
template <class Scalar>
Memory::Vector<Scalar> read(stackvm::NativeCallContext &context, std::int64_t handle)
{
    Memory::Vector<Scalar> values;
    if (!handle) return values;
    auto *storage = get(context, handle);
    if (!storage) return values;
    values.reserve(storage->values.size());
    for (const auto &value : storage->values) values.push_back(decode<Scalar>(value));
    return values;
}
/**
 * @brief 构造与 C 入口同一表示的记录，发布失败时仍由本地所有者清理。
 * @tparam Values 数值容器类型。
 * @param context 当前运行时上下文。
 * @param values 输入序列。
 * @return 新记录句柄，失败为零。
 */
template <class Values>
std::int64_t make(stackvm::NativeCallContext &context, const Values &values)
{
    const auto *type = static_cast<const stackvm::NativeRecordType *>(descriptor());
    if (!type)
    {
        context.fail("线性代数存储类型登记失败");
        return 0;
    }
    auto storage = std::make_unique<Storage>();
    storage->values.reserve(values.size());
    for (const auto &value : values) storage->values.push_back(encode<typename Values::value_type>(value));
    const auto handle = context.makeNativeRecord(*type, storage.get());
    if (handle) storage.release();
    return handle;
}
}  // namespace scalar_buffer
