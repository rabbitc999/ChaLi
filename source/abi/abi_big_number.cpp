#include "chali_standard.h"
#include "allocator/chali_allocator.hpp"
#include <memory>
#include <cstring>
#include <limits>
#include <type_traits>
#ifndef STACKVM_WITH_BIGINT
#define STACKVM_WITH_BIGINT 1
#endif
#ifndef STACKVM_WITH_BIGFLOAT
#define STACKVM_WITH_BIGFLOAT STACKVM_WITH_BIGINT
#endif
#if STACKVM_WITH_BIGFLOAT && !STACKVM_WITH_BIGINT
#error "numeric glue requires the integer backend for decimal arithmetic"
#endif

#if STACKVM_WITH_BIGINT
#include "bigint.hpp"
#endif
#if STACKVM_WITH_BIGFLOAT
#include "bigfloat.hpp"
#include "bigfloat_math.hpp"
#endif
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <stdexcept>

/**
 * @brief 大数的宿主记录、复制协议与操作实现，语言运行时不持有其数值布局。
 */
namespace
{
/** @brief 从受检查帧读取整数位模式。
 * @param frame 当前帧。
 * @param index 参数位置。
 * @return 整数或按既有规则向零取整的浮点值。
 */
int64_t integerArgument(const ChaliAbiFrame *frame, int64_t index)
{
    int64_t value = 0;
    if (!chali_abi_argument_integer(frame, index, &value)) throw std::invalid_argument("大数参数不是整数槽");
    return value;
}
/** @brief 从受检查帧读取浮点参数。
 * @param frame 当前帧。
 * @param index 参数位置。
 * @return 浮点值。
 */
double floatingArgument(const ChaliAbiFrame *frame, int64_t index)
{
    double value = 0;
    if (!chali_abi_argument_real(frame, index, &value)) throw std::invalid_argument("大数参数不是数值槽");
    return value;
}
/** @brief 复制 C 字符串视图，不将借用带过后续分配。
 * @param frame 当前帧。
 * @param address 文本句柄。
 * @param out 接收独立文本。
 * @return 成功为真。
 */
bool readText(const ChaliAbiFrame *frame, size_t address, Memory::String *out)
{
    const unsigned char *data = nullptr;
    size_t size = 0;
    if (!chali_abi_bytes(frame, address, &data, &size)) return false;
    out->assign(data ? reinterpret_cast<const char *>(data) : "", size);
    return true;
}
/** @brief 创建 C 协议拥有型文本。
 * @param frame 当前帧。
 * @param text 原始文本。
 * @return 文本句柄，失败为零。
 */
int64_t makeText(const ChaliAbiFrame *frame, const Memory::String &text) { return chali_abi_string(frame, text.data(), text.size()); }
/** @brief 从现有运行时输入流消费一项并立即回收临时 String。
 * @param frame 当前帧。
 * @param text 接收独立文本。
 * @param position 接收输入位置。
 * @return 有下一项且读取成功为真。
 */
bool takeInput(const ChaliAbiFrame *frame, Memory::String &text, uint64_t *position)
{
    const auto handle = chali_abi_input_token(frame, position);
    if (!handle) return false;
    try
    {
        const bool ok = readText(frame, handle, &text);
        chali_abi_string_release(frame, handle);
        return ok;
    } catch (...)
    {
        chali_abi_string_release(frame, handle);
        throw;
    }
}

#if STACKVM_WITH_BIGINT
/**
 * @brief 胶水独占的大数记录，复制构造产生独立数值。
 */
struct BigIntRecord
{
    bigint::BigInt number;
};
/** @brief 复制独立数值记录；异常在 C 回调内转为空指针。
 * @param source 存活记录。
 * @return 新副本，失败为空。
 */
void *cloneBigIntRecord(const void *source)
try
{
    return new BigIntRecord(*static_cast<const BigIntRecord *>(source));
} catch (...) { return nullptr; }
/** @brief 销毁拥有的数值记录。
 * @param record 待销毁对象。
 */
void destroyBigIntRecord(void *record) { delete static_cast<BigIntRecord *>(record); }
/** @brief 查询或有界写入数值的默认十进制文本。
 * @param record 存活记录。
 * @param data 写入缓冲，查询时为空。
 * @param capacity 缓冲容量。
 * @param required 接收所需字节数。
 * @return 成功为一；异常或容量不足为零。
 */
int textBigIntRecord(const void *record, char *data, size_t capacity, size_t *required)
try
{
    if (!record || !required) return 0;
    const auto *value = static_cast<const BigIntRecord *>(record);
    const auto text = value->number.toString();
    *required = text.size();
    if (!data) return capacity == 0;
    if (capacity < text.size()) return 0;
    if (!text.empty()) std::memcpy(data, text.data(), text.size());
    return 1;
} catch (...) { return 0; }
/** @brief 通过 C 句柄借用实际数值内核，保留操作名称的诊断。
 * @param ctx 当前 C 帧。
 * @param handle 存活句柄。
 * @param who 操作名称。
 * @return 借用指针，失败为空。
 */
BigIntRecord *bigIntRecord(ChaliAbiFrame *ctx, int64_t handle, const char *who)
{
    auto *value = static_cast<BigIntRecord *>(chali_abi_record_probe(ctx, handle, "BigInteger"));
    if (!value) chali_abi_fail(ctx, (Memory::String(who) + " 用的 BigInteger 已经释放过了（句柄 " + Memory::toString(handle) + "）").c_str());
    return value;
}

#endif
#if STACKVM_WITH_BIGFLOAT
/**
 * @brief 胶水独占的大数记录，复制构造产生独立数值。
 */
struct BigFloatRecord
{
    bigfloat::BigFloat decimal;
    int decimalPrecision = 0;
};
/** @brief 复制独立数值记录；异常在 C 回调内转为空指针。
 * @param source 存活记录。
 * @return 新副本，失败为空。
 */
void *cloneBigFloatRecord(const void *source)
try
{
    return new BigFloatRecord(*static_cast<const BigFloatRecord *>(source));
} catch (...) { return nullptr; }
/** @brief 销毁拥有的数值记录。
 * @param record 待销毁对象。
 */
void destroyBigFloatRecord(void *record) { delete static_cast<BigFloatRecord *>(record); }
/** @brief 查询或有界写入数值的默认十进制文本。
 * @param record 存活记录。
 * @param data 写入缓冲，查询时为空。
 * @param capacity 缓冲容量。
 * @param required 接收所需字节数。
 * @return 成功为一；异常或容量不足为零。
 */
int textBigFloatRecord(const void *record, char *data, size_t capacity, size_t *required)
try
{
    if (!record || !required) return 0;
    const auto *value = static_cast<const BigFloatRecord *>(record);
    const auto text = value->decimal.toString(0);
    *required = text.size();
    if (!data) return capacity == 0;
    if (capacity < text.size()) return 0;
    if (!text.empty()) std::memcpy(data, text.data(), text.size());
    return 1;
} catch (...) { return 0; }
/** @brief 通过 C 句柄借用实际数值内核，保留操作名称的诊断。
 * @param ctx 当前 C 帧。
 * @param handle 存活句柄。
 * @param who 操作名称。
 * @return 借用指针，失败为空。
 */
BigFloatRecord *bigFloatRecord(ChaliAbiFrame *ctx, int64_t handle, const char *who)
{
    auto *value = static_cast<BigFloatRecord *>(chali_abi_record_probe(ctx, handle, "BigDecimal"));
    if (!value) chali_abi_fail(ctx, (Memory::String(who) + " 用的 BigDecimal 已经释放过了（句柄 " + Memory::toString(handle) + "）").c_str());
    return value;
}

#endif
#if !STACKVM_WITH_BIGINT
/**
 * @brief 为未编入大整数胶水的构建返回原始裁剪诊断。
 * @param ctx 本次调用状态。
 * @return 零占位值。
 */
inline int64_t bigIntDisabled(ChaliAbiFrame *ctx)
{
    chali_abi_fail(ctx, (Memory::String("这个构建没有把大整数编进来（附加功能）：用 STACKVM_WITH_BIGINT=1 重建宿主胶水")).c_str());
    return (0);
}
#endif
#if STACKVM_WITH_BIGINT

/**
 * @brief 收下一个算好的大整数（并返回它的句柄）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @param value 待转换、保存或参与计算的值。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t nativeNewBigInt(ChaliAbiFrame *ctx, bigint::BigInt value)
{
    auto *record = new BigIntRecord{ std::move(value) };
    std::unique_ptr<BigIntRecord> owner(record);
    const auto handle = chali_abi_record_make_textual(ctx, "BigInteger", record, destroyBigIntRecord, cloneBigIntRecord, textBigIntRecord);
    if (handle) owner.release();
    return (handle);
}

/* ---- 大整数（bint）---- */

/**
 * @brief 执行大整数从整数构造并封装运行时结果。
 *
 * `__std_bint_from_int(值) -> bint`。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintFromInt(ChaliAbiFrame *ctx)
{
    return nativeNewBigInt(ctx, bigint::BigInt((ctx->kinds[0] == ChaliAbiSlotFloat) ? static_cast<std::int64_t>(floatingArgument(ctx, 0)) : integerArgument(ctx, 0)));
}

/**
 * @brief 从无符号整数完整构造大数，保留 uint64 的高位。
 * @param ctx 原生调用上下文。
 * @retval 新建的大数对象句柄。
 */
inline int64_t bintFromUInt(ChaliAbiFrame *ctx)
{
    return nativeNewBigInt(ctx, bigint::BigInt::fromU64(static_cast<std::uint64_t>(integerArgument(ctx, 0))));
}

/**
 * @brief 执行大整数从文本构造并封装运行时结果。
 *
 * `__std_bint_from_str(十进制文本地址) -> bint`。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintFromStr(ChaliAbiFrame *ctx)
{
    Memory::String text;
    if (!readText(ctx, static_cast<std::size_t>(integerArgument(ctx, 0)), &text))
    {
        chali_abi_fail(ctx, (Memory::String("构造大整数时读不到文本（地址 " + Memory::toString(integerArgument(ctx, 0)) + "）")).c_str());
        return (0);
    }
    bool ok = false;
    const bigint::BigInt value = bigint::BigInt::fromString(text, &ok);
    if (!ok)
    {
        chali_abi_fail(ctx, (Memory::String("不是合法的大整数文本：\"" + text + "\"（只认可选的 +/- 号加十进制数字）")).c_str());
        return (0);
    }
    return nativeNewBigInt(ctx, value);
}

/**
 * @brief 执行大整数从浮点数构造并封装运行时结果。
 *
 * `__std_bint_from_flt(值) -> bint`（按向零截断）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintFromFloat(ChaliAbiFrame *ctx)
{
    const double value = (ctx->kinds[0] == ChaliAbiSlotFloat) ? floatingArgument(ctx, 0) : static_cast<double>(integerArgument(ctx, 0));
    return nativeNewBigInt(ctx, bigint::BigInt(static_cast<std::int64_t>(value)));
}

/**
 * @brief 执行大整数算术运算并封装运行时结果。
 *
 * 大整数的 `+ - * / %`：取两个句柄 → 算 → 把结果装进**新的**对象。
 * @param op `+` / `-` / `*` / `/` / `%`。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintArith(ChaliAbiFrame *ctx, char op)
{
    auto *left = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数运算");
    if (left == nullptr) return (0);
    auto *right = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数运算");
    if (right == nullptr) return (0);
    if ((op == '/' || op == '%') && right->number.isZero())
    {
        chali_abi_fail(ctx, (Memory::String(Memory::String("大整数 ") + op + " 0：除数是零")).c_str());
        return (0);
    }
    switch (op)
    {
    case '+': return nativeNewBigInt(ctx, left->number + right->number);
    case '-': return nativeNewBigInt(ctx, left->number - right->number);
    case '*': return nativeNewBigInt(ctx, left->number * right->number);
    case '/': return nativeNewBigInt(ctx, left->number / right->number);
    default: return nativeNewBigInt(ctx, left->number % right->number);
    }
}

/**
 * @brief 执行大整数加法并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintAdd(ChaliAbiFrame *ctx) { return bintArith(ctx, '+'); }
/**
 * @brief 执行大整数减法并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintSub(ChaliAbiFrame *ctx) { return bintArith(ctx, '-'); }
/**
 * @brief 执行大整数乘法并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintMul(ChaliAbiFrame *ctx) { return bintArith(ctx, '*'); }
/**
 * @brief 执行大整数除法并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintDiv(ChaliAbiFrame *ctx) { return bintArith(ctx, '/'); }
/**
 * @brief 执行大整数取余并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintMod(ChaliAbiFrame *ctx) { return bintArith(ctx, '%'); }

/**
 * @brief 执行大整数取反并封装运行时结果。
 *
 * `__std_bint_neg(值) -> bint`：取负。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintNeg(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数取负");
    if (value == nullptr) return (0);
    return nativeNewBigInt(ctx, -value->number);
}

/**
 * @brief 执行大整数比较并封装运行时结果。
 *
 * `__std_bint_cmp(左, 右) -> -1 / 0 / 1`。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintCmp(ChaliAbiFrame *ctx)
{
    auto *left = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数比较");
    if (left == nullptr) return (0);
    auto *right = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数比较");
    if (right == nullptr) return (0);
    if (left->number < right->number) return (-1);
    if (right->number < left->number) return (1);
    return (0);
}

/**
 * @brief 执行大整数转换为文本并封装运行时结果。
 *
 * `__std_bint_str(值) -> 新串地址`：转成十进制文本（`.str()`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintToStr(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数转字符串");
    if (value == nullptr) return (0);
    return (static_cast<std::int64_t>(makeText(ctx, value->number.toString())));
}

/**
 * @brief 大整数 → 64 位整数：取**低 64 位**（二进制补码截断，和 C 的整数转换一个脾气）。
 *
 * 不能借 `toDouble()` 绕（`double` 只有 53 位有效位，`(int)9223372036854775807` 会
 * 因为四舍五入溢出成 0）；这里按 2^32 进制两位两位地取余数拼出来。
 * @param value 大整数。
 * @return 值。
 */
inline std::int64_t bigIntToInt64(const bigint::BigInt &value)
{
    const bigint::BigInt base(4294967296LL); /* 2^32 */
    bigint::BigInt current = value.abs();
    std::uint64_t low = 0u;
    for (int index = 0; index < 2; ++index)
    {
        bigint::BigInt quotient;
        bigint::BigInt remainder;
        if (!current.divmod(base, &quotient, &remainder)) break;
        low |= static_cast<std::uint64_t>(remainder.toDouble()) << (32 * index);
        current = quotient;
    }
    const std::int64_t bits = static_cast<std::int64_t>(low);
    return (value.sign() < 0) ? -bits : bits;
}

/**
 * @brief 执行大整数转换为整数并封装运行时结果。
 *
 * `__std_bint_to_int(值) -> 整数`（超出 64 位取低 64 位）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintToInt(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数转 int");
    if (value == nullptr) return (0);
    return (bigIntToInt64(value->number));
}

/**
 * @brief 执行大整数转换为浮点数并封装运行时结果。
 *
 * `__std_bint_to_flt(值) -> 浮点`。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline double bintToFloat(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数转 double");
    if (value == nullptr) return (0.0);
    return (value->number.toDouble());
}

/**
 * @brief 跑一段会抛异常的大数运算，把异常翻译成 C 错误。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @param what 运算名称（拼进错误信息）。
 * @param operation 待执行的运算（返回结果槽）。
 * @retval 封装为原生值的运行时结果。
 */
template <typename Operation>
inline int64_t nativeGuardedBig(ChaliAbiFrame *ctx, const char *what, Operation operation)
{
    try
    {
        return operation();
    } catch (const std::exception &error)
    {
        chali_abi_fail(ctx, (Memory::String(Memory::String(what) + "：" + error.what())).c_str());
        return (0);
    }
}

/* ---- 大整数扩展方法：幂 / 数论 / 素性 / 位运算 / 进制 / 开方 ---- */

/**
 * @brief `__std_bint_pow(值, 指数) -> bint`（`.pow(e)`，指数为非负 int64）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintPow(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数幂运算");
    if (value == nullptr) return (0);
    const std::int64_t exponent = integerArgument(ctx, 1);
    if (exponent < 0)
    {
        chali_abi_fail(ctx, (Memory::String("BigInteger.pow 的指数不能是负数")).c_str());
        return (0);
    }
    return nativeGuardedBig(ctx, "大整数幂运算", [&]() -> int64_t {
        return nativeNewBigInt(ctx, value->number.pow(static_cast<std::uint64_t>(exponent)));
    });
}

/**
 * @brief `__std_bint_powmod(值, 指数, 模) -> bint`（`.powmod(e, m)`，指数是大整数）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintPowMod(ChaliAbiFrame *ctx)
{
    return nativeGuardedBig(ctx, "大整数模幂", [&]() -> int64_t {
        auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数模幂");
        if (value == nullptr) return (0);
        auto *exponent = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数模幂");
        if (exponent == nullptr) return (0);
        auto *modulus = bigIntRecord(ctx, integerArgument(ctx, 2), "大整数模幂");
        if (modulus == nullptr) return (0);
        return nativeNewBigInt(ctx, value->number.powmod(exponent->number, modulus->number));
    });
}

/**
 * @brief `__std_bint_gcd(左, 右) -> bint`（`.gcd(other)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintGcd(ChaliAbiFrame *ctx)
{
    auto *left = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数 gcd");
    if (left == nullptr) return (0);
    auto *right = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数 gcd");
    if (right == nullptr) return (0);
    return nativeNewBigInt(ctx, bigint::BigInt::gcd(left->number, right->number));
}

/**
 * @brief `__std_bint_lcm(左, 右) -> bint`（`.lcm(other)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintLcm(ChaliAbiFrame *ctx)
{
    auto *left = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数 lcm");
    if (left == nullptr) return (0);
    auto *right = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数 lcm");
    if (right == nullptr) return (0);
    return nativeNewBigInt(ctx, bigint::BigInt::lcm(left->number, right->number));
}

/**
 * @brief `__std_bint_mod_nonneg(值, 模) -> bint`（`.mod(m)`，结果落在 `[0, |m|)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintModNonNeg(ChaliAbiFrame *ctx)
{
    return nativeGuardedBig(ctx, "大整数非负取模", [&]() -> int64_t {
        auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数非负取模");
        if (value == nullptr) return (0);
        auto *modulus = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数非负取模");
        if (modulus == nullptr) return (0);
        return nativeNewBigInt(ctx, value->number.modNonNegative(modulus->number));
    });
}

/**
 * @brief `__std_bint_mod_inverse(值, 模) -> bint`（`.modInverse(m)`，不互素报错）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintModInverse(ChaliAbiFrame *ctx)
{
    return nativeGuardedBig(ctx, "大整数模逆元", [&]() -> int64_t {
        auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数模逆元");
        if (value == nullptr) return (0);
        auto *modulus = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数模逆元");
        if (modulus == nullptr) return (0);
        return nativeNewBigInt(ctx, bigint::BigInt::modInverse(value->number, modulus->number));
    });
}

/**
 * @brief `__std_bint_is_probable_prime(值, 轮数) -> bool`（`.isProbablePrime(rounds)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintIsProbablePrime(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数素性测试");
    if (value == nullptr) return (0);
    const std::int64_t rounds = integerArgument(ctx, 1);
    if (rounds < 0)
    {
        chali_abi_fail(ctx, (Memory::String("素性测试的轮数不能是负数")).c_str());
        return (0);
    }
    return (value->number.isProbablePrime(static_cast<std::size_t>(rounds)) ? 1 : 0);
}

/**
 * @brief `__std_bint_next_probable_prime(值) -> bint`（`.nextProbablePrime()`，25 轮）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintNextProbablePrime(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "找下一个素数");
    if (value == nullptr) return (0);
    return nativeGuardedBig(ctx, "找下一个素数", [&]() -> int64_t {
        return nativeNewBigInt(ctx, value->number.nextProbablePrime());
    });
}

/**
 * @brief `__std_bint_bit_length(值) -> 整数`（`.bitLength()`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintBitLength(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数位长");
    if (value == nullptr) return (0);
    return (static_cast<std::int64_t>(value->number.bitLength()));
}

/**
 * @brief 大整数按位运算（`&` / `|` / `^`，只支持非负数）。
 * @param op 运算字符。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintBitwise(ChaliAbiFrame *ctx, char op)
{
    return nativeGuardedBig(ctx, "大整数位运算", [&]() -> int64_t {
        auto *left = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数位运算");
        if (left == nullptr) return (0);
        auto *right = bigIntRecord(ctx, integerArgument(ctx, 1), "大整数位运算");
        if (right == nullptr) return (0);
        switch (op)
        {
        case '&': return nativeNewBigInt(ctx, left->number & right->number);
        case '|': return nativeNewBigInt(ctx, left->number | right->number);
        default: return nativeNewBigInt(ctx, left->number ^ right->number);
        }
    });
}

/**
 * @brief `__std_bint_bit_and(左, 右) -> bint`（`.bitAnd(other)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintBitAnd(ChaliAbiFrame *ctx) { return bintBitwise(ctx, '&'); }
/**
 * @brief `__std_bint_bit_or(左, 右) -> bint`（`.bitOr(other)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintBitOr(ChaliAbiFrame *ctx) { return bintBitwise(ctx, '|'); }
/**
 * @brief `__std_bint_bit_xor(左, 右) -> bint`（`.bitXor(other)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintBitXor(ChaliAbiFrame *ctx) { return bintBitwise(ctx, '^'); }

/**
 * @brief `__std_bint_str_radix(值, 进制) -> 新串地址`（`.strRadix(base)`，2..36 进制）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintToStrRadix(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数转进制文本");
    if (value == nullptr) return (0);
    const std::int64_t base = integerArgument(ctx, 1);
    if (base < 2 || base > 36)
    {
        chali_abi_fail(ctx, (Memory::String("进制必须在 2..36 之间，现在是 " + Memory::toString(base))).c_str());
        return (0);
    }
    return nativeGuardedBig(ctx, "大整数转进制文本", [&]() -> int64_t {
        return (static_cast<std::int64_t>(
            makeText(ctx, value->number.toString(static_cast<unsigned int>(base)))));
    });
}

/**
 * @brief `__std_bint_iroot(值, k) -> bint`（`.iroot(k)`，整数 k 次方根向下取整）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bintIRoot(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数开方");
    if (value == nullptr) return (0);
    const std::int64_t k = integerArgument(ctx, 1);
    if (k <= 0)
    {
        chali_abi_fail(ctx, (Memory::String("根次必须是正整数")).c_str());
        return (0);
    }
    return nativeGuardedBig(ctx, "大整数开方", [&]() -> int64_t {
        return nativeNewBigInt(ctx, bigint::integerRoot(value->number, static_cast<std::uint64_t>(k)));
    });
}

/**
 * @brief `__std_read_bint() -> bint`：把下一项输入按十进制读成一个大整数（`input_bstr`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t readBigInt(ChaliAbiFrame *ctx)
{
    Memory::String text;
    uint64_t inputPosition = 0;
    if (!takeInput(ctx, text, &inputPosition))
    {
        chali_abi_fail(ctx, (Memory::String("输入不够（第 " + Memory::toString(inputPosition + 1u) + " 项缺失）")).c_str());
        return (0);
    }
    bool ok = false;
    const bigint::BigInt value = bigint::BigInt::fromString(text, &ok);
    if (!ok)
    {
        chali_abi_fail(ctx, (Memory::String("输入项不是合法的大整数：\"" + text + "\"")).c_str());
        return (0);
    }
    return nativeNewBigInt(ctx, value);
}

#else /* !STACKVM_WITH_BIGINT：大整数没编进来，这几个名字都指向"一句人话" */

/* 名字照样在表里（调用点不用改），但一调就报"这个构建没有把大整数编进来" ——
 * 比"函数入口不存在"清楚得多。 */
#define STACKVM_BINT_STUB(name) \
    int64_t name(ChaliAbiFrame *cx) { return bigIntDisabled(cx); }

STACKVM_BINT_STUB(bintFromInt)
STACKVM_BINT_STUB(bintFromUInt)
STACKVM_BINT_STUB(bintFromStr)
STACKVM_BINT_STUB(bintFromFloat)
STACKVM_BINT_STUB(bintAdd)
STACKVM_BINT_STUB(bintSub)
STACKVM_BINT_STUB(bintMul)
STACKVM_BINT_STUB(bintDiv)
STACKVM_BINT_STUB(bintMod)
STACKVM_BINT_STUB(bintNeg)
STACKVM_BINT_STUB(bintCmp)
STACKVM_BINT_STUB(bintToStr)
STACKVM_BINT_STUB(bintToInt)
STACKVM_BINT_STUB(bintToFloat)
STACKVM_BINT_STUB(bintPow)
STACKVM_BINT_STUB(bintPowMod)
STACKVM_BINT_STUB(bintGcd)
STACKVM_BINT_STUB(bintLcm)
STACKVM_BINT_STUB(bintModNonNeg)
STACKVM_BINT_STUB(bintModInverse)
STACKVM_BINT_STUB(bintIsProbablePrime)
STACKVM_BINT_STUB(bintNextProbablePrime)
STACKVM_BINT_STUB(bintBitLength)
STACKVM_BINT_STUB(bintBitAnd)
STACKVM_BINT_STUB(bintBitOr)
STACKVM_BINT_STUB(bintBitXor)
STACKVM_BINT_STUB(bintToStrRadix)
STACKVM_BINT_STUB(bintIRoot)
STACKVM_BINT_STUB(readBigInt)

#undef STACKVM_BINT_STUB

#endif /* STACKVM_WITH_BIGINT */

#if STACKVM_WITH_BIGFLOAT

/**
 * @brief 在对象存储中创建大浮点数对象。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @param value 待转换、保存或参与计算的值。
 * @param precision 大浮点数的计算精度。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t nativeNewBigFloat(ChaliAbiFrame *ctx, bigfloat::BigFloat value, int precision = 0)
{
    auto *record = new BigFloatRecord{ std::move(value), precision };
    std::unique_ptr<BigFloatRecord> owner(record);
    const auto handle = chali_abi_record_make_textual(ctx, "BigDecimal", record, destroyBigFloatRecord, cloneBigFloatRecord, textBigFloatRecord);
    if (handle) owner.release();
    return (handle);
}

/**
 * @brief 执行大浮点数从整数构造并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatFromInt(ChaliAbiFrame *ctx)
{
    return nativeNewBigFloat(ctx, bigfloat::BigFloat::fromInt(
                                      bigint::BigInt((ctx->kinds[0] == ChaliAbiSlotFloat) ? static_cast<std::int64_t>(floatingArgument(ctx, 0)) : integerArgument(ctx, 0))));
}

/**
 * @brief 从无符号整数完整构造大数，保留 uint64 的高位。
 * @param ctx 原生调用上下文。
 * @retval 新建的大数对象句柄。
 */
inline int64_t bfloatFromUInt(ChaliAbiFrame *ctx)
{
    return nativeNewBigFloat(ctx, bigfloat::BigFloat::fromInt(
                                      bigint::BigInt::fromU64(static_cast<std::uint64_t>(integerArgument(ctx, 0)))));
}

/**
 * @brief 执行大浮点数从浮点数构造并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatFromFloat(ChaliAbiFrame *ctx)
{
    return nativeNewBigFloat(ctx, bigfloat::BigFloat::fromDouble(
                                      (ctx->kinds[0] == ChaliAbiSlotFloat) ? floatingArgument(ctx, 0) : static_cast<double>(integerArgument(ctx, 0))));
}

/**
 * @brief 执行大浮点数从文本构造并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatFromStr(ChaliAbiFrame *ctx)
{
    Memory::String text;
    if (!readText(ctx, static_cast<std::size_t>(integerArgument(ctx, 0)), &text))
    {
        chali_abi_fail(ctx, (Memory::String("构造大浮点数时读不到文本")).c_str());
        return (0);
    }
    bool ok = false;
    const bigfloat::BigFloat value = bigfloat::BigFloat::fromString(text, &ok);
    if (!ok)
    {
        chali_abi_fail(ctx, (Memory::String("不是合法的大浮点数文本：\"" + text + "\"")).c_str());
        return (0);
    }
    return nativeNewBigFloat(ctx, value);
}

/**
 * @brief 执行大浮点数从大整数构造并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatFromBigInt(ChaliAbiFrame *ctx)
{
    auto *value = bigIntRecord(ctx, integerArgument(ctx, 0), "大整数转大浮点数");
    if (value == nullptr) return (0);
    return nativeNewBigFloat(ctx, bigfloat::BigFloat::fromInt(value->number));
}

/**
 * @brief 校验大浮点数操作数并执行指定算术运算。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @param op 运算符文本或类别。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatArith(ChaliAbiFrame *ctx, char op)
{
    auto *left = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数运算");
    if (left == nullptr) return (0);
    auto *right = bigFloatRecord(ctx, integerArgument(ctx, 1), "大浮点数运算");
    if (right == nullptr) return (0);
    if (op == '/' && right->decimal.isZero())
    {
        chali_abi_fail(ctx, (Memory::String("大浮点数除数是零")).c_str());
        return (0);
    }
    /* 结果的精度取两个操作数精度的**最大值**（而不是只看左操作数）。 */
    const int precision = std::max(left->decimalPrecision, right->decimalPrecision);
    if (op == '+')
    {
        auto result = left->decimal + right->decimal;
        return nativeNewBigFloat(ctx, precision ? result.round(precision) : result, precision);
    }
    if (op == '-')
    {
        auto result = left->decimal - right->decimal;
        return nativeNewBigFloat(ctx, precision ? result.round(precision) : result, precision);
    }
    if (op == '*')
    {
        auto result = left->decimal * right->decimal;
        return nativeNewBigFloat(ctx, precision ? result.round(precision) : result, precision);
    }
    const int digits = precision != 0 ? precision : 34;
    return nativeNewBigFloat(ctx, left->decimal.divide(right->decimal, digits), precision);
}

