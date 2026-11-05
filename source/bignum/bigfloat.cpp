#if !defined(NO_CHALI_BIG_INT) && !defined(NO_CHALI_BIG_FLOAT)
/**
 * @file bigfloat.cpp
 * @brief C 大浮点服务的 C++ 所有权、运算符和异常兼容层。
 *
 * 策略与参考实现一致：所有运算都在整数上做，**多算几位再舍入**到目标有效位数，
 * 所以 `0.1 + 0.2` 精确等于 `0.3`（十进制浮点，不是二进制浮点）。
 * 大规模乘法直接吃到 `BigInt` 的 NTT 内核（O(n log n)）。
 */

#include "bigfloat_c_adapter.hpp"

#include <cstdlib>
#include <stdexcept>

namespace bigfloat
{

// ---------------------------------------------------------------------------
//  解析 / 规范化
// ---------------------------------------------------------------------------

/**
 * @brief 从十进制文本构造：`123.456` / `-0.5` / `.5` / `1e100` / `2.5E-3` 都行。
 * @param text 文本。
 * @throws std::invalid_argument 文本不是合法十进制小数时。
 */
BigFloat::BigFloat(const Memory::String &text)
{
    *this = fromString(text, nullptr);
}

/**
 * @brief 从 64 位有符号整数**隐式**构造。
 * @param value 值。
 */
BigFloat::BigFloat(std::int64_t value)
{
    *this = fromInt(bigint::BigInt(value));
}

/**
 * @brief 从 double **隐式**构造（语义同 `fromDouble`：最短往返写法）。
 * @param value 值。
 */
BigFloat::BigFloat(double value)
{
    *this = fromDouble(value);
}

/**
 * @brief 显式转 64 位有符号整数（向零截断；大数按位截断）。
 * @retval 取原数值向零截断后的 64 位有符号整数。
 */
BigFloat::operator std::int64_t() const
{
    const bigint::BigInt truncated = trunc();
    if (truncated.sign() < 0) return -static_cast<std::int64_t>(truncated.abs());
    return static_cast<std::int64_t>(truncated);
}

/**
 * @brief 显式转双精度浮点（可能丢精度；按 17 位有效数字往返）。
 * @retval 保存转换结果的双精度浮点值。
 */
BigFloat::operator double() const
{
    if (isZero()) return 0.0;
    return std::strtod(toString(17).c_str(), nullptr);
}

/**
 * @brief 从十进制文本构造（不抛异常版）。
 * @param text 文本。
 * @param ok 非空时写入是否解析成功。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::fromString(const Memory::String &text, bool *ok)
{
    cAdapter::Work work;
    int valid = 0;
    cAdapter::check(chali_bigfloat_parse(&work.value, text.data(), text.size(), &valid));
    if (ok) *ok = valid != 0;
    return CFloatAccess::take(work.value);
}

/**
 * @brief 从整数构造（值就是那个整数）。
 * @param value 待转换、保存或参与计算的值。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::fromInt(const bigint::BigInt &value)
{
    cAdapter::Work work;
    cAdapter::check(chali_bigfloat_from_integer(&work.value, &CFloatAccess::integer(value)));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 从 `double` 构造：取**能往返同一个 double 的最短十进制写法**。
 *
 * 参考实现的 `EigenMath_BigFloatFromDouble` 直接 `%.17g`，于是 `0.1` 会变成
 * `0.10000000000000001`；这里改成从 1 位起逐位试 `%.*g`、用 `strtod` 读回来
 * 比对，第一个能往返的位数就是结果 —— 所以 `fromDouble(0.1).toString() == "0.1"`，
 * 而 `fromDouble(0.1) + fromDouble(0.2)` 精确等于 `"0.3"`。
 * 非有限值（NaN / ±inf）按 0 处理（与参考实现一致）。
 * 注意：内部用 `snprintf` / `strtod`，跟 `printf` 一样**认当前 locale 的小数点**
 * （缺省的 `"C"` locale 下就是 `.`）；要跨 locale 稳定就自己先解析好文本。
 * @param value 值。
 * @param digits 有效位数**上限**（缺省 17 = double 的往返精度；只在 1..17 内生效）。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::fromDouble(double value, int digits)
{
    cAdapter::Work work;
    cAdapter::check(chali_bigfloat_from_double(&work.value, value, digits));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 规范化数值内部表示并移除冗余零位。
 */
void BigFloat::normalize()
{
    cAdapter::Work work;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_normalized(&work.value, &view));
    *this = CFloatAccess::take(work.value);
}

