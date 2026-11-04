/**
 * @brief 将已确定的静态类型显示为源码函数类型，避免暴露闭包环境或卫生绑定键。
 * @param type 完整静态类型。
 * @param depth 当前递归深度。
 * @return 用户可读的类型；未确定的泛型闭包返回空串，由编辑器保留符号签名。
 */
Memory::String EditorTypeText(const std::shared_ptr<MType> &type, std::size_t depth = 0)
{
    if (!type || depth >= 128) return {};
    if (type->kind == MType::Func)
    {
        if (!type->name.empty())
        {
            const auto generic = g_astGenericClosures.find(type->name);
            if (generic == g_astGenericClosures.end()) return {};
            const auto syntax = N(generic->second.syntax);
            int body = syntax.kids[2];
            if (N(body).kind == MNode::Block)
            {
                const auto statements = N(static_cast<int>(N(body).i)).kids;
                if (statements.size() != 1) return {};
                body = statements[0];
            }
            if (N(body).kind == MNode::Return && N(body).kids.size() == 1) body = N(body).kids[0];
            auto result = syntax.type >= 0 ? T(syntax.type) : std::shared_ptr<MType>{};
            if (!result && N(body).kind == MNode::Var)
                for (const int parameter : N(syntax.kids[1]).kids)
                    if (N(parameter).s == N(body).s && (!N(body).bindingIdentity || N(parameter).bindingIdentity == N(body).bindingIdentity)) result = T(N(parameter).type);
            if (!result && N(body).kind == MNode::Cast && N(body).type >= 0) result = T(N(body).type);
            if (!result) return {};
            auto text = EditorTypeText(result, depth + 1) + " fn<";
            bool first = true;
            for (const int parameter : N(syntax.kids[0]).kids)
            {
                if (!first) text += ", ";
                first = false;
                text += N(parameter).s;
            }
            text += ">(";
            first = true;
            for (const int parameter : N(syntax.kids[1]).kids)
            {
                if (!first) text += ", ";
                first = false;
                text += EditorTypeText(T(N(parameter).type), depth + 1);
            }
            return text + ")";
        }
        auto resultType = EditorTypeText(type->elem, depth + 1);
        if (type->elem && type->elem->kind == MType::Func) resultType = "(" + resultType + ")";
        auto text = resultType + " fn(";
        for (std::size_t index = 0; index < type->paramTypes.size(); ++index)
        {
            if (index) text += ", ";
            if (type->variadic && index + 1 == type->paramTypes.size()) text += "...";
            text += EditorTypeText(type->paramTypes[index], depth + 1);
        }
        return text + ")";
    }
    if (type->kind == MType::Ptr) return EditorTypeText(type->elem, depth + 1) + (type->readOnly ? " const&" : " mut&");
    if (type->kind == MType::Array)
    {
        auto text = EditorTypeText(type->elem, depth + 1);
        if (type->dims.empty()) return text + "[]";
        for (const auto dimension : type->dims) text += "[" + Memory::toString(dimension) + "]";
        return text;
    }
    if (type->kind == MType::Struct)
    {
        const auto instance = chali_generic::session.made.find(BareTypeName(type->name));
        if (instance != chali_generic::session.made.end())
        {
            auto text = instance->second.generic->name + "<";
            for (std::size_t index = 0; index < instance->second.args.size(); ++index)
            {
                if (index) text += ", ";
                text += instance->second.args[index];
            }
            return text + ">";
        }
    }
    return Describe(type);
}

Memory::Map<int, Memory::String> g_editorMacroInferredTypes;
Memory::Map<std::uint32_t, Memory::String> g_editorMacroBindingTypes;

/**
 * @brief 收集 auto 原始声明的最终静态类型，供编辑器显示宏与高阶闭包的真实结果。
 * @param id 原始变量声明节点。
 * @param type 完整推导得到的类型。
 */
void RecordEditorTypeHint(int id, const std::shared_ptr<MType> &type)
{
    if (!chali_editor::compilerAnalysis.enabled()) return;
    const auto &node = N(id);
    const auto text = EditorTypeText(type);
    if (text.empty()) return;
    g_editorMacroInferredTypes[id] = text;
    if (node.bindingIdentity) g_editorMacroBindingTypes[node.bindingIdentity] = text;
    if (node.syntaxContext || node.s.empty() || node.s.rfind("__chali_", 0) == 0) return;
    const auto file = node.sourceLine && node.sourceLine <= g_sourceFiles.size() ? g_sourceFiles[node.sourceLine - 1] : g_sourceFallback;
    const auto line = node.sourceLine && node.sourceLine <= g_sourceLines.size() && g_sourceLines[node.sourceLine - 1] ? g_sourceLines[node.sourceLine - 1] : node.sourceLine;
    chali_editor::compilerAnalysis.addType({ std::string(file.data(), file.size()), std::string(node.s.data(), node.s.size()), std::string(text.data(), text.size()), line, node.sourceColumn });
}