/**
 * @brief 执行大浮点数加法并封装运行时结果。
 * @param c 原生调用上下文。
 * @param a 第一个操作数或节点编号。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatAdd(ChaliAbiFrame *c) { return bfloatArith(c, '+'); }
/**
 * @brief 执行大浮点数减法并封装运行时结果。
 * @param c 原生调用上下文。
 * @param a 第一个操作数或节点编号。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatSub(ChaliAbiFrame *c) { return bfloatArith(c, '-'); }
/**
 * @brief 执行大浮点数乘法并封装运行时结果。
 * @param c 原生调用上下文。
 * @param a 第一个操作数或节点编号。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatMul(ChaliAbiFrame *c) { return bfloatArith(c, '*'); }
/**
 * @brief 执行大浮点数除法并封装运行时结果。
 * @param c 原生调用上下文。
 * @param a 第一个操作数或节点编号。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatDiv(ChaliAbiFrame *c) { return bfloatArith(c, '/'); }

/**
 * @brief 执行大浮点数取反并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatNeg(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数取负");
    return value ? nativeNewBigFloat(ctx, -value->decimal, value->decimalPrecision) : (0);
}

/**
 * @brief 执行大浮点数比较并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatCmp(ChaliAbiFrame *ctx)
{
    auto *left = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数比较");
    auto *right = bigFloatRecord(ctx, integerArgument(ctx, 1), "大浮点数比较");
    return (left && right) ? (left->decimal.compare(right->decimal)) : (0);
}

/**
 * @brief 执行大浮点数转换为文本并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatToStr(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数转字符串");
    if (value == nullptr) return (0);
    return (static_cast<std::int64_t>(makeText(ctx, value->decimal.toString(0))));
}

/**
 * @brief 执行大浮点数转换为整数并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatToInt(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数转 int");
    if (value == nullptr) return (0);
    bigint::BigInt result = value->decimal.mantissa();
    const int exponent = value->decimal.exponent10();
    bigint::BigInt scale(1);
    for (int i = 0; i < std::abs(exponent); ++i) scale *= bigint::BigInt(10);
    result = exponent >= 0 ? result * scale : result / scale;
    return (bigIntToInt64(result));
}

/**
 * @brief 执行大浮点数转换为浮点数并封装运行时结果。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline double bfloatToFloat(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数转 double");
    if (value == nullptr) return (0.0);
    return (std::strtod(value->decimal.toString(17).c_str(), nullptr));
}

/**
 * @brief 设置大浮点数对象的有效精度。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatSetValuePrecision(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "BigDecimal.setPrecision");
    if (value == nullptr) return (0);
    const std::int64_t digits = integerArgument(ctx, 1);
    if (digits < 1 || digits > 100000)
    {
        chali_abi_fail(ctx, (Memory::String("BigDecimal 有效位数必须在 1..100000 之间，现在是 " + Memory::toString(digits))).c_str());
        return (0);
    }
    const int previous = value->decimalPrecision != 0 ? value->decimalPrecision :
                                                        34;
    value->decimal = value->decimal.round(static_cast<int>(digits));
    value->decimalPrecision = static_cast<int>(digits);
    return (previous);
}

/**
 * @brief 提取序列化所需的大浮点数有效精度状态。
 * @param ctx 原生调用上下文。
 * @return 显式有效位数；零表示沿用默认精度。
 */
