/**
 * @file aggregate_optimize.cpp
 * @brief 在已证明新目标的初始化中合并聚合返回槽及最终搬运。
 */
#include "aggregate_optimize.hpp"
#include <cstdlib>
#include <cmath>
#include <map>
#include <set>

namespace ast
{
namespace
{
/**
 * @brief 递归克隆 AST，避免优化修改调用方保存的原始程序。
 * @param node 原节点。
 * @return 独立节点树，类型布局保持共享且只读。
 */
NodePtr cloneNode(const NodePtr &node)
{
    if (!node) return nullptr;
    auto copy = Memory::makeShared<Node>(*node);
    copy->kids.clear();
    for (const auto &child : node->kids) copy->kids.push_back(cloneNode(child));
    return copy;
}

/**
 * @brief 识别降低器生成的返回槽以及元组、枚举构造存储。
 * @param name 存储名称。
 * @return 属于内部返回存储时返回 true。
 */
bool isReturnStorage(const Memory::String &name)
{
    return name.rfind("__chali_ret_v$", 0) == 0u || name == "__chali_tuple_value" || name == "__chali_enum_value";
}

/**
 * @brief 检查初始化实参是否观察或导出目标存储，保留自引用初始化的旧路径。
 * @param node 实参表达式树。
 * @param name 新目标符号。
 * @return 表达式中出现该目标时返回 true。
 */
bool referencesDestination(const NodePtr &node, const Memory::String &name)
{
    if (!node) return false;
    if (node->kind == Node::Kind::Var && node->text == name) return true;
    for (const auto &child : node->kids)
        if (referencesDestination(child, name)) return true;
    return false;
}

/**
 * @brief 看门狗可能在安全点调用用户代码，相关程序保守保留原存储和步骤。
 * @param node 待检查函数体。
 * @return 出现看门狗调用时返回 true。
 */
bool containsWatchdog(const NodePtr &node)
{
    if (!node) return false;
    if (node->kind == Node::Kind::Call && node->text.find("watchdog") != Memory::String::npos) return true;
    for (const auto &child : node->kids)
        if (containsWatchdog(child)) return true;
    return false;
}

/**
 * @brief 收集明确的返回暂存及标签，拒绝未识别的聚合返回形态。
 * @param node 函数树。
 * @param returns 返回暂存名称集合。
 * @param labels 本函数标签集合。
 * @param valid 是否仍满足所有返回值经返回暂存传递。
 */
void collectReturns(const NodePtr &node, std::set<Memory::String> &returns,
                    std::set<Memory::String> &labels, bool &valid)
{
    if (!node) return;
    if (node->kind == Node::Kind::Declare && isReturnStorage(node->text)
        && node->declared && node->declared->kind == Type::Kind::Struct) returns.insert(node->text);
    if (node->kind == Node::Kind::Label) labels.insert(node->text);
    if (node->kind == Node::Kind::Return && !node->kids.empty())
    {
        const auto &value = node->kids[0];
        if (!value || (value->kind == Node::Kind::IntLit ? value->intValue != 0 : value->kind != Node::Kind::Var || !isReturnStorage(value->text))) valid = false;
    }
    for (const auto &child : node->kids) collectReturns(child, returns, labels, valid);
}

/**
 * @brief 将返回暂存替换为调用者目标，重命名局部标签以保留原入口。
 * @param node 待改写节点。
 * @param returns 待合并返回暂存。
 * @param destination 隐藏目标形参。
 * @param labels 原标签到变体标签的映射。
 * @return 改写后的节点。
 */
NodePtr redirectReturns(NodePtr node, const std::set<Memory::String> &returns,
                        const Memory::String &destination,
                        const std::map<Memory::String, Memory::String> &labels)
{
    if (!node) return node;
    if (node->kind == Node::Kind::Declare && returns.count(node->text)) return Node::block({});
    if (node->kind == Node::Kind::Assign && returns.count(node->text)
        && node->kids.size() == 1u && node->kids[0] && node->kids[0]->kind == Node::Kind::StructNew)
    {
        Memory::Vector<NodePtr> stores;
        const auto &values = node->kids[0]->kids;
        for (std::size_t i = 0; i < values.size(); ++i)
        {
            const auto &value = values[i];
            if (value && ((value->kind == Node::Kind::IntLit && value->intValue == 0) || (value->kind == Node::Kind::FloatLit && value->floatValue == 0.0 && !std::signbit(value->floatValue)))) continue;
            stores.push_back(Node::storeAt(Node::offset(Node::var(destination), Node::i(static_cast<std::int64_t>(i)), 8),
                                           cloneNode(value), value && value->kind == Node::Kind::FloatLit ? Type::f64() : Type::i64()));
        }
        return Node::block(std::move(stores));
    }
    if (node->kind == Node::Kind::Var && returns.count(node->text)) node->text = destination;
    if (node->kind == Node::Kind::Label || node->kind == Node::Kind::Jump)
    {
        const auto found = labels.find(node->text);
        if (found != labels.end()) node->text = found->second;
    }
    for (auto &child : node->kids) child = redirectReturns(child, returns, destination, labels);
    return node;
}

/**
 * @brief 目标地址变体的原函数和隐藏参数名。
 */
struct Variant
{
    Function function;
    Memory::String destination;
};

/**
 * @brief 为原入口生成变体描述，只有识别全部返回形态时允许合并。
 * @param source 原函数。
 * @param index 唯一编号。
 * @param types 程序布局。
 * @param variant 成功时保存内部函数。
 * @return 可安全生成变体时为 true。
 */
bool makeVariant(const Function &source, std::size_t index,
                 const Memory::Vector<TypePtr> &types, Variant &variant)
{
    if (!source.result || source.result->kind != Type::Kind::Struct) return false;
    std::set<Memory::String> returns, labels;
    bool valid = true;
    collectReturns(source.body, returns, labels, valid);
    if (!valid || returns.empty()) return false;
    variant.function = source;
    variant.function.owner.clear();
    variant.function.name = "__chali_into$" + Memory::toString(index) + "$" + source.quadLabel();
    variant.destination = "__chali_destination$" + Memory::toString(index);
    if (!source.owner.empty())
    {
        TypePtr owner = Type::i64();
        for (const auto &type : types)
            if (type && type->name == source.owner) owner = type;
        variant.function.parameters.emplace_back("self", owner);
    }
    variant.function.parameters.emplace_back(variant.destination, Type::i64());
    std::map<Memory::String, Memory::String> renamed;
    for (const auto &label : labels)
        renamed[label] = label == "__chali_cleanup." + source.quadLabel() ? "__chali_cleanup." + variant.function.name : variant.function.name + "$" + label;
    variant.function.body = redirectReturns(cloneNode(source.body), returns, variant.destination, renamed);
    return true;
}

/**
 * @brief 改写已标记的新聚合初始化，其他赋值及间接调用全部保留。
 * @param node 初始化或普通节点。
 * @param variants 可用目标地址函数。
 * @param used 实际调用到的变体标签。
 * @return 保持求值顺序的目标调用或原节点。
 */
NodePtr rewriteCalls(NodePtr node, const std::map<Memory::String, Variant> &variants,
                     std::set<Memory::String> &used)
{
    if (!node) return node;
    for (auto &child : node->kids) child = rewriteCalls(child, variants, used);
    if (node->kind != Node::Kind::Block || node->typeName != "__fresh_aggregate_destination"
        || node->kids.size() < 3u) return node;
    const auto &save = node->kids[1];
    const auto &transfer = node->kids[2];
    if (!save || save->kind != Node::Kind::Assign || save->kids.size() != 1u
        || !save->kids[0] || save->kids[0]->kind != Node::Kind::Call
        || !transfer || transfer->kind != Node::Kind::Block || transfer->kids.empty()) return node;
    const auto &statement = transfer->kids[0];
    if (!statement || statement->kind != Node::Kind::ExprStmt || statement->kids.size() != 1u) return node;
    const auto &copy = statement->kids[0];
    if (!copy || copy->kind != Node::Kind::Call || copy->text != "__chali_copy_struct" || copy->kids.size() != 3u
        || !copy->kids[0] || copy->kids[0]->kind != Node::Kind::Var) return node;
    const auto found = variants.find(save->kids[0]->text);
    if (found == variants.end()) return node;
    auto call = cloneNode(save->kids[0]);
    if (call->names.size() != call->kids.size()) return node;
    if (referencesDestination(call, copy->kids[0]->text)) return node;
    call->text = found->second.function.name;
    call->names.push_back(found->second.destination);
    call->kids.push_back(cloneNode(copy->kids[0]));
    Memory::Vector<NodePtr> body{ Node::exprStmt(call) };
    for (std::size_t i = 1; i < transfer->kids.size(); ++i) body.push_back(transfer->kids[i]);
    used.insert(found->first);
    return Node::block(std::move(body));
}
}  // namespace

/**
 * @brief 合并直接聚合返回与新变量存储，保留原调用约定作为完整回退路径。
 * @param input 已降低程序。
 * @return 独立节点的优化程序。
 */
Program optimizeAggregateDestinations(const Program &input)
{
    const auto *disabled = std::getenv("CHALI_NO_AGGREGATE_OPT");
    if (disabled && *disabled) return input;
    for (const auto &function : input.functions)
        if (containsWatchdog(function.body)) return input;
    Program result = input;
    std::map<Memory::String, Variant> variants;
    for (std::size_t i = 0; i < input.functions.size(); ++i)
    {
        Variant variant;
        if (makeVariant(input.functions[i], i, input.types, variant))
            variants.emplace(input.functions[i].quadLabel(), std::move(variant));
    }
    std::set<Memory::String> used, emitted;
    for (auto &function : result.functions) function.body = rewriteCalls(cloneNode(function.body), variants, used);
    while (emitted.size() < used.size())
    {
        const auto pending = used;
        for (const auto &name : pending)
        {
            if (!emitted.insert(name).second) continue;
            auto function = variants.at(name).function;
            function.body = rewriteCalls(cloneNode(function.body), variants, used);
            result.functions.push_back(std::move(function));
        }
    }
    return result;
}
}  // namespace ast