/**
 * @brief 将合并源码位置映射回原文件，以 JSON 保存可跳转的调用或定义位置。
 * @param line 合并源码行号。
 * @param column 源码列号。
 * @return 原文件位置对象；未知位置返回空对象。
 */
std::string EditorMacroLocation(std::size_t line, std::size_t column)
{
    if (!line) return "{}";
    const auto &file = line <= g_sourceFiles.size() && !g_sourceFiles[line - 1].empty() ? g_sourceFiles[line - 1] : g_sourceFallback;
    const auto original = line <= g_sourceLines.size() && g_sourceLines[line - 1] ? g_sourceLines[line - 1] : line;
    return "{\"file\":" + chali_editor::jsonString(std::string_view(file.data(), file.size())) + ",\"line\":" + std::to_string(original) + ",\"column\":" + std::to_string(column) + "}";
}

/**
 * @brief 隐藏名称及类型拼写中的分配序号，保留辅助名称的可读部分。
 * @param name 原始名称、生成名称或类型拼写。
 * @return 不含内部展开序号的显示名。
 */
Memory::String EditorMacroName(const Memory::String &name)
{
    if (name.rfind("__chali_lambda_", 0) == 0) return "fn";
    auto result = name;
    for (auto begin = result.find("__chali_decl_"); begin != Memory::String::npos; begin = result.find("__chali_decl_", begin))
    {
        auto end = begin + 13;
        bool valid = true;
        for (int part = 0; part < 2; ++part)
        {
            const auto digits = end;
            while (end < result.size() && result[end] >= '0' && result[end] <= '9') ++end;
            if (digits == end || end == result.size() || result[end] != '_')
            {
                valid = false;
                break;
            }
            ++end;
        }
        if (valid)
            result.erase(begin, end - begin);
        else
            begin += 13;
    }
    return result;
}

/**
 * @brief 将已经确认的泛型符号显示为定义名和具体实参，不改变普通标识符。
 * @param symbol 具体实例名或具体类型实参。
 * @param includeModule 是否显示定义模块，默认用户模块省略前缀。
 * @param depth 当前展开深度，限制异常递归实例的显示规模。
 * @return 可读的泛型拼写；非实例保留原始名称。
 */
Memory::String EditorGenericSymbol(const Memory::String &symbol, bool includeModule = true, unsigned depth = 0)
{
    auto found = chali_generic::session.made.find(symbol);
    if (found == chali_generic::session.made.end()) found = chali_generic::session.made.find(BareTypeName(symbol));
    if (found == chali_generic::session.made.end() || depth >= 64) return EditorMacroName(symbol);
    const auto &instance = found->second;
    const auto &generic = *instance.generic;
    auto result = generic.sourceName.empty() ? BareTypeName(generic.name) : generic.sourceName;
    if (includeModule && !generic.module.empty() && generic.module != "program") result = generic.module + "::" + result;
    result += '<';
    for (std::size_t i = 0; i < instance.args.size(); ++i)
    {
        if (i) result += ", ";
        result += EditorGenericSymbol(instance.args[i], true, depth + 1);
    }
    return result + '>';
}

#include "editor_macro_effects.cpp"

/**
 * @brief 将宏生成的节点显示为有界的结构树，不重新打印或重新解析可执行源码。
 * @param id 节点编号。
 * @param depth 当前深度。
 * @param budget 剩余节点预算。
 * @param symbols 本树内卫生身份到稳定可读符号的映射。
 * @param actualExpression 根宏在语义检查后的实际表达式，负值沿用首次展开树。
 * @return 合法 JSON 结构树。
 */
