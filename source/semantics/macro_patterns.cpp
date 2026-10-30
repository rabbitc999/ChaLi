/**
 * @brief 将宏模式候选还原为普通匹配分支，复用语言的载荷与穷尽检查。
 * @param root 已替换实参的宏模板根。
 * @return 模式位置、嵌套层次与展开预算合法时返回真。
 */
bool NormalizeMacroPatterns(int root)
{
    std::size_t budget = 1000000;
    Memory::Set<int> seen;
    std::function<bool(int, std::size_t)> visit;
    visit = [&](int id, std::size_t depth) {
        if (id < 0 || !seen.insert(id).second) return true;
        if (depth >= 256 || !budget--) return Fail("宏模式展开超过深度或规模预算");
        const auto node = N(id);
        if (node.kind == MNode::MacroCall || node.kind == MNode::MacroArgument || node.kind == MNode::MacroDefinition) return true;
        if (node.kind == MNode::MacroPatternFragment) return Fail("pattern 片段只能用于 match 模式位置");
        if (node.kind == MNode::Case && node.i >= 0)
        {
            Memory::Map<Memory::String, std::uint32_t> bindings;
            bool imported = node.s2 == "__chali_macro_pattern";
            std::function<bool(int, std::size_t)> contains;
            contains = [&](int current, std::size_t level) {
                if (current < 0 || level >= 256) return false;
                const auto value = N(current);
                if (value.kind == MNode::MacroPatternFragment) return true;
                for (const int child : value.kids)
                    if (contains(child, level + 1)) return true;
                return false;
            };
            imported = imported || contains(static_cast<int>(node.i), depth + 1);
            std::function<bool(int, bool, std::size_t)> pattern;
            pattern = [&](int current, bool payload, std::size_t level) {
                if (current < 0) return true;
                if (level >= 256 || !budget--) return Fail("宏模式结构超过深度或规模预算");
                auto value = N(current);
                if (value.kind == MNode::MacroPatternFragment)
                {
                    const auto alternatives = N(value.kids[0]).kids;
                    if (alternatives.size() != 1) return Fail("嵌套载荷的 pattern 参数不能包含多个备选模式");
                    g_nodes[current] = N(alternatives[0]);
                    return pattern(current, payload, level + 1);
                }
                if (imported && payload && value.kind == MNode::Var && !value.syntaxContext && value.s != "_")
                    return BindMacroCallerPattern(current, bindings, level + 1);
                if (value.kind == MNode::Call && value.kids.size() == 2)
                    return pattern(value.kids[1], true, level + 1);
                for (const int child : value.kids)
                    if (!pattern(child, payload || value.kind == MNode::EnumCtor || value.kind == MNode::TypeQualifiedRef, level + 1)) return false;
                return true;
            };
            if (!pattern(static_cast<int>(node.i), false, depth + 1)) return false;
            std::function<bool(int, std::size_t)> rebind;
            rebind = [&](int current, std::size_t level) {
                if (current < 0) return true;
                if (level >= 256 || !budget--) return Fail("宏模式绑定超过深度或规模预算");
                auto value = N(current);
                if (value.kind == MNode::Var && !value.syntaxContext && bindings.count(value.s))
                {
                    g_nodes[current].bindingIdentity = 0;
                    g_nodes[current].bindingContext = 0;
                }
                for (const int child : value.kids)
                    if (!rebind(child, level + 1)) return false;
                if (value.kind == MNode::Block && !rebind(static_cast<int>(value.i), level + 1)) return false;
                return rebind(value.guard, level + 1);
            };
            if (!rebind(node.guard, depth + 1)) return false;
            for (const int child : node.kids)
                if (!rebind(child, depth + 1)) return false;
            const auto value = N(static_cast<int>(node.i));
            if (value.kind == MNode::Var && value.s == "_") g_nodes[id].i = -1;
        }
        if (node.kind == MNode::Switch && node.s == "match" && node.kids.size() >= 2)
        {
            const int cases = node.kids[1];
            Memory::Vector<int> expanded;
            for (const int branch : N(cases).kids)
            {
                const auto item = N(branch);
                if (item.i >= 0 && N(static_cast<int>(item.i)).kind == MNode::MacroPatternFragment)
                {
                    const auto alternatives = N(N(static_cast<int>(item.i)).kids[0]).kids;
                    for (const int alternative : alternatives)
                    {
                        if (!budget--) return Fail("宏模式分支超过规模预算");
                        const int copy = M_New(MNode::Case);
                        g_nodes[copy] = item;
                        g_nodes[copy].i = alternative;
                        g_nodes[copy].type2 = item.type2 > 0 ? item.type2 : branch + 1;
                        g_nodes[copy].s2 = "__chali_macro_pattern";
                        expanded.push_back(copy);
                    }
                }
                else
                    expanded.push_back(branch);
            }
            g_nodes[cases].kids = std::move(expanded);
        }
        for (const int child : N(id).kids)
            if (!visit(child, depth + 1)) return false;
        if (!visit(node.guard, depth + 1)) return false;
        const bool group = node.kind == MNode::Declare && node.kids.size() == 1 && node.kids[0] >= 0 && N(node.kids[0]).kind == MNode::List;
        const bool indexed = node.kind == MNode::Block || node.kind == MNode::Case || node.kind == MNode::NewPtr || ((node.kind == MNode::Declare || node.kind == MNode::Field) && !group && node.i >= 0);
        return !indexed || visit(static_cast<int>(N(id).i), depth + 1);
    };
    return visit(root, 0);
}
