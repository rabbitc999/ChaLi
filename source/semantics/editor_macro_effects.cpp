/**
 * @file editor_macro_effects.cpp
 * @brief 对真实展开树汇总绑定、借用与控制出口，不推断传递纯度。
 */
/**
 * @brief 汇总当前展开的直接效果，未解析控制出口明确留作未知。
 * @param root 生产展开树的根节点。
 * @param actualExpression 语义检查后的实际表达式；负值表示使用首次展开树。
 * @return 有界 JSON 对象，不包含内部节点编号或生成名。
 */
std::string EditorMacroEffects(int root, int actualExpression = -1)
{
    Memory::Set<int> members;
    Memory::Vector<int> pending{ root };
    bool truncated = false;
    while (!pending.empty())
    {
        const int id = pending.back();
        pending.pop_back();
        if (id < 0 || static_cast<std::size_t>(id) >= g_nodes.size() || members.count(id)) continue;
        if (members.size() >= 10000)
        {
            truncated = true;
            break;
        }
        members.insert(id);
        const auto node = N(id);
        if (node.kind == MNode::Lambda || node.kind == MNode::GenericLambda || node.kind == MNode::FuncDef || node.kind == MNode::MethodDef) continue;
        if (id == root && actualExpression >= 0)
            pending.push_back(actualExpression);
        else if (node.kind != MNode::MacroArgument)
            pending.insert(pending.end(), node.kids.begin(), node.kids.end());
        if (node.kind == MNode::Block) pending.push_back(static_cast<int>(node.i));
        if (node.guard >= 0) pending.push_back(node.guard);
    }
    std::size_t bindings = 0, shared = 0, mutableBorrows = 0;
    Memory::Set<Memory::String> caller, local, unresolved;
    for (const int id : members)
    {
        const auto &node = N(id);
        const bool declaration = node.kind == MNode::Declare || node.kind == MNode::Field || node.kind == MNode::MethodDef || node.kind == MNode::FuncDef || node.kind == MNode::StructDecl || node.kind == MNode::EnumDecl || node.kind == MNode::InterfaceDecl || node.kind == MNode::AssociatedDef || node.kind == MNode::AssociatedDecl;
        if (declaration && node.syntaxContext) ++bindings;
        if (node.s2 == "__chali_macro_const_ref") ++shared;
        if (node.s2 == "__chali_macro_mut_ref") ++mutableBorrows;
        Memory::String effect;
        if (node.kind == MNode::Return)
            effect = "return";
        else if (node.kind == MNode::Continue)
            effect = "continue";
        else if (node.kind == MNode::Break)
            effect = node.kids.empty() ? "break" : "yield";
        else if (node.kind == MNode::Try)
            effect = "propagation";
        if (effect.empty()) continue;
        if (node.controlTarget < 0)
            unresolved.insert(effect);
        else if (members.count(node.controlTarget))
            local.insert(effect);
        else
            caller.insert(effect);
    }
    const auto array = [](const Memory::Set<Memory::String> &items) {
        std::string result = "[";
        bool first = true;
        for (const auto &item : items)
        {
            if (!first) result += ',';
            first = false;
            result += chali_editor::jsonString(std::string_view(item.data(), item.size()));
        }
        return result + ']';
    };
    const auto &category = N(root).s2;
    const auto label = category == "expr" || category == "stmt" || category == "member" || category == "item" ? category : Memory::String("legacy");
    return "{\"resultCategory\":" + chali_editor::jsonString(std::string_view(label.data(), label.size()))
           + ",\"introducedBindings\":" + std::to_string(bindings) + ",\"sharedBorrows\":" + std::to_string(shared)
           + ",\"mutableBorrows\":" + std::to_string(mutableBorrows) + ",\"callerControl\":" + array(caller)
           + ",\"localControl\":" + array(local) + ",\"unresolvedControl\":" + array(unresolved)
           + ",\"truncated\":" + (truncated ? "true" : "false") + '}';
}
