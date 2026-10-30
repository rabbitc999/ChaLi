/**
 * @brief 替换宏模板的类型参数，具体实现与延迟表达式展开共用。
 * @param root 模板根。
 * @param arguments 类型参数到类型池编号的映射。
 * @param error 接收失败原因。
 * @return 类型替换成功时返回真。
 */
bool ApplyAstMacroTypes(int root, const std::map<std::string, int> &arguments, std::string &error);

Memory::Set<int> g_macroMemberActive;
Memory::Set<int> g_macroMemberReads;

/**
 * @brief 在登记布局和方法前原子展开一个类型中的成员宏。
 * @param owner 类型声明节点。
 * @return 成功返回真；失败不发布部分成员。
 */
bool ExpandAstMacroMembers(int owner)
{
    const auto declaration = N(owner);
    if (declaration.kind != MNode::StructDecl && declaration.kind != MNode::InterfaceDecl && declaration.kind != MNode::EnumDecl) return true;
    if (!g_macroRegistry) return true;
    const auto slot = declaration.kind == MNode::EnumDecl ? 1u : 0u;
    if (declaration.kids.size() <= slot) return true;
    if (!g_macroMemberActive.insert(owner).second)
    {
        g_macroMemberReads.insert(owner);
        return true;
    }
    struct RestoreMemberExpansion
    {
        int owner;
        /** @brief 无论展开成功或失败均撤销当前类型的活动标记。 */
        ~RestoreMemberExpansion()
        {
            g_macroMemberActive.erase(owner);
            g_macroMemberReads.erase(owner);
        }
    } restore{ owner };
    const int list = declaration.kids[slot];
    const auto original = N(list).kids;
    const auto nodeStart = g_nodes.size(), typeStart = g_types.size();
    Memory::Vector<int> members;
    Memory::Set<int> generated;
    std::size_t budget = 1000000;
    std::function<bool(int, std::size_t)> expand = [&](int id, std::size_t depth) {
        if (depth >= 256 || !budget--) return ReportAstMacroFailure(id, "成员宏展开超过深度或规模预算");
        const auto node = N(id);
        if (node.kind != MNode::MacroCall)
        {
            if (node.kind != MNode::Field && node.kind != MNode::MethodDef && node.kind != MNode::AssociatedDef && node.kind != MNode::AssociatedDecl)
                return ReportAstMacroFailure(id, "成员宏只能生成字段、方法、关联类型或成员宏调用");
            if (declaration.kind == MNode::InterfaceDecl && node.kind == MNode::Field)
                return ReportAstMacroFailure(id, "接口不能包含生成字段");
            if (declaration.kind != MNode::InterfaceDecl && node.kind == MNode::MethodDef && (node.kids.size() < 2 || node.kids[1] < 0))
                return ReportAstMacroFailure(id, "普通类型的生成方法必须提供方法体");
            members.push_back(id);
            if (depth || node.macroExpansion >= 0) generated.insert(id);
            return true;
        }
        std::string error;
        if (IsMacroCompileDiagnostic(id)) return PrepareMacroControls(id, {}, error) || ReportAstMacroFailure(id, Memory::String(error.data(), error.size()));
        const auto resolve = [&](int generic, int, int explicitType) {
            if (explicitType >= 0) return N(explicitType).type;
            error = "成员宏类型参数需要显式提供：" + std::string(N(generic).s.data(), N(generic).s.size());
            return -1;
        };
        const int result = g_macroRegistry->instantiate(id, error, resolve, ApplyAstMacroTypes);
        if (result < 0) return ReportAstMacroFailure(id, Memory::String(error.data(), error.size()));
        if (N(result).s2 != "member") return ReportAstMacroFailure(id, "类型成员位置需要 -> member 宏");
        if (!NormalizeMacroPatterns(result)) return false;
        std::function<bool(int)> append = [&](int child) {
            if (N(child).kind == MNode::MacroTemplate)
            {
                for (const int nested : N(N(child).kids[0]).kids)
                    if (!append(nested)) return false;
                return true;
            }
            return expand(child, depth + 1);
        };
        for (const int child : N(N(result).kids[0]).kids)
            if (!append(child)) return false;
        RecordEditorMacro(id, result);
        return true;
    };
    bool valid = true;
    for (const int member : original)
        if (!expand(member, 0))
        {
            valid = false;
            break;
        }
    if (valid)
        for (std::size_t i = 0; i < members.size() && valid; ++i)
        {
            const auto &a = N(members[i]);
            if (generated.count(members[i]) && a.kind == MNode::Field && g_macroMemberReads.count(owner))
                valid = ReportAstMacroFailure(members[i], "成员生成与本类型字段元数据存在循环依赖");
            for (std::size_t j = 0; j < i && valid; ++j)
            {
                const auto &b = N(members[j]);
                if (a.s != b.s || (!generated.count(members[i]) && !generated.count(members[j]))) continue;
                bool conflict = a.kind != MNode::MethodDef || b.kind != MNode::MethodDef;
                if (!conflict)
                {
                    const auto &ap = N(a.kids[0]).kids, &bp = N(b.kids[0]).kids;
                    conflict = ap.size() == bp.size();
                    for (std::size_t p = 0; conflict && p < ap.size(); ++p) conflict = SameType(T(N(ap[p]).type), T(N(bp[p]).type));
                }
                if (conflict) valid = ReportAstMacroFailure(members[i], "生成成员名称或签名冲突：" + a.s);
            }
        }
    if (!valid)
    {
        g_nodes.resize(nodeStart);
        g_types.resize(typeStart);
        return false;
    }
    std::function<void(int)> receiver = [&](int id) {
        if (id < 0) return;
        auto &node = g_nodes[id];
        if (node.kind == MNode::Var && node.s == "self" && !node.bindingIdentity) node.s2 = "__chali_macro_receiver";
        for (const int child : node.kids) receiver(child);
        if (node.kind == MNode::Block) receiver(static_cast<int>(node.i));
    };
    for (const int member : members)
        if (generated.count(member)) receiver(member);
    g_nodes[list].kids = std::move(members);
    if (std::any_of(original.begin(), original.end(), [](int id) { return N(id).kind == MNode::MacroCall; }))
    {
        const auto previousModule = g_parseModule;
        g_parseModule = declaration.s.find("::") == Memory::String::npos ? declaration.module : Memory::String{};
        M_BindAssociatedTypes(declaration.s, list);
        g_parseModule = previousModule;
    }
    return true;
}

