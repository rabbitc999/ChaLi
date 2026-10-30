/**
 * @brief 将字段访问宏转换为普通成员 AST，保留对象的借用与访问检查。
 * @param id 待处理的宏调用。
 * @return 非成员宏或转换成功时返回真。
 */
bool PrepareMacroMember(int id)
{
    const auto node = N(id);
    if (node.kind != MNode::MacroCall || node.s != "member") return true;
    if (node.kids.size() != 1 || node.kids[0] < 0 || N(node.kids[0]).kids.size() != 1)
        return Fail("$member 需要一组对象与字段描述符参数");
    const auto arguments = N(N(node.kids[0]).kids[0]).kids;
    if (arguments.size() != 2) return Fail("$member 需要对象与字段描述符两个参数");
    const auto expression = [&](int argument) {
        const auto value = N(argument);
        return value.kind == MNode::MacroArgument && value.kids.size() == chali_macro::FragmentAlternativeCount ? value.kids[0] : -1;
    };
    const int object = expression(arguments[0]), field = expression(arguments[1]);
    if (object < 0 || field < 0 || !NormalizeMacroMetadata(field, 0)) return Fail("$member 参数必须是对象表达式与字段描述符");
    const auto descriptor = N(field).compileMetadata;
    if (!descriptor || descriptor->kind != chali_macro::MetadataValue::Field) return Fail("$member 第二个参数必须是字段描述符");
    auto member = node;
    member.kind = MNode::Member;
    member.s = descriptor->owner->fields[descriptor->field].name;
    member.kids = { object };
    member.s2 = "__chali_metadata_member:" + descriptor->owner->type->name;
    g_nodes[id] = std::move(member);
    return true;
}

/**
 * @brief 按声明类型展开可证明的常量初始化式，保持窄整数语义并检测依赖环。
 * @param id 使用常量的节点。
 * @param initializer 常量初始化式。
 * @param declaredType 声明类型编号。
 * @param depth 当前语法深度。
 * @param active 当前常量依赖集合。
 * @param visit 继续归一化子树的回调。
 * @param error 接收循环依赖等错误。
 * @return 常量展开成功时返回真。
 */
bool SubstituteMacroConstant(int id, int initializer, int declaredType, std::size_t depth, Memory::Set<int> &active,
                             const std::function<bool(int, std::size_t, bool)> &visit, std::string &error)
{
    const auto source = N(id);
    if (!active.insert(initializer).second)
    {
        error = "编译期常量存在循环依赖：" + std::string(source.s.data(), source.s.size());
        return false;
    }
    g_nodes[id] = N(initializer);
    if (declaredType >= 0)
    {
        const auto type = T(declaredType);
        if (type->kind == MType::Int || type->kind == MType::Double || type->kind == MType::Bool || type->kind == MType::Char)
        {
            g_nodes[id].kind = MNode::Cast;
            g_nodes[id].type = declaredType;
            g_nodes[id].type2 = 0;
            g_nodes[id].kids = { initializer };
            g_nodes[id].s.clear();
            g_nodes[id].s2.clear();
        }
    }
    g_nodes[id].sourceLine = source.sourceLine;
    g_nodes[id].sourceColumn = source.sourceColumn;
    const bool success = visit(id, depth + 1, true);
    active.erase(initializer);
    return success;
}

/**
 * @brief 在卫生绑定前处理字段循环、常量分支和编译期诊断，仅构造普通 AST。
 * @param root 已替换泛型类型的独立模板。
 * @param inputs 只读宏实参，条件只允许使用其中的常量表达式。
 * @param error 接收失败原因。
 * @return 准备成功时返回真。
 */
