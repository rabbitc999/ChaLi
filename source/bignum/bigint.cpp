#if !defined(NO_CHALI_BIG_INT)
/**
 * @file bigint.cpp
 * @brief 大整数 C 内核的 C++ 所有权与异常兼容层。
 */
#include "bigint.hpp"
#include <stdexcept>
#include <utility>

namespace bigint
{
namespace
{
/**
 * @brief 将 C 分配接到原有预算和所有者。
 * @param context 所有者。
 * @param bytes 大小。
 * @return 地址或空。
 */
void *allocate(void *context, std::size_t bytes) noexcept
{
    try
    {
        return Memory::allocate(static_cast<Memory::RuntimeAllocator *>(context), bytes, alignof(std::max_align_t));
    } catch (...) { return nullptr; }
}
/**
 * @brief 通过原所有者释放 C 存储。
 * @param context 所有者。
 * @param pointer 地址。
 * @param bytes 大小。
 */
void release(void *context, void *pointer, std::size_t bytes) noexcept
{
    (void)bytes;
    Memory::release(static_cast<Memory::RuntimeAllocator *>(context), pointer);
}
/**
 * @brief 捕获当前分配上下文，空所有者保持原默认池的延迟创建。
 * @return C 分配接口。
 */
ChaliCAllocator currentAllocator() { return { Memory::currentRuntimeAllocator(), allocate, release }; }
/**
 * @brief 独占 C 文本，构造 C++ 文本失败时仍能释放。
 */
struct Text
{
    ChaliBigString value{};
    /**
     * @brief 将工作区绑定到当前所有者。
     */
    Text() { value.allocator = currentAllocator(); }
    /**
     * @brief 归还临时 C 文本。
     */
    ~Text() { chali_big_string_clear(&value); }
};
}  // namespace
/**
 * @brief 将 C 状态恢复为既有 C++ 异常。
 * @param status 状态。
 * @param message 操作诊断。
 */
void BigInt::check(ChaliCStatus status, const char *message)
{
    if (status == CHALI_C_OK) return;
    if (status == CHALI_C_OUT_OF_MEMORY) throw std::bad_alloc();
    if (status == CHALI_C_RANGE_ERROR) throw std::length_error(message);
    if (status == CHALI_C_DOMAIN_ERROR) throw std::domain_error(message);
    throw std::invalid_argument(message);
}
/**
 * @brief 构造无分配的零，保留当前所有者。
 */
BigInt::BigInt()
{
    auto allocator = currentAllocator();
    check(chali_bigint_init(&value_, &allocator));
}
/**
 * @brief 按当前所有者复制。
 * @param other 原值。
 */
BigInt::BigInt(const BigInt &other) : BigInt() { check(chali_bigint_copy(&value_, &other.value_)); }
/**
 * @brief 移动 C 存储与原始所有者。
 * @param other 原值。
 */
BigInt::BigInt(BigInt &&other) noexcept : value_(other.value_)
{
    other.value_.sign = 0;
    other.value_.limbs.data = nullptr;
    other.value_.limbs.length = other.value_.limbs.capacity = 0;
}
/**
 * @brief 析构 C 存储。
 */
BigInt::~BigInt() { chali_bigint_clear(&value_); }
/**
 * @brief 保留目标所有者复制。
 * @param other 原值。
 * @return 当前对象。
 */
BigInt &BigInt::operator=(const BigInt &other)
{
    check(chali_bigint_copy(&value_, &other.value_));
    return *this;
}
/**
 * @brief 释放旧存储并接管来源。
 * @param other 原值。
 * @return 当前对象。
 */
BigInt &BigInt::operator=(BigInt &&other) noexcept
{
    if (this != &other)
    {
        chali_bigint_clear(&value_);
        value_ = other.value_;
        other.value_.sign = 0;
        other.value_.limbs.data = nullptr;
        other.value_.limbs.length = other.value_.limbs.capacity = 0;
    }
    return *this;
}
/**
 * @brief 从有符号机器整数构造。
 * @param value 数值。
 */
BigInt::BigInt(std::int64_t value) : BigInt() { check(chali_bigint_set_i64(&value_, value)); }
/**
 * @brief 从无符号机器整数构造。
 * @param value 数值。
 * @return 大整数。
 */
BigInt BigInt::fromU64(std::uint64_t value)
{
    BigInt out;
    check(chali_bigint_set_u64(&out.value_, value));
    return out;
}
/**
 * @brief 解析十进制，非法输入保持既有零结果。
 * @param decimal 文本。
 */
BigInt::BigInt(const Memory::String &decimal) : BigInt()
{
    const auto status = chali_bigint_parse(&value_, decimal.data(), decimal.size(), 0);
    if (status != CHALI_C_INVALID_ARGUMENT) check(status);
}
/**
 * @brief 解析严格十进制。
 * @param decimal 文本。
 * @param ok 成功标记。
 * @return 数值或零。
 */
BigInt BigInt::fromString(const Memory::String &decimal, bool *ok)
{
    BigInt out;
    const auto status = chali_bigint_parse(&out.value_, decimal.data(), decimal.size(), 0);
    if (ok) *ok = status == CHALI_C_OK;
    if (status != CHALI_C_INVALID_ARGUMENT) check(status);
    return out;
}
/**
 * @brief 按指定进制解析。
 * @param text 文本。
 * @param base 进制。
 * @param ok 成功标记。
 * @return 数值或零。
 */
BigInt BigInt::fromString(const Memory::String &text, unsigned base, bool *ok)
{
    if (base < 2 || base > 36) throw std::invalid_argument("bigint: 进制必须在 2..36");
    BigInt out;
    const auto status = chali_bigint_parse(&out.value_, text.data(), text.size(), base);
    if (ok) *ok = status == CHALI_C_OK;
    if (status != CHALI_C_INVALID_ARGUMENT) check(status);
    return out;
}
/**
 * @brief 从 C 文本创建既有字符串类型。
 * @param base 进制。
 * @return 独占文本。
 */
Memory::String BigInt::format(unsigned base) const
{
    if (isZero()) return Memory::String("0");
    Text text;
    check(chali_bigint_to_string(&text.value, &value_, base));
    return Memory::String(text.value.data, text.value.length);
}
/**
 * @brief 十进制输出。
 * @return 独占文本。
 */
Memory::String BigInt::toString() const { return format(0); }
/**
 * @brief 按指定进制输出。
 * @param base 进制。
 * @return 独占文本。
 */
Memory::String BigInt::toString(unsigned base) const
{
    if (base < 2 || base > 36) throw std::invalid_argument("bigint: 进制必须在 2..36");
    return format(base);
}
/**
 * @brief 十进制位数，工作区使用当前上下文。
 * @return 位数。
 */
std::size_t BigInt::decimalDigits() const
{
    std::size_t count = 0;
    auto allocator = currentAllocator();
    check(chali_bigint_decimal_digits(&value_, &count, &allocator));
    return count;
}
/**
 * @brief 取幅值低位。
 * @return 幅值低 64 位。
 */
std::uint64_t BigInt::toU64() const { return chali_bigint_to_u64(&value_); }
/**
 * @brief 保持既有幅值低位强转。
 * @return 有符号位模式。
 */
std::int64_t BigInt::toI64() const { return static_cast<std::int64_t>(toU64()); }
/**
 * @brief 显式整数转换。
 * @return 有符号位模式。
 */
BigInt::operator std::int64_t() const { return toI64(); }
/**
 * @brief 浮点近似。
 * @return 近似数。
 */
double BigInt::toDouble() const { return chali_bigint_to_double(&value_); }
/**
 * @brief 显式浮点转换。
 * @return 近似数。
 */
BigInt::operator double() const { return toDouble(); }
/**
 * @brief 绝对值副本。
 * @return 非负副本。
 */
BigInt BigInt::abs() const
{
    BigInt out(*this);
    if (out.value_.sign) out.value_.sign = 1;
    return out;
}
/**
 * @brief 取负副本。
 * @return 相反数。
 */
BigInt BigInt::operator-() const
{
    BigInt out(*this);
    out.value_.sign = -out.value_.sign;
    return out;
}
/**
 * @brief 比较整数。
 * @param other 另一值。
 * @return 三态比较值。
 */
int BigInt::compare(const BigInt &other) const { return chali_bigint_compare(&value_, &other.value_); }

/**
 * @brief 调用 C 有符号算术。
 * @param left 左值。
 * @param right 右值。
 * @return 运算结果。
 */
BigInt operator+(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_add(&out.value_, &left.value_, &right.value_));
    return out;
}
/**
 * @brief 调用 C 有符号算术。
 * @param left 左值。
 * @param right 右值。
 * @return 运算结果。
 */
