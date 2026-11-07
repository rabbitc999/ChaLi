/* 已绑定存储与控制流的适配层；构图后核对来源契约，函数摘要传播后再检查失效访问。 */
using MBorrowPlace = chali_borrow_checker::Place;
using MBorrowFlowNode = chali_borrow_checker::FlowNode;
/**
 * @brief 单个函数的已绑定源码事实，避免离开作用域后重新查找同名变量。
 */
struct MBorrowFlowCapture
{
    Memory::Map<int, MBorrowPlace> variables;
    Memory::Map<Memory::String, Memory::String> names;
    Memory::Map<Memory::String, Memory::String> parameters;
    Memory::Map<Memory::String, std::shared_ptr<MType>> parameterTypes;
    Memory::Set<Memory::String> globals;
    Memory::Map<int, std::size_t> depths;
    Memory::Map<int, Memory::Vector<MBorrowPlace>> lambdaMoves;
    Memory::Map<int, Memory::Vector<MBorrowPlace>> lambdaReads;
    Memory::Map<int, Memory::Vector<MBorrowPlace>> lambdaBorrows;
    Memory::Map<int, bool> references;
    Memory::Map<int, std::shared_ptr<MType>> types;
    Memory::Map<int, Memory::String> calls;
    Memory::Map<int, bool> managed;
    Memory::Map<int, MBorrowPlace> declarations;
    Memory::Map<int, bool> declarationReferences;
    struct Store
    {
        int target;
        bool binding;
    };
    Memory::Map<int, Memory::Vector<Store>> stores;  ///< 按实际值求值节点记录赋值、容器与 Any 的借用写入。
    Memory::Map<int, MBorrowPlace> redeclarations;
    Memory::Map<int, MBorrowPlace> foreachBindings;
    Memory::Set<int> foreachNoDefault;
    Memory::Map<int, std::pair<MBorrowPlace, int>> foreachSnapshots;
    Memory::Set<int> storageReleases;                                  ///< 明确释放堆存储的 drop() 调用。
    Memory::Set<int> terminalCalls;                                    ///< 确认为 Drop 协议的显式调用，不包括普通同名方法。
    Memory::Map<int, std::pair<ast::NodePtr, ast::NodePtr>> releases;  ///< 显式终结及其路径守卫。
    Memory::Set<int> valueLoops;                                       ///< 实际分配了结果槽的循环，独立语句也可能接收并丢弃结果。
};
/**
 * @brief 调用点的实际存储与待填充的引用移动效果。
 */
struct MBorrowFlowCall
{
    std::size_t entry = 0;
    Memory::String callee;
    Memory::Vector<MBorrowPlace> arguments;
    Memory::Set<MBorrowPlace> applied;
};
/**
 * @brief 函数退出状态及引用形参到调用实参的对应关系。
 */
struct MBorrowFunctionFlow
{
    Memory::String label;
    Memory::Vector<Memory::String> parameters;
    Memory::Vector<MBorrowFlowCall> calls;
    std::size_t exit = 0;
};
Memory::Vector<chali_borrow_checker::FlowGraph> g_borrowFlows;
Memory::Vector<MBorrowFunctionFlow> g_borrowFunctionFlows;
MBorrowFlowCapture *g_borrowFlowCapture = nullptr;
/**
 * @brief 保存拥有捕获内部引用的外部来源，结构字段按真实布局逐层展开。
 * @param id 闭包表达式节点。
 * @param source 捕获值的来源存储路径。
 * @param type 捕获值类型。
 * @param depth 当前字段嵌套层数，限制递归类型展开。
 */
void CaptureOwnedClosureBorrows(int id, MBorrowPlace source, const std::shared_ptr<MType> &type, unsigned depth = 0)
{
    if (!g_borrowFlowCapture || !type || depth > 16) return;
    if (type->kind == MType::Ptr || IsRuntimeArray(type))
    {
        source.path.push_back("*");
        g_borrowFlowCapture->lambdaBorrows[id].push_back(std::move(source));
    }
    else if (type->kind == MType::Struct)
    {
        const auto definition = g_structs.find(type->name);
        if (definition == g_structs.end()) return;
        for (const auto &field : definition->second.fields)
        {
            auto child = source;
            child.path.push_back("." + field.name);
            CaptureOwnedClosureBorrows(id, std::move(child), field.type, depth + 1);
        }
    }
}
/**
 * @brief 保存表达式变量身份及已推导类型，保留字段引用和影子变量区别。
 * @param id 源码节点。
 * @param type 已推导类型。
 * @return 无。
 */
void CaptureBorrowExpression(int id, const std::shared_ptr<MType> &type)
{
    if (!g_borrowFlowCapture || id < 0) return;
    if (type)
    {
        g_borrowFlowCapture->types[id] = type;
        if (HasBorrowedValue(type)) g_borrowFlowCapture->depths[id] = BorrowSourceDepth(id);
        g_borrowFlowCapture->references[id] = type->kind == MType::Ptr || IsRuntimeArray(type);
        const auto value = type->kind == MType::Ptr ? type->elem : type;
        g_borrowFlowCapture->managed[id] = value && (value->kind == MType::Struct || IsOwnedKind(value));
    }
    std::function<void(int)> capture = [&](int current) {
        if (current < 0) return;
        const auto node = N(current);
        if (node.kind == MNode::Block || node.kind == MNode::FuncDef) return;
        if (node.kind == MNode::Var)
        {
            const auto variable = FindNodeVar(node);
            if (variable)
            {
                g_borrowFlowCapture->variables[current] = { variable->emitted, {} };
                g_borrowFlowCapture->names[variable->emitted] = node.s;
                for (const auto &entry : g_scope.front())
                    if (&entry.second == variable) g_borrowFlowCapture->globals.insert(variable->emitted);
                g_borrowFlowCapture->references[current] = variable->type->kind == MType::Ptr || IsRuntimeArray(variable->type);
                const auto value = variable->type->kind == MType::Ptr ? variable->type->elem : variable->type;
                g_borrowFlowCapture->managed[current] = value && (value->kind == MType::Struct || IsOwnedKind(value));
            }
            else if (!g_owner.empty() && (node.s == "self" || FieldType(g_owner, node.s)))
            {
                g_borrowFlowCapture->variables[current] = { "self:" + g_owner,
                                                            node.s == "self" ? Memory::Vector<Memory::String>{} : Memory::Vector<Memory::String>{ "." + node.s } };
            }
            return;
        }
        for (const int child : node.kids) capture(child);
    };
    capture(id);
}
/**
 * @brief 保存局部声明最终存储槽，引用槽与被引用对象独立建模。
 * @param id 声明节点。
 * @param variable 已完成初始化的变量。
 * @return 无。
 */