bool PrepareMacroControls(int root, const std::map<std::string, int> &inputs, std::string &error)
{
    using Meta = chali_macro::MetadataValue;
    Memory::Map<Memory::String, Meta> environment;
    Memory::Map<Memory::String, int> localBindings;
    Memory::Vector<Meta> fieldPath;
    Memory::Set<int> statementLists;
    Memory::Set<int> activeConstants;
    std::size_t budget = 1000000;
    std::function<bool(int, std::size_t, bool)> visit;
    visit = [&](int id, std::size_t depth, bool constant) {
        if (id < 0) return true;
        if (depth >= 256 || !budget--) return Fail("宏编译期控制超过深度或规模预算");
        const auto node = N(id);
        if (node.kind == MNode::MacroTemplate && node.kids.size() == 2) statementLists.insert(node.kids[0]);
        if (node.kind == MNode::Block && node.i >= 0) statementLists.insert(static_cast<int>(node.i));
        if (node.kind == MNode::Case && !node.kids.empty()) statementLists.insert(node.kids[0]);
        if (IsMacroCompileDiagnostic(id))
            return PrepareMacroDiagnostic(id, [&](int child) { return visit(child, depth + 1, true); }, fieldPath, error);
        if (node.kind == MNode::MacroFor) return true;
        const bool localScope = node.kind == MNode::Block || node.kind == MNode::Case || node.kind == MNode::FuncDef || node.kind == MNode::MethodDef || node.kind == MNode::GenericLambda || node.kind == MNode::For || node.kind == MNode::Foreach;
        struct RestoreFieldEnvironment
        {
            Memory::Map<Memory::String, Meta> &current;
            Memory::Map<Memory::String, Meta> saved;
            Memory::Map<Memory::String, int> &locals;
            Memory::Map<Memory::String, int> savedLocals;
            bool active;
            /** @brief 退出普通词法作用域后恢复外层字段迭代绑定。 */
            ~RestoreFieldEnvironment()
            {
                if (active)
                {
                    current = std::move(saved);
                    locals = std::move(savedLocals);
                }
            }
        } restore{ environment, localScope ? environment : Memory::Map<Memory::String, Meta>{}, localBindings, localScope ? localBindings : Memory::Map<Memory::String, int>{}, localScope };
        if (node.kind == MNode::MacroCall && node.s == "count")
        {
            if (node.kids.size() != 1 || node.kids[0] < 0 || N(node.kids[0]).kids.size() != 1) return Fail("$count 需要一个变参来源");
            const auto group = N(N(node.kids[0]).kids[0]);
            if (group.kids.size() != 1) return Fail("$count 需要一个变参来源");
            const auto fragment = N(group.kids[0]);
            if (fragment.kind != MNode::MacroArgument || fragment.kids.empty() || fragment.kids[0] < 0) return Fail("$count 参数必须是变参形参名");
            const auto name = N(fragment.kids[0]);
            const auto source = inputs.find(std::string(name.s.data(), name.s.size()));
            if (name.kind != MNode::Var || source == inputs.end() || N(source->second).kind != MNode::MacroVariadic) return Fail("$count 参数必须是变参形参名");
            SetMacroMetadataInteger(id, N(source->second).kids.size());
            return true;
        }
        if (node.kind == MNode::Var && node.syntaxContext && environment.count(node.s))
        {
            SetMacroMetadata(id, environment.at(node.s));
            return true;
        }
        if (constant && node.kind == MNode::Var && node.syntaxContext && !node.bindingIdentity && localBindings.count(node.s))
        {
            const auto declaration = N(localBindings.at(node.s));
            if (declaration.kind != MNode::Declare || declaration.type2 != 1 || declaration.kids.empty() || declaration.kids[0] < 0)
            {
                error = "编译期条件不能读取生成代码中的运行时局部变量：" + std::string(node.s.data(), node.s.size());
                return false;
            }
            return SubstituteMacroConstant(id, declaration.kids[0], declaration.type, depth, activeConstants, visit, error);
        }
        if (constant && (node.kind == MNode::MacroHole || (node.kind == MNode::Var && node.syntaxContext && !node.bindingIdentity)))
        {
            const auto actual = inputs.find(std::string(node.s.data(), node.s.size()));
            if (actual != inputs.end())
            {
                const auto fragment = N(actual->second);
                if (fragment.kind != MNode::MacroArgument || fragment.kids.empty() || fragment.kids[0] < 0) return Fail("编译期条件的参数必须是常量表达式");
                g_nodes[id] = N(fragment.kids[0]);
                return visit(id, depth + 1, true);
            }
        }
        if (constant && node.kind == MNode::Var)
        {
            const auto binding = FindNodeVar(node);
            if (binding && binding->isConst && binding->constantExpression >= 0 && IsStandardOpcode(binding->type, "String"))
            {
                std::string text;
                if (!ReadMacroDiagnosticString(binding->constantExpression, text, error)) return false;
                SetMacroMetadataString(id, Memory::String(text.data(), text.size()));
                return true;
            }
        }
        if (constant && (node.kind == MNode::Var || node.kind == MNode::EnumCtor))
        {
            const auto binding = node.kind == MNode::Var ? FindNodeVar(node) : nullptr;
            if (!binding || (binding->isConst && binding->constantExpression < 0))
            {
                int declaredType = -1;
                const int initializer = FindMacroSyntaxConstant(node, error, &declaredType);
                if (!error.empty()) return false;
                if (initializer >= 0) return SubstituteMacroConstant(id, initializer, declaredType, depth, activeConstants, visit, error);
            }
        }
        if (node.kind == MNode::MacroFieldFor)
        {
            if (!visit(node.kids[0], depth + 1, true)) return false;
            const auto metadata = N(node.kids[0]).compileMetadata;
            if (!metadata || metadata->kind != Meta::Fields) return Fail("字段循环来源必须是 $fields<T>()");
            const auto saved = environment;
            const auto savedLocals = localBindings;
            const int list = M_List();
            for (std::size_t index = 0; index < metadata->owner->fields.size(); ++index)
            {
                Meta field = *metadata;
                field.kind = Meta::Field;
                field.field = index;
                environment = saved;
                localBindings = savedLocals;
                environment[node.s] = field;
                fieldPath.push_back(field);
                chali_macro::AstSubstitution copy(g_nodes, budget, &g_types);
                const auto before = g_nodes.size();
                const int body = copy.instantiate(node.kids[1], {}, node.syntaxContext, error);
                if (body < 0) return false;
                const auto cost = g_nodes.size() - before;
                if (cost >= budget) return Fail("字段循环生成节点超过规模预算");
                budget -= cost;
                if (!visit(body, depth + 1, false)) return false;
                fieldPath.pop_back();
                g_nodes[body].s2 = "__chali_field_iteration";
                M_ListAdd(list, body);
            }
            environment = saved;
            localBindings = savedLocals;
            g_nodes[id].kind = MNode::MacroTemplate;
            g_nodes[id].kids = { list, -1 };
            g_nodes[id].s.clear();
            return true;
        }
        if (node.kind == MNode::MacroCompileIf)
        {
            if (!visit(node.kids[0], depth + 1, true)) return false;
            MConstant value;
            if (!EvaluateConstant(node.kids[0], &value) || value.kind != MConstant::Boolean) return Fail("$if 条件必须是编译期 bool 常量");
            const int selected = node.kids[value.integer ? 1 : 2];
            if (selected < 0)
            {
                g_nodes[id].kind = MNode::MacroTemplate;
                g_nodes[id].kids = { M_List(), -1 };
                return true;
            }
            g_nodes[id] = N(selected);
            return visit(id, depth + 1, false);
        }
        if (node.kind == MNode::Case && node.i >= 0)
        {
            std::function<bool(int, bool, std::size_t)> mask;
            mask = [&](int current, bool payload, std::size_t level) {
                if (current < 0) return true;
                if (level >= 256 || !budget--) return Fail("字段模板模式遮蔽超过预算");
                const auto pattern = N(current);
                if (payload && pattern.kind == MNode::Var)
                {
                    environment.erase(pattern.s);
                    localBindings[pattern.s] = current;
                }
                if (pattern.kind == MNode::Call && pattern.kids.size() == 2) return mask(pattern.kids[1], true, level + 1);
                for (const int child : pattern.kids)
                    if (!mask(child, payload || pattern.kind == MNode::EnumCtor || pattern.kind == MNode::TypeQualifiedRef, level + 1)) return false;
                return true;
            };
            if (!mask(static_cast<int>(node.i), false, depth + 1) || !visit(static_cast<int>(node.i), depth + 1, constant)) return false;
        }
        if (node.kind == MNode::Declare && node.kids.size() == 1 && node.kids[0] >= 0 && N(node.kids[0]).kind == MNode::List)
            for (const int child : N(node.kids[0]).kids)
            {
                if (N(child).type < 0 && node.type >= 0) g_nodes[child].type = node.type;
                if (N(child).type2 < 0) g_nodes[child].type2 = node.type2;
            }
        if (node.kind == MNode::GenericLambda && !visit(node.type2, depth + 1, constant)) return false;
        for (std::size_t index = 0; index < node.kids.size(); ++index)
        {
            if (node.kind == MNode::Foreach && index == 1)
            {
                environment.erase(node.s);
                localBindings[node.s] = id;
            }
            if (!visit(node.kids[index], depth + 1, constant)) return false;
        }
        for (const int annotation : node.annotations)
            if (!visit(annotation, depth + 1, constant)) return false;
        if (!visit(node.guard, depth + 1, constant)) return false;
        const bool group = node.kind == MNode::Declare && node.kids.size() == 1 && node.kids[0] >= 0 && N(node.kids[0]).kind == MNode::List;
        const bool indexed = node.kind == MNode::Block || node.kind == MNode::NewPtr || ((node.kind == MNode::Declare || node.kind == MNode::Field) && !group && node.i >= 0);
        if (indexed && !visit(static_cast<int>(node.i), depth + 1, constant)) return false;
        if ((node.kind == MNode::Declare && !group) || node.kind == MNode::Params || node.kind == MNode::Foreach)
        {
            environment.erase(node.s);
            localBindings[node.s] = id;
        }
        if (node.kind == MNode::List && statementLists.count(id))
        {
            Memory::Vector<int> statements;
            for (const int child : N(id).kids)
            {
                if (child < 0)
                {
                    statements.push_back(child);
                    continue;
                }
                const auto statement = N(child);
                if (statement.s2 == "__chali_compile_assert_success") continue;
                if (statement.kind == MNode::ExprStmt && statement.kids.size() == 1 && N(statement.kids[0]).s2 == "__chali_compile_assert_success") continue;
                statements.push_back(child);
            }
            g_nodes[id].kids = std::move(statements);
        }
        if (!NormalizeMacroMetadata(id, 0) || !PrepareMacroMember(id)) return false;
        if (constant && node.kind == MNode::Bin && node.s == "+" && node.kids.size() == 2 && N(node.kids[0]).kind == MNode::Str && N(node.kids[1]).kind == MNode::Str)
        {
            std::string text;
            if (!ReadMacroDiagnosticString(id, text, error)) return false;
            SetMacroMetadataString(id, Memory::String(text.data(), text.size()));
        }
        if (constant && node.kind == MNode::Bin && (node.s == "==" || node.s == "!=") && node.kids.size() == 2 && N(node.kids[0]).kind == MNode::Str && N(node.kids[1]).kind == MNode::Str)
        {
            std::string left, right;
            const auto &first = N(node.kids[0]).s;
            const auto &second = N(node.kids[1]).s;
            if (!chali_literal::decodeString(std::string_view(first.data(), first.size()), left, error) || !chali_literal::decodeString(std::string_view(second.data(), second.size()), right, error)) return false;
            g_nodes[id].kind = MNode::Bool;
            g_nodes[id].i = (left == right) == (node.s == "==");
            g_nodes[id].kids.clear();
            g_nodes[id].s.clear();
        }
        if (N(id).kind == MNode::MacroArgument && !N(id).kids.empty() && N(id).kids[0] >= 0)
        {
            const auto descriptor = N(N(id).kids[0]).compileMetadata;
            if (descriptor && descriptor->kind == Meta::Type)
            {
                const int type = M_New(MNode::TupleTypeItem);
                g_nodes[type].type = M_AddType(Memory::makeShared<MType>(*descriptor->type));
                g_nodes[id].kids[2] = type;
                g_nodes[id].kids[0] = -1;
            }
        }
        return true;
    };
    if (visit(root, 0, false)) return true;
    if (error.empty()) error = "宏编译期字段或条件准备失败";
    return false;
}
