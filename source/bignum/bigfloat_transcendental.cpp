#if !defined(NO_CHALI_BIG_INT) && !defined(NO_CHALI_BIG_FLOAT)
#include "bigfloat_math.hpp"
#include "bigfloat_math_detail.hpp"
#include "bigfloat_c_adapter.hpp"
#include <stdexcept>

namespace bigfloat
{
namespace mathDetail
{
/**
 * @brief 检查精度并保留保护位，限制中间结果的工作规模。
 * @param digits 当前层请求的有效位数，包含嵌套调用的保护位，范围为 1..250000。
 * @param extra 本次消去或参数约化所需的额外位数。
 * @return 加入 32 位保护位的工作精度。
 */
int workPrecision(int digits, std::int64_t extra)
{
    int work = 0;
    ChaliBigFloatError error{};
    const auto status = chali_bigfloat_work_precision(digits, extra, &work, &error);
    cAdapter::checkMath(status, error);
    return work;
}
/**
 * @brief 通过尾数位数与十进制指数取得幅值的数量级。
 * @param value 输入十进制数。
 * @return 最高有效十进制位的指数，零返回 0。
 */
std::int64_t decimalOrder(const BigFloat &value)
{
    bigint::BigInt owner;
    const auto view = CFloatAccess::view(value);
    std::int64_t order = 0;
    cAdapter::check(chali_bigfloat_decimal_order(&view, &order, &CFloatAccess::integer(owner).limbs.allocator));
    return order;
}
}  // namespace mathDetail

/**
 * @brief 使用 Gauss–Legendre AGM 迭代计算圆周率。
 * @param digits 结果有效位数。
 * @return 舍入后的圆周率。
 */
BigFloat pi(int digits)
{
    cAdapter::Work work;
    ChaliBigFloatError error{};
    const auto status = chali_bigfloat_pi(&work.value, digits, &error);
    cAdapter::checkMath(status, error);
    return CFloatAccess::take(work.value);
}
/**
 * @brief 将指数约化到小区间并以 Taylor 级数和重复平方计算指数。
 * @param value 实数指数。
 * @param digits 结果有效位数。
 * @return 舍入后的自然指数。
 */
BigFloat exp(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_exp);
}
/**
 * @brief 按十进制数量级分离输入，并使用对数级数求精确工作精度结果。
 * @param value 严格为正的实数。
 * @param digits 结果有效位数。
 * @return 舍入后的自然对数。
 */
BigFloat ln(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_ln);
}
/**
 * @brief 计算常用对数，十的整数幂直接返回精确指数。
 * @param value 正实数。
 * @param digits 结果有效位数。
 * @return 舍入后的常用对数。
 */
BigFloat log10(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_log10);
}
/**
 * @brief 用定精度二进制幂或指数对数计算实数乘方。
 * @param value 底数。
 * @param exponent 实数指数。
 * @param digits 结果有效位数。
 * @return 舍入后的实数幂。
 */
BigFloat pow(const BigFloat &value, const BigFloat &exponent, int digits)
{
    cAdapter::Work work;
    ChaliBigFloatError error{};
    const auto input = CFloatAccess::view(value), power = CFloatAccess::view(exponent);
    const auto status = chali_bigfloat_pow(&work.value, &input, &power, digits, &error);
    cAdapter::checkMath(status, error);
    return CFloatAccess::take(work.value);
}
/**
 * @brief 将十进制指数对齐到整数模运算，避免有限精度除法误判商。
 * @param value 被除数。
 * @param divisor 非零除数。
 * @return 精确余数，符号跟随被除数。
 */
BigFloat fmod(const BigFloat &value, const BigFloat &divisor)
{
    cAdapter::Work work;
    ChaliBigFloatError error{};
    const auto input = CFloatAccess::view(value), modulus = CFloatAccess::view(divisor);
    const auto status = chali_bigfloat_fmod(&work.value, &input, &modulus, &error);
    cAdapter::checkMath(status, error);
    return CFloatAccess::take(work.value);
}
}  // namespace bigfloat
#endif