BigInt operator-(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_subtract(&out.value_, &left.value_, &right.value_));
    return out;
}
/**
 * @brief 调用 C 有符号算术。
 * @param left 左值。
 * @param right 右值。
 * @return 运算结果。
 */
BigInt operator*(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_multiply(&out.value_, &left.value_, &right.value_));
    return out;
}
/**
 * @brief 除法适配。
 * @param left 被除数。
 * @param right 除数。
 * @return 商。
 */
BigInt operator/(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_divmod(&out.value_, nullptr, &left.value_, &right.value_), "bigint: 除以零");
    return out;
}
/**
 * @brief 余数适配。
 * @param left 被除数。
 * @param right 除数。
 * @return 余数。
 */
BigInt operator%(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_divmod(nullptr, &out.value_, &left.value_, &right.value_), "bigint: 除以零");
    return out;
}
/**
 * @brief 复合运算并接管结果。
 * @param other 右值。
 * @return 当前对象。
 */
BigInt &BigInt::operator+=(const BigInt &other)
{
    *this = *this + other;
    return *this;
}
/**
 * @brief 复合运算并接管结果。
 * @param other 右值。
 * @return 当前对象。
 */
BigInt &BigInt::operator-=(const BigInt &other)
{
    *this = *this - other;
    return *this;
}
/**
 * @brief 复合运算并接管结果。
 * @param other 右值。
 * @return 当前对象。
 */
