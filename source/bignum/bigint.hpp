#include "../../source/allocator/chali_allocator.hpp"
/**
 * @file bigint.hpp
 * @brief C11 大整数内核的 C++17 所有权、运算符和异常兼容接口。
 *
 * 基 2^32 小端幅值；预算分配经 C 回调转接原所有者。
 * 乘法组合学校算法、Karatsuba、Toom-Cook 3 与双素数 NTT+CRT；除法沿用 Knuth / Burnikel-Ziegler。
 * 运算实现位于 chali_bigint*.c；本接口不包含独立的 C++ 大整数算法。
 */

#ifndef PARSE_ENGINE_BIGINT_HPP
#define PARSE_ENGINE_BIGINT_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include "chali_bigint.h"

namespace bigfloat
{
struct CFloatAccess;
}
namespace bigint
{

struct ExtendedGcd;  ///< 扩展欧几里得结果（定义在类后）。

/**
 * @brief 任意精度有符号大整数。
 */
class BigInt
{
public:
    /**
     * @brief 造一个 0。
     */
    BigInt();
    /**
     * @brief 复制到当前分配上下文。
     * @param other 来源。
     */
    BigInt(const BigInt &other);
    /**
     * @brief 接管原分配所有权。
     * @param other 来源。
     */
    BigInt(BigInt &&other) noexcept;
    /** @brief 释放 C 内核的存储。 */
    ~BigInt();
    /**
     * @brief 保留目标所有者复制。
     * @param other 来源。
     * @return 当前对象。
     */
    BigInt &operator=(const BigInt &other);
    /**
     * @brief 接管原所有者移动。
     * @param other 来源。
     * @return 当前对象。
     */
    BigInt &operator=(BigInt &&other) noexcept;

    /**
     * @brief 从 64 位有符号整数构造（`INT64_MIN` 也正确）。
     *
     * **隐式转换**：`int64 → BigInt` 直接赋值 / 传参可用。
     * @param value 值。
     */
    BigInt(std::int64_t value);

    /**
     * @brief 显式转 64 位有符号整数（按位截断，与 `toI64` 一致）。
     *
     * 收窄必须**显式**写：`static_cast<std::int64_t>(value)`。
     */
    explicit operator std::int64_t() const;

    /**
     * @brief 显式转双精度浮点（可能丢精度，与 `toDouble` 一致）。
     */
    explicit operator double() const;

    /**
     * @brief 从 64 位**无符号**整数构造（`uint64_t` 上界也不会被当成 -1）。
     *
     * 与参考实现的 `EigenMath_BigIntFromU64` 对应：`int64_t` 构造只吃有符号范围，
     * 大于 `INT64_MAX` 的无符号值请显式用这个。
     * @param value 值。
     */
    static BigInt fromU64(std::uint64_t value);

    /**
     * @brief 从十进制字符串构造（可带 `+` / `-` 号，允许前导 0）。
     * @param decimal 十进制文本。
     * @throws std::invalid_argument 文本不是合法十进制整数时。
     */
    explicit BigInt(const Memory::String &decimal);

    /**
     * @brief 从十进制字符串构造（不抛异常版）。
     * @param decimal 十进制文本。
     * @param ok 非空时写入是否解析成功。
     * @return 解析成功时的值；失败返回 0。
     */
    static BigInt fromString(const Memory::String &decimal, bool *ok = nullptr);

    /**
     * @brief 十进制字符串（负数带 `-`；0 输出 `"0"`）。
     */
    [[nodiscard]] Memory::String toString() const;

    /**
     * @brief 是不是 0。
     */
    [[nodiscard]] bool isZero() const { return value_.sign == 0; }

    /**
     * @brief 符号：-1 / 0 / 1。
     */
    [[nodiscard]] int sign() const { return value_.sign; }

    /**
     * @brief 十进制位数（0 算 1 位）。
     */
    [[nodiscard]] std::size_t decimalDigits() const;

    /**
     * @brief 有效 limb（32 位）个数；0 是 0 个。
     */
    [[nodiscard]] std::size_t limbCount() const { return value_.limbs.length; }

    /**
     * @brief NTT 候选下限：两边 limb 数之和达到它后再比较补齐成本。
     *
     * 默认 6144；它是候选下限，仍按变换补齐规模比较成本；`1` 强制可支持的多块输入走 NTT。
     * @param limbs 两侧块数之和的候选下限。
     */
    static void setMultiplicationThreshold(std::size_t limbs);
    /**
     * @brief 当前 NTT 阈值。
     * @return 自动分派开始比较 NTT 成本的两侧合计块数。
     */
    [[nodiscard]] static std::size_t multiplicationThreshold();

