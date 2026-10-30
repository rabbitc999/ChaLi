/**
 * @brief 判断调用是否属于宏编译期诊断原语。
 * @param id 待检查节点。
 * @return 是编译期错误或断言调用时返回真。
 */
bool IsMacroCompileDiagnostic(int id)
{
    return id >= 0 && N(id).kind == MNode::MacroCall && (N(id).s == "compile_error" || N(id).s == "compile_assert");
}

/**
 * @brief 在符号登记前查找可见的源码 const 初始化式，不读取可变全局值。
 * @param reference 常量变量或限定模块引用。
 * @param error 接收可见名称歧义。
 * @param declaredType 非空时接收声明的类型编号，用于保持数值位宽和符号。
 * @return const 初始化式编号，未找到返回负一。
 */
int FindMacroSyntaxConstant(const MNode &reference, std::string &error, int *declaredType = nullptr)
{
    if (reference.kind != MNode::Var && (reference.kind != MNode::EnumCtor || !reference.kids.empty())) return -1;
    if (reference.kind == MNode::Var && reference.bindingIdentity)
    {
        int result = -1, score = 0;
        const auto context = reference.bindingContext ? reference.bindingContext : reference.syntaxContext;
        for (std::size_t index = 0; index < g_nodes.size(); ++index)
        {
            const auto &declaration = g_nodes[index];
            if (declaration.kind != MNode::Declare || declaration.bindingIdentity != reference.bindingIdentity || declaration.type2 != 1 || declaration.kids.empty() || declaration.kids[0] < 0 || N(declaration.kids[0]).kind == MNode::List) continue;
            const int rank = declaration.syntaxContext == context ? 2 : 1;
            if (rank < score) continue;
            score = rank;
            result = declaration.kids[0];
            if (declaredType) *declaredType = declaration.type;
        }
        if (result >= 0) return result;
    }
    const bool qualified = reference.kind == MNode::EnumCtor;
    const auto name = qualified ? reference.s2 : reference.s;
    const auto module = qualified ? reference.s : reference.module;
    Memory::Set<Memory::String> visible{ "std::lang" };
    for (const int root : g_top)
        if (N(root).kind == MNode::ImportDecl && N(root).module == reference.module) visible.insert(N(root).s);
    int result = -1, best = 0;
    for (const int root : g_top)
    {
        const auto group = N(root);
        if (group.kind != MNode::Declare || group.kids.size() != 1 || group.kids[0] < 0 || N(group.kids[0]).kind != MNode::List) continue;
        for (const int child : N(group.kids[0]).kids)
        {
            const auto declaration = N(child);
            if (group.type2 != 1 && declaration.type2 != 1) continue;
            const auto declarationModule = declaration.module.empty() ? group.module : declaration.module;
            const int rank = declarationModule == module ? 2 : !qualified && visible.count(declarationModule) ? 1 :
                                                                                                                0;
            if (!rank || rank < best) continue;
            if (declaration.s != name || declaration.kids.empty() || declaration.kids[0] < 0) continue;
            if (reference.bindingIdentity && reference.bindingIdentity != declaration.bindingIdentity) continue;
            if (rank == best && result >= 0 && result != declaration.kids[0])
            {
                error = "编译期常量名称存在歧义：" + std::string(name.data(), name.size());
                return -1;
            }
            result = declaration.kids[0];
            if (declaredType) *declaredType = declaration.type >= 0 ? declaration.type : group.type;
            best = rank;
        }
    }
    return result;
}

/**
 * @brief 读取已归一化的诊断字符串，不执行用户函数或运行期格式化。
 * @param id 字符串常量或字符串连接节点。
 * @param value 接收按语言规则解码的 UTF-8 字节。
 * @param error 接收错误原因。
 * @param depth 当前递归深度。
 * @return 所有操作数都是常量字符串时返回真。
 */
