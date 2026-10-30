/**
 * @file custom_derive.cpp
 * @brief 通过已有成员宏实现同名接口派生，保持布局和辅助名称卫生。
 */
/**
 * @brief 从卫生成员名移除本次声明前缀，保留用户原始名称的其余部分。
 * @param name 编译器生成或普通成员名。
 * @return 原始声明名；不是生成名时保持原样。
 */
Memory::String CustomDerivedMemberName(const Memory::String &name)
{
    if (name.rfind("__chali_decl_", 0) != 0) return name;
    std::size_t at = 13;
    for (int part = 0; part < 2; ++part)
    {
        const auto begin = at;
        while (at < name.size() && name[at] >= '0' && name[at] <= '9') ++at;
        if (at == begin || at == name.size() || name[at] != '_') return name;
        ++at;
    }
    return name.substr(at);
}

/**
 * @brief 查找与派生宏同模块同名的具体接口。
 * @param definition 实际选中的宏定义。
 * @return 接口声明节点；未实例化或缺失时返回负一。
 */
int CustomDerivedInterface(const MNode &definition)
{
    const auto qualified = ModuleSymbol(definition.module, definition.s);
    for (const int root : g_top)
        if (N(root).kind == MNode::InterfaceDecl && (N(root).s == qualified || (N(root).module == definition.module && N(root).s == definition.s))) return root;
    return -1;
}

/**
 * @brief 按普通宏入口展开自定义派生，仅公开满足配对接口的方法。
 * @param owner 具体结构体或枚举声明。
 * @param trait 用户请求的宏／接口名称。
 * @param implemented 接收具体接口名称。
 * @return 成功时发布方法，失败或实例化重试时恢复原成员及生成池。
 */