int64_t bfloatSerializationPrecision(ChaliAbiFrame *ctx)
{
    const auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "BigDecimal 序列化精度");
    return (value ? value->decimalPrecision : 0);
}

/* ---- 大浮点数扩展方法：幂 / 绝对值 / 取整 / 开方 ---- */

/**
 * @brief `__std_bfloat_pow(值, 指数) -> bfloat`（`.pow(e)`，精确整数次幂）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatPow(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数幂运算");
    if (value == nullptr) return (0);
    const std::int64_t exponent = integerArgument(ctx, 1);
    if (exponent < 0)
    {
        chali_abi_fail(ctx, (Memory::String("BigDecimal.pow 的指数不能是负数（负次幂请用除法自己算）")).c_str());
        return (0);
    }
    return nativeGuardedBig(ctx, "大浮点数幂运算", [&]() -> int64_t {
        return nativeNewBigFloat(ctx, value->decimal.pow(static_cast<std::uint64_t>(exponent)),
                                 value->decimalPrecision);
    });
}

/**
 * @brief `__std_bfloat_abs(值) -> bfloat`（`.abs()`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatAbs(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数绝对值");
    if (value == nullptr) return (0);
    return nativeNewBigFloat(ctx, value->decimal.abs(), value->decimalPrecision);
}

/**
 * @brief `__std_bfloat_is_integer(值) -> bool`（`.isInteger()`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatIsInteger(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数整数判断");
    if (value == nullptr) return (0);
    return (value->decimal.isInteger() ? 1 : 0);
}

/**
 * @brief 大浮点数取整家族（floor / ceil / trunc，结果是大整数）。
 * @param mode 取整方式：'f' 向负无穷 / 'c' 向正无穷 / 't' 向零。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatRoundToBigInt(ChaliAbiFrame *ctx, char mode)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数取整");
    if (value == nullptr) return (0);
    switch (mode)
    {
    case 'f': return nativeNewBigInt(ctx, value->decimal.floor());
    case 'c': return nativeNewBigInt(ctx, value->decimal.ceil());
    default: return nativeNewBigInt(ctx, value->decimal.trunc());
    }
}

/**
 * @brief `__std_bfloat_floor(值) -> bint`（`.floor()`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatFloor(ChaliAbiFrame *ctx) { return bfloatRoundToBigInt(ctx, 'f'); }
/**
 * @brief `__std_bfloat_ceil(值) -> bint`（`.ceil()`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatCeil(ChaliAbiFrame *ctx) { return bfloatRoundToBigInt(ctx, 'c'); }
/**
 * @brief `__std_bfloat_trunc(值) -> bint`（`.trunc()`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatTrunc(ChaliAbiFrame *ctx) { return bfloatRoundToBigInt(ctx, 't'); }

/**
 * @brief `__std_bfloat_sqrt(值, 有效位数) -> bfloat`（`.sqrt(digits)`）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatSqrt(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数开平方");
    if (value == nullptr) return (0);
    const std::int64_t digits = integerArgument(ctx, 1);
    if (digits < 1 || digits > 100000)
    {
        chali_abi_fail(ctx, (Memory::String("BigDecimal.sqrt 的有效位数必须在 1..100000 之间，现在是 " + Memory::toString(digits))).c_str());
        return (0);
    }
    return nativeGuardedBig(ctx, "大浮点数开平方", [&]() -> int64_t {
        return nativeNewBigFloat(ctx, value->decimal.sqrt(static_cast<int>(digits)), value->decimalPrecision);
    });
}

/**
 * @brief `__std_bfloat_root(值, k, 有效位数) -> bfloat`（`.root(k, digits)`，k 次方根）。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bfloatRoot(ChaliAbiFrame *ctx)
{
    auto *value = bigFloatRecord(ctx, integerArgument(ctx, 0), "大浮点数开 k 次方");
    if (value == nullptr) return (0);
    const std::int64_t k = integerArgument(ctx, 1);
    const std::int64_t digits = integerArgument(ctx, 2);
    if (k <= 0)
    {
        chali_abi_fail(ctx, (Memory::String("BigDecimal.root 的根次必须是正整数")).c_str());
        return (0);
    }
    if (digits < 1 || digits > 100000)
    {
        chali_abi_fail(ctx, (Memory::String("BigDecimal.root 的有效位数必须在 1..100000 之间，现在是 " + Memory::toString(digits))).c_str());
        return (0);
    }
    return nativeGuardedBig(ctx, "大浮点数开 k 次方", [&]() -> int64_t {
        return nativeNewBigFloat(ctx, value->decimal.root(static_cast<std::uint64_t>(k), static_cast<int>(digits)),
                                 value->decimalPrecision);
    });
}

/**
 * @brief 从对象句柄读取大浮点数。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t readBigFloat(ChaliAbiFrame *ctx)
{
    Memory::String text;
    uint64_t inputPosition = 0;
    if (!takeInput(ctx, text, &inputPosition))
    {
        chali_abi_fail(ctx, (Memory::String("输入不够，取不到大浮点数")).c_str());
        return (0);
    }
    bool ok = false;
    const bigfloat::BigFloat value = bigfloat::BigFloat::fromString(text, &ok);
    if (!ok)
    {
        chali_abi_fail(ctx, (Memory::String("输入项不是合法的大浮点数：\"" + text + "\"")).c_str());
        return (0);
    }
    return nativeNewBigFloat(ctx, value);
}

/**
 * @brief 使用共享十进制数学后端计算泛型 std::math 的大浮点分支。
 * @param ctx 当前宿主状态，保留定义域或精度错误。
 * @return 新建的 BigDecimal，保持实际计算有效位数。
 */
