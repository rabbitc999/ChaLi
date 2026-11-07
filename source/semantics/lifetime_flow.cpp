/**
 * @file lifetime_flow.cpp
 * @brief 用已绑定存储的控制流状态核对显式借用来源，避免按源码名字猜测别名。
 */

/**
 * @brief 从检查点状态提取别名、聚合及调用结果可能携带的引用来源。
 * @param builder 已绑定源码事实。
 * @param state 当前控制流状态。
 * @param id 表达式节点。
 * @param depth 表达式递归预算。
 * @return 具体存储来源并集；未知时为空，由调用者保守拒绝。
 */
Memory::Set<MBorrowPlace> LifetimeFlowOrigins(const MBorrowFlowBuilder &builder, const chali_borrow_checker::FlowState &state,
                                              int id, unsigned depth = 0)
{
    if (id < 0 || depth > 64u) return {};
    const auto node = N(id);
    const auto place = builder.place(id);
    auto origins = chali_borrow_checker::resolve(state, place);
    if (!place.root.empty())
    {
        if (!builder.reference(id))
        {
            Memory::Set<MBorrowPlace> nested;
            for (const auto &root : origins)
                for (const auto &entry : state.origins)
                    if (root.root == entry.first.root && root.path.size() <= entry.first.path.size()
                        && std::equal(root.path.begin(), root.path.end(), entry.first.path.begin()))
                        nested.insert(entry.second.begin(), entry.second.end());
            if (!nested.empty()) return nested;
        }
        return origins;
    }
    Memory::Vector<int> children;
    const Memory::Vector<int> *selected = nullptr;
    if (node.kind == MNode::Call && node.kids.size() > 1u)
    {
        children = N(node.kids[1]).kids;
        selected = LifetimeCallRetParams(id);
        const auto callee = N(node.kids[0]);
        if (!selected && (callee.kind == MNode::Member || callee.kind == MNode::Arrow) && !callee.kids.empty())
            children.push_back(callee.kids[0]);
    }
    else if (node.kind == MNode::Tuple || node.kind == MNode::EnumCtor || node.kind == MNode::Cast || node.kind == MNode::Un)
        children = node.kids;
    for (std::size_t i = 0; i < children.size(); ++i)
    {
        if (selected && std::find(selected->begin(), selected->end(), static_cast<int>(i)) == selected->end()) continue;
        const auto type = builder.capture.types.find(children[i]);
        if (type != builder.capture.types.end() && !HasBorrowedValue(type->second) && N(children[i]).kind != MNode::Un) continue;
        const auto borrowed = LifetimeFlowOrigins(builder, state, children[i], depth + 1u);
        origins.insert(borrowed.begin(), borrowed.end());
    }
    return origins;
}

/**
 * @brief 核对具体退出点及引用初始化、重新绑定时的契约来源。
 * @param builder 函数的已绑定控制流及检查点。
 * @param states 检查点入口的来源状态。
 * @param reachable 可达标记。
 */
