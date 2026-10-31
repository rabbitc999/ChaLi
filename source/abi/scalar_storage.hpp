/** @file scalar_storage.hpp @brief 数值内核私有的定长标量槽及转换，不作为 C 宿主头暴露。 */
#pragma once
#include "../allocator/chali_allocator.hpp"
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace scalar_buffer
{
/** @brief 保留整数或双精度位模式的十六字节无所有权标量槽。 */
struct alignas(8) Slot
{
    std::int64_t bits = 0;
    bool floating = false;
};
static_assert(sizeof(Slot) == 16, "numeric slot layout");
/** @brief 使用运行时分配器保存数值，clear 保留容量。 */
struct Storage
{ Memory::Vector<Slot> values; };
/**
 * @brief 以原有数值转换规则读取指定精度，整数转换先经过有符号 64 位槽。
 * @tparam Scalar 目标基础类型。
 * @param slot 无所有权数值槽。
 * @return 指定精度的值。
 */
template <class Scalar>
Scalar decode(const Slot &slot)
{
    double real = 0;
    if (slot.floating) std::memcpy(&real, &slot.bits, sizeof(real));
    if constexpr (std::is_floating_point_v<Scalar>)
        return static_cast<Scalar>(slot.floating ? real : static_cast<double>(slot.bits));
    else
        return static_cast<Scalar>(slot.floating ? static_cast<std::int64_t>(real) : slot.bits);
}
/**
 * @brief 编码基础数值，保留浮点位模式，整数仍使用原有八字节表示。
 * @tparam Scalar 来源基础类型。
 * @param value 已按来源类型缩窄的值。
 * @return 可独立复制的数值槽。
 */
template <class Scalar>
Slot encode(Scalar value)
{
    Slot result;
    if constexpr (std::is_floating_point_v<Scalar>)
    {
        const double real = static_cast<double>(value);
        std::memcpy(&result.bits, &real, sizeof(real));
        result.floating = true;
    }
    else
        result.bits = static_cast<std::int64_t>(value);
    return result;
}
/**
 * @brief 取得与 C 服务共用的进程级描述符，供尚未迁移的私有数值内核使用。
 * @return 不透明描述符，登记失败为空；不向公开 C ABI 暴露运行时类型。
 */
const void *descriptor();
}  // namespace scalar_buffer