int64_t bfloatMath(ChaliAbiFrame *ctx)
{
    auto *left = bigFloatRecord(ctx, integerArgument(ctx, 0), "高精度数学");
    auto *right = bigFloatRecord(ctx, integerArgument(ctx, 2), "高精度数学");
    if (!left || !right) return (0);
    const int precision = std::max(left->decimalPrecision, right->decimalPrecision);
    const int digits = precision ? precision : 34;
    try
    {
        bigfloat::BigFloat result;
        bool exact = false;
        switch (integerArgument(ctx, 1))
        {
        case 0: result = bigfloat::sin(left->decimal, digits); break;
        case 1: result = bigfloat::cos(left->decimal, digits); break;
        case 2: result = bigfloat::tan(left->decimal, digits); break;
        case 3: result = bigfloat::asin(left->decimal, digits); break;
        case 4: result = bigfloat::acos(left->decimal, digits); break;
        case 5: result = bigfloat::atan(left->decimal, digits); break;
        case 6: result = bigfloat::exp(left->decimal, digits); break;
        case 7: result = bigfloat::ln(left->decimal, digits); break;
        case 8: result = bigfloat::log10(left->decimal, digits); break;
        case 9: result = left->decimal.sqrt(digits); break;
        case 10:
            result = bigfloat::BigFloat::fromInt(left->decimal.floor());
            exact = true;
            break;
        case 11:
            result = bigfloat::BigFloat::fromInt(left->decimal.ceil());
            exact = true;
            break;
        case 12:
            result = bigfloat::BigFloat::fromInt(left->decimal.sign() < 0 ? (left->decimal - bigfloat::BigFloat("0.5")).ceil() : (left->decimal + bigfloat::BigFloat("0.5")).floor());
            exact = true;
            break;
        case 13:
            result = bigfloat::BigFloat::fromInt(left->decimal.trunc());
            exact = true;
            break;
        case 14: result = bigfloat::pow(left->decimal, right->decimal, digits); break;
        case 15:
            result = bigfloat::fmod(left->decimal, right->decimal);
            exact = true;
            break;
        default: throw std::invalid_argument("高精度数学运算编号无效");
        }
        return nativeNewBigFloat(ctx, std::move(result), exact ? precision : digits);
    } catch (const std::exception &error)
    {
        chali_abi_fail(ctx, (Memory::String(Memory::String("std::math 高精度计算：") + error.what())).c_str());
        return (0);
    }
}