void CaptureBorrowDeclaration(int id, const MVar *variable)
{
    if (!g_borrowFlowCapture || !variable) return;
    g_borrowFlowCapture->declarations[id] = { variable->emitted, {} };
    g_borrowFlowCapture->names[variable->emitted] = N(id).s;
    g_borrowFlowCapture->declarationReferences[id] = variable->type->kind == MType::Ptr || IsRuntimeArray(variable->type);
}
/**
 * @brief 保存语义确认的借用写入，由控制流按实际求值路径检查契约。
 * @param target 写入目标，包括容器接收者。
 * @param value 要保存的引用或含引用的值。
 * @param binding 是否重绑引用槽，而非写入其指向的对象。
 */
void CaptureBorrowStore(int target, int value, bool binding)
{
    if (!g_borrowFlowCapture || target < 0 || value < 0) return;
    const auto valueType = RawExprType(value);
    if (!HasBorrowedValue(valueType) && !IsRuntimeArray(valueType)) return;
    CaptureBorrowExpression(target, RawExprType(target));
    CaptureBorrowExpression(value, valueType);
    auto &stores = g_borrowFlowCapture->stores[value];
    if (std::none_of(stores.begin(), stores.end(), [&](const MBorrowFlowCapture::Store &store) {
            return store.target == target && store.binding == binding;
        })) stores.push_back({target, binding});
}
#include "borrow_return_sources.cpp"

/**
 * @brief 从绑定事实构造显式后继图，分支和循环不依赖语义分析的访问顺序。
 */
