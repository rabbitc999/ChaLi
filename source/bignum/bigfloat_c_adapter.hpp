/**
 * @file bigfloat_c_adapter.hpp
 * @brief C 大浮点服务的私有所有权与异常适配。
 */
#ifndef CHALI_BIGFLOAT_C_ADAPTER_HPP
#define CHALI_BIGFLOAT_C_ADAPTER_HPP
#include "bigfloat.hpp"
#include "chali_bigfloat.h"
#include "chali_bigfloat_math.h"
#include <stdexcept>

namespace bigfloat
{
/** @brief 仅供数值适配层使用的 C 布局访问，不暴露新的语言接口。 */
struct CFloatAccess
{
    /**
     * @brief 借用整数存储，不转移所有权。
     * @param value 原整数。
     * @return 只读 C 整数布局。
     */
    static const ChaliBigInt &integer(const bigint::BigInt &value) { return value.value_; }
    /**
     * @brief 取得可接收 C 结果的整数布局。
     * @param value 接收对象。
     * @return C 整数布局。
     */
    static ChaliBigInt &integer(bigint::BigInt &value) { return value.value_; }
    /**
     * @brief 建立只读的浅借用视图，调用方不得释放其中存储。
     * @param value 原浮点数。
     * @return C 浮点布局。
     */
    static ChaliBigFloat view(const BigFloat &value) { return { value.mant_.value_, value.exp10_ }; }
    /**
     * @brief 接管独占 C 结果并清空源存储身份。
     * @param value 可移动 C 数值。
     * @return 原接口拥有的大浮点数。
     */
    static BigFloat take(ChaliBigFloat &value)
    {
        BigFloat result;
        chali_bigint_clear(&result.mant_.value_);
        result.mant_.value_ = value.mantissa;
        result.exp10_ = value.exponent10;
        value.mantissa.sign = 0;
        value.mantissa.limbs.data = nullptr;
        value.mantissa.limbs.length = value.mantissa.limbs.capacity = 0;
        value.exponent10 = 0;
        return result;
    }
};
namespace cAdapter
{
/**
 * @brief 将 C 错误恢复为原有 C++ 数值异常类别。
 * @param status C 操作状态。
 * @param message 原接口诊断。
 */
inline void check(ChaliCStatus status, const char *message = "bigfloat: 数值操作失败")
{
    if (status == CHALI_C_OK) return;
    if (status == CHALI_C_OUT_OF_MEMORY) throw std::bad_alloc();
    if (status == CHALI_C_RANGE_ERROR) throw std::overflow_error(message);
    if (status == CHALI_C_DOMAIN_ERROR) throw std::domain_error(message);
    throw std::invalid_argument(message);
}
/** @brief 绑定当前数值预算的 C 浮点临时值。 */
struct Work
{
    ChaliBigFloat value{};
    /**
     * @brief 从无分配的整数捕获当前分配所有者。
     */
    Work()
    {
        bigint::BigInt owner;
        check(chali_bigfloat_init(&value, &CFloatAccess::integer(owner).limbs.allocator));
    }
    /**
     * @brief 归还临时存储，即使 C++ 异常传播也保持配对释放。
     */
    ~Work() { chali_bigfloat_clear(&value); }
    Work(const Work &) = delete;
    Work &operator=(const Work &) = delete;
};
/** @brief 独占 C 文本，在复制到语言字符串失败时仍负责回收。 */
struct Text
{
    ChaliBigString value{};
    /**
     * @brief 捕获当前数值所有者。
     */
    Text()
    {
        bigint::BigInt owner;
        value.allocator = CFloatAccess::integer(owner).limbs.allocator;
    }
    /**
     * @brief 归还 C 文本。
     */
    ~Text() { chali_big_string_clear(&value); }
    Text(const Text &) = delete;
    Text &operator=(const Text &) = delete;
};
/**
 * @brief 调用带有效位数的一元 C 服务。
 * @param input 输入数值。
 * @param digits 有效位数。
 * @param operation C 运算入口。
 * @return 接管后的兼容数值。
 */
inline BigFloat unary(const BigFloat &input, int digits,
                      ChaliCStatus (*operation)(ChaliBigFloat *, const ChaliBigFloat *, int))
{
    Work work;
    const auto view = CFloatAccess::view(input);
    check(operation(&work.value, &view, digits));
    return CFloatAccess::take(work.value);
}
/**
 * @brief 按原显示接口取得拥有的语言字符串。
 * @param input 输入数值。
 * @param digits 有效位数。
 * @param plain 是否恒用普通记法。
 * @return 独占字符串。
 */
inline Memory::String format(const BigFloat &input, int digits, bool plain)
{
    Text work;
    const auto view = CFloatAccess::view(input);
    check(chali_bigfloat_to_string(&work.value, &view, digits, plain));
    return Memory::String(work.value.data, work.value.length);
}
/**
 * @brief 恢复高精度数学原有的精度、定义域及收敛异常。
 * @param status C 操作状态。
 * @param error 不拥有消息存储的调用诊断。
 */
inline void checkMath(ChaliCStatus status, const ChaliBigFloatError &error)
{
    if (status == CHALI_C_OK) return;
    if (status == CHALI_C_OUT_OF_MEMORY) throw std::bad_alloc();
    if (!error.message)
    {
        check(status);
        return;
    }
    if (error.kind == CHALI_BIGFLOAT_ERROR_PRECISION) throw std::out_of_range(error.message);
    if (error.kind == CHALI_BIGFLOAT_ERROR_OVERFLOW) throw std::overflow_error(error.message);
    if (error.kind == CHALI_BIGFLOAT_ERROR_DOMAIN) throw std::domain_error(error.message);
    if (error.kind == CHALI_BIGFLOAT_ERROR_CONVERGENCE) throw std::runtime_error(error.message);
    check(status, error.message);
}
/**
 * @brief 调用有独立诊断的一元 C 数学服务。
 * @param input 输入值。
 * @param digits 结果有效位数。
 * @param operation C 数学入口。
 * @return 接管后的兼容数值。
 */
inline BigFloat mathUnary(const BigFloat &input, int digits,
                          ChaliCStatus (*operation)(ChaliBigFloat *, const ChaliBigFloat *, int, ChaliBigFloatError *))
{
    Work work;
    ChaliBigFloatError error{};
    const auto view = CFloatAccess::view(input);
    checkMath(operation(&work.value, &view, digits, &error), error);
    return CFloatAccess::take(work.value);
}
}  // namespace cAdapter
}  // namespace bigfloat
#endif
