/**
 * @file borrow_call_effects.cpp
 * @brief 将具体函数的引用移动退出状态传播到调用者，递归调用迭代到不动点。
 */

/**
 * @brief 推导引用形参可能失效的投影路径，并补入调用点控制流。
 * @return 无；只扩充已完成单态化的借用控制流，不修改源码或运行时协议。
 */
void ResolveBorrowCallEffects()
{
    using Effects = Memory::Set<std::pair<std::size_t, MBorrowPlace>>;
    Memory::Map<Memory::String, Effects> summaries;
    bool changed;
    do
    {
        changed = false;
        for (std::size_t i = 0; i < g_borrowFlows.size(); ++i)
        {
            const auto &function = g_borrowFunctionFlows[i];
            Memory::Vector<chali_borrow_checker::FlowState> states;
            Memory::Vector<bool> reachable;
            chali_borrow_checker::checkFlow(g_borrowFlows[i], &states, &reachable);
            if (!reachable[function.exit]) continue;
            for (const auto &invalid : states[function.exit].invalid)
            {
                if (invalid.second != chali_borrow_checker::FlowState::Moved) continue;
                for (std::size_t parameter = 0; parameter < function.parameters.size(); ++parameter)
                {
                    const auto &root = function.parameters[parameter];
                    if (root.empty() || invalid.first.root != root) continue;
                    auto path = invalid.first.path;
                    if (!path.empty() && path.front() == "$external")
                        path.erase(path.begin());
                    else if (!path.empty() && path.front() == "$borrowed")
                    {
                        path.erase(path.begin());
                        path.push_back("*");
                    }
                    else if (root.rfind("self:", 0u) != 0u)
                        continue;
                    changed = summaries[function.label].insert({ parameter, { {}, std::move(path) } }).second || changed;
                }
            }
        }
        for (std::size_t i = 0; i < g_borrowFlows.size(); ++i)
        {
            auto &graph = g_borrowFlows[i];
            for (auto &site : g_borrowFunctionFlows[i].calls)
            {
                const auto summary = summaries.find(site.callee);
                if (summary == summaries.end()) continue;
                for (const auto &effect : summary->second)
                {
                    if (effect.first >= site.arguments.size()) continue;
                    auto target = site.arguments[effect.first];
                    if (target.root.empty()) continue;
                    target.path.insert(target.path.end(), effect.second.path.begin(), effect.second.path.end());
                    if (target.path.size() > 16u)
                    {
                        target.path.resize(16u);
                        target.path.back() = "$any";
                    }
                    if (!site.applied.insert(target).second) continue;
                    const auto entry = graph.nodes[site.entry];
                    const auto index = graph.nodes.size();
                    graph.nodes.push_back({ MBorrowFlowNode::Move, std::move(target), {}, entry.next, entry.line, site.callee, entry.operation });
                    graph.nodes[site.entry].next = { index };
                    changed = true;
                }
            }
        }
    } while (changed);
}