    /**
     * @brief NTT 的短侧保护阈值：较短操作数不超过该 limb 数时保留第一阶段算法。
     *
     * 默认 96；不再控制学校算法基线，0 按 1 处理；强制 NTT 模式优先。
     * @param limbs 较短操作数不参与 NTT 成本比较的最大块数。
     */
    static void setKaratsubaThreshold(std::size_t limbs);
    /**
     * @brief 查询兼容接口保存的 NTT 短侧保护阈值。
     * @return 自动分派保留第一阶段算法的短侧块数上限。
     */
    [[nodiscard]] static std::size_t karatsubaThreshold();

    /**
     * @brief Burnikel-Ziegler 除法的递归基线；除数不超过该 limb 数时使用长除法。
     *
     * 默认 256、最小 32；设置为很大可强制使用长除法。
     * @param limbs 使用 Knuth 长除法的最大除数块数。
     */
    static void setDivisionThreshold(std::size_t limbs);
    /**
     * @brief 当前 Burnikel-Ziegler 递归基线。
     * @return 使用 Knuth 长除法的除数块数上限。
     */
    [[nodiscard]] static std::size_t divisionThreshold();

    /**
     * @brief 截断成 64 位无符号整数（只取低 64 位，符号位丢弃）。
     */
    [[nodiscard]] std::uint64_t toU64() const;

    /**
     * @brief 截断成 64 位有符号整数（按位截断，与 C 的强转一致）。
     */
    [[nodiscard]] std::int64_t toI64() const;

    /**
     * @brief 转双精度浮点（可能丢精度；符号跟着走）。
     */
    [[nodiscard]] double toDouble() const;

    /**
     * @brief 绝对值。
     */
    [[nodiscard]] BigInt abs() const;

    /**
     * @brief 比较。
     * @return 小于 / 等于 / 大于分别返回 -1 / 0 / 1。
     */
    [[nodiscard]] int compare(const BigInt &other) const;

    // ---- 算术（都是值语义，不修改操作数）----
    friend BigInt operator+(const BigInt &left, const BigInt &right);
    friend BigInt operator-(const BigInt &left, const BigInt &right);
    friend BigInt operator*(const BigInt &left, const BigInt &right);
    /**
     * @throws std::domain_error 除数为零时。
     */
    friend BigInt operator/(const BigInt &left, const BigInt &right);
    /**
     * @throws std::domain_error 除数为零时。
     */
    friend BigInt operator%(const BigInt &left, const BigInt &right);

    /**
     * @brief 一元负号。
     */
    [[nodiscard]] BigInt operator-() const;

    BigInt &operator+=(const BigInt &other);
    BigInt &operator-=(const BigInt &other);
    BigInt &operator*=(const BigInt &other);
    BigInt &operator/=(const BigInt &other);
    BigInt &operator%=(const BigInt &other);

    BigInt &operator++();    ///< 前置 ++（`++a`）。
    BigInt operator++(int);  ///< 后置 ++（`a++`）。
    BigInt &operator--();    ///< 前置 --。
    BigInt operator--(int);  ///< 后置 --。

    friend bool operator==(const BigInt &left, const BigInt &right);
    friend bool operator!=(const BigInt &left, const BigInt &right);
    friend bool operator<(const BigInt &left, const BigInt &right);
    friend bool operator<=(const BigInt &left, const BigInt &right);
    friend bool operator>(const BigInt &left, const BigInt &right);
    friend bool operator>=(const BigInt &left, const BigInt &right);

    /**
     * @brief 同号幂：`*this ^ exponent`（二进制快速幂，`0^0 = 1`）。
     * @param exponent 指数（无符号）。
     */
    [[nodiscard]] BigInt pow(std::uint64_t exponent) const;

    /**
     * @brief 除取整：`this / divisor` 与余数（向零截断，`this = q × divisor + r`）。
     * @param divisor 除数。
     * @param quotient 非空时写入商。
     * @param remainder 非空时写入余数。
     * @return 除数为零返回 false（此时不写输出）。
     */
    bool divmod(const BigInt &divisor, BigInt *quotient, BigInt *remainder) const;

    /**
     * @brief 最大公约数（欧几里得，取绝对值；`gcd(0, 0) = 0`）。
     */
    [[nodiscard]] static BigInt gcd(const BigInt &left, const BigInt &right);