BigInt &BigInt::operator*=(const BigInt &other)
{
    *this = *this * other;
    return *this;
}
/**
 * @brief 复合运算并接管结果。
 * @param other 右值。
 * @return 当前对象。
 */
BigInt &BigInt::operator/=(const BigInt &other)
{
    *this = *this / other;
    return *this;
}
/**
 * @brief 复合运算并接管结果。
 * @param other 右值。
 * @return 当前对象。
 */
BigInt &BigInt::operator%=(const BigInt &other)
{
    *this = *this % other;
    return *this;
}
/**
 * @brief 前置递增。
 * @return 当前对象。
 */
BigInt &BigInt::operator++()
{
    *this += BigInt(1);
    return *this;
}
/**
 * @brief 后置递增。
 * @return 原值。
 */
BigInt BigInt::operator++(int)
{
    BigInt out(*this);
    ++*this;
    return out;
}
/**
 * @brief 前置递减。
 * @return 当前对象。
 */
BigInt &BigInt::operator--()
{
    *this -= BigInt(1);
    return *this;
}
/**
 * @brief 后置递减。
 * @return 原值。
 */
BigInt BigInt::operator--(int)
{
    BigInt out(*this);
    --*this;
    return out;
}
/**
 * @brief 调用 C 比较。
 * @param left 左值。
 * @param right 右值。
 * @return 关系成立时为真。
 */
bool operator==(const BigInt &left, const BigInt &right) { return left.compare(right) == 0; }
/**
 * @brief 调用 C 比较。
 * @param left 左值。
 * @param right 右值。
 * @return 关系成立时为真。
 */
bool operator!=(const BigInt &left, const BigInt &right) { return left.compare(right) != 0; }
/**
 * @brief 调用 C 比较。
 * @param left 左值。
 * @param right 右值。
 * @return 关系成立时为真。
 */
bool operator<(const BigInt &left, const BigInt &right) { return left.compare(right) < 0; }
/**
 * @brief 调用 C 比较。
 * @param left 左值。
 * @param right 右值。
 * @return 关系成立时为真。
 */
bool operator<=(const BigInt &left, const BigInt &right) { return left.compare(right) <= 0; }
/**
 * @brief 调用 C 比较。
 * @param left 左值。
 * @param right 右值。
 * @return 关系成立时为真。
 */
bool operator>(const BigInt &left, const BigInt &right) { return left.compare(right) > 0; }
/**
 * @brief 调用 C 比较。
 * @param left 左值。
 * @param right 右值。
 * @return 关系成立时为真。
 */
bool operator>=(const BigInt &left, const BigInt &right) { return left.compare(right) >= 0; }
/**
 * @brief 商余接口，除零按旧接口返回假。
 * @param divisor 除数。
 * @param quotient 商。
 * @param remainder 余数。
 * @return 除数非零。
 */
bool BigInt::divmod(const BigInt &divisor, BigInt *quotient, BigInt *remainder) const
{
    if (divisor.isZero()) return false;
    if (isZero())
    {
        if (quotient) *quotient = BigInt();
        if (remainder) *remainder = BigInt();
        return true;
    }
    auto allocator = currentAllocator();
    auto status = chali_bigint_divmod_work(quotient ? &quotient->value_ : nullptr, remainder ? &remainder->value_ : nullptr, &value_, &divisor.value_, &allocator);
    if (status == CHALI_C_DOMAIN_ERROR) return false;
    check(status);
    return true;
}
/**
 * @brief 整数快速幂。
 * @param exponent 指数。
 * @return 幂。
 */