std::string EditorMacroTree(int id, std::size_t depth, std::size_t &budget, Memory::Map<std::uint32_t, Memory::String> &symbols, int actualExpression = -1)
{
    if (id < 0 || static_cast<std::size_t>(id) >= g_nodes.size()) return "null";
    if (!budget || depth >= 128) return "{\"kind\":\"truncated\"}";
    --budget;
    const auto &node = N(id);
    const char *kind = "node";
    switch (node.kind)
    {
    case MNode::MacroTemplate: kind = "template"; break;
    case MNode::MacroCall: kind = "macro_call"; break;
    case MNode::MacroImplement: kind = "implements"; break;
    case MNode::MacroArgument: kind = "source_fragment"; break;
    case MNode::List: kind = "sequence"; break;
    case MNode::Var: kind = "identifier"; break;
    case MNode::Int: kind = "integer"; break;
    case MNode::Str: kind = "string"; break;
    case MNode::Bin: kind = "binary"; break;
    case MNode::Un: kind = "unary"; break;
    case MNode::Call: kind = "call"; break;
    case MNode::Declare: kind = "declaration"; break;
    case MNode::ExprStmt: kind = "expression_statement"; break;
    case MNode::Block: kind = "block"; break;
    case MNode::Loop: kind = node.s.empty() ? "loop" : "value_block"; break;
    case MNode::While: kind = "while"; break;
    case MNode::For: kind = "for"; break;
    case MNode::Foreach: kind = "foreach"; break;
    case MNode::DoWhile: kind = "do_while"; break;
    case MNode::Switch: kind = node.s == "match" ? "match" : "switch"; break;
    case MNode::Case: kind = "case"; break;
    case MNode::Break: kind = node.kids.empty() ? "break" : "yield"; break;
    case MNode::Continue: kind = "continue"; break;
    case MNode::Try: kind = "propagation"; break;
    case MNode::Return: kind = "return"; break;
    case MNode::Lambda: kind = "closure"; break;
    case MNode::FuncDef: kind = "function"; break;
    case MNode::MethodDef: kind = "method"; break;
    case MNode::Field: kind = "field"; break;
    case MNode::StructDecl: kind = "struct"; break;
    case MNode::InterfaceDecl: kind = "interface"; break;
    case MNode::EnumDecl: kind = "enum"; break;
    case MNode::EnumVariant: kind = "variant"; break;
    case MNode::AssociatedDef:
    case MNode::AssociatedDecl: kind = "associated_type"; break;
    case MNode::Params: kind = "parameter"; break;
    case MNode::Cast: kind = "cast"; break;
    case MNode::Member:
    case MNode::Arrow: kind = "member"; break;
    case MNode::EnumCtor:
    case MNode::TypeQualifiedRef: kind = "qualified_member"; break;
    default: break;
    }
    const auto name = EditorMacroName(node.s);
    auto result = std::string("{\"kind\":") + chali_editor::jsonString(kind) + ",\"name\":" + chali_editor::jsonString(std::string_view(name.data(), name.size()));
    result += ",\"origin\":" + chali_editor::jsonString(MacroControlContext(node) ? "definition" : "argument");
    result += ",\"source\":" + EditorMacroLocation(node.sourceLine, node.sourceColumn);
    if (node.metadataLine)
        result += ",\"metadataSource\":" + EditorMacroLocation(node.metadataLine, node.metadataColumn);
    if (depth == 0) result += ",\"effects\":" + EditorMacroEffects(id, actualExpression);
    if (node.controlTarget >= 0 && static_cast<std::size_t>(node.controlTarget) < g_nodes.size())
    {
        const auto &target = N(node.controlTarget);
        result += ",\"controlTarget\":" + EditorMacroLocation(target.sourceLine, target.sourceColumn);
    }
    if (node.macroExpansion >= 0 && static_cast<std::size_t>(node.macroExpansion) < g_nodes.size())
    {
        const auto &frame = N(node.macroExpansion);
        if (frame.i >= 0 && static_cast<std::size_t>(frame.i) < g_nodes.size())
        {
            const auto &definition = N(static_cast<int>(frame.i));
            result += ",\"definition\":" + EditorMacroLocation(definition.sourceLine, definition.sourceColumn);
        }
    }
    if (node.bindingIdentity)
    {
        auto found = symbols.find(node.bindingIdentity);
        const auto symbolName = node.kind == MNode::EnumCtor || node.kind == MNode::TypeQualifiedRef ? EditorMacroName(node.s2) : name;
        if (found == symbols.end()) found = symbols.emplace(node.bindingIdentity, symbolName + "@" + Memory::toString(symbols.size() + 1)).first;
        result += ",\"symbol\":" + chali_editor::jsonString(std::string_view(found->second.data(), found->second.size()));
        const bool declaration = node.kind == MNode::AssociatedDef || node.kind == MNode::AssociatedDecl || node.kind == MNode::Declare || node.kind == MNode::Params || node.kind == MNode::Field || node.kind == MNode::MethodDef || node.kind == MNode::FuncDef || node.kind == MNode::StructDecl || node.kind == MNode::EnumDecl || node.kind == MNode::InterfaceDecl;
        result += declaration ? ",\"declares\":true" : ",\"declares\":false";
    }
    if (node.s2 == "__chali_macro_value_cache" || node.s2 == "__chali_macro_const_ref" || node.s2 == "__chali_macro_mut_ref")
        result += ",\"parameterCache\":" + chali_editor::jsonString(node.s2 == "__chali_macro_const_ref" ? "borrow" : node.s2 == "__chali_macro_mut_ref" ? "borrow_mut" :
                                                                                                                                                           "value");
    else if (node.s2.rfind("__chali_metadata_member:", 0) == 0)
    {
        const auto owner = EditorMacroName(node.s2.substr(24));
        result += ",\"field\":{\"owner\":" + chali_editor::jsonString(std::string_view(owner.data(), owner.size())) + ",\"name\":" + chali_editor::jsonString(std::string_view(node.s.data(), node.s.size())) + "}";
    }
    else if (!node.s2.empty() && node.s2.rfind("__chali_", 0) != 0)
    {
        auto member = EditorMacroName(node.s2);
        if (node.kind == MNode::EnumCtor || node.kind == MNode::TypeQualifiedRef)
        {
            auto instance = chali_generic::session.made.find(node.s2);
            if (instance == chali_generic::session.made.end()) instance = chali_generic::session.made.find(node.s + "::" + node.s2);
            if (instance != chali_generic::session.made.end() && instance->second.generic->module == node.s)
                member = EditorGenericSymbol(instance->first, false);
        }
        result += ",\"member\":" + chali_editor::jsonString(std::string_view(member.data(), member.size()));
    }
    if (node.kind == MNode::Int) result += ",\"value\":" + std::to_string(node.i);
    if (node.type >= 0)
    {
        const auto type = EditorMacroName(EditorTypeText(T(node.type)));
        result += ",\"type\":" + chali_editor::jsonString(std::string_view(type.data(), type.size()));
    }
    else
    {
        const auto direct = g_editorMacroInferredTypes.find(id);
        const auto binding = g_editorMacroBindingTypes.find(node.bindingIdentity);
        if (direct != g_editorMacroInferredTypes.end() || binding != g_editorMacroBindingTypes.end())
        {
            const auto type = EditorMacroName(direct != g_editorMacroInferredTypes.end() ? direct->second : binding->second);
            result += ",\"type\":" + chali_editor::jsonString(std::string_view(type.data(), type.size()));
        }
    }
    result += ",\"children\":[";
    bool first = true;
    if (node.kind != MNode::MacroArgument)
    {
        auto children = node.kids;
        if (node.kind == MNode::Block || node.kind == MNode::Case) children.push_back(static_cast<int>(node.i));
        if (node.guard >= 0) children.push_back(node.guard);
        if (depth == 0 && actualExpression >= 0) children = { actualExpression };
        for (const int child : children)
        {
            if (!first) result += ',';
            first = false;
            result += EditorMacroTree(child, depth + 1, budget, symbols);
        }
    }
    return result + "]}";
}