void CheckLifetimeFlow(const MBorrowFlowBuilder &builder, const Memory::Vector<chali_borrow_checker::FlowState> &states,
                       const Memory::Vector<bool> &reachable)
{
    if (!g_lifetimeCurrent) return;
    const auto &contract = *g_lifetimeCurrent;
    auto checks = builder.contractChecks;
    const auto originalCount = checks.size();
    for (std::size_t i = 0; i < originalCount; ++i)
    {
        const auto check = checks[i];
        if (check.destination.root.empty() || check.node >= states.size() || !reachable[check.node]) continue;
        Memory::Set<Memory::String> targets;
        for (const auto &place : chali_borrow_checker::resolve(states[check.node], check.destination))
        {
            auto root = place.root;
            const auto heap = root.find("$heap");
            if (heap != Memory::String::npos) root.resize(heap);
            const auto name = builder.capture.names.find(root);
            if (name != builder.capture.names.end()) targets.insert(name->second);
        }
        for (const auto &name : targets)
            if (name != check.target && contract.borrows.count(name))
            {
                auto alias = check;
                alias.target = name;
                alias.destination = {};
                checks.push_back(std::move(alias));
            }
    }
    for (const auto &check : checks)
    {
        if (!reachable[check.node]) continue;
        const Memory::Vector<Memory::String> *declared = nullptr;
        if (check.target == "#ret")
        {
            if (!contract.hasRet || contract.retInvalid) continue;
            const auto type = builder.capture.types.find(check.expression);
            if (type != builder.capture.types.end() && !HasBorrowedValue(type->second)) continue;
            declared = &contract.retSources;
        }
        else
        {
            const auto found = contract.borrows.find(check.target);
            if (found == contract.borrows.end()) continue;
            declared = &found->second;
        }
        const auto &state = states[check.node];
        Memory::Set<Memory::String> allowed;
        for (const auto &source : *declared)
        {
            const auto parameter = builder.capture.parameters.find(source);
            if (parameter != builder.capture.parameters.end())
            {
                allowed.insert(parameter->second);
                continue;
            }
            for (const auto &name : builder.capture.names)
                if (name.second == source)
                {
                    const auto resolved = chali_borrow_checker::resolve(state, { name.first, { "*" } });
                    if (resolved.empty())
                        allowed.insert(name.first);
                    else
                        for (const auto &place : resolved) allowed.insert(place.root);
                }
        }
        auto origins = LifetimeFlowOrigins(builder, state, check.expression);
        Memory::Set<Memory::String> undeclared;
        if (origins.empty())
        {
            const auto depth = builder.capture.depths.find(check.expression);
            if (depth == builder.capture.depths.end() || depth->second != 0u) undeclared.insert("无法确定的引用来源");
        }
        for (const auto &origin : origins)
        {
            if (allowed.count(origin.root) || builder.capture.globals.count(origin.root)
                || origin.root.rfind("$allocation:", 0u) == 0u || origin.root.rfind("$returnedAllocation:", 0u) == 0u
                || origin.root.find("$heap") != Memory::String::npos) continue;
            bool staticParameter = false;
            for (const auto &name : contract.statics)
            {
                const auto parameter = builder.capture.parameters.find(name);
                staticParameter = staticParameter || (parameter != builder.capture.parameters.end() && parameter->second == origin.root);
            }
            if (staticParameter) continue;
            const auto name = builder.capture.names.find(origin.root);
            undeclared.insert(name == builder.capture.names.end() ? origin.root : name->second);
        }
        if (undeclared.empty()) continue;
        Memory::String listed;
        for (const auto &name : undeclared)
        {
            if (!listed.empty()) listed += "、";
            listed += name;
        }
        const auto code = check.target == "#ret" ? "[CHL1211] 返回值" : "[CHL1212] " + check.target;
        LifetimeReport(code + Memory::String("借用了契约未声明的来源 ") + listed + "；当前契约为 "
                           + LifetimeContractText(check.target, *declared),
                       builder.graph.nodes[check.node].line);
    }
}

/**
 * @brief 防止显式输出借用契约把较短实参写入寿命更长的借用容器。
 * @param callId 已解析的调用节点。
 * @param symbol 供诊断展示的函数名。
 * @param arguments 实参节点。
 * @param line 调用行。
 */
void CheckLifetimeBorrowArguments(int callId, const Memory::String &symbol, const Memory::Vector<int> &arguments, std::size_t line)
{
    const auto label = LifetimeCallSymbol(callId);
    const auto signature = g_funcs.find(label);
    if (signature == g_funcs.end()) return;
    const auto &params = signature->second.params;
    const auto count = !params.empty() && params.back().variadic ? arguments.size() : params.size();
    const auto contract = FindLifetimeContract(label, count);
    if (!contract) return;
    for (const auto &borrow : contract->borrows)
        for (std::size_t target = 0; target < params.size() && target < arguments.size(); ++target)
        {
            if (params[target].source != borrow.first || params[target].type->kind != MType::Ptr) continue;
            Memory::Set<Memory::String> visited;
            if (!ContainsBorrowedReferences(params[target].type->elem, &visited)) continue;
            for (const auto &source : borrow.second)
                for (std::size_t input = 0; input < params.size() && input < arguments.size(); ++input)
                    if (params[input].source == source)
                    {
                        CaptureBorrowStore(arguments[target], arguments[input], false);
                        if (BorrowSourceDepth(arguments[input]) > BorrowSourceDepth(arguments[target]))
                            LifetimeReport("[CHL1204] " + symbol + " 的 " + borrow.first + " 不能保存寿命更短的实参 " + source, line);
                    }
        }
}
