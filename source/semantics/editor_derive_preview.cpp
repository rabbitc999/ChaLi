/**
 * @file editor_derive_preview.cpp
 * @brief 从已经注入的派生方法读取展开预览，不重新执行代码生成。
 */

/**
 * @brief 为内建派生保留请求位置及已有的外层宏来源，不伪造宏定义。
 * @param owner 被派生的声明编号。
 * @param traits 内建派生名称，空集合不建立来源帧。
 * @return 实际来源帧编号；没有内建派生时返回负一。
 */
int BeginDerivedExpansion(int owner, const Memory::Vector<Memory::String> &traits)
{
    if (traits.empty()) return -1;
    const auto declaration = N(owner);
    Memory::String name = "#derive(";
    for (std::size_t i = 0; i < traits.size(); ++i)
    {
        if (i) name += ", ";
        name += traits[i];
    }
    name += ')';
    int definition = -1;
    if (traits.size() == 1 && (traits[0] == "Debug" || traits[0] == "Json"))
        for (const auto id : g_top)
            if (N(id).kind == MNode::InterfaceDecl && BareTypeName(N(id).s) == traits[0]
                && N(id).module == (traits[0] == "Json" ? "std::json" : "std::lang")) definition = id;
    const int frame = M_New(MNode::MacroExpansionFrame, name, definition);
    g_nodes[frame].sourceLine = declaration.deriveLine ? declaration.deriveLine : declaration.sourceLine;
    g_nodes[frame].sourceColumn = declaration.deriveLine ? declaration.deriveColumn : declaration.sourceColumn;
    g_nodes[frame].kids = { declaration.macroExpansion };
    g_nodes[owner].macroExpansion = frame;
    return frame;
}

/**
 * @brief 为生产方法中的字段访问和枚举模式补充真实声明位置。
 * @param owner 被派生的声明编号。
 * @param members 已成功注入的方法编号。
 */
void RecordDerivedMemberSources(int owner, const Memory::Vector<int> &members)
{
    if (!chali_editor::compilerAnalysis.enabled()) return;
    const auto declaration = N(owner);
    Memory::Map<Memory::String, int> fields, variants;
    for (const int id : N(declaration.kids[0]).kids)
        if (N(id).kind == MNode::Field && N(id).type2 != 1)
            fields[N(id).s] = id;
        else if (N(id).kind == MNode::EnumVariant)
            variants[N(id).s] = id;
    std::size_t budget = 100000;
    using Bindings = Memory::Map<Memory::String, int>;
    const auto mark = [](int id, int source) {
        g_nodes[id].metadataLine = N(source).sourceLine;
        g_nodes[id].metadataColumn = N(source).sourceColumn;
    };
    std::function<void(int, unsigned, Bindings)> visit = [&](int id, unsigned depth, Bindings bindings) {
        if (id < 0 || static_cast<std::size_t>(id) >= g_nodes.size() || depth >= 128 || !budget) return;
        --budget;
        const auto node = N(id);
        if ((node.kind == MNode::Member || node.kind == MNode::Arrow) && !node.kids.empty()
            && N(node.kids[0]).kind == MNode::Var)
        {
            const auto base = N(node.kids[0]).s;
            const auto field = fields.find(node.s);
            if (field != fields.end() && (base == "self" || base == "value" || base == "other" || base == "__chali_clone")) mark(id, field->second);
        }
        if (node.kind == MNode::Var)
        {
            const auto binding = bindings.find(node.s);
            if (binding != bindings.end()) mark(id, binding->second);
        }
        if (node.kind == MNode::EnumCtor && node.s == declaration.s)
        {
            const auto variant = variants.find(node.s2);
            if (variant != variants.end()) mark(id, variant->second);
        }
        if (node.kind == MNode::Case && node.i >= 0)
        {
            int pattern = static_cast<int>(node.i);
            if (N(pattern).kind == MNode::Call && N(pattern).kids.size() == 2)
            {
                const auto callee = N(N(pattern).kids[0]);
                const auto variant = variants.find(callee.s2);
                if (callee.kind == MNode::EnumCtor && callee.s == declaration.s && variant != variants.end()
                    && !N(variant->second).kids.empty())
                {
                    const auto payloads = N(N(variant->second).kids[0]).kids;
                    const auto arguments = N(N(pattern).kids[1]).kids;
                    for (std::size_t i = 0; i < payloads.size() && i < arguments.size(); ++i)
                        if (N(arguments[i]).kind == MNode::Var) bindings[N(arguments[i]).s] = payloads[i];
                }
            }
            visit(pattern, depth + 1, bindings);
        }
        for (const int child : node.kids) visit(child, depth + 1, bindings);
        if (node.kind == MNode::Block) visit(static_cast<int>(node.i), depth + 1, bindings);
        if (node.guard >= 0) visit(node.guard, depth + 1, bindings);
    };
    for (const int member : members) visit(member, 0, {});
}