// ---------------------------------------------------------------------------
//  舍入 / 输出 / 比较
// ---------------------------------------------------------------------------

/**
 * @brief 舍入到 `digits` 位有效数字（四舍五入、远离零；`digits ≤ 0` 抛异常）。
 * @param digits 有效位数。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::round(int digits) const
{
    if (digits <= 0) throw std::invalid_argument("bigfloat: 有效位数必须是正数");
    return cAdapter::unary(*this, digits, chali_bigfloat_round);
}

/**
 * @brief 十进制文本，保留 `digits` 位有效数字。
 *
 * 指数不大时输出普通记法（`123.456`、`0.00123`），指数太大或太小改用
 * 科学记数法（`1.23e+100`、`4.5e-30`）—— 与 `%.17g` 同一口径。
 * @param digits 有效位数；`≤ 0` 表示**不舍入**（按值本身精确输出）。
 *        缺省 17，与 `%.17g` 同量级（所以 `1e20 + 1` 会显示成 `1e+20`）。
 * @retval 生成或读取的结果文本。
 */
Memory::String BigFloat::toString(int digits) const
{
    return cAdapter::format(*this, digits, false);
}

/**
 * @brief 十进制文本，**恒定用普通记法**（不切科学记数法）。
 *
 * 对应参考实现的 `EigenMath_BigFloatToDecimalString`（"供 infix / LaTeX 输出用"）：
 * `BigFloat("1e20").toPlainString()` → `"100000000000000000000"`，
 * `(BigFloat("1e20") + BigFloat("1")).toPlainString()` → `"100000000000000000001"`。
 * @param digits 有效位数；`≤ 0`（缺省）表示不舍入、按值本身精确输出。
 * @retval 生成或读取的结果文本。
 */
Memory::String BigFloat::toPlainString(int digits) const
{
    return cAdapter::format(*this, digits, true);
}

/**
 * @brief 比较当前数值与另一数值的大小。
 * @param other 参与当前运算或比较的另一对象。
 * @retval 当前值小于、等于、大于另一值时分别为负数、0、正数。
 */
int BigFloat::compare(const BigFloat &other) const
{
    bigint::BigInt owner;
    const auto left = CFloatAccess::view(*this), right = CFloatAccess::view(other);
    int order = 0;
    cAdapter::check(chali_bigfloat_compare(&left, &right, &order, &CFloatAccess::integer(owner).limbs.allocator));
    return order;
}

// ---------------------------------------------------------------------------
//  四则
// ---------------------------------------------------------------------------

/**
 * @brief 计算两个数值的和。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat operator+(const BigFloat &left, const BigFloat &right)
{
    cAdapter::Work work;
    const auto a = CFloatAccess::view(left), b = CFloatAccess::view(right);
    cAdapter::check(chali_bigfloat_add(&work.value, &a, &b, 0));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 计算数值之差或当前数值的相反数。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat operator-(const BigFloat &left, const BigFloat &right)
{
    cAdapter::Work work;
    const auto a = CFloatAccess::view(left), b = CFloatAccess::view(right);
    cAdapter::check(chali_bigfloat_add(&work.value, &a, &b, 1));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 一元负号。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::operator-() const
{
    cAdapter::Work work;
    const auto view = CFloatAccess::view(*this);
    cAdapter::check(chali_bigfloat_sign_change(&work.value, &view, 0));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 计算两个数值的乘积。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 两个数值相乘得到的积。
 */
