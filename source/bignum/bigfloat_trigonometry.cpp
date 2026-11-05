#if !defined(NO_CHALI_BIG_INT) && !defined(NO_CHALI_BIG_FLOAT)
#include "bigfloat_math.hpp"
#include "bigfloat_math_detail.hpp"
#include "bigfloat_c_adapter.hpp"
#include <stdexcept>

namespace bigfloat
{

/**
 * @brief 计算带消去保护位的高精度正弦。
 * @param value 弧度角。
 * @param digits 结果有效位数。
 * @return 舍入后的正弦值。
 */
BigFloat sin(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_sin);
}
/**
 * @brief 计算带消去保护位的高精度余弦。
 * @param value 弧度角。
 * @param digits 结果有效位数。
 * @return 舍入后的余弦值。
 */
BigFloat cos(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_cos);
}
/**
 * @brief 使用同一次角度约化计算正切的分子分母。
 * @param value 弧度角。
 * @param digits 结果有效位数。
 * @return 舍入后的正切；检测到零余弦时抛出定义域异常。
 */
BigFloat tan(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_tan);
}
/**
 * @brief 计算任意有限实数的反正切主值。
 * @param value 待求反正切的实数。
 * @param digits 结果有效位数。
 * @return 舍入后的弧度角。
 */
BigFloat atan(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_atan);
}
/**
 * @brief 通过平方根与反正切计算反正弦，保留接近端点的消去信息。
 * @param value 位于 [-1,1] 的实数。
 * @param digits 结果有效位数。
 * @return 舍入后的反正弦主值。
 */
BigFloat asin(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_asin);
}
/**
 * @brief 使用半角恒等式计算反余弦，避免一附近的主值消去。
 * @param value 位于 [-1,1] 的实数。
 * @param digits 结果有效位数。
 * @return 舍入后的反余弦主值。
 */
BigFloat acos(const BigFloat &value, int digits)
{
    return cAdapter::mathUnary(value, digits, chali_bigfloat_acos);
}
}  // namespace bigfloat
#endif
