/** @file abi_scalar.cpp @brief 数值缓冲的 C 帧入口，支持全部基础标量，不使用通用值封送。 */
#include "chali_standard.h"
#include "scalar_storage.hpp"
#include <memory>

namespace scalar_buffer
{
constexpr const char *typeName = "std::linalg::ScalarStorage";
/** @brief 销毁数值缓冲及其最终容量。
 * @param value 拥有的记录。
 */
void CHALI_ABI_CALL destroy(void *value) { delete static_cast<Storage *>(value); }
/** @brief 深复制纯数值槽，失败不跨 C 边界抛出异常。
 * @param value 只读源记录。
 * @return 独立副本，失败为空。
 */
void *CHALI_ABI_CALL clone(const void *value)
try
{
    return new Storage(*static_cast<const Storage *>(value));
} catch (...) { return nullptr; }
/** @brief 发布完整数值缓冲，失败时仍保留本地所有权。
 * @param frame 当前 C 帧。
 * @param value 待转移的记录。
 * @return 新句柄，失败为零。
 */
int64_t publish(const ChaliAbiFrame *frame, std::unique_ptr<Storage> value)
{
    const auto handle = chali_abi_record_make_clonable(frame, typeName, value.get(), destroy, clone);
    if (handle) value.release();
    return handle;
}
/** @brief 校验并借用数值记录。
 * @param frame 当前 C 帧。
 * @param handle 记录句柄。
 * @return 存活记录，失败为空并保留原诊断。
 */
Storage *get(const ChaliAbiFrame *frame, int64_t handle)
{
    auto *value = static_cast<Storage *>(chali_abi_record_probe(frame, handle, typeName));
    if (!value) chali_abi_fail(frame, "线性代数存储 opcode 已释放或类型不匹配");
    return value;
}
/** @brief 兼容直接 opcode 接收者及语言引用槽，始终使用受检查的逻辑读。
 * @param frame 当前 C 帧。
 * @param reference 直接句柄或句柄槽地址。
 * @param handle 接收实际句柄。
 * @return 解码成功为真。
 */
bool referenceHandle(const ChaliAbiFrame *frame, int64_t reference, int64_t &handle)
{
    if (chali_abi_record_probe(frame, reference, typeName))
    {
        handle = reference;
        return true;
    }
    unsigned char bytes[8];
    if (!chali_abi_read(frame, reference, bytes, sizeof(bytes)))
    {
        chali_abi_fail(frame, "opcode 引用槽不可读");
        return false;
    }
    uint64_t bits = 0;
    for (unsigned index = 0; index < 8; ++index) bits |= uint64_t(bytes[index]) << (8 * index);
    handle = int64_t(bits);
    return true;
}
/** @brief 编译期固定数值缓冲操作。 */
enum class Operation
{
    New,
    Drop,
    Copy,
    Len,
    Clear,
    At,
    Set,
    Push
};
/** @brief 通过固定标量类型及 C 帧执行访问，错误不越过 C 边界。
 * @tparam Scalar 目标基础类型。
 * @tparam op 编译期操作。
 * @param frame 实参及返回帧。
 */
template <class Scalar, Operation op>
void CHALI_ABI_CALL entry(ChaliAbiFrame *frame)
try
{
    if constexpr (op == Operation::New)
    {
        chali_abi_return_integer(frame, publish(frame, std::make_unique<Storage>()));
        return;
    }
    int64_t handle = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    if constexpr (op == Operation::Drop)
    {
        // 旧接口对零值和已释放记录不重复报错。
        if (chali_abi_record_probe(frame, handle, typeName)) chali_abi_record_release(frame, handle);
        chali_abi_return_integer(frame, 0);
        return;
    }
    if constexpr (op == Operation::Copy)
    {
        if (!handle)
            chali_abi_return_integer(frame, publish(frame, std::make_unique<Storage>()));
        else if (auto *value = get(frame, handle))
            chali_abi_return_integer(frame, publish(frame, std::make_unique<Storage>(*value)));
        return;
    }
    if constexpr (op == Operation::Clear || op == Operation::Set || op == Operation::Push)
    {
        int64_t resolved = 0;
        if (!referenceHandle(frame, handle, resolved)) return;
        handle = resolved;
    }
    auto *storage = get(frame, handle);
    if (!storage) return;
    if constexpr (op == Operation::Len)
    {
        chali_abi_return_integer(frame, int64_t(storage->values.size()));
        return;
    }
    if constexpr (op == Operation::Clear)
    {
        storage->values.clear();
        chali_abi_return_integer(frame, 0);
        return;
    }
    int64_t index = 0;
    if constexpr (op == Operation::At || op == Operation::Set)
    {
        if (!chali_abi_argument_integer(frame, 1, &index)) return;
        if (index < 0 || uint64_t(index) >= storage->values.size())
        {
            chali_abi_fail(frame, "线性代数存储下标越界");
            return;
        }
    }
    if constexpr (op == Operation::At)
    {
        const Scalar value = decode<Scalar>(storage->values[size_t(index)]);
        if constexpr (std::is_floating_point_v<Scalar>)
            chali_abi_return_real(frame, double(value));
        else
            chali_abi_return_integer(frame, int64_t(value));
    }
    if constexpr (op == Operation::Set || op == Operation::Push)
    {
        constexpr int64_t argument = op == Operation::Set ? 2 : 1;
        Scalar value{};
        if constexpr (std::is_floating_point_v<Scalar>)
        {
            double raw = 0;
            if (!chali_abi_argument_real(frame, argument, &raw)) return;
            value = static_cast<Scalar>(raw);
        }
        else
        {
            int64_t raw = 0;
            if (!chali_abi_argument_integer(frame, argument, &raw)) return;
            value = static_cast<Scalar>(raw);
        }
        if constexpr (op == Operation::Set)
            storage->values[size_t(index)] = encode(value);
        else
            storage->values.push_back(encode(value));
        chali_abi_return_integer(frame, 0);
    }
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "数值缓冲服务失败");
}
}  // namespace scalar_buffer