bool ExpandCustomDerivedDeclaration(int owner, const Memory::String &trait, Memory::String &implemented)
{
    if (!g_macroRegistry) return Fail("自定义派生需要已登记的同名成员宏：" + trait);
    if (trait.find('<') != Memory::String::npos) return Fail("自定义派生的实现者类型由编译器传入，请只写派生名称");
    const auto declaration = N(owner);
    const auto slot = declaration.kind == MNode::EnumDecl ? 1u : 0u;
    const auto previousTop = g_top;
    const auto previousMembers = declaration.kids.size() > slot ? N(declaration.kids[slot]).kids : Memory::Vector<int>{};
    const auto nodeStart = g_nodes.size(), typeStart = g_types.size(), nameStart = g_names.size();
    const auto fileStart = g_sourceFiles.size(), lineStart = g_sourceLines.size();
    const auto previousModule = g_parseModule;
    const auto previousMade = chali_generic::session.made;
    const auto previousPending = chali_generic::session.pending;
    const auto previousImplemented = chali_generic::session.implemented;
    const auto previousEmitted = chali_generic::session.emitted;
    struct RollbackCustomDerive
    {
        std::function<void()> action;
        /** @brief 失败和异常重试均不保留半成品声明。 */
        ~RollbackCustomDerive()
        {
            if (action) action();
        }
    } rollback{ [&] {
        g_nodes[owner] = declaration;
        if (declaration.kids.size() > slot) g_nodes[declaration.kids[slot]].kids = previousMembers;
        g_top = previousTop;
        g_nodes.resize(nodeStart);
        g_types.resize(typeStart);
        g_names.resize(nameStart);
        g_sourceFiles.resize(fileStart);
        g_sourceLines.resize(lineStart);
        g_parseModule = previousModule;
        chali_generic::session.made = previousMade;
        chali_generic::session.pending = previousPending;
        chali_generic::session.implemented = previousImplemented;
        chali_generic::session.emitted = previousEmitted;
    } };
    if (g_nodes[owner].kids.size() <= slot)
    {
        const int list = M_List();
        g_nodes[owner].kids.push_back(list);
        g_nodes[owner].i = -1;
    }
    const int members = N(owner).kids[slot];
    const int group = M_New(MNode::MacroArgumentGroup, "", '(');
    const int arguments = M_New(MNode::MacroArguments);
    g_nodes[arguments].kids = { group };
    const int type = M_New(MNode::Var);
    g_nodes[type].type = M_AddType(NamedType(declaration.s));
    const int types = M_List();
    M_ListAdd(types, type);
    const int call = M_New(MNode::MacroCall, trait);
    g_nodes[call].kids = { arguments, types };
    g_nodes[call].module = declaration.module;
    g_nodes[call].sourceLine = declaration.deriveLine ? declaration.deriveLine : declaration.sourceLine;
    g_nodes[call].sourceColumn = declaration.deriveColumn ? declaration.deriveColumn : declaration.sourceColumn;
    g_nodes[call].macroExpansion = declaration.macroExpansion;
    M_ListAdd(members, call);
    if (!ExpandAstMacroMembers(owner)) return false;
    int frame = -1;
    for (std::size_t index = nodeStart; index < g_nodes.size(); ++index)
        if (g_nodes[index].kind == MNode::MacroExpansionFrame && g_nodes[index].s == trait)
        {
            frame = static_cast<int>(index);
            break;
        }
    if (frame < 0) return ReportAstMacroFailure(call, "自定义派生没有产生宏定义来源");
    const auto definition = N(static_cast<int>(N(frame).i));
    if (N(definition.kids[2]).kids.size() != 1) return ReportAstMacroFailure(call, "自定义派生宏需要一个实现者类型参数");
    int interface = CustomDerivedInterface(definition);
    if (interface < 0)
    {
        const chali_generic::Generic *generic = nullptr;
        for (const auto &entry : chali_generic::session.generics)
            if (entry.second.module == definition.module && entry.second.sourceName == definition.s)
            {
                generic = &entry.second;
                break;
            }
        if (!generic || !chali_generic::wordAt(chali_generic::trim(generic->declaration), 0, "interface"))
            return ReportAstMacroFailure(call, "自定义派生需要宏同模块的同名接口：" + trait);
        if (generic->params.size() != 1 || generic->params[0].constant)
            return ReportAstMacroFailure(call, "自定义派生接口需要一个实现者类型参数");
        Memory::String source, error;
        Memory::Vector<preprocess::LineOrigin> origins;
        if (!chali_generic::instantiate(ModuleSymbol(definition.module, definition.s), { AnnotationName(declaration) }, source, error, &origins))
            return ReportAstMacroFailure(call, error);
        const auto top = g_top;
        const auto first = g_nodes.size();
        chali::Engine parser;
        int value = 0;
        g_parseModule.clear();
        const auto status = parser.runText(source, &value);
        const auto added = g_top;
        g_top = top;
        g_parseModule = previousModule;
        if (status != chali::Status::Ok) return ReportAstMacroFailure(call, "自定义派生接口实例无法解析：" + trait);
        const auto base = g_sourceFiles.size();
        for (const auto &origin : origins)
        {
            g_sourceFiles.push_back(origin.file);
            g_sourceLines.push_back(origin.line);
        }
        M_ShiftNodeLines(first, base);
        g_top.insert(g_top.end(), added.begin(), added.end());
        for (const int root : added)
            if (N(root).kind == MNode::InterfaceDecl && N(root).module == definition.module
                && (N(root).s == definition.s || N(root).s.rfind(definition.s + "__", 0) == 0 || N(root).s.rfind(generic->name + "__", 0) == 0))
                interface = root;
        if (interface < 0) return ReportAstMacroFailure(call, "找不到实例化后的派生接口：" + trait);
    }
    RefreshMacroActiveSyntax();
    if (!ResolveImportedTypeNames(false)) return false;
    const auto contract = ReadMacroTypeMembers(NamedType(N(interface).s), true);
    if (!contract) return false;
    Memory::Map<Memory::String, Memory::String> published;
    Memory::Vector<int> generated;
    MReflectionBuilder names;
    names.indexDeclarations();
    for (const int member : N(members).kids)
    {
        if (std::find(previousMembers.begin(), previousMembers.end(), member) != previousMembers.end()) continue;
        const auto method = N(member);
        if (method.kind != MNode::MethodDef) return ReportAstMacroFailure(call, "自定义派生只能生成方法；字段和关联类型请在类型中声明");
        generated.push_back(member);
        const auto originalName = CustomDerivedMemberName(method.s);
        for (const auto &required : contract->methods)
        {
            const auto parameters = N(method.kids[0]).kids;
            if (required.name != originalName || required.isStatic != (method.type2 == 1) || required.parameters.size() != parameters.size()) continue;
            const auto actualResult = MacroResolveAssociated(T(method.type), declaration.s);
            const auto expectedResult = MacroResolveAssociated(required.result, declaration.s);
            bool same = actualResult && expectedResult && names.key(actualResult) == names.key(expectedResult);
            for (std::size_t index = 0; same && index < parameters.size(); ++index)
            {
                const auto actual = MacroResolveAssociated(T(N(parameters[index]).type), declaration.s);
                const auto expected = MacroResolveAssociated(required.parameters[index].type, declaration.s);
                same = actual && expected && names.key(actual) == names.key(expected) && ((N(parameters[index]).i != 0) == required.parameters[index].variadic);
            }
            if (same) published[method.s] = required.name;
        }
    }
    std::size_t budget = 1000000;
    std::function<bool(int, unsigned)> publish = [&](int id, unsigned depth) {
        if (id < 0) return true;
        if (depth >= 256 || !budget--) return ReportAstMacroFailure(call, "派生成员名称绑定超过预算");
        auto &node = g_nodes[id];
        const auto name = published.find(node.s);
        if (name != published.end()) node.s = name->second;
        if (node.kind == MNode::Member || node.kind == MNode::Arrow || node.kind == MNode::TypeQualifiedRef || node.kind == MNode::EnumCtor)
        {
            const auto qualified = published.find(node.s2);
            if (qualified != published.end()) node.s2 = qualified->second;
        }
        const auto children = node.kids;
        for (const int child : children)
            if (!publish(child, depth + 1)) return false;
        return node.kind != MNode::Block || publish(static_cast<int>(node.i), depth + 1);
    };
    for (const int member : generated)
        if (!publish(member, 0)) return false;
    for (const int member : generated)
        if (!ExpandAstMacroNode(member, *g_macroRegistry, 0, budget)) return false;
    const auto all = N(members).kids;
    for (const int member : generated)
        for (const int existing : all)
        {
            if (member == existing || N(existing).kind != MNode::MethodDef || N(member).s != N(existing).s) continue;
            const auto left = N(N(member).kids[0]).kids, right = N(N(existing).kids[0]).kids;
            bool same = left.size() == right.size();
            for (std::size_t index = 0; same && index < left.size(); ++index) same = SameType(T(N(left[index]).type), T(N(right[index]).type));
            if (same) return ReportAstMacroFailure(call, "派生方法不覆盖已有成员：" + CustomDerivedMemberName(N(member).s));
        }
    implemented = N(interface).s;
    rollback.action = {};
    return true;
}