    /**
     * @brief 最小公倍数（`lcm(0, x) = 0`）。
     */
    [[nodiscard]] static BigInt lcm(const BigInt &left, const BigInt &right);

    /**
     * @brief 同余幂：`base ^ exponent mod modulus`（快速幂，逐步取模；模为零抛异常）。
     * @param exponent 指数（无符号）。
     * @param modulus 模数。
     */
    [[nodiscard]] BigInt powmod(std::uint64_t exponent, const BigInt &modulus) const;

    /**
     * @brief 同余幂（**指数也是大整数**的快速幂）：`base ^ exponent mod modulus`。
     *
     * 素性测试等"指数大到装不进 64 位"的场景用这个；逐步取模，
     * 结果落在 `[0, |modulus|)`。模为零抛 `std::domain_error`。
     * @param exponent 指数（大整数，负指数抛 `std::invalid_argument`）。
     * @param modulus 模数。
     */
    [[nodiscard]] BigInt powmod(const BigInt &exponent, const BigInt &modulus) const;

    // ---- 位运算 / 位查询 ----
    // 位语义只对**非负整数**定义（内部是"符号 + 幅值"表示，做带符号补码位运算
    // 得先把两个操作数都扩到无穷位，不值当）；操作数为负时抛 `std::domain_error`。

    /**
     * @brief 按位与（两个操作数都非负；负数抛 `std::domain_error`）。
     */
    friend BigInt operator&(const BigInt &left, const BigInt &right);
    /**
     * @brief 按位或（两个操作数都非负；负数抛 `std::domain_error`）。
     */
    friend BigInt operator|(const BigInt &left, const BigInt &right);
    /**
     * @brief 按位异或（两个操作数都非负；负数抛 `std::domain_error`）。
     */
    friend BigInt operator^(const BigInt &left, const BigInt &right);
    /**
     * @brief 按位取反：补口语义 `~x = -x - 1`（只此一个允许负数结果）。
     */
    [[nodiscard]] BigInt operator~() const;

    /**
     * @brief 二进制位长度（0 返回 0）。
     */
    [[nodiscard]] std::size_t bitLength() const;
    /**
     * @brief 取第 bit 位是不是 1。
     */
    [[nodiscard]] bool bitAt(std::size_t bit) const;
    /**
     * @brief 把第 bit 位置 1。
     */
    void setBit(std::size_t bit);
    /**
     * @brief 把第 bit 位清 0。
     */
    void clearBit(std::size_t bit);
    /**
     * @brief 把第 bit 位取反。
     */
    void flipBit(std::size_t bit);
    /**
     * @brief 左移 bits 位（就地）。
     */
    void shiftLeft(std::size_t bits);
    /**
     * @brief 右移 bits 位（就地，向零截断）。
     */
    void shiftRight(std::size_t bits);

    // ---- 判断 / 最值 ----

    /**
     * @brief 是不是偶数（0 算偶数）。
     */
    [[nodiscard]] bool isEven() const { return (value_.limbs.length == 0) || (static_cast<const std::uint32_t *>(value_.limbs.data)[0] & 1u) == 0u; }
    /**
     * @brief 是不是奇数。
     */
    [[nodiscard]] bool isOdd() const { return !(value_.limbs.length == 0) && (static_cast<const std::uint32_t *>(value_.limbs.data)[0] & 1u) != 0u; }
    /**
     * @brief 是不是负数。
     */
    [[nodiscard]] bool isNegative() const { return value_.sign < 0; }
    /**
     * @brief 是不是正数（0 不算）。
     */
    [[nodiscard]] bool isPositive() const { return value_.sign > 0; }

    /**
     * @brief 两者中较小的一个。
     */
    [[nodiscard]] static const BigInt &min(const BigInt &left, const BigInt &right)
    {
        return (left.compare(right) <= 0) ? left : right;
    }
    /**
     * @brief 两者中较大的一个。
     */
    [[nodiscard]] static const BigInt &max(const BigInt &left, const BigInt &right)
    {
        return (left.compare(right) >= 0) ? left : right;
    }

    /**
     * @brief 非负取模：结果落在 `[0, |modulus|)`（`operator%` 的余数符号跟被除数，
     * 要"数学上"的余数用这个）。模为零抛 `std::domain_error`。
     */
    [[nodiscard]] BigInt modNonNegative(const BigInt &modulus) const;

    /**
     * @brief 扩展欧几里得：同时给出 `gcd(a, b)` 与裴蜀系数 `x`、`y`。
     *
     * 返回的 `g` 非负；`x`、`y` 按带符号的 `a`、`b` 计算（即恒有
     * `a × x + b × y = g`）。
     */
    [[nodiscard]] static ExtendedGcd extendedGcd(const BigInt &a, const BigInt &b);

