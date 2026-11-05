#include "chali_standard.h"
#include "allocator/chali_allocator.hpp"
#include "allocator/c_heap_allocator.hpp"
#include <memory>
#include <cstring>

#include <exception>
#include <limits>
#include <mutex>
#include <stdexcept>
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
#include "cas/allocator.h"
#include "cas/detail/calculus.h"
#include "cas/detail/parser.h"
#include "cas/detail/polynomial.h"
#include "cas/detail/printer.h"
#include "cas/detail/trigonometry.h"
#if !defined(NO_CHALI_BIG_FLOAT)
#include "bigfloat.hpp"
#endif
#endif

namespace
{
/**
 * @brief 直接读取 C 帧整数槽，错误时抛至本入口的异常隔离边界。
 * @param frame 当前帧。
 * @param index 参数下标。
 * @return 整数位模式。
 */
int64_t integerArgument(const ChaliAbiFrame *frame, int64_t index)
{
    int64_t value = 0;
    if (!chali_abi_argument_integer(frame, index, &value)) throw std::invalid_argument("CAS 参数不是整数槽");
    return value;
}

#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
using namespace Cas::Detail;
/**
 * @brief 持有不可变表达式根节点的独立引用。
 */
struct ExpressionRecord
{ Ptr root; };
/**
 * @brief 串行保护 CAS 全局符号、内存池与节点引用计数。
 * @return 覆盖进程生命周期的递归互斥量。
 */
std::recursive_mutex &casMutex()
{
    static auto *guard = ::Memory::makeCHeapUnique<std::recursive_mutex>().release();
    return *guard;
}
/**
 * @brief 为另一个运行时记录保留不可变根节点。
 * @param source 已校验的表达式记录。
 * @return 具有独立所有权的记录。
 */
void *cloneExpression(const void *source)
try
{
    std::lock_guard<std::recursive_mutex> lock(casMutex());
    return ::Memory::makeCHeapUnique<ExpressionRecord>(ExpressionRecord{ static_cast<const ExpressionRecord *>(source)->root.share() }).release();
} catch (...) { return nullptr; }
/**
 * @brief 在 CAS 锁保护下释放根节点引用。
 * @param record 待销毁的表达式记录。
 */
void destroyExpression(void *record)
{
    std::lock_guard<std::recursive_mutex> lock(casMutex());
    ::Memory::destroyCHeap(static_cast<ExpressionRecord *>(record));
}
/**
 * @brief 有界查询或写入规范中缀文本，不改变表达式。
 * @param record 存活记录。
 * @param data 输出缓冲，查询时为空。
 * @param capacity 输出容量。
 * @param required 接收完整字节数。
 * @return 成功为一；容量不足或异常为零。
 */
int textExpression(const void *record, char *data, size_t capacity, size_t *required)
try
{
    if (!record || !required) return 0;
    std::lock_guard<std::recursive_mutex> lock(casMutex());
    const auto text = toText(static_cast<const ExpressionRecord *>(record)->root.get());
    *required = text.size();
    if (!data) return !capacity;
    if (capacity < text.size()) return 0;
    if (!text.empty()) std::memcpy(data, text.data(), text.size());
    return 1;
} catch (...) { return 0; }
/**
 * @brief 从 C 对象表借用表达式，不改变根节点所有权。
 * @param state 当前 C 帧。
 * @param value 语言对象句柄。
 * @return 活动表达式，错误时为空。
 */
ExpressionRecord *expression(ChaliAbiFrame *state, int64_t value)
{
    auto *record = static_cast<ExpressionRecord *>(chali_abi_record_probe(state, value, "std::cas::Expr"));
    if (!record) chali_abi_fail(state, (::Memory::String("std::cas 用的 std::cas::Expr 已经释放过了（句柄 ") + ::Memory::toString(value) + "）").c_str());
    return record;
}
/**
 * @brief 向 C 运行时移交一个不可变表达式根，失败回收未移交对象。
 * @param state 当前 C 帧。
 * @param root 独立持有的非空根引用。
 * @return 新记录句柄，失败为零。
 */
int64_t makeExpression(ChaliAbiFrame *state, Ptr root)
{
    if (!root) throw std::runtime_error("CAS 无法计算给定表达式");
    auto owner = ::Memory::makeCHeapUnique<ExpressionRecord>(ExpressionRecord{ std::move(root) });
    const auto result = chali_abi_record_make_textual(state, "std::cas::Expr", owner.get(), destroyExpression, cloneExpression, textExpression);
    if (result) owner.release();
    return result;
}
/**
 * @brief 从 C 文本视图复制完整 UTF-8 字节。
 * @param state 当前帧。
 * @param value String 句柄。
 * @return 独立宿主文本；读取失败抛出受边界捕获的异常。
 */
std::string readText(ChaliAbiFrame *state, int64_t value)
{
    const unsigned char *bytes = nullptr;
    size_t length = 0;
    if (!chali_abi_bytes(state, value, &bytes, &length)) throw std::invalid_argument("CAS 无法读取字符串参数");
    return std::string(bytes ? reinterpret_cast<const char *>(bytes) : "", length);
}
/**
 * @brief 创建向语言移交的 C 拥有型 String。
 * @param state 当前帧。
 * @param text 原始文本。
 * @return 字符串句柄，失败为零。
 */
template <class Text>
int64_t makeText(ChaliAbiFrame *state, const Text &text)
{ return chali_abi_string(state, text.data(), text.size()); }
/**
 * @brief 验证微积分自变量确实是单个符号。
 * @param text 自变量的源文本。
 * @return AST 符号表中的编号。
 */
std::uint32_t variableId(const std::string &text)
{
    auto parsed = parseExpression(text);
    if (!parsed.ok || !parsed.expression || parsed.expression->kind() != NodeKind::Symbol)
        throw std::invalid_argument("CAS 自变量必须是单个符号");
    return parsed.expression->symbolId();
}
/**
 * @brief 创建精确的零表达式，供默认初始化使用。
 * @param state 当前 C 帧及诊断通道。
 * @return 值为零的新表达式记录句柄。
 */
int64_t casNew(ChaliAbiFrame *state)
{
    Cas::Allocator::initialize();
    return makeExpression(state, Ptr(Node::makeInteger(BigInt(0))));
}
/**
 * @brief 解析字符串为规范化不可变 AST，保留原始解析错误位置。
 * @param state 当前 C 帧及诊断通道。
 * @return 成功解析并拥有 AST 的记录句柄。
 */
int64_t casParse(ChaliAbiFrame *state)
{
    const auto text = readText(state, integerArgument(state, 0));
    auto parsed = parseExpression(text);
    if (!parsed.ok) throw std::invalid_argument("CAS 表达式解析失败（字节 " + std::to_string(parsed.position) + "）：" + parsed.message);
    return makeExpression(state, std::move(parsed.expression));
}
/**
 * @brief 输出 AST 的规范文本或 LaTeX。
 * @param state 当前 C 帧及诊断通道。
 * @return 规范文本或 LaTeX 文本的 String 句柄。
 */
int64_t casText(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    if (!value) return (0);
    const auto text = integerArgument(state, 1) ? toLatex(value->root.get()) : toText(value->root.get());
    return (makeText(state, ::Memory::String(text.data(), text.size())));
}
/**
 * @brief 查询根节点种类，不执行额外化简。
 * @param state 当前 C 帧及诊断通道。
 * @return 对应语言 NodeKind 的整数编码。
 */
int64_t casKind(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    return (value ? static_cast<std::int64_t>(value->root->kind()) : 0);
}
/**
 * @brief 读取符号、函数或命名常数的名称。
 * @param state 当前 C 帧及诊断通道。
 * @return 符号或函数名称的 String 句柄。
 */
int64_t casName(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    if (!value) return (0);
    std::string text = value->root->name() ? value->root->name() : "";
    if (value->root->kind() == NodeKind::Constant) text = toText(value->root.get());
    return (makeText(state, ::Memory::String(text.data(), text.size())));
}
/**
 * @brief 查询根节点直接子节点数量。
 * @param state 当前 C 帧及诊断通道。
 * @return 根节点直接子节点的数量。
 */
int64_t casSize(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    return (value ? static_cast<std::int64_t>(value->root->size()) : 0);
}
/**
 * @brief 返回独立持有的子 AST，父表达式释放后仍可使用。
 * @param state 当前 C 帧及诊断通道。
 * @return 独立保留指定子树的记录句柄。
 */
int64_t casChild(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    if (!value) return (0);
    const auto index = integerArgument(state, 1);
    if (index < 0 || static_cast<std::uint64_t>(index) >= value->root->size()) throw std::out_of_range("CAS 子节点下标越界");
    return makeExpression(state, Ptr::borrow(value->root->operand(static_cast<std::size_t>(index))));
}
/**
 * @brief 返回整数或有理数的精确分子、分母文本。
 * @param state 当前 C 帧及诊断通道。
 * @return 分子或分母十进制文本的 String 句柄。
 */
int64_t casNumberPart(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    if (!value) return (0);
    if (!value->root->isNumber()) throw std::domain_error("CAS 只有数值节点可以读取分子和分母");
    const auto text = integerArgument(state, 1) ? value->root->denominator().toString() : value->root->numerator().toString();
    return (makeText(state, ::Memory::String(text.data(), text.size())));
}
/**
 * @brief 直接组合并化简两个 AST 的四则运算或整数/符号幂。
 * @param state 当前 C 帧及诊断通道。
 * @return 四则运算或幂计算得到的新 AST 记录句柄。
 */
int64_t casBinary(ChaliAbiFrame *state)
{
    const auto *left = expression(state, integerArgument(state, 0));
    const auto *right = expression(state, integerArgument(state, 1));
    if (!left || !right) return (0);
    Ptr result;
    switch (integerArgument(state, 2))
    {
    case 0: result = makeAdd(left->root.share(), right->root.share()); break;
    case 1: result = makeAdd(left->root.share(), makeNegate(right->root.share())); break;
    case 2: result = makeMul(left->root.share(), right->root.share()); break;
    case 3:
        if (right->root->isZero()) throw std::domain_error("CAS 除数不能为零");
        result = makeDivide(left->root.share(), right->root.share());
        break;
    case 4: result = makePow(left->root.share(), right->root.share()); break;
    default: throw std::invalid_argument("CAS 二元运算编号无效");
    }
    return makeExpression(state, std::move(result));
}
/**
 * @brief 比较两棵规范 AST 的结构和值是否相等。
 * @param state 当前 C 帧及诊断通道。
 * @return 规范 AST 结构和值相等时为真。
 */
int64_t casEqual(ChaliAbiFrame *state)
{
    const auto *left = expression(state, integerArgument(state, 0));
    const auto *right = expression(state, integerArgument(state, 1));
    return (left && right && left->root->equals(right->root.get()));
}
/**
 * @brief 执行取负、化简、展开、因式分解、通分、求导或不定积分。
 * @param state 当前 C 帧及诊断通道。
 * @return 完成指定一元符号变换的新 AST 记录句柄。
 */
int64_t casUnary(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    if (!value) return (0);
    const auto operation = integerArgument(state, 1);
    Ptr result;
    switch (operation)
    {
    case 0: result = makeNegate(value->root.share()); break;
    case 1: result = simplify(value->root); break;
    case 2: result = expand(value->root); break;
    case 3: result = factor(value->root); break;
    case 4: result = togetherDeep(value->root); break;
    case 5:
    {
        const auto order = integerArgument(state, 3);
        if (order < 0 || order > 1000) throw std::out_of_range("CAS 导数阶数必须在 0..1000 之间");
        result = differentiateN(value->root, variableId(readText(state, integerArgument(state, 2))), static_cast<int>(order));
        break;
    }
    case 6:
    {
        const auto id = variableId(readText(state, integerArgument(state, 2)));
        result = integrate(value->root, id);
        if (!result)
        {
            Ptr symbol(Node::makeSymbolById(id));
            result = Ptr(Node::makeFunction("integrate", { value->root.get(), symbol.get() }));
        }
        break;
    }
    default: throw std::invalid_argument("CAS 一元运算编号无效");
    }
    return makeExpression(state, std::move(result));
}
/**
 * @brief 在 AST 上执行替换、极限、Taylor 展开或定积分。
 * @param state 当前 C 帧及诊断通道。
 * @return 替换、极限、展开或定积分得到的新 AST 记录句柄。
 */
int64_t casTransform(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    const auto *point = expression(state, integerArgument(state, 1));
    const auto *upper = expression(state, integerArgument(state, 2));
    if (!value || !point || !upper) return (0);
    const auto id = variableId(readText(state, integerArgument(state, 3)));
    Ptr result;
    switch (integerArgument(state, 4))
    {
    case 0: result = substitute(value->root, id, point->root); break;
    case 1: result = limit(value->root, id, point->root); break;
    case 2:
    {
        const auto order = integerArgument(state, 5);
        if (order < 0 || order > 1000) throw std::out_of_range("CAS Taylor 阶数必须在 0..1000 之间");
        result = taylorExpand(value->root, id, point->root, static_cast<int>(order));
        break;
    }
    case 3: result = definiteIntegral(value->root, id, point->root, upper->root); break;
    default: throw std::invalid_argument("CAS AST 变换编号无效");
    }
    return makeExpression(state, std::move(result));
}
/**
 * @brief 将精确有理数通过共享 BigFloat 后端转换为指定精度小数。
 * @param state 当前 C 帧及诊断通道。
 * @return 指定位数十进制结果的 String 句柄。
 */
int64_t casDecimal(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    if (!value) return (0);
    if (!value->root->isNumber()) throw std::domain_error("CAS 小数转换要求精确数值节点，请先替换自由变量");
    const auto digits = integerArgument(state, 1);
    if (digits < 1 || digits > 100000) throw std::out_of_range("CAS 小数有效位数必须在 1..100000 之间");
#if !defined(NO_CHALI_BIG_FLOAT)
    const auto numerator = bigfloat::BigFloat::fromInt(value->root->numerator().backend());
    const auto denominator = bigfloat::BigFloat::fromInt(value->root->denominator().backend());
    const auto text = numerator.divide(denominator, static_cast<int>(digits)).toString(0);
    return (makeText(state, text));
#else
    throw std::runtime_error("CAS 小数转换需要启用 BigDecimal 后端");
#endif
}
/**
 * @brief 将数学函数组合为保留精确常量的符号 AST。
 * @param state 当前宿主调用及诊断状态。
 * @return 新建并化简的符号表达式记录。
 */
int64_t casMath(ChaliAbiFrame *state)
{
    const auto *value = expression(state, integerArgument(state, 0));
    const auto *other = expression(state, integerArgument(state, 2));
    if (!value || !other) return (0);
    auto name = readText(state, integerArgument(state, 1));
    if (name == "ln") name = "log";
    if (name == "sqrt") return makeExpression(state, makePow(value->root.share(), makeNumber(BigInt(1), BigInt(2))));
    if (name == "log10")
    {
        auto numerator = Ptr(Node::makeFunction("log", { value->root.get() }));
        auto ten = Ptr(Node::makeInteger(BigInt(10)));
        auto denominator = Ptr(Node::makeFunction("log", { ten.get() }));
        return makeExpression(state, simplify(makeDivide(std::move(numerator), std::move(denominator))));
    }
    if (value->root->isNumber())
    {
        const auto numerator = value->root->numerator();
        const auto denominator = value->root->denominator();
        auto quotient = numerator / denominator;
        const auto remainder = numerator % denominator;
        if (name == "floor") return makeExpression(state, Ptr(Node::makeInteger(quotient)));
        if (name == "ceil") return makeExpression(state, Ptr(Node::makeInteger(quotient + BigInt(remainder.isZero() ? 0 : 1))));
        if (name == "trunc") return makeExpression(state, Ptr(Node::makeInteger(quotient + BigInt(numerator.isNegative() && !remainder.isZero() ? 1 : 0))));
        if (name == "round")
        {
            auto rounded = (numerator.abs() * BigInt(2) + denominator) / (denominator * BigInt(2));
            if (numerator.isNegative()) rounded = -rounded;
            return makeExpression(state, Ptr(Node::makeInteger(rounded)));
        }
        if (name == "fmod" && other->root->isNumber())
        {
            if (other->root->isZero()) throw std::domain_error("CAS 余数除数不能为零");
            const auto n = numerator * other->root->denominator();
            const auto d = denominator * other->root->numerator();
            auto q = n / d;
            if (n.isNegative() != d.isNegative() && !(n % d).isZero()) q = q + BigInt(1);
            auto product = makeMul(Ptr(Node::makeInteger(q)), other->root.share());
            return makeExpression(state, makeAdd(value->root.share(), makeNegate(std::move(product))));
        }
    }
    auto result = Ptr(Node::makeFunction(name, { value->root.get() }));
    if (name == "fmod") result = Ptr(Node::makeFunction(name, { value->root.get(), other->root.get() }));
    return makeExpression(state, trigSimplify(simplify(result)));
}
#else
/**
 * @brief 为显式裁剪 CAS 的宿主保留可诊断的入口。
 * @param state 当前调用诊断状态。
 * @return 零占位值并报告裁剪错误。
 */
int64_t casDisabled(ChaliAbiFrame *state)
{
    chali_abi_fail(state, "这个构建未编入 std::cas（需要标准库及大整数后端）");
    return (0);
}
#endif

/**
 * @brief 为 C 复制入口保留独立表达式根引用。
 * @param frame 当前帧。
 * @return 新记录句柄。
 */
int64_t copyExpression(ChaliAbiFrame *frame)
{
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
    return chali_abi_record_clone(frame, integerArgument(frame, 0), "std::cas::Expr", cloneExpression);
#else
    return casDisabled(frame);
#endif
}
/**
 * @brief 释放 C 表达式记录，根引用仍由其余副本独立持有。
 * @param frame 当前帧。
 * @return 零占位值。
 */
int64_t releaseExpression(ChaliAbiFrame *frame)
{
    chali_abi_record_release(frame, integerArgument(frame, 0));
    return 0;
}
/**
 * @brief 捕获 CAS 操作异常，完整错误文本无法分配时使用静态兜底。
 * @param frame 当前帧。
 * @param message 异常原始消息。
 */
void casFailure(ChaliAbiFrame *frame, const char *message) noexcept
{
    try
    {
        const auto text = std::string("CAS：") + message;
        chali_abi_fail(frame, text.c_str());
    } catch (...) { chali_abi_fail(frame, "CAS：操作失败"); }
}
/**
 * @brief 在统一 C 帧、CAS 锁与进程级数值分配作用域内执行操作。
 * @tparam Operation 不分配参数封送列表的数值操作。
 * @tparam Arity 精确参数数量。
 * @param frame 当前帧。
 */
template <auto Operation, int Arity>
void invoke(ChaliAbiFrame *frame)
try
{
    if (chali_abi_check_frame(frame, Arity, nullptr) != ChaliAbiOk)
    {
        chali_abi_fail(frame, "invalid CAS call frame");
        return;
    }
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
    std::lock_guard<std::recursive_mutex> lock(casMutex());
    ::Memory::AllocationScope allocationScope(::Memory::defaultRuntimeAllocator());
#endif
    chali_abi_return_integer(frame, Operation(frame));
} catch (const std::exception &error) { casFailure(frame, error.what()); } catch (...)
{
    casFailure(frame, "未知操作异常");
}
}  // namespace
/**
 * @brief 取得 CAS 符号表达式的直接 C 入口及现有完整限定名别名。
 * @param count 接收入口数，可为空。
 * @return 进程期有效的只读表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_cas_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_math", invoke<casMath, 3>, 3 },
#else
        { "__std_cas_math", invoke<casDisabled, 3>, 3 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_math", invoke<casMath, 3>, 3 },
#else
        { "std::cas::__std_cas_math", invoke<casDisabled, 3>, 3 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_new", invoke<casNew, 0>, 0 },
#else
        { "__std_cas_new", invoke<casDisabled, 0>, 0 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_new", invoke<casNew, 0>, 0 },
#else
        { "std::cas::__std_cas_new", invoke<casDisabled, 0>, 0 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_parse", invoke<casParse, 1>, 1 },
#else
        { "__std_cas_parse", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_parse", invoke<casParse, 1>, 1 },
#else
        { "std::cas::__std_cas_parse", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_text", invoke<casText, 2>, 2 },
#else
        { "__std_cas_text", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_text", invoke<casText, 2>, 2 },
#else
        { "std::cas::__std_cas_text", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_kind", invoke<casKind, 1>, 1 },
#else
        { "__std_cas_kind", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_kind", invoke<casKind, 1>, 1 },
#else
        { "std::cas::__std_cas_kind", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_name", invoke<casName, 1>, 1 },
#else
        { "__std_cas_name", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_name", invoke<casName, 1>, 1 },
#else
        { "std::cas::__std_cas_name", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_size", invoke<casSize, 1>, 1 },
#else
        { "__std_cas_size", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_size", invoke<casSize, 1>, 1 },
#else
        { "std::cas::__std_cas_size", invoke<casDisabled, 1>, 1 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_child", invoke<casChild, 2>, 2 },
#else
        { "__std_cas_child", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_child", invoke<casChild, 2>, 2 },
#else
        { "std::cas::__std_cas_child", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_number_part", invoke<casNumberPart, 2>, 2 },
#else
        { "__std_cas_number_part", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_number_part", invoke<casNumberPart, 2>, 2 },
#else
        { "std::cas::__std_cas_number_part", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_binary", invoke<casBinary, 3>, 3 },
#else
        { "__std_cas_binary", invoke<casDisabled, 3>, 3 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_binary", invoke<casBinary, 3>, 3 },
#else
        { "std::cas::__std_cas_binary", invoke<casDisabled, 3>, 3 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_equal", invoke<casEqual, 2>, 2 },
#else
        { "__std_cas_equal", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_equal", invoke<casEqual, 2>, 2 },
#else
        { "std::cas::__std_cas_equal", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_unary", invoke<casUnary, 4>, 4 },
#else
        { "__std_cas_unary", invoke<casDisabled, 4>, 4 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_unary", invoke<casUnary, 4>, 4 },
#else
        { "std::cas::__std_cas_unary", invoke<casDisabled, 4>, 4 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_transform", invoke<casTransform, 6>, 6 },
#else
        { "__std_cas_transform", invoke<casDisabled, 6>, 6 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_transform", invoke<casTransform, 6>, 6 },
#else
        { "std::cas::__std_cas_transform", invoke<casDisabled, 6>, 6 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "__std_cas_decimal", invoke<casDecimal, 2>, 2 },
#else
        { "__std_cas_decimal", invoke<casDisabled, 2>, 2 },
#endif
#if !defined(NO_CHALI_STD) && !defined(NO_CHALI_STD_CAS) && !defined(NO_CHALI_BIG_INT)
        { "std::cas::__std_cas_decimal", invoke<casDecimal, 2>, 2 },
#else
        { "std::cas::__std_cas_decimal", invoke<casDisabled, 2>, 2 },
#endif
        { "__std_cas_copy", invoke<copyExpression, 1>, 1 },
        { "__std_cas_drop", invoke<releaseExpression, 1>, 1 },
        { "std::cas::__std_cas_copy", invoke<copyExpression, 1>, 1 },
        { "std::cas::__std_cas_drop", invoke<releaseExpression, 1>, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}

#define CHALI_AOT_BIND_MODULE cas
#include "aot_bind_module.h"