BigInt BigInt::pow(std::uint64_t exponent) const
{
    BigInt out;
    check(chali_bigint_pow(&out.value_, &value_, exponent));
    return out;
}
/**
 * @brief 最大公约数。
 * @param left 左值。
 * @param right 右值。
 * @return 公约数。
 */
BigInt BigInt::gcd(const BigInt &left, const BigInt &right)
{
    BigInt out;
    check(chali_bigint_gcd(&out.value_, &left.value_, &right.value_));
    return out;
}
/**
 * @brief 最小公倍数。
 * @param left 左值。
 * @param right 右值。
 * @return 公倍数。
 */
BigInt BigInt::lcm(const BigInt &left, const BigInt &right)
{
    BigInt out;
    check(chali_bigint_lcm(&out.value_, &left.value_, &right.value_));
    return out;
}
/**
 * @brief 非负模。
 * @param modulus 模数。
 * @return 余数。
 */
BigInt BigInt::modNonNegative(const BigInt &modulus) const
{
    BigInt out;
    check(chali_bigint_mod(&out.value_, &value_, &modulus.value_), "bigint: 模数为零");
    return out;
}
/**
 * @brief 大指数模幂。
 * @param exponent 指数。
 * @param modulus 模数。
 * @return 结果。
 */
BigInt BigInt::powmod(const BigInt &exponent, const BigInt &modulus) const
{
    if (modulus.isZero()) throw std::domain_error("bigint: 模数为零");
    if (exponent.isNegative()) throw std::invalid_argument("bigint: 负指数没有模幂（请先求模逆元）");
    BigInt out;
    check(chali_bigint_powmod(&out.value_, &value_, &exponent.value_, &modulus.value_));
    return out;
}
/**
 * @brief 机器指数模幂。
 * @param exponent 指数。
 * @param modulus 模数。
 * @return 结果。
 */
BigInt BigInt::powmod(std::uint64_t exponent, const BigInt &modulus) const
{
    if (modulus.isZero()) throw std::domain_error("bigint: 模数为零");
    return powmod(fromU64(exponent), modulus);
}
/**
 * @brief 扩展欧几里得。
 * @param a 左值。
 * @param b 右值。
 * @return 三元结果。
 */
ExtendedGcd BigInt::extendedGcd(const BigInt &a, const BigInt &b)
{
    ExtendedGcd out;
    check(chali_bigint_extended_gcd(&out.gcd.value_, &out.x.value_, &out.y.value_, &a.value_, &b.value_));
    return out;
}
/**
 * @brief 模逆元。
 * @param value 数值。
 * @param modulus 模数。
 * @return 逆元。
 */
BigInt BigInt::modInverse(const BigInt &value, const BigInt &modulus)
{
    if (modulus.isZero()) throw std::domain_error("bigint: 模数为零");
    BigInt out;
    check(chali_bigint_inverse(&out.value_, &value.value_, &modulus.value_), "bigint: 模逆元不存在（两数不互素）");
    return out;
}
/**
 * @brief 概率素性。
 * @param rounds 轮数。
 * @return 判定。
 */
bool BigInt::isProbablePrime(std::size_t rounds) const
{
    int out = 0;
    auto allocator = currentAllocator();
    check(chali_bigint_is_probable_prime(&value_, rounds, &out, &allocator));
    return out != 0;
}
/**
 * @brief 下一个概率素数。
 * @param rounds 轮数。
 * @return 素数。
 */
BigInt BigInt::nextProbablePrime(std::size_t rounds) const
{
    BigInt out;
    check(chali_bigint_next_probable_prime(&out.value_, &value_, rounds));
    return out;
}
/**
 * @brief 调用 C 非负位运算。
 * @param left 左值。
 * @param right 右值。
 * @return 位运算结果。
 */
BigInt operator&(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_bitwise(&out.value_, &left.value_, &right.value_, 0), "bigint: 位运算只支持非负整数");
    return out;
}
/**
 * @brief 调用 C 非负位运算。
 * @param left 左值。
 * @param right 右值。
 * @return 位运算结果。
 */
BigInt operator|(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_bitwise(&out.value_, &left.value_, &right.value_, 1), "bigint: 位运算只支持非负整数");
    return out;
}
/**
 * @brief 调用 C 非负位运算。
 * @param left 左值。
 * @param right 右值。
 * @return 位运算结果。
 */