/**
 * @brief 在注册语言类型前展开模块级声明宏，失败时不发布部分声明。
 * @param registry 已收集全部可见宏的注册表。
 * @return 全部声明展开成功时返回真。
 */
bool ExpandAstMacroDeclarations(chali_macro::Registry &registry)
{
    const auto original = g_top;
    const auto nodeStart = g_nodes.size();
    const auto typeStart = g_types.size();
    Memory::Vector<int> declarations;
    std::size_t budget = 1000000;
    std::function<bool(int, std::size_t)> expand;
    expand = [&](int id, std::size_t depth) {
        if (depth >= 256 || !budget) return ReportAstMacroFailure(id, "声明宏展开超过深度或规模预算");
        --budget;
        const auto node = N(id);
        if (node.kind != MNode::MacroCall)
        {
            declarations.push_back(id);
            return true;
        }
        ReductionSourceScope source(id);
        std::string error;
        if (IsMacroCompileDiagnostic(id))
        {
            if (!PrepareMacroControls(id, {}, error)) return ReportAstMacroFailure(id, Memory::String(error.data(), error.size()));
            return true;
        }
        const auto resolve = [&](int generic, int, int explicitType) {
            if (explicitType < 0)
            {
                error = "声明宏类型参数需要显式提供：" + std::string(N(generic).s.data(), N(generic).s.size());
                return -1;
            }
            return N(explicitType).type;
        };
        const int expanded = registry.instantiate(id, error, resolve, ApplyAstMacroTypes);
        if (expanded < 0) return ReportAstMacroFailure(id, Memory::String(error.data(), error.size()));
        if (!NormalizeMacroPatterns(expanded)) return false;
        if (N(expanded).s2 != "item") return ReportAstMacroFailure(id, "模块声明位置需要 -> item 宏");
        const auto items = N(N(expanded).kids[0]).kids;
        for (const int child : items)
        {
            const auto kind = N(child).kind;
            if (kind != MNode::FuncDef && kind != MNode::StructDecl && kind != MNode::EnumDecl && kind != MNode::InterfaceDecl && kind != MNode::MacroCall)
                return ReportAstMacroFailure(id, "声明宏只能生成函数、结构体、枚举、接口或声明宏调用");
            if (!expand(child, depth + 1)) return false;
        }
        RecordEditorMacro(id, expanded);
        return true;
    };
    for (const int id : original)
        if (!expand(id, 0))
        {
            g_nodes.resize(nodeStart);
            g_types.resize(typeStart);
            return false;
        }
    g_top = std::move(declarations);
    return true;
}