/**
 * @brief 构造指定有效位数的高精度圆周率。
 * @param ctx 当前宿主调用状态。
 * @return 保留对应计算精度的 BigDecimal 记录。
 */
int64_t bfloatPi(ChaliAbiFrame *ctx)
{
    const auto digits = integerArgument(ctx, 0);
    if (digits < 1 || digits > 100000)
    {
        chali_abi_fail(ctx, (Memory::String("圆周率有效位数必须在 1..100000 之间")).c_str());
        return (0);
    }
    return nativeNewBigFloat(ctx, bigfloat::pi(static_cast<int>(digits)), static_cast<int>(digits));
}

#else

/**
 * @brief 报告当前构建未启用大浮点数功能。
 * @param ctx 当前 C 调用帧，直接提供参数槽与错误通道。
 * @retval 封装为原生值的运行时结果。
 */
inline int64_t bigFloatDisabled(ChaliAbiFrame *ctx)
{
    chali_abi_fail(ctx, (Memory::String("这个构建没有把大浮点数编进来：用 STACKVM_WITH_BIGFLOAT=1 重建虚拟机")).c_str());
    return (0);
}
#define STACKVM_BFLOAT_STUB(name) \
    int64_t name(ChaliAbiFrame *c) { return bigFloatDisabled(c); }