struct MBorrowFlowBuilder
{
    const MBorrowFlowCapture &capture;
    chali_borrow_checker::FlowGraph graph;
    mutable Memory::Map<Memory::String, MBorrowOrigins> summaries;
    Memory::Vector<MBorrowFlowCall> calls;
    Memory::Vector<int> defers;
    /**
     * @brief 实际求值后的契约来源检查点。
     */
    struct ContractCheck
    {
        std::size_t node;
        Memory::String target;
        MBorrowPlace source;
        int expression;
        MBorrowPlace destination;  ///< 可经别名解析到契约目标；声明与返回检查可为空。
    };
    Memory::Vector<ContractCheck> contractChecks;
    std::size_t functionExit = 0;
    std::size_t loopPeelBudget = 0;  ///< 按本函数源码节点数限制循环首轮展开，避免嵌套循环指数增长。
    /**
     * @brief 已解析的控制出口及跳转后仍保留的延迟清理深度。
     */
    struct ControlTarget
    {
        int first;
        std::size_t second;
        std::size_t deferDepth;
    };
    Memory::Vector<ControlTarget> valueStops;  ///< 值表达式出口独立于普通循环出口，带值退出可穿透循环。
    Memory::Vector<ControlTarget> controlStops;                      ///< 解析后出口身份与当前图中的后继。
    Memory::Vector<ControlTarget> controlRepeats;                    ///< 解析后循环身份与当前图中的下一轮入口。
    /**
     * @brief 将方法体返回来源替换为调用点的具体接收者或实参。
     * @param id 调用节点。
     * @return 来源集合，新分配保留专用标记。
     */
    MBorrowOrigins methodOrigins(int id) const
    {
        const auto call = capture.calls.find(id);
        if (call == capture.calls.end()) return {};
        const auto dot = call->second.rfind('.');
        if (dot == Memory::String::npos) return {};
        const auto owner = call->second.substr(0, dot);
        const auto definition = g_structs.find(owner);
        if (definition == g_structs.end()) return {};
        auto summary = summaries.find(call->second);
        if (summary == summaries.end())
        {
            MBorrowOrigins origins;
            for (const auto &method : definition->second.methods)
                if (!method.superseded && method.name == call->second.substr(dot + 1) && method.bodyId >= 0)
                {
                    MBorrowReturnSummary extractor(owner, method);
                    extractor.statement(method.bodyId);
                    origins = std::move(extractor.returned);
                    break;
                }
            summary = summaries.emplace(call->second, std::move(origins)).first;
        }
        const auto node = N(id);
        const bool directStatic = node.kind == MNode::EnumCtor && node.type2 == 1;
        const auto callee = directStatic ? MNode{} : N(node.kids[0]);
        const auto arguments = directStatic ? node.kids : node.kids.size() > 1 ? N(node.kids[1]).kids : Memory::Vector<int>{};
        MBorrowOrigins result;
        for (const auto &origin : summary->second)
        {
            if (origin.root == "$fresh")
            {
                result.insert(origin);
                continue;
            }
            MBorrowPlace concrete;
            if (origin.root == "$self")
            {
                if (directStatic || (callee.kind != MNode::Member && callee.kind != MNode::Arrow) || callee.kids.empty()) continue;
                concrete = place(callee.kids[0]);
            }
            else if (origin.root.compare(0, 5, "$arg:") == 0)
            {
                const auto index = static_cast<std::size_t>(std::strtoull(origin.root.c_str() + 5, nullptr, 10));
                if (index < arguments.size()) concrete = place(arguments[index]);
            }
            if (concrete.root.empty()) continue;
            concrete.path.insert(concrete.path.end(), origin.path.begin(), origin.path.end());
            result.insert(std::move(concrete));
        }
        return result;
    }
    /**
     * @brief 添加一项操作。
     * @param kind 操作。
     * @param target 目标。
     * @param source 来源。
     * @param next 后继。
     * @param id 源码。
     * @return 新节点。
     */
    std::size_t add(MBorrowFlowNode::Kind kind, MBorrowPlace target, MBorrowPlace source, std::size_t next, int id)
    {
        const auto index = graph.nodes.size();
        graph.nodes.push_back({ kind, std::move(target), std::move(source), {}, id >= 0 ? (N(id).expansionLine ? N(id).expansionLine : N(id).sourceLine) : 0u, id >= 0 && !N(id).s.empty() ? N(id).s : Memory::String("表达式"), id });
        if (next != static_cast<std::size_t>(-1)) graph.nodes.back().next.push_back(next);
        return index;
    }
    /**
     * @brief 在退出边按后登记先执行的顺序检查仍存活的延迟回调借用。
     * @param next 清理完成后的后继。
     * @param depth 本次退出保留的外层延迟动作数量。
     * @return 清理检查入口。
     */
    std::size_t cleanup(std::size_t next, std::size_t depth)
    {
        for (std::size_t i = depth; i < defers.size(); ++i)
        {
            const int id = defers[i];
            next = add(MBorrowFlowNode::CheckCaptures, { "$defer:" + Memory::toString(id), {} }, {}, next, id);
            graph.nodes[next].label = "defer";
        }
        return next;
    }
    /**
     * @brief 在实参求值后保留具体调用的引用移动效果占位。
     * @param id 调用节点。
     * @param arguments 显式实参。
     * @param next 调用后继。
     * @param receiver 实例接收者；静态调用为负数。
     * @return 调用效果入口。
     */
    std::size_t callEffects(int id, const Memory::Vector<int> &arguments, std::size_t next, int receiver = -1)
    {
        const auto called = capture.calls.find(id);
        if (called == capture.calls.end()) return next;
        const auto entry = add(MBorrowFlowNode::Nop, {}, {}, next, id);
        MBorrowFlowCall site;
        site.entry = entry;
        site.callee = called->second;
        if (receiver >= 0) site.arguments.push_back(place(receiver));
        for (const int argument : arguments) site.arguments.push_back(place(argument));
        calls.push_back(std::move(site));
        return entry;
    }
    /**
     * @brief 为含堆释放的循环分离首轮入口，保留游标重绑与释放状态的路径关联。
     * @param begin 本循环的首个图节点。
     * @param repeat 后续迭代入口；首轮回边直接进入原循环。
     * @param entry 原首轮入口。
     * @return 预算允许时返回首轮副本入口，否则保守复用原入口。
     */
    std::size_t peelLoop(std::size_t begin, std::size_t repeat, std::size_t entry)
    {
        const auto end = graph.nodes.size();
        const auto count = end - begin;
        if (entry < begin || entry >= end || count > loopPeelBudget || std::none_of(graph.nodes.begin() + begin, graph.nodes.end(),
                                                  [](const MBorrowFlowNode &node) { return node.kind == MBorrowFlowNode::Destroy; })) return entry;
        loopPeelBudget -= count;
        const auto offset = end - begin;
        for (auto index = begin; index < end; ++index)
        {
            auto node = graph.nodes[index];
            for (auto &next : node.next)
                if (next != repeat && next >= begin && next < end) next += offset;
            graph.nodes.push_back(std::move(node));
        }
        const auto callCount = calls.size();
        for (std::size_t i = 0; i < callCount; ++i)
            if (calls[i].entry >= begin && calls[i].entry < end)
            {
                auto call = calls[i];
                call.entry += offset;
                calls.push_back(std::move(call));
            }
        const auto checkCount = contractChecks.size();
        for (std::size_t i = 0; i < checkCount; ++i)
            if (contractChecks[i].node >= begin && contractChecks[i].node < end)
            {
                auto check = contractChecks[i];
                check.node += offset;
                contractChecks.push_back(std::move(check));
            }
        return entry + offset;
    }
    /**
     * @brief 保存引用值出口的来源，再连接该出口应执行的清理。
     * @param value 出口值表达式。
     * @param target 已解析的值出口。
     * @return 实参求值入口。
     */
    std::size_t exitValue(int value, const ControlTarget &target)
    {
        auto next = cleanup(target.second, target.deferDepth);
        if (value < 0) return next;
        const auto type = capture.types.find(target.first);
        if (type != capture.types.end() && IsRuntimeArray(type->second) && !HasBorrowedValue(type->second->elem))
            next = add(MBorrowFlowNode::Allocate, place(target.first, true),
                       { "$yieldedArray:" + Memory::toString(value), {} }, next, value);
        else if (reference(target.first))
            next = add(MBorrowFlowNode::Bind, place(target.first, true), place(value), next, value);
        return expression(value, next);
    }
    /**
     * @brief 引用节点是否指向存储。
     * @param id 源码节点。
     * @return 引用时为 true。
     */
    bool reference(int id) const
    {
        const auto found = capture.references.find(id);
        return found != capture.references.end() && found->second;
    }
    /**
     * @brief 判断表达式结果是否可能携带闭包及其借用来源。
     * @param id 待检查的源码表达式。
     * @return 函数值或含函数字段的聚合返回 true。
     */
    bool carriesCaptures(int id) const
    {
        const auto found = capture.types.find(id);
        if (found == capture.types.end()) return false;
        Memory::Set<Memory::String> visited;
        std::function<bool(const std::shared_ptr<MType> &)> contains = [&](const std::shared_ptr<MType> &type) {
            if (!type) return false;
            if (type->kind == MType::Func) return true;
            if (type->kind != MType::Struct || !visited.insert(type->name).second) return false;
            const auto definition = g_structs.find(type->name);
            if (definition == g_structs.end()) return false;
            for (const auto &field : definition->second.fields)
                if (contains(field.type)) return true;
            return false;
        };
        return contains(found->second);
    }
    /**
     * @brief 获取带解引用分段的对象路径。
     * @param id 源码节点。
     * @param slot 是否保留最终引用槽。
     * @return 存储路径。
     */
    MBorrowPlace place(int id, bool slot = false) const
    {
        if (id < 0) return {};
        const auto node = N(id);
        if (node.kind == MNode::GenericClosureSelect && !node.kids.empty()) return place(node.kids[0], slot);
        if (capture.lambdaReads.count(id)) return { "$closure:" + Memory::toString(id), {} };
        MBorrowPlace result;
        const auto variable = capture.variables.find(id);
        if (variable != capture.variables.end())
            result = variable->second;
        else if (node.kind == MNode::Call && node.kids.size() > 1 && (StandardCallName(N(node.kids[0])) == "__move" || StandardCallName(N(node.kids[0])) == "copy") && N(node.kids[1]).kids.size() == 1u)
            return place(N(node.kids[1]).kids[0], slot);
        else if (node.kind == MNode::Loop && reference(id))
            result = { "$value:" + Memory::toString(id), {} };
        else if (node.kind == MNode::Call && carriesCaptures(id))
            return { "$callvalue:" + Memory::toString(id), {} };
        else if (node.kind == MNode::EnumCtor && node.type2 == 1 && reference(id) && !methodOrigins(id).empty())
            result = { "$call:" + Memory::toString(id), {} };
        else if (node.kind == MNode::Call && reference(id) && !node.kids.empty())
        {
            const auto callee = N(node.kids[0]);
            if (!methodOrigins(id).empty()) result = { "$call:" + Memory::toString(id), {} };
            if (callee.kind != MNode::Member && callee.kind != MNode::Arrow && node.kids.size() > 1)
                for (const int argument : N(node.kids[1]).kids)
                    if (reference(argument) && !place(argument).root.empty())
                    {
                        result = { "$call:" + Memory::toString(id), {} };
                        break;
                    }
        }
        else if ((node.kind == MNode::Member || node.kind == MNode::Arrow || node.kind == MNode::Index) && !node.kids.empty())
        {
            result = place(node.kids[0]);
            if (node.kind == MNode::Index)
            {
                const auto index = N(node.kids[1]);
                result.path.push_back(index.kind == MNode::Int ? "[" + Memory::toString(index.i) + "]" : Memory::String("[*]"));
            }
            else
                result.path.push_back("." + node.s);
        }
        else if ((node.kind == MNode::Un || node.kind == MNode::Cast) && !node.kids.empty())
            return place(node.kids[0], slot);
        if (!result.root.empty() && reference(id) && !slot) result.path.push_back("*");
        return result;
    }
    /**
     * @brief 只计算目标地址中的下标和接收者副作用。
     * @param id 目标。
     * @param next 后继。
     * @return 入口。
     */
    std::size_t address(int id, std::size_t next)
    {
        if (id < 0) return next;
        const auto node = N(id);
        if (node.kind == MNode::Index) return address(node.kids[0], expression(node.kids[1], next));
        if (node.kind == MNode::Un && node.s == "&" && !node.kids.empty())
        {
            const auto found = capture.types.find(id);
            const auto target = found != capture.types.end() && found->second && found->second->kind == MType::Ptr ? found->second->elem : nullptr;
            // 对拥有型胶水值建立借用也要验证源仍存活，不能用取地址绕过移动限制。
            if (target && (FindOpcodeDefinition(target) || HasNonCopyableValue(target)))
                next = add(MBorrowFlowNode::Read, place(node.kids[0]), {}, next, id);
            return address(node.kids[0], next);
        }
        if ((node.kind == MNode::Member || node.kind == MNode::Arrow || node.kind == MNode::Un || node.kind == MNode::Cast) && !node.kids.empty())
            return address(node.kids[0], next);
        if (node.kind == MNode::Call || node.kind == MNode::Loop || node.kind == MNode::Block || (node.kind == MNode::EnumCtor && node.type2 == 1 && reference(id))) return expression(id, next);
        return next;
    }
    /**
     * @brief 识别引用赋值，与通过引用写入普通值区分。
     * @param target 目标。
     * @param value 来源。
     * @return 重绑时为 true。
     */
    bool binding(int target, int value) const
    {
        if (!reference(target) || value < 0) return false;
        const auto node = N(value);
        return (node.kind == MNode::Un && node.s == "&") || node.kind == MNode::NewPtr;
    }
    /**
     * @brief 复制函数值及元组、枚举载荷中保存的闭包借用来源。
     * @param target 接收值的存储路径。
     * @param value 来源表达式。
     * @param next 复制后的控制流后继。
     * @return 元数据复制操作的入口。
     */
    std::size_t copyCaptures(const MBorrowPlace &target, int value, std::size_t next)
    {
        if (value < 0) return add(MBorrowFlowNode::CopyCaptures, target, {}, next, value);
        const auto node = N(value);
        if (node.kind == MNode::Tuple)
        {
            for (std::size_t i = node.kids.size(); i > 0; --i)
            {
                auto field = target;
                field.path.push_back(".__" + Memory::toString(i - 1));
                next = copyCaptures(field, node.kids[i - 1], next);
            }
            return add(MBorrowFlowNode::CopyCaptures, target, {}, next, value);
        }
        const auto type = capture.types.find(value);
        if (node.kind == MNode::EnumCtor && type != capture.types.end() && type->second)
        {
            const auto variant = FindEnumVariant(type->second->name, node.s2);
            if (variant)
                for (std::size_t i = 0; i < variant->payloads.size() && i < node.kids.size(); ++i)
                {
                    auto field = target;
                    field.path.push_back("." + variant->payloads[i].storageName);
                    next = copyCaptures(field, node.kids[i], next);
                }
            return add(MBorrowFlowNode::CopyCaptures, target, {}, next, value);
        }
        return add(MBorrowFlowNode::CopyCaptures, target, place(value), next, value);
    }
    /**
     * @brief 复制聚合中直接引用字段及枚举活动载荷的来源。
     * @param target 目标聚合路径。
     * @param value 来源节点。
     * @param next 后继。
     * @return 入口。
     */
    std::size_t aggregateBindings(const MBorrowPlace &target, int value, std::size_t next)
    {
        const auto found = capture.types.find(value);
        if (found == capture.types.end() || !found->second || found->second->kind != MType::Struct) return next;
        const auto type = found->second;
        const auto node = N(value);
        const auto layout = g_structs.find(type->name);
        const auto sourcePlace = place(value);
        if (layout != g_structs.end() && (node.kind == MNode::EnumCtor || sourcePlace.root.empty()))
            for (const auto &field : layout->second.fields)
                if (field.type->kind == MType::Ptr && (node.kind != MNode::EnumCtor || !FindEnumVariant(type->name, node.s2) || std::none_of(FindEnumVariant(type->name, node.s2)->payloads.begin(), FindEnumVariant(type->name, node.s2)->payloads.end(), [&](const MEnumPayload &payload) { return payload.storageName == field.name; })))
                {
                    auto slot = target;
                    slot.path.push_back("." + field.name);
                    next = add(MBorrowFlowNode::Bind, slot, {}, next, value);
                }
        if (node.kind == MNode::EnumCtor)
        {
            const auto variant = FindEnumVariant(type->name, node.s2);
            if (variant)
                for (std::size_t i = 0; i < variant->payloads.size() && i < node.kids.size(); ++i)
                {
                    if (variant->payloads[i].type->kind != MType::Ptr) continue;
                    auto slot = target;
                    slot.path.push_back("." + variant->payloads[i].storageName);
                    next = add(MBorrowFlowNode::Bind, slot, place(node.kids[i]), next, value);
                }
        }
        else
        {
            const auto definition = g_structs.find(type->name);
            const auto source = place(value);
            if (definition != g_structs.end() && !source.root.empty())
                for (const auto &field : definition->second.fields)
                {
                    if (field.type->kind != MType::Ptr) continue;
                    auto slot = target;
                    slot.path.push_back("." + field.name);
                    auto origin = source;
                    origin.path.push_back("." + field.name);
                    origin.path.push_back("*");
                    next = add(MBorrowFlowNode::Bind, slot, origin, next, value);
                }
        }
        return next;
    }
    /**
     * @brief 构造表达式顺序，短路分支保留条件边。
     * @param id 表达式。
     * @param next 后继。
     * @return 入口。
     */
    std::size_t expression(int id, std::size_t next)
    {
        if (id < 0) return next;
        const auto node = N(id);
        if (const auto stores = capture.stores.find(id); stores != capture.stores.end())
            for (const auto &store : stores->second)
            {
                const auto root = capture.names.find(place(store.target, true).root);
                if (root == capture.names.end()) continue;
                const auto point = add(MBorrowFlowNode::Nop, {}, {}, next, id);
                contractChecks.push_back({point, root->second, place(id), id, place(store.target, store.binding)});
                next = point;
            }
        const auto reads = capture.lambdaReads.find(id);
        if (reads != capture.lambdaReads.end())
        {
            const auto borrows = capture.lambdaBorrows.find(id);
            if (borrows != capture.lambdaBorrows.end())
                for (const auto &source : borrows->second)
                    next = add(MBorrowFlowNode::CaptureBorrow, place(id), source, next, id);
            const auto moves = capture.lambdaMoves.find(id);
            if (moves != capture.lambdaMoves.end())
                for (auto it = moves->second.rbegin(); it != moves->second.rend(); ++it)
                    next = add(MBorrowFlowNode::Move, *it, {}, next, id);
            for (auto it = reads->second.rbegin(); it != reads->second.rend(); ++it)
            {
                next = add(MBorrowFlowNode::InheritCaptures, place(id), *it, next, id);
                next = add(MBorrowFlowNode::Read, *it, {}, next, id);
            }
            return add(MBorrowFlowNode::CopyCaptures, place(id), {}, next, id);
        }
        const auto moves = capture.lambdaMoves.find(id);
        if (moves != capture.lambdaMoves.end())
        {
            for (auto it = moves->second.rbegin(); it != moves->second.rend(); ++it)
                next = add(MBorrowFlowNode::Read, *it, {}, add(MBorrowFlowNode::Move, *it, {}, next, id), id);
            return next;
        }
        if (node.kind == MNode::Block) return statement(id, next, next, next);
        if (node.kind == MNode::Assign)
        {
            const int target = node.kids[0], value = node.kids[1];
            if (reference(target) && N(value).kind == MNode::NewPtr)
            {
                const auto slot = place(target, true);
                const auto allocation = add(MBorrowFlowNode::Allocate, slot,
                                            { "$allocation:" + Memory::toString(id), {} }, next, id);
                return address(target, expression(value, allocation));
            }
            next = copyCaptures(place(target), value, next);
            const auto incoming = N(value);
            const bool movedReplacement = incoming.kind == MNode::Call && !incoming.kids.empty()
                                          && StandardCallName(N(incoming.kids[0])) == "__move";
            auto entry = add(binding(target, value) ? MBorrowFlowNode::Bind : movedReplacement ? MBorrowFlowNode::Initialize : MBorrowFlowNode::Write,
                             place(target, binding(target, value)), place(value), next, id);
            const auto storedType = capture.types.count(value) ? capture.types.at(value) : nullptr;
            if (binding(target, value) || (storedType && HasBorrowedValue(storedType->kind == MType::Ptr ? storedType->elem : storedType)))
            {
                const auto root = capture.names.find(place(target, true).root);
                if (root != capture.names.end())
                    contractChecks.push_back({entry, root->second, place(value), value, place(target, binding(target, value))});
            }
            if (!binding(target, value)) entry = aggregateBindings(place(target), value, entry);
            entry = address(target, entry);
            return binding(target, value) ? address(value, entry) : expression(value, entry);
        }
        if (node.kind == MNode::Un && node.s == "&") return address(node.kids[0], next);
        if (node.kind == MNode::EnumCtor && node.type2 == 1) next = callEffects(id, node.kids, next);
        if (node.kind == MNode::EnumCtor && node.type2 == 1 && reference(id))
        {
            const auto origins = methodOrigins(id);
            if (!origins.empty())
            {
                Memory::Vector<std::size_t> choices;
                for (const auto &origin : origins)
                    choices.push_back(add(origin.root == "$fresh" ? MBorrowFlowNode::Allocate : MBorrowFlowNode::Bind,
                                          place(id, true), origin.root == "$fresh" ? MBorrowPlace{ "$returnedAllocation:" + Memory::toString(id), {} } : origin, next, id));
                const auto branch = add(MBorrowFlowNode::Nop, {}, {}, static_cast<std::size_t>(-1), id);
                graph.nodes[branch].next = std::move(choices);
                next = branch;
                for (auto it = node.kids.rbegin(); it != node.kids.rend(); ++it) next = expression(*it, next);
                return next;
            }
        }
        if (node.kind == MNode::Call && !node.kids.empty())
        {
            const auto callee = N(node.kids[0]);
            const auto arguments = node.kids.size() > 1 ? N(node.kids[1]).kids : Memory::Vector<int>{};
            if (carriesCaptures(id) && StandardCallName(callee) != "__move" && StandardCallName(callee) != "copy")
            {
                // 不透明调用可能返回实参或接收者持有的函数值，保守保留所有外部借用。
                for (const int argument : arguments)
                    next = add(MBorrowFlowNode::InheritCaptures, place(id), place(argument), next, id);
                if ((callee.kind == MNode::Member || callee.kind == MNode::Arrow) && !callee.kids.empty())
                    next = add(MBorrowFlowNode::InheritCaptures, place(id), place(callee.kids[0]), next, id);
                next = add(MBorrowFlowNode::CopyCaptures, place(id), {}, next, id);
            }
            if (reference(id))
            {
                const auto origins = methodOrigins(id);
                Memory::Vector<std::size_t> choices;
                for (const auto &origin : origins)
                    choices.push_back(add(origin.root == "$fresh" ? MBorrowFlowNode::Allocate : MBorrowFlowNode::Bind,
                                          place(id, true), origin.root == "$fresh" ? MBorrowPlace{ "$returnedAllocation:" + Memory::toString(id), {} } : origin, next, id));
                if (!choices.empty())
                {
                    const auto branch = add(MBorrowFlowNode::Nop, {}, {}, static_cast<std::size_t>(-1), id);
                    graph.nodes[branch].next = std::move(choices);
                    next = branch;
                }
            }
            // 普通函数默认关联所有引用实参，显式契约限定来源集合。
            if (reference(id) && callee.kind != MNode::Member && callee.kind != MNode::Arrow && methodOrigins(id).empty())
            {
                Memory::Vector<std::size_t> choices;
                const Memory::Vector<int> *sources = nullptr;
                if (const auto call = capture.calls.find(id); call != capture.calls.end())
                    if (const auto signature = g_funcs.find(call->second); signature != g_funcs.end())
                    {
                        const auto &params = signature->second.params;
                        const auto count = !params.empty() && params.back().variadic ? arguments.size() : params.size();
                        sources = LifetimeRetParams(call->second, count);
                    }
                for (std::size_t index = 0; index < arguments.size(); ++index)
                {
                    if (sources && std::find(sources->begin(), sources->end(), static_cast<int>(index)) == sources->end()) continue;
                    const int argument = arguments[index];
                    if (reference(argument) && !place(argument).root.empty())
                        choices.push_back(add(MBorrowFlowNode::Bind, place(id, true), place(argument), next, id));
                }
                if (!choices.empty())
                {
                    const auto branch = add(MBorrowFlowNode::Nop, {}, {}, static_cast<std::size_t>(-1), id);
                    graph.nodes[branch].next = std::move(choices);
                    next = branch;
                }
            }
            if (StandardCallName(callee) == "__move" && arguments.size() == 1u)
                next = add(capture.managed.count(arguments[0]) && capture.managed.at(arguments[0]) ? MBorrowFlowNode::Move : MBorrowFlowNode::MoveScalar, place(arguments[0]), {}, next, id);
            else if ((callee.kind == MNode::Member || callee.kind == MNode::Arrow) && capture.terminalCalls.count(id) && !callee.kids.empty())
                next = add(capture.storageReleases.count(id) ? MBorrowFlowNode::Destroy : MBorrowFlowNode::Drop, place(callee.kids[0]), {}, next, id);
            next = callEffects(id, arguments, next,
                               (callee.kind == MNode::Member || callee.kind == MNode::Arrow) && !callee.kids.empty() ? callee.kids[0] : -1);
            // 实参可能移动或释放捕获来源，调用前再检查已求值的闭包环境。
            if (!arguments.empty() && !place(node.kids[0]).root.empty())
                next = add(MBorrowFlowNode::CheckCaptures, place(node.kids[0]), {}, next, id);
            for (auto it = arguments.rbegin(); it != arguments.rend(); ++it) next = expression(*it, next);
            if ((callee.kind == MNode::Member || callee.kind == MNode::Arrow) && !callee.kids.empty()) next = capture.terminalCalls.count(id) ? address(callee.kids[0], next) : expression(callee.kids[0], next);
            // 释放移动后的载荷存储仍合法，但不能经已移动的引用绑定释放。
            if (capture.storageReleases.count(id) && !callee.kids.empty()) next = add(MBorrowFlowNode::Read, place(callee.kids[0], true), {}, next, id);
            if (callee.kind != MNode::Member && callee.kind != MNode::Arrow) next = expression(node.kids[0], next);
            return next;
        }
        if (node.kind == MNode::Loop)
        {
            valueStops.push_back({ id, next, defers.size() });
            controlStops.push_back({ id, next, defers.size() });
            const auto entry = node.s.empty() ? statement(id, next, next, next) : statement(node.kids[0], next, next, next);
            controlStops.pop_back();
            valueStops.pop_back();
            return entry;
        }
        if (node.kind == MNode::Bin && (node.s == "&&" || node.s == "||"))
        {
            const auto right = expression(node.kids[1], next);
            const auto branch = add(MBorrowFlowNode::Nop, {}, {}, next, id);
            graph.nodes[branch].next.push_back(right);
            const auto left = N(node.kids[0]);
            if (left.kind == MNode::Bool) graph.nodes[branch].next = { (left.i != 0) == (node.s == "&&") ? right : next };
            return expression(node.kids[0], branch);
        }
        const auto target = place(id);
        if (!target.root.empty() && (node.kind == MNode::Var || node.kind == MNode::Member || node.kind == MNode::Arrow || node.kind == MNode::Index || (node.kind == MNode::Un && node.s == "*")))
        {
            auto read = add(MBorrowFlowNode::Read, target, {}, next, id);
            if (reference(id))
                read = add(MBorrowFlowNode::Read, place(id, true), {}, read, id);
            return address(id, read);
        }
        for (auto it = node.kids.rbegin(); it != node.kids.rend(); ++it) next = expression(*it, next);
        return next;
    }
    /**
     * @brief 构造结构化语句控制流及循环回边。
     * @param id 语句。
     * @param next 后继。
     * @param stop break 目标。
     * @param repeat continue 目标。
     * @return 入口。
     */
    std::size_t statement(int id, std::size_t next, std::size_t stop, std::size_t repeat)
    {
        if (id < 0) return next;
        const auto node = N(id);
        if (node.kind == MNode::ExprStmt && !node.kids.empty() && N(node.kids[0]).kind == MNode::Loop && N(node.kids[0]).s == "block")
            return statement(N(node.kids[0]).kids[0], next, stop, repeat);
        if (node.kind == MNode::ExprStmt && !node.kids.empty() && N(node.kids[0]).kind == MNode::Loop && N(node.kids[0]).s.empty())
            return statement(node.kids[0], next, stop, repeat);
        if (node.kind == MNode::Destructure && !node.i)
        {
            const MBorrowPlace temporary{ "$destructure:" + Memory::toString(id), {} };
            std::function<void(int, MBorrowPlace)> initialize = [&](int pattern, MBorrowPlace source) {
                const auto found = capture.declarations.find(pattern);
                if (found != capture.declarations.end())
                {
                    next = add(MBorrowFlowNode::CopyCaptures, found->second, source, next, pattern);
                    next = add(MBorrowFlowNode::Initialize, found->second, {}, next, pattern);
                }
                else
                {
                    const auto children = N(pattern).kids;
                    for (std::size_t i = 0; i < children.size(); ++i)
                    {
                        auto field = source;
                        field.path.push_back(".__" + Memory::toString(i));
                        initialize(children[i], std::move(field));
                    }
                }
            };
            initialize(node.kids[0], temporary);
            return expression(node.kids[1], copyCaptures(temporary, node.kids[1], next));
        }
        if (node.kind == MNode::Block || node.kind == MNode::List)
        {
            const auto children = node.kind == MNode::Block ? N(node.i).kids : node.kids;
            const auto depth = defers.size();
            for (const int child : children)
                if (N(child).kind == MNode::Defer) defers.push_back(child);
            next = cleanup(next, depth);
            for (auto it = children.rbegin(); it != children.rend(); ++it)
            {
                if (N(*it).kind == MNode::Defer) defers.pop_back();
                next = statement(*it, next, stop, repeat);
            }
            return next;
        }
        if (node.kind == MNode::Declare && (capture.declarations.count(id) || (!node.kids.empty() && N(node.kids[0]).kind == MNode::List)))
        {
            const auto declarations = capture.declarations.count(id) ? Memory::Vector<int>{ id } : N(node.kids[0]).kids;
            for (auto it = declarations.rbegin(); it != declarations.rend(); ++it)
            {
                const auto found = capture.declarations.find(*it);
                if (found == capture.declarations.end()) continue;
                const int value = N(*it).kids.empty() ? -1 : N(*it).kids[0];
                const auto ref = capture.declarationReferences.find(*it);
                if (ref != capture.declarationReferences.end() && ref->second && value >= 0)
                {
                    auto source = place(value);
                    const bool allocation = N(value).kind == MNode::NewPtr;
                    if (allocation) source = { found->second.root + "$heap", {} };
                    next = add(allocation ? MBorrowFlowNode::Allocate : MBorrowFlowNode::Bind, found->second, source, next, *it);
                    contractChecks.push_back({ next, N(*it).s, source, value });
                    next = allocation ? expression(value, next) : address(value, next);
                }
                else
                {
                    const auto copied = copyCaptures(found->second, value, next);
                    const auto write = add(MBorrowFlowNode::Initialize, found->second, {}, copied, *it);
                    const auto type = capture.types.find(value);
                    if (type != capture.types.end() && HasBorrowedValue(type->second->kind == MType::Ptr ? type->second->elem : type->second))
                        contractChecks.push_back({ write, N(*it).s, place(value), value });
                    next = expression(value, aggregateBindings(found->second, value, write));
                }
                // 每次执行声明均创建新的绑定，包括循环内重复到达的声明。
                const auto refBinding = capture.declarationReferences.find(*it);
                if (refBinding != capture.declarationReferences.end() && refBinding->second)
                    next = add(MBorrowFlowNode::Initialize, found->second, {}, next, *it);
                const auto previous = capture.redeclarations.find(*it);
                if (previous != capture.redeclarations.end())
                    next = add(MBorrowFlowNode::Redeclare, previous->second, {}, next, *it);
            }
            return next;
        }
        if (node.kind == MNode::If)
        {
            auto alternate = statement(node.kids[3], next, stop, repeat);
            const auto elifs = N(node.kids[2]).kids;
            for (auto it = elifs.rbegin(); it != elifs.rend(); ++it)
            {
                const auto branch = N(*it);
                const auto test = add(MBorrowFlowNode::Nop, {}, {}, alternate, *it);
                const auto armEntry = statement(branch.kids[1], next, stop, repeat);
                graph.nodes[test].next.push_back(armEntry);
                const auto condition = N(branch.kids[0]);
                if (condition.kind == MNode::Bool) graph.nodes[test].next = { condition.i ? armEntry : alternate };
                alternate = expression(branch.kids[0], test);
            }
            const auto test = add(MBorrowFlowNode::Nop, {}, {}, alternate, id);
            const auto thenEntry = statement(node.kids[1], next, stop, repeat);
            graph.nodes[test].next.push_back(thenEntry);
            const auto condition = N(node.kids[0]);
            if (condition.kind == MNode::Bool) graph.nodes[test].next = { graph.nodes[test].next[condition.i ? 1u : 0u] };
            return expression(node.kids[0], test);
        }
        if (node.kind == MNode::While || node.kind == MNode::For || node.kind == MNode::DoWhile || node.kind == MNode::Foreach || node.kind == MNode::Loop)
        {
            const int fallback = node.kind == MNode::For && node.kids.size() > 4u ? node.kids[4] :
                                 node.kind == MNode::Foreach && node.kids.size() > 2u ? node.kids[2] : -1;
            const auto exit = fallback >= 0 ? statement(fallback, next, stop, repeat) : next;
            const auto loopBegin = graph.nodes.size();
            const auto test = add(MBorrowFlowNode::Nop, {}, {}, exit, id);
            const int condition = node.kind == MNode::While ? node.kids[0] : node.kind == MNode::For ? node.kids[1] :
                                                                         node.kind == MNode::DoWhile ? node.kids[1] :
                                                                                                       -1;
            const auto testEntry = expression(condition, test);
            const auto snapshot = capture.foreachSnapshots.find(id);
            const auto saveSnapshot = [&](std::size_t successor) {
                if (snapshot == capture.foreachSnapshots.end()) return successor;
                const auto copied = copyCaptures(snapshot->second.first, snapshot->second.second, successor);
                const auto initialized = add(MBorrowFlowNode::Initialize, snapshot->second.first, {}, copied, id);
                return expression(snapshot->second.second, initialized);
            };
            const auto update = node.kind == MNode::For ? statement(node.kids[2], testEntry, stop, repeat) : saveSnapshot(testEntry);
            const auto broken = saveSnapshot(exit);
            const int body = node.kind == MNode::For ? node.kids[3] : node.kind == MNode::Loop ? node.kids[0] :
                                                                  node.kind == MNode::DoWhile  ? node.kids[0] :
                                                                                                 node.kids[1];
            const bool ownValue = capture.valueLoops.count(id) != 0u;
            if (ownValue) valueStops.push_back({ id, next, defers.size() });
            controlStops.push_back({ id, broken, defers.size() });
            controlRepeats.push_back({ id, update, defers.size() });
            auto bodyEntry = statement(body, update, broken, update);
            controlRepeats.pop_back();
            controlStops.pop_back();
            if (ownValue) valueStops.pop_back();
            const auto iteration = capture.foreachBindings.find(id);
            if (iteration != capture.foreachBindings.end())
                bodyEntry = add(MBorrowFlowNode::Initialize, iteration->second, {}, bodyEntry, id);
            graph.nodes[test].next.push_back(bodyEntry);
            if (node.kind == MNode::Loop || (node.kind == MNode::For && condition < 0))
                graph.nodes[test].next = { bodyEntry };
            else if (condition >= 0 && N(condition).kind == MNode::Bool)
                graph.nodes[test].next = { N(condition).i ? bodyEntry : exit };
            const auto firstEntry = peelLoop(loopBegin, update, node.kind == MNode::DoWhile ? bodyEntry : testEntry);
            if (node.kind == MNode::For) return statement(node.kids[0], firstEntry, stop, repeat);
            if (node.kind == MNode::Foreach)
            {
                auto entry = firstEntry;
                if (fallback >= 0 && iteration != capture.foreachBindings.end())
                    entry = add(capture.foreachNoDefault.count(id) ? MBorrowFlowNode::Destroy : MBorrowFlowNode::Initialize,
                                iteration->second, {}, entry, id);
                if (snapshot != capture.foreachSnapshots.end())
                    entry = add(capture.foreachNoDefault.count(id) ? MBorrowFlowNode::Destroy : MBorrowFlowNode::Initialize,
                                snapshot->second.first, {}, entry, id);
                return expression(node.kids[0], entry);
            }
            return firstEntry;
        }
        if (node.kind == MNode::Switch)
        {
            const auto branch = add(MBorrowFlowNode::Nop, {}, {}, next, id);
            for (const int item : N(node.kids[1]).kids)
            {
                const auto arm = N(item);
                if (!arm.kids.empty())
                {
                    const auto armEntry = statement(arm.kids[0], next, stop, repeat);
                    const auto guardEntry = expression(arm.guard, armEntry);
                    graph.nodes[branch].next.push_back(guardEntry);
                }
            }
            return expression(node.kids[0], branch);
        }
        if (node.kind == MNode::Break)
        {
            if (node.controlTarget >= 0)
            {
                for (auto target = controlStops.rbegin(); target != controlStops.rend(); ++target)
                    if (target->first == node.controlTarget)
                        return exitValue(node.kids.empty() ? -1 : node.kids[0], *target);
                return Fail("借用控制流缺少已解析的退出目标"), next;
            }
            if (!node.kids.empty() && !valueStops.empty())
                return exitValue(node.kids[0], valueStops.back());
            const auto depth = controlStops.empty() ? 0u : controlStops.back().deferDepth;
            return node.kids.empty() ? cleanup(stop, depth) : expression(node.kids[0], cleanup(stop, depth));
        }
        if (node.kind == MNode::Continue)
        {
            if (node.controlTarget >= 0)
            {
                for (auto target = controlRepeats.rbegin(); target != controlRepeats.rend(); ++target)
                    if (target->first == node.controlTarget) return cleanup(target->second, target->deferDepth);
                return Fail("借用控制流缺少已解析的继续目标"), next;
            }
            return cleanup(repeat, controlRepeats.empty() ? 0u : controlRepeats.back().deferDepth);
        }
        if (node.kind == MNode::Return)
        {
            const auto exit = cleanup(functionExit, 0);
            if (node.kids.empty()) return exit;
            const auto check = add(MBorrowFlowNode::Nop, {}, {}, exit, id);
            contractChecks.push_back({ check, "#ret", place(node.kids[0]), node.kids[0] });
            return expression(node.kids[0], check);
        }
        if (node.kind == MNode::Defer)
            return expression(node.kids[0], add(MBorrowFlowNode::CopyCaptures, { "$defer:" + Memory::toString(id), {} }, place(node.kids[0]), next, id));

        return expression(id, next);
    }
};
/**
 * @brief 用控制流合流后的真实来源核对返回与保存契约。
 * @param builder 已绑定表达式及契约检查点。
 * @param states 各节点入口状态。
 * @param reachable 可达标记。
 */
