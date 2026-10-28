/** @file interface_mono.cpp
 * @brief 派生抽象接口继承方法的单态 C++ O3 参考。
 */
#include <cstdint>
/** @brief 声明被派生接口继承的操作。 */
struct Operation
{
    /** @brief 通过基类析构时正确收尾。 */
    virtual ~Operation() = default;
    /** @brief 计算归并值。
     * @param value 输入整数。
     * @return 受 31 位掩码约束的结果。
     */
    virtual std::int64_t apply(std::int64_t value) const = 0;
};
/** @brief 添加一个方法并继承原操作的派生接口。 */
struct NamedOperation : Operation
{
    /** @brief 查询偏移。
     * @return 当前偏移值。
     */
    virtual std::int64_t bias() const = 0;
};
/** @brief 单态调用使用的具体实现。 */
struct Multiply final : NamedOperation
{
    std::int64_t factor = 0, offset = 0;
    /** @brief 执行乘加。
     * @param value 输入整数。
     * @return 掩码后的乘加结果。
     */
    std::int64_t apply(std::int64_t value) const override { return (value * factor + offset) & 2147483647; }
    /** @brief 查询偏移。
     * @return 当前偏移值。
     */
    std::int64_t bias() const override { return offset; }
};
static Multiply receiver;
/** @brief 在计时前设置对象字段。 */
extern "C" void feature_setup() { receiver.factor = 3; receiver.offset = 7; }
/** @brief 通过派生接口调用继承方法。
 * @param operation 当前接口引用。
 * @param value 输入整数。
 * @return 操作结果。
 */
static std::int64_t invoke(const NamedOperation &operation, std::int64_t value) { return operation.apply(value); }
/** @brief 执行单态接口归并。
 * @param seed 初始归并值。
 * @return 归并校验值。
 */
extern "C" std::int64_t feature_work(std::int64_t seed)
{
    const NamedOperation &derived = receiver;
    auto acc = seed;
    for (std::int64_t i = 0; i < 262144; ++i) acc = invoke(derived, acc);
    return acc;
}