bool ReadMacroDiagnosticString(int id, std::string &value, std::string &error, std::size_t depth = 0)
{
    if (id < 0 || depth >= 256)
    {
        error = "编译期诊断字符串超过深度预算";
        return false;
    }
    const auto node = N(id);
    if (node.kind == MNode::Var)
    {
        const auto binding = FindNodeVar(node);
        if (binding && binding->isConst && binding->constantExpression >= 0)
            return ReadMacroDiagnosticString(binding->constantExpression, value, error, depth + 1);
    }
    if (node.kind == MNode::Var || node.kind == MNode::EnumCtor)
    {
        const int initializer = FindMacroSyntaxConstant(node, error);
        if (!error.empty()) return false;
        if (initializer >= 0) return ReadMacroDiagnosticString(initializer, value, error, depth + 1);
    }
    if (node.kind == MNode::Str)
    {
        if (node.s.size() > 1048576)
        {
            error = "编译期诊断字符串超过字节预算";
            return false;
        }
        return chali_literal::decodeString(std::string_view(node.s.data(), node.s.size()), value, error);
    }
    if (node.kind == MNode::Bin && node.s == "+" && node.kids.size() == 2)
    {
        std::string left, right;
        if (!ReadMacroDiagnosticString(node.kids[0], left, error, depth + 1) || !ReadMacroDiagnosticString(node.kids[1], right, error, depth + 1)) return false;
        if (left.size() > 1048576 || right.size() > 1048576 - left.size())
        {
            error = "编译期诊断字符串超过字节预算";
            return false;
        }
        value = left + right;
        return true;
    }
    error = "编译期诊断消息必须是字符串常量";
    return false;
}

/**
 * @brief 实现宏错误和断言，成功断言仅保留可在语句表中删除的 bool 标记。
 * @param id 内建诊断调用。
 * @param normalize 在当前字段与实参环境中归一化常量子树的回调。
 * @param fields 当前嵌套字段遍历路径。
 * @param error 接收单条可定位到宏、字段和实参的诊断。
 * @return 断言成立时返回真，显式错误或非法参数返回假。
 */
bool PrepareMacroDiagnostic(int id, const std::function<bool(int)> &normalize,
                            const Memory::Vector<chali_macro::MetadataValue> &fields, std::string &error)
{
    const auto call = N(id);
    if (!IsMacroCompileDiagnostic(id)) return true;
    const bool assertion = call.s == "compile_assert";
    if (call.kids.size() != 1 || call.kids[0] < 0 || N(call.kids[0]).kids.size() != 1)
    {
        error = "编译期诊断需要一组圆括号参数";
        return false;
    }
    const auto group = N(N(call.kids[0]).kids[0]);
    if (group.i != '(' || (assertion ? group.kids.empty() || group.kids.size() > 2 : group.kids.size() != 1))
    {
        error = assertion ? "$compile_assert 需要 bool 常量和可选的字符串消息" : "$compile_error 需要一个字符串常量";
        return false;
    }
    Memory::Vector<int> arguments;
    for (const int item : group.kids)
    {
        const auto fragment = N(item);
        if (fragment.kind != MNode::MacroArgument || fragment.kids.empty() || fragment.kids[0] < 0)
        {
            error = "编译期诊断参数必须是完整常量表达式";
            return false;
        }
        if (!normalize(fragment.kids[0])) return false;
        arguments.push_back(fragment.kids[0]);
    }
    bool passed = false;
    if (assertion)
    {
        MConstant condition;
        if (!EvaluateConstant(arguments[0], &condition) || condition.kind != MConstant::Boolean)
        {
            error = "$compile_assert 条件必须是编译期 bool 常量";
            return false;
        }
        passed = condition.integer != 0;
    }
    std::string message = "条件不成立";
    if (!assertion || arguments.size() == 2)
        if (!ReadMacroDiagnosticString(arguments[assertion ? 1 : 0], message, error)) return false;
    if (passed)
    {
        auto &node = g_nodes[id];
        node.kind = MNode::Bool;
        node.i = 1;
        node.type = -1;
        node.s.clear();
        node.s2 = "__chali_compile_assert_success";
        node.kids.clear();
        node.compileMetadata.reset();
        return true;
    }
    error = assertion ? "编译期断言失败：" : "编译期错误：";
    error += message;
    for (const auto &field : fields)
    {
        const auto &name = field.owner->fields[field.field].name;
        error += "\n  字段 " + std::string(field.owner->name.data(), field.owner->name.size()) + "." + std::string(name.data(), name.size()) + "（序号 " + std::to_string(field.field) + "）";
    }
    const auto &argument = N(arguments[0]);
    const auto sourceLine = argument.expansionLine ? argument.expansionLine : argument.sourceLine;
    const auto sourceColumn = argument.expansionLine ? argument.expansionColumn : argument.sourceColumn;
    if (sourceLine)
    {
        const auto line = sourceLine <= g_sourceLines.size() && g_sourceLines[sourceLine - 1] ? g_sourceLines[sourceLine - 1] : sourceLine;
        const auto file = sourceLine <= g_sourceFiles.size() && !g_sourceFiles[sourceLine - 1].empty() ? g_sourceFiles[sourceLine - 1] : g_sourceFallback;
        error += "\n  诊断实参位置 " + std::string(file.data(), file.size()) + ":" + std::to_string(line) + ":" + std::to_string(sourceColumn);
    }
    return false;
}