/**
 * @brief 取得覆盖全部基础标量的数值缓冲 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 生命周期覆盖进程的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_scalar_records(size_t *count)
{
    using namespace scalar_buffer;
    static const ChaliAbiRecord records[] = {
        { "std::linalg::__std_scalarAt__bool", entry<bool, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__char", entry<std::uint8_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__float32", entry<float, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__float64", entry<double, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__int16", entry<std::int16_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__int32", entry<std::int32_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__int64", entry<std::int64_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__int8", entry<std::int8_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__uint16", entry<std::uint16_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__uint32", entry<std::uint32_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__uint64", entry<std::uint64_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarAt__uint8", entry<std::uint8_t, Operation::At>, 2 },
        { "std::linalg::__std_scalarClear", entry<std::int64_t, Operation::Clear>, 1 },
        { "std::linalg::__std_scalarCopy", entry<std::int64_t, Operation::Copy>, 1 },
        { "std::linalg::__std_scalarDrop", entry<std::int64_t, Operation::Drop>, 1 },
        { "std::linalg::__std_scalarLen", entry<std::int64_t, Operation::Len>, 1 },
        { "std::linalg::__std_scalarNew", entry<std::int64_t, Operation::New>, 0 },
        { "std::linalg::__std_scalarPush__bool", entry<bool, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__char", entry<std::uint8_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__float32", entry<float, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__float64", entry<double, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__int16", entry<std::int16_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__int32", entry<std::int32_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__int64", entry<std::int64_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__int8", entry<std::int8_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__uint16", entry<std::uint16_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__uint32", entry<std::uint32_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__uint64", entry<std::uint64_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarPush__uint8", entry<std::uint8_t, Operation::Push>, 2 },
        { "std::linalg::__std_scalarSet__bool", entry<bool, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__char", entry<std::uint8_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__float32", entry<float, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__float64", entry<double, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__int16", entry<std::int16_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__int32", entry<std::int32_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__int64", entry<std::int64_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__int8", entry<std::int8_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__uint16", entry<std::uint16_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__uint32", entry<std::uint32_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__uint64", entry<std::uint64_t, Operation::Set>, 3 },
        { "std::linalg::__std_scalarSet__uint8", entry<std::uint8_t, Operation::Set>, 3 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