BigInt operator^(const BigInt &left, const BigInt &right)
{
    BigInt out;
    BigInt::check(chali_bigint_bitwise(&out.value_, &left.value_, &right.value_, 2), "bigint: 位运算只支持非负整数");
    return out;
}
/**
 * @brief 保留补码等式定义。
 * @return 取反结果。
 */
BigInt BigInt::operator~() const { return -*this - BigInt(1); }
/**
 * @brief 查询有效位数。
 * @return 位数。
 */
std::size_t BigInt::bitLength() const { return chali_bigint_bit_length(&value_); }
/**
 * @brief 查询幅值位。
 * @param bit 位号。
 * @return 位值。
 */
bool BigInt::bitAt(std::size_t bit) const { return chali_bigint_bit_at(&value_, bit) != 0; }
/**
 * @brief 设置位。
 * @param bit 位号。
 */
void BigInt::setBit(std::size_t bit) { check(chali_bigint_change_bit(&value_, bit, 1)); }
/**
 * @brief 清除位。
 * @param bit 位号。
 */
void BigInt::clearBit(std::size_t bit) { check(chali_bigint_change_bit(&value_, bit, 0)); }
/**
 * @brief 翻转位。
 * @param bit 位号。
 */
void BigInt::flipBit(std::size_t bit) { check(chali_bigint_change_bit(&value_, bit, 2)); }
/**
 * @brief 左移幅值。
 * @param bits 位数。
 */
void BigInt::shiftLeft(std::size_t bits)
{
    if (!isZero() && bits && value_.limbs.allocator.context != Memory::currentRuntimeAllocator())
    {
        BigInt shifted(*this);
        check(chali_bigint_shift_left(&shifted.value_, bits));
        *this = std::move(shifted);
    }
    else
        check(chali_bigint_shift_left(&value_, bits));
}
/**
 * @brief 右移幅值。
 * @param bits 位数。
 */
void BigInt::shiftRight(std::size_t bits)
{
    if (!isZero() && bits && bits / 32 < value_.limbs.length && value_.limbs.allocator.context != Memory::currentRuntimeAllocator())
    {
        BigInt shifted(*this);
        chali_bigint_shift_right(&shifted.value_, bits);
        *this = std::move(shifted);
    }
    else
        chali_bigint_shift_right(&value_, bits);
}
/**
 * @brief 设置 NTT 阈值。
 * @param limbs 阈值。
 */
void BigInt::setMultiplicationThreshold(std::size_t limbs) { chali_bigint_set_multiplication_threshold(limbs); }
/**
 * @brief 读取 NTT 阈值。
 * @return 阈值。
 */
std::size_t BigInt::multiplicationThreshold() { return chali_bigint_multiplication_threshold(); }
/**
 * @brief 设置 Karatsuba 阈值。
 * @param limbs 阈值。
 */
void BigInt::setKaratsubaThreshold(std::size_t limbs) { chali_bigint_set_karatsuba_threshold(limbs); }
/**
 * @brief 读取 Karatsuba 阈值。
 * @return 阈值。
 */
std::size_t BigInt::karatsubaThreshold() { return chali_bigint_karatsuba_threshold(); }
/**
 * @brief 设置除法阈值。
 * @param limbs 阈值。
 */
void BigInt::setDivisionThreshold(std::size_t limbs) { chali_bigint_set_division_threshold(limbs); }
/**
 * @brief 读取除法阈值。
 * @return 阈值。
 */
std::size_t BigInt::divisionThreshold() { return chali_bigint_division_threshold(); }
/**
 * @brief 整数平方根。
 * @param value 非负数。
 * @return 根。
 */
BigInt integerSqrt(const BigInt &value)
{
    BigInt out;
    BigInt::check(chali_bigint_sqrt(&out.value_, &value.value_), "bigint: 负数没有整数平方根");
    return out;
}
/**
 * @brief 整数 k 次根。
 * @param value 非负数。
 * @param degree 根次。
 * @return 根。
 */
BigInt integerRoot(const BigInt &value, std::uint64_t degree)
{
    if (!degree) throw std::invalid_argument("bigint: 根次必须为正");
    BigInt out;
    BigInt::check(chali_bigint_root(&out.value_, &value.value_, degree), "bigint: 负数没有整数 k 次方根");
    return out;
}
/**
 * @brief 十进制缩放因子，运算仍由 C 快速幂完成。
 * @param exponent 指数。
 * @return 十的幂。
 */
BigInt pow10(std::uint64_t exponent) { return BigInt(10).pow(exponent); }
}  // namespace bigint

#endif
