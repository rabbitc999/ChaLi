/**
 * @file bigfloat_roots.cpp
 * @brief C 十进制幂、方根与整数取整服务的兼容适配。
 */
#if !defined(NO_CHALI_BIG_FLOAT) && !defined(NO_CHALI_BIG_INT)
#include "bigfloat_c_adapter.hpp"
#include <limits>
#include <stdexcept>
namespace bigfloat
{
// ---------------------------------------------------------------------------
//  平方根
// ---------------------------------------------------------------------------

/**
 * @brief 平方根，结果保留 `digits` 位有效数字。
 * @param digits 有效位数。
 * @throws std::domain_error 负数开方时。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::sqrt(int digits) const
{
    if (digits <= 0) throw std::invalid_argument("bigfloat: 有效位数必须是正数");
    if (sign() < 0) throw std::domain_error("bigfloat: 负数不能开平方");
    return cAdapter::unary(*this, digits, chali_bigfloat_sqrt);
}

// ---------------------------------------------------------------------------
//  整数次幂 / 绝对值 / 取整 / k 次方根
// ---------------------------------------------------------------------------

/**
 * @brief 精确整数次幂：`*this ^ exponent`。
 * @param exponent 指数（无符号；`0` 次幂为 1）。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::pow(std::uint64_t exponent) const
{
    cAdapter::Work work;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_pow_integer(&work.value, &view, exponent), "bigfloat: 幂运算的十进制指数溢出");
    return CFloatAccess::take(work.value);
}

/**
 * @brief 绝对值。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::abs() const
{
    cAdapter::Work work;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_sign_change(&work.value, &view, 1));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 是不是整数（`5.0`、`1e20` 是；`0.5`、`1e-3` 不是；0 是）。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool BigFloat::isInteger() const
{
    bigint::BigInt owner;
    const auto view = CFloatAccess::view(*this);
    int integer = 0;
    cAdapter::check(chali_bigfloat_is_integer(&view, &integer, &CFloatAccess::integer(owner).limbs.allocator));
    return integer != 0;
}

/**
 * @brief 向零截断。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
bigint::BigInt BigFloat::trunc() const
{
    bigint::BigInt result;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_to_integer(&CFloatAccess::integer(result), &view, 0));
    return result;
}

/**
 * @brief 向负无穷取整。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
bigint::BigInt BigFloat::floor() const
{
    bigint::BigInt result;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_to_integer(&CFloatAccess::integer(result), &view, -1));
    return result;
}

/**
 * @brief 向正无穷取整。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
bigint::BigInt BigFloat::ceil() const
{
    bigint::BigInt result;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_to_integer(&CFloatAccess::integer(result), &view, 1));
    return result;
}

/**
 * @brief `k` 次方根，结果保留 `digits` 位有效数字（牛顿迭代）。
 * @param k 根次（≥ 1）。
 * @param digits 有效位数。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::root(std::uint64_t k, int digits) const
{
    if (k == 0) throw std::invalid_argument("bigfloat: 根次必须为正");
    if (digits > (std::numeric_limits<int>::max() - 6) / 2)
        throw std::overflow_error("bigfloat: 方根精度超出内部计算范围");
    if (digits <= 0) throw std::invalid_argument("bigfloat: 有效位数必须是正数");
    if (sign() < 0) throw std::domain_error("bigfloat: 负数不能开 k 次方");
    cAdapter::Work work;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_root(&work.value, &view, k, digits));
    return CFloatAccess::take(work.value);
}

}  // namespace bigfloat
#endif
