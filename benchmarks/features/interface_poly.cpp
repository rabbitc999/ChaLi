/** @file interface_poly.cpp
 * @brief 派生抽象接口继承方法的三类型 C++ O3 参考。
 */
#include <cstdint>
/** @brief 声明被继承的整数操作。 */
struct Operation
{
    /** @brief 通过基类析构时正确收尾。 */
    virtual ~Operation() = default;
    /** @brief 执行当前类型的运算。
     * @param value 输入整数。
     * @return 运算结果。
     */
    virtual std::int64_t apply(std::int64_t value) const = 0;
};
/** @brief 继承操作并声明偏移查询的派生接口。 */
struct NamedOperation : Operation
{
    /** @brief 查询偏移。
     * @return 当前偏移值。
     */
    virtual std::int64_t bias() const = 0;
};
/** @brief 执行乘加的具体类型。 */
struct Multiply final : NamedOperation
{
    std::int64_t factor = 0, offset = 0;
    /** @brief 执行乘加。
     * @param value 输入整数。
     * @return 掩码后的结果。
     */
    std::int64_t apply(std::int64_t value) const override { return (value * factor + offset) & 2147483647; }
    /** @brief 查询偏移。
     * @return 当前偏移值。
     */
    std::int64_t bias() const override { return offset; }
};
/** @brief 执行乘减的具体类型。 */
struct Subtract final : NamedOperation
{
    std::int64_t factor = 0, offset = 0;
    /** @brief 执行乘减。
     * @param value 输入整数。
     * @return 掩码后的结果。
     */
    std::int64_t apply(std::int64_t value) const override { return (value * factor - offset) & 2147483647; }
    /** @brief 查询偏移。
     * @return 当前偏移值。
     */
    std::int64_t bias() const override { return offset; }
};
/** @brief 执行乘法与异或的具体类型。 */
struct ExclusiveOr final : NamedOperation
{
    std::int64_t factor = 0, offset = 0;
    /** @brief 执行乘法与异或。
     * @param value 输入整数。
     * @return 掩码后的结果。
     */
    std::int64_t apply(std::int64_t value) const override { return ((value * factor) ^ offset) & 2147483647; }
    /** @brief 查询偏移。
     * @return 当前偏移值。
     */
    std::int64_t bias() const override { return offset; }
};
static Multiply first;
static Subtract second;
static ExclusiveOr third;
/** @brief 在计时前设置三个对象。 */
extern "C" void feature_setup()
{
    first.factor = 3; first.offset = 7;
    second.factor = 5; second.offset = 11;
    third.factor = 7; third.offset = 13;
}
/** @brief 按 2:1:1 比例选择对象。
 * @param index 当前序号对四取余的结果。
 * @return 选定对象的派生接口引用。
 */
static const NamedOperation &choose(std::int64_t index)
{
    if (index == 1) return second;
    if (index == 2) return third;
    return first;
}
/** @brief 通过同一接口调用位置执行操作。
 * @param operation 当前接口引用。
 * @param value 输入整数。
 * @return 操作结果。
 */
static std::int64_t invoke(const NamedOperation &operation, std::int64_t value) { return operation.apply(value); }
/** @brief 执行三类型接口归并。
 * @param seed 初始归并值。
 * @return 归并校验值。
 */
extern "C" std::int64_t feature_work(std::int64_t seed)
{
    auto acc = seed;
    for (std::int64_t i = 0; i < 262144; ++i) acc = invoke(choose((i + seed) & 3), acc);
    return acc;
}