void CheckLifetimeFlow(const MBorrowFlowBuilder &builder, const Memory::Vector<chali_borrow_checker::FlowState> &states,
                       const Memory::Vector<bool> &reachable);
/**
 * @brief 完成函数借用控制流事实，并核对已绑定来源的显式契约。
 * @param body 函数体。
 * @param capture 已绑定事实。
 * @param label 具体函数的调用标签。
 * @param parameters 按调用实参顺序排列的引用形参存储，非引用形参为空。
 * @return 无。
 */
void FinishBorrowFlow(int body, const MBorrowFlowCapture &capture, const Memory::String &label, const Memory::Vector<Memory::String> &parameters)
{
    MBorrowFlowBuilder builder{ capture, {} };
    Memory::Set<int> sourceNodes;
    Memory::Vector<int> pending{body};
    while (!pending.empty())
    {
        const int id = pending.back();
        pending.pop_back();
        if (id < 0 || !sourceNodes.insert(id).second) continue;
        if (N(id).kind == MNode::Block) pending.push_back(static_cast<int>(N(id).i));
        for (const int child : N(id).kids) pending.push_back(child);
    }
    builder.loopPeelBudget = sourceNodes.size() * 8u;
    builder.graph.nodes.push_back({});
    const auto end = builder.add(MBorrowFlowNode::Nop, {}, {}, static_cast<std::size_t>(-1), body);
    builder.functionExit = end;
    auto entry = builder.statement(body, end, end, end);
    for (const auto &parameter : capture.parameterTypes)
    {
        const auto root = capture.parameters.at(parameter.first);
        std::function<void(std::shared_ptr<MType>, MBorrowPlace, Memory::Vector<Memory::String>, unsigned)> fields;
        fields = [&](std::shared_ptr<MType> type, MBorrowPlace slot, Memory::Vector<Memory::String> path, unsigned depth) {
            if (!type || depth > 16u) return;
            if (type->kind == MType::Ptr)
            {
                auto source = path;
                source.insert(source.begin(), "$borrowed");
                entry = builder.add(MBorrowFlowNode::Bind, slot, { root, std::move(source) }, entry, -1);
                return;
            }
            if (type->kind != MType::Struct) return;
            const auto definition = g_structs.find(type->name);
            if (definition == g_structs.end()) return;
            for (const auto &field : definition->second.fields)
            {
                auto child = slot;
                child.path.push_back("." + field.name);
                auto member = path;
                member.push_back("." + field.name);
                fields(field.type, std::move(child), std::move(member), depth + 1u);
            }
        };
        auto type = parameter.second;
        MBorrowPlace slot{ root, {} };
        if (type && type->kind == MType::Ptr)
        {
            type = type->elem;
            slot.path.push_back("$external");
        }
        fields(type, std::move(slot), {}, 0u);
    }
    builder.graph.nodes[0].next.push_back(entry);
    Memory::Vector<chali_borrow_checker::FlowState> states;
    Memory::Vector<bool> reachable;
    chali_borrow_checker::checkFlow(builder.graph, &states, &reachable);
    CheckLifetimeFlow(builder, states, reachable);
    for (const auto &merge : chali_resource_cleanup::coalesce(builder.graph, states, reachable))
    {
        const auto found = capture.releases.find(merge.operation);
        if (found == capture.releases.end() || !found->second.first) continue;
        const auto operation = found->second.first;
        if (merge.redundant)
            *operation = *ast::Node::block({});
        else if (found->second.second)
        {
            const auto original = std::make_shared<ast::Node>(*operation);
            *operation = *ast::Node::ifThenElse(found->second.second, original, nullptr);
        }
    }
    g_borrowFunctionFlows.push_back({ label, parameters, std::move(builder.calls), end });
    g_borrowFlows.push_back(std::move(builder.graph));
}

#include "borrow_call_effects.cpp"