STACKVM_BFLOAT_STUB(bfloatFromInt)
STACKVM_BFLOAT_STUB(bfloatFromUInt)
STACKVM_BFLOAT_STUB(bfloatFromFloat)
STACKVM_BFLOAT_STUB(bfloatFromStr)
STACKVM_BFLOAT_STUB(bfloatFromBigInt)
STACKVM_BFLOAT_STUB(bfloatAdd)
STACKVM_BFLOAT_STUB(bfloatSub)
STACKVM_BFLOAT_STUB(bfloatMul)
STACKVM_BFLOAT_STUB(bfloatDiv)
STACKVM_BFLOAT_STUB(bfloatNeg)
STACKVM_BFLOAT_STUB(bfloatCmp)
STACKVM_BFLOAT_STUB(bfloatToStr)
STACKVM_BFLOAT_STUB(bfloatToInt)
STACKVM_BFLOAT_STUB(bfloatToFloat)
STACKVM_BFLOAT_STUB(bfloatSetValuePrecision)
STACKVM_BFLOAT_STUB(bfloatSerializationPrecision)
STACKVM_BFLOAT_STUB(bfloatPow)
STACKVM_BFLOAT_STUB(bfloatAbs)
STACKVM_BFLOAT_STUB(bfloatIsInteger)
STACKVM_BFLOAT_STUB(bfloatFloor)
STACKVM_BFLOAT_STUB(bfloatCeil)
STACKVM_BFLOAT_STUB(bfloatTrunc)
STACKVM_BFLOAT_STUB(bfloatSqrt)
STACKVM_BFLOAT_STUB(bfloatRoot)
STACKVM_BFLOAT_STUB(readBigFloat)
STACKVM_BFLOAT_STUB(bfloatMath)
STACKVM_BFLOAT_STUB(bfloatPi)
#undef STACKVM_BFLOAT_STUB