    /**
     * @brief 模逆元：`value × inverse ≡ 1 (mod modulus)`。
     *
     * 模为零抛 `std::domain_error`；`gcd(|value|, |modulus|) ≠ 1`（不可逆）抛
     * `std::domain_error`。结果落在 `[0, |modulus|)`。
     */
    [[nodiscard]] static BigInt modInverse(const BigInt &value, const BigInt &modulus);

    /**
     * @brief 概率素性测试（Miller-Rabin）。
     *
     * 底数由固定种子的 xorshift 生成，同一输入多轮测试**完全确定**（可复现）。
     * 缺省 25 轮，误判概率远小于实际使用需要；`rounds = 0` 等价于只做试除。
     * @param rounds Miller-Rabin 轮数。
     */
    [[nodiscard]] bool isProbablePrime(std::size_t rounds = 25) const;

    /**
     * @brief 严格大于当前值的下一个（概率）素数；当前值已是素数也不会返回它自己。
     */
    [[nodiscard]] BigInt nextProbablePrime(std::size_t rounds = 25) const;

    /**
     * @brief `base`（2..36）进制字符串：数字用 `0-9a-z`，负数带 `-`，0 输出 `"0"`。
     * @param base 进制；不在 2..36 抛 `std::invalid_argument`。
     */
    [[nodiscard]] Memory::String toString(unsigned int base) const;

    /**
     * @brief 从 `base`（2..36）进制字符串构造（不抛异常版）。
     *
     * 可带 `+` / `-` 号与前后空白；字母不分大小写。文本里有不属于该进制的
     * 字符时返回 0 并把 `*ok` 写成 false。
     * @param text 文本。
     * @param base 进制；不在 2..36 抛 `std::invalid_argument`。
     * @param ok 非空时写入是否解析成功。
     */
    static BigInt fromString(const Memory::String &text, unsigned int base, bool *ok = nullptr);

    /**
     * @brief 借用只读 C 数值视图，供数值与符号服务互操作。
     * @return 有效期与当前整数一致的 C 存储，调用方不得修改或释放。
     */
    [[nodiscard]] const ChaliBigInt &cValue() const noexcept { return value_; }

private:
    friend struct bigfloat::CFloatAccess;
    ChaliBigInt value_{};
    /**
     * @brief 将 C 内核状态恢复为原有异常协议。
     * @param status C 内核返回状态。
     * @param message 失败诊断。
     */
    static void check(ChaliCStatus status, const char *message = "bigint: 数值操作失败");
    /**
     * @brief 把 C 内核的文本复制为调用方拥有的字符串。
     * @param base 进制，零表示默认十进制协议。
     * @return 按当前预算分配的字符串。
     */
    Memory::String format(unsigned base) const;
    friend BigInt integerSqrt(const BigInt &value);
    friend BigInt integerRoot(const BigInt &value, std::uint64_t degree);
};

/**
 * @brief 扩展欧几里得的结果：`a × x + b × y = g = gcd(a, b)`。
 */
struct ExtendedGcd
{
    BigInt gcd;  ///< 最大公约数（非负）。
    BigInt x;    ///< 满足 `a × x + b × y = gcd(a, b)` 的系数。
    BigInt y;    ///< 同上。
};

/**
 * @brief 整数平方根（向下取整）：二分 / 牛顿迭代，只用大整数的加减乘除。
 *
 * 大浮点的 `sqrt` 靠它（先放大到 2×digits 位有效数字再开方）。
 * @param value 非负整数；为负时抛 `std::domain_error`。
 */
[[nodiscard]] BigInt integerSqrt(const BigInt &value);

/**
 * @brief 整数 `k` 次方根（向下取整）：二分，只用大整数的加减乘除与幂。
 *
 * `k = 2` 等价于 `integerSqrt`。`k = 0` 抛 `std::invalid_argument`；
 * `value` 为负抛 `std::domain_error`。
 * @param value 非负整数。
 * @param k 根次（≥ 1）。
 */
[[nodiscard]] BigInt integerRoot(const BigInt &value, std::uint64_t k);

/**
 * @brief 10 的 `exponent` 次幂（大浮点对齐指数时大量使用；幂用 NTT 乘法）。
 */
[[nodiscard]] BigInt pow10(std::uint64_t exponent);

}  // namespace bigint

#endif  // PARSE_ENGINE_BIGINT_HPP
