/**
 * @brief 替换宏模板的类型参数，具体实现与延迟表达式展开共用。
 * @param root 模板根。
 * @param arguments 类型参数到类型池编号的映射。
 * @param error 接收失败原因。
 * @return 类型替换成功时返回真。
 */
bool ApplyAstMacroTypes(int root, const std::map<std::string, int> &arguments, std::string &error);

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