/**
 * @brief 序列化同一生产语法树中的内建派生成员。
 * @param owner 类型声明编号。
 * @param members 实际注入的方法编号。
 * @param name 经过消歧的派生显示名称。
 * @return 有界的只读展开树，失效的语义重放记录返回空串。
 */
std::string EditorDerivedTree(int owner, const Memory::Vector<int> &members, const Memory::String &name)
{
    if (owner < 0 || static_cast<std::size_t>(owner) >= g_nodes.size()) return {};
    const auto &declaration = N(owner);
    if (declaration.kind != MNode::StructDecl && declaration.kind != MNode::EnumDecl) return {};
    const auto type = EditorGenericSymbol(declaration.s);
    auto result = std::string("{\"kind\":\"derive\",\"name\":")
                  + chali_editor::jsonString(std::string_view(name.data(), name.size()))
                  + ",\"origin\":\"definition\",\"source\":" + EditorMacroLocation(declaration.deriveLine, declaration.deriveColumn)
                  + ",\"type\":" + chali_editor::jsonString(std::string_view(type.data(), type.size())) + ",\"children\":[";
    std::size_t budget = 10000;
    Memory::Map<std::uint32_t, Memory::String> symbols;
    EditorFieldSymbols fields;
    bool first = true;
    const auto inputs = N(declaration.kids[0]).kids;
    if (!inputs.empty())
    {
        result += "{\"kind\":\"metadata\",\"name\":\"";
        result += declaration.kind == MNode::EnumDecl ? "variants" : "fields";
        result += "\",\"origin\":\"argument\",\"children\":[";
        bool firstInput = true;
        for (const int input : inputs)
        {
            const auto node = N(input);
            if (!((node.kind == MNode::Field && node.type2 != 1) || node.kind == MNode::EnumVariant)) continue;
            if (!firstInput) result += ',';
            firstInput = false;
            result += EditorMacroTree(input, 2, budget, symbols, -1, &fields);
        }
        result += "]}";
        first = false;
    }
    for (const int member : members)
    {
        if (member < 0 || static_cast<std::size_t>(member) >= g_nodes.size() || N(member).kind != MNode::MethodDef) return {};
        if (!first) result += ',';
        first = false;
        result += EditorMacroTree(member, 1, budget, symbols, -1, &fields);
    }
    return result + "]}";
}

/**
 * @brief 记录生产派生方法及其真实调用来源，最终树在语义检查完成后读取。
 * @param owner 类型声明编号。
 * @param members 已成功注入的方法编号。
 * @param traits 本次内建派生名称。
 */
void RecordEditorDerived(int owner, const Memory::Vector<int> &members, const Memory::Vector<Memory::String> &traits)
{
    if (!chali_editor::compilerAnalysis.enabled() || members.empty() || traits.empty()) return;
    const auto declaration = N(owner);
    auto name = Memory::String("#derive(");
    for (std::size_t i = 0; i < traits.size(); ++i)
    {
        if (i) name += ", ";
        name += traits[i];
    }
    name += ')';
    const auto sourceLine = declaration.deriveLine;
    const auto file = sourceLine && sourceLine <= g_sourceFiles.size() ? g_sourceFiles[sourceLine - 1] : g_sourceFallback;
    const auto line = sourceLine && sourceLine <= g_sourceLines.size() && g_sourceLines[sourceLine - 1] ? g_sourceLines[sourceLine - 1] : sourceLine;
    chali_editor::compilerAnalysis.addMacro({ std::string(file.data(), file.size()), std::string(name.data(), name.size()),
                                              EditorDerivedTree(owner, members, name), line, declaration.deriveColumn,
                                              [owner, members, name] { return EditorDerivedTree(owner, members, name); } });
}