/**
 * @brief 记录实际执行过的卫生模板实例化及其调用来源。
 * @param call 尚未被替换的调用节点。
 * @param root 实例化的模板根节点。
 */
void RecordEditorMacro(int call, int root)
{
    if (!chali_editor::compilerAnalysis.enabled()) return;
    const auto &node = N(call);
    const auto sourceLine = node.expansionLine ? node.expansionLine : node.sourceLine;
    const auto column = node.expansionLine ? node.expansionColumn : node.sourceColumn;
    const auto file = sourceLine && sourceLine <= g_sourceFiles.size() ? g_sourceFiles[sourceLine - 1] : g_sourceFallback;
    const auto line = sourceLine && sourceLine <= g_sourceLines.size() && g_sourceLines[sourceLine - 1] ? g_sourceLines[sourceLine - 1] : sourceLine;
    std::size_t budget = 10000;
    Memory::Map<std::uint32_t, Memory::String> symbols;
    const auto frame = N(root).macroExpansion;
    const auto complete = [root, frame, call] {
        if (root < 0 || static_cast<std::size_t>(root) >= g_nodes.size() || N(root).kind != MNode::MacroTemplate || N(root).macroExpansion != frame) return std::string{};
        std::size_t budget = 10000;
        Memory::Map<std::uint32_t, Memory::String> symbols;
        const int expression = call >= 0 && static_cast<std::size_t>(call) < g_nodes.size() && N(call).kind != MNode::MacroCall && N(call).kind != MNode::MacroTemplate ? call : -1;
        return EditorMacroTree(root, 0, budget, symbols, expression);
    };
    chali_editor::compilerAnalysis.addMacro({ std::string(file.data(), file.size()), std::string(node.s.data(), node.s.size()), EditorMacroTree(root, 0, budget, symbols), line, column, complete });
}