/**
 * @brief 在派生元数据请求新泛型实例时补充声明后重试，不让内部请求逃出编译入口。
 * @return 所有派生完成时为真；解析、约束或预算失败时保留诊断。
 */
bool ExpandReadyDerivedDeclarations()
{
    for (std::size_t pass = 0; pass < 256; ++pass)
    {
        const bool custom = std::any_of(g_top.begin(), g_top.end(), [](int id) {
            const auto &node = N(id);
            return !node.derivesExpanded && std::any_of(node.derives.begin(), node.derives.end(), [](const Memory::String &trait) {
                return trait != "Equal" && trait != "Hash" && trait != "Clone";
            });
        });
        if (!custom) return ExpandDerivedDeclarations();
        const auto nodes = g_nodes;
        const auto types = g_types;
        const auto names = g_names;
        const auto roots = g_top;
        const auto files = g_sourceFiles;
        const auto lines = g_sourceLines;
        const auto made = chali_generic::session.made;
        const auto pending = chali_generic::session.pending;
        const auto implemented = chali_generic::session.implemented;
        const auto emitted = chali_generic::session.emitted;
        const auto analysis = chali_editor::compilerAnalysis;
        try
        {
            RefreshMacroActiveSyntax();
            if (!ResolveImportedTypeNames(false)) return false;
            return ExpandDerivedDeclarations();
        } catch (const MInstantiationRequest &request)
        {
            // 同一类型前面的派生也必须撤销，重试不能再次生成其已发布的方法。
            g_nodes = nodes;
            g_types = types;
            g_names = names;
            g_top = roots;
            g_sourceFiles = files;
            g_sourceLines = lines;
            chali_generic::session.made = made;
            chali_generic::session.pending = pending;
            chali_generic::session.implemented = implemented;
            chali_generic::session.emitted = emitted;
            chali_editor::compilerAnalysis = analysis;
            Memory::String source, error;
            Memory::Vector<preprocess::LineOrigin> origins;
            if (!chali_generic::instantiate(request.name, request.args, source, error, &origins)) return Fail(error);
            if (source.empty()) return Fail("派生元数据实例化未产生新声明：" + request.name);
            const auto top = g_top;
            const auto module = g_parseModule;
            const auto first = g_nodes.size(), diagnostic = g_parseDiagnostics.size();
            chali::Engine parser;
            int value = 0;
            g_parseModule.clear();
            const auto status = parser.runText(source, &value);
            g_parseModule = module;
            const auto base = g_sourceFiles.size();
            for (const auto &origin : origins)
            {
                g_sourceFiles.push_back(origin.file);
                g_sourceLines.push_back(origin.line);
            }
            M_ShiftNodeLines(first, base);
            M_ShiftParseDiagnostics(diagnostic, base);
            if (status != chali::Status::Ok) return Fail("派生所需泛型声明无法解析：" + request.name);
            if (!g_parseDiagnostics.empty())
            {
                for (const auto &item : g_parseDiagnostics) AddDiagnostic(item.message, item.line, item.column);
                g_parseDiagnostics.clear();
                RestoreFirstDiagnostic();
                return false;
            }
            if (!ExpandAdditionalAstMacros()) return false;
            g_top.insert(g_top.begin(), top.begin(), top.end());
        }
    }
    return Fail("派生元数据实例化超过 256 轮，请检查递归依赖");
}