BigFloat operator*(const BigFloat &left, const BigFloat &right)
{
    cAdapter::Work work;
    const auto a = CFloatAccess::view(left), b = CFloatAccess::view(right);
    cAdapter::check(chali_bigfloat_multiply(&work.value, &a, &b));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 除法并指定有效位数（`operator/` 用 34 位）。
 * @param divisor 除数。
 * @param digits 商保留的有效位数。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat BigFloat::divide(const BigFloat &divisor, int digits) const
{
    if (divisor.isZero()) throw std::domain_error("bigfloat: 除以零");
    if (digits <= 0) throw std::invalid_argument("bigfloat: 有效位数必须是正数");
    cAdapter::Work work;
    const auto a = CFloatAccess::view(*this), b = CFloatAccess::view(divisor);
    cAdapter::check(chali_bigfloat_divide(&work.value, &a, &b, digits));
    return CFloatAccess::take(work.value);
}

/**
 * @brief 计算两个数值相除的商。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 计算得到的大数值；复合赋值运算返回当前对象。
 */
BigFloat operator/(const BigFloat &left, const BigFloat &right)
{
    return left.divide(right, 34);
}

/**
 * @brief 将右操作数累加到当前值。
 * @param other 参与当前运算或比较的另一对象。
 * @retval 更新后的当前数值对象引用。
 */
BigFloat &BigFloat::operator+=(const BigFloat &other)
{
    *this = *this + other;
    return *this;
}
/**
 * @brief 从当前值减去右操作数。
 * @param other 参与当前运算或比较的另一对象。
 * @retval 更新后的当前数值对象引用。
 */
BigFloat &BigFloat::operator-=(const BigFloat &other)
{
    *this = *this - other;
    return *this;
}
/**
 * @brief 将当前值乘以右操作数。
 * @param other 参与当前运算或比较的另一对象。
 * @retval 更新后的当前数值对象引用。
 */
BigFloat &BigFloat::operator*=(const BigFloat &other)
{
    *this = *this * other;
    return *this;
}
/**
 * @brief 将当前值除以右操作数。
 * @param other 参与当前运算或比较的另一对象。
 * @retval 更新后的当前数值对象引用。
 */
BigFloat &BigFloat::operator/=(const BigFloat &other)
{
    *this = *this / other;
    return *this;
}

/**
 * @brief 判断两个值是否相等。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 两个值相等时为 true，否则为 false。
 */
bool operator==(const BigFloat &left, const BigFloat &right)
{
    return left.compare(right) == 0;
}
/**
 * @brief 判断两个值是否不相等。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 两个值不相等时为 true，否则为 false。
 */
bool operator!=(const BigFloat &left, const BigFloat &right)
{
    return left.compare(right) != 0;
}
/**
 * @brief 判断左值是否小于右值。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 左值小于右值时为 true，否则为 false。
 */
bool operator<(const BigFloat &left, const BigFloat &right)
{
    return left.compare(right) < 0;
}
/**
 * @brief 判断左值是否小于或等于右值。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 左值小于或等于右值时为 true，否则为 false。
 */
bool operator<=(const BigFloat &left, const BigFloat &right)
{
    return left.compare(right) <= 0;
}
/**
 * @brief 判断左值是否大于右值。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 左值大于右值时为 true，否则为 false。
 */
bool operator>(const BigFloat &left, const BigFloat &right)
{
    return left.compare(right) > 0;
}
/**
 * @brief 判断左值是否大于或等于右值。
 * @param left 左操作数。
 * @param right 右操作数。
 * @retval 左值大于或等于右值时为 true，否则为 false。
 */
bool operator>=(const BigFloat &left, const BigFloat &right)
{
    return left.compare(right) >= 0;
}

}  // namespace bigfloat

#endif