/**
 * @brief 从已登记类型与方法表导出宏生成的公开声明，不从模板文本猜测符号。
 * @return 无返回值；跳过卫生私有名及未确定类型，声明总预算为十六 MiB。
 */
void RecordEditorGeneratedDeclarations()
{
    if (!chali_editor::compilerAnalysis.enabled()) return;
    std::vector<chali_editor::GeneratedDeclaration> declarations;
    std::size_t bytes = 0;
    const auto visible = [](const Memory::String &name) {
        return !name.empty() && name.find("__chali_") == Memory::String::npos;
    };
    const auto text = [](const Memory::String &value) { return std::string(value.data(), value.size()); };
    const auto append = [&](const MNode &node, const Memory::String &name, const Memory::String &owner,
                            const char *kind, const Memory::String &type, const Memory::String &signature, bool isStatic) {
        if (!visible(name) || declarations.size() >= 100000 || !node.sourceLine) return;
        const auto file = node.sourceLine <= g_sourceFiles.size() ? g_sourceFiles[node.sourceLine - 1] : g_sourceFallback;
        if (file.empty() || file.front() == '<') return;
        const auto amount = file.size() + name.size() + owner.size() + type.size() + signature.size() + node.module.size() + 128u;
        if (amount > 16u * 1024u * 1024u - bytes) return;
        bytes += amount;
        chali_editor::GeneratedDeclaration declaration;
        declaration.file = text(file);
        declaration.name = text(BareTypeName(name));
        declaration.module = text(node.module);
        declaration.owner = text(BareTypeName(owner));
        declaration.kind = kind;
        declaration.type = text(type);
        declaration.signature = text(signature);
        declaration.line = node.sourceLine <= g_sourceLines.size() && g_sourceLines[node.sourceLine - 1]
                               ? g_sourceLines[node.sourceLine - 1] : node.sourceLine;
        declaration.column = node.sourceColumn;
        declaration.isStatic = isStatic;
        if (node.macroExpansion >= 0 && static_cast<std::size_t>(node.macroExpansion) < g_nodes.size())
            declaration.macro = text(N(node.macroExpansion).s);
        declarations.push_back(std::move(declaration));
    };
    for (const int id : g_top)
    {
        const auto &node = N(id);
        if (node.kind != MNode::StructDecl || !visible(node.s)) continue;
        const auto definition = g_structs.find(node.s);
        if (definition == g_structs.end() || g_opcodeTypes.count(node.s)) continue;
        const bool generated = node.generatedDeclaration || node.macroExpansion >= 0;
        if (generated) append(node, node.s, {}, "struct", {}, "struct " + BareTypeName(node.s), false);
        if (node.kids.empty()) continue;
        for (const int memberId : N(node.kids[0]).kids)
        {
            const auto &member = N(memberId);
            if ((!generated && !member.generatedDeclaration && member.macroExpansion < 0) || !visible(member.s)) continue;
            if (member.kind == MNode::Field && member.type2 != 1)
            {
                const auto field = std::find_if(definition->second.fields.begin(), definition->second.fields.end(),
                                                 [&](const MField &value) { return value.name == member.s; });
                if (field == definition->second.fields.end()) continue;
                const auto type = EditorTypeText(field->type);
                if (visible(type)) append(member, member.s, node.s, "member", type, type + " " + member.s, false);
            }
            else if (member.kind == MNode::MethodDef)
            {
                for (const auto &method : definition->second.methods)
                {
                    if (method.implicitDeclaration || method.superseded || method.source != member.s
                        || (member.kids.size() > 1u && method.bodyId != member.kids[1])) continue;
                    const auto result = EditorTypeText(method.result);
                    if (!visible(result)) continue;
                    Memory::String signature = method.isStatic ? "static " : "";
                    signature += result + " " + method.source + "(";
                    bool valid = true;
                    for (std::size_t index = 0; index < method.params.size(); ++index)
                    {
                        const auto &parameter = method.params[index];
                        const auto type = EditorTypeText(parameter.type);
                        if (!visible(type)) { valid = false; break; }
                        if (index) signature += ", ";
                        signature += type + (parameter.variadic ? "... " : " ") + EditorMacroName(parameter.source);
                    }
                    if (valid) append(member, method.source, node.s, "function", result, signature + ")", method.isStatic);
                }
            }
        }
    }
    chali_editor::compilerAnalysis.setGeneratedDeclarations(std::move(declarations));
}