#endif

/** @brief 复制 C 数值记录。
 * @param frame 当前帧。
 * @return 独立副本句柄。
 */
template <bool Decimal>
int64_t copyNumber(ChaliAbiFrame *frame)
{
#if STACKVM_WITH_BIGINT
    if constexpr (!Decimal) return chali_abi_record_clone(frame, integerArgument(frame, 0), "BigInteger", cloneBigIntRecord);
#endif
#if STACKVM_WITH_BIGFLOAT
    if constexpr (Decimal) return chali_abi_record_clone(frame, integerArgument(frame, 0), "BigDecimal", cloneBigFloatRecord);
#endif
    chali_abi_fail(frame, "数值后端未编入");
    return 0;
}
/** @brief 严格释放 C 数值记录。
 * @param frame 当前帧。
 * @return 零占位值。
 */
int64_t dropNumber(ChaliAbiFrame *frame)
{
    chali_abi_record_release(frame, integerArgument(frame, 0));
    return 0;
}
/** @brief 在 C 边界执行一个直接读取帧的数值操作，捕获所有异常。
 * @tparam Operation 返回整数句柄或浮点值的数值内核。
 * @tparam Arity 此入口要求的精确参数数量。
 * @param frame 已检查参数数量及槽种类的调用帧。
 */
template <auto Operation, int Arity>
void invoke(ChaliAbiFrame *frame)
try
{
    if (chali_abi_check_frame(frame, Arity, nullptr) != ChaliAbiOk) { chali_abi_fail(frame, "invalid numeric call frame"); return; }
    const auto result = Operation(frame);
    if constexpr (std::is_floating_point_v<decltype(result)>)
        chali_abi_return_real(frame, result);
    else
        chali_abi_return_integer(frame, result);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "大数 C 操作失败");
}
}  // namespace
/** @brief 获取大整数及高精度小数的直接 C 入口表。
 * @param count 接收表项数，可为空。
 * @return 生命周期为整个进程的只读表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_big_number_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "__std_bint_from_int", invoke<bintFromInt, 1>, 1 },
        { "__std_bint_from_uint", invoke<bintFromUInt, 1>, 1 },
        { "__std_bint_from_str", invoke<bintFromStr, 1>, 1 },
        { "__std_bint_from_flt", invoke<bintFromFloat, 1>, 1 },
        { "__std_bint_add", invoke<bintAdd, 2>, 2 },
        { "__std_bint_sub", invoke<bintSub, 2>, 2 },
        { "__std_bint_mul", invoke<bintMul, 2>, 2 },
        { "__std_bint_div", invoke<bintDiv, 2>, 2 },
        { "__std_bint_mod", invoke<bintMod, 2>, 2 },
        { "__std_bint_neg", invoke<bintNeg, 1>, 1 },
        { "__std_bint_cmp", invoke<bintCmp, 2>, 2 },
        { "__std_bint_str", invoke<bintToStr, 1>, 1 },
        { "__std_bint_to_int", invoke<bintToInt, 1>, 1 },
        { "__std_bint_to_flt", invoke<bintToFloat, 1>, 1 },
        { "__std_bint_pow", invoke<bintPow, 2>, 2 },
        { "__std_bint_powmod", invoke<bintPowMod, 3>, 3 },
        { "__std_bint_gcd", invoke<bintGcd, 2>, 2 },
        { "__std_bint_lcm", invoke<bintLcm, 2>, 2 },
        { "__std_bint_mod_nonneg", invoke<bintModNonNeg, 2>, 2 },
        { "__std_bint_mod_inverse", invoke<bintModInverse, 2>, 2 },
        { "__std_bint_is_probable_prime", invoke<bintIsProbablePrime, 2>, 2 },
        { "__std_bint_next_probable_prime", invoke<bintNextProbablePrime, 1>, 1 },
        { "__std_bint_bit_length", invoke<bintBitLength, 1>, 1 },
        { "__std_bint_bit_and", invoke<bintBitAnd, 2>, 2 },
        { "__std_bint_bit_or", invoke<bintBitOr, 2>, 2 },
        { "__std_bint_bit_xor", invoke<bintBitXor, 2>, 2 },
        { "__std_bint_str_radix", invoke<bintToStrRadix, 2>, 2 },
        { "__std_bint_iroot", invoke<bintIRoot, 2>, 2 },
        { "__std_read_bint", invoke<readBigInt, 0>, 0 },
        { "__std_input_bint", invoke<readBigInt, 0>, 0 },
        { "__std_bfloat_from_int", invoke<bfloatFromInt, 1>, 1 },
        { "__std_bfloat_from_uint", invoke<bfloatFromUInt, 1>, 1 },
        { "__std_bfloat_from_flt", invoke<bfloatFromFloat, 1>, 1 },
        { "__std_bfloat_from_str", invoke<bfloatFromStr, 1>, 1 },
        { "__std_bfloat_from_bint", invoke<bfloatFromBigInt, 1>, 1 },
        { "__std_bfloat_add", invoke<bfloatAdd, 2>, 2 },
        { "__std_bfloat_sub", invoke<bfloatSub, 2>, 2 },
        { "__std_bfloat_mul", invoke<bfloatMul, 2>, 2 },
        { "__std_bfloat_div", invoke<bfloatDiv, 2>, 2 },
        { "__std_bfloat_neg", invoke<bfloatNeg, 1>, 1 },
        { "__std_bfloat_cmp", invoke<bfloatCmp, 2>, 2 },
        { "__std_bfloat_str", invoke<bfloatToStr, 1>, 1 },
        { "__std_bfloat_math", invoke<bfloatMath, 3>, 3 },
        { "std::math::__std_bfloat_math", invoke<bfloatMath, 3>, 3 },
        { "__std_bfloat_pi", invoke<bfloatPi, 1>, 1 },
        { "std::math::__std_bfloat_pi", invoke<bfloatPi, 1>, 1 },
        { "__std_bfloat_to_int", invoke<bfloatToInt, 1>, 1 },
        { "__std_bfloat_pow", invoke<bfloatPow, 2>, 2 },
        { "__std_bfloat_abs", invoke<bfloatAbs, 1>, 1 },
        { "__std_bfloat_is_integer", invoke<bfloatIsInteger, 1>, 1 },
        { "__std_bfloat_floor", invoke<bfloatFloor, 1>, 1 },
        { "__std_bfloat_ceil", invoke<bfloatCeil, 1>, 1 },
        { "__std_bfloat_trunc", invoke<bfloatTrunc, 1>, 1 },
        { "__std_bfloat_sqrt", invoke<bfloatSqrt, 2>, 2 },
        { "__std_bfloat_root", invoke<bfloatRoot, 3>, 3 },
        { "__std_bfloat_to_flt", invoke<bfloatToFloat, 1>, 1 },
        { "__std_bfloat_set_value_precision", invoke<bfloatSetValuePrecision, 2>, 2 },
        { "std::reflect::__serdeBigPrecision", invoke<bfloatSerializationPrecision, 1>, 1 },
        { "__std_read_bfloat", invoke<readBigFloat, 0>, 0 },
        { "__std_input_bfloat", invoke<readBigFloat, 0>, 0 },
        { "__std_bint_copy", invoke<copyNumber<false>, 1>, 1 },
        { "__std_bint_drop", invoke<dropNumber, 1>, 1 },
        { "__std_bfloat_copy", invoke<copyNumber<true>, 1>, 1 },
        { "__std_bfloat_drop", invoke<dropNumber, 1>, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
