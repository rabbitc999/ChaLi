/**
 * @brief 将目标平台与构建标志宏替换为普通常量，供编译期分支及运行时表达式共用。
 * @param id 当前 AST 节点。
 * @param error 接收无效参数或下标的诊断。
 * @return 是环境内建宏时返回真，包括带诊断的调用。
 */
bool ExpandAstMacroEnvironment(int id, Memory::String &error)
{
    auto &node = g_nodes[id];
    const bool indexed = node.kind == MNode::Index && node.kids.size() == 2 && N(node.kids[0]).kind == MNode::MacroCall && N(node.kids[0]).s == "build";
    const int callId = indexed ? node.kids[0] : id;
    const auto call = N(callId);
    if (call.kind != MNode::MacroCall || (call.s != "system" && call.s != "cpu" && call.s != "build")) return false;
    if (call.kids.size() != 1 || (call.s != "build" && call.kids[0] >= 0))
    {
        error = "目标与构建标志内建宏不接受调用参数或泛型参数";
        return true;
    }
    if (call.s == "build")
    {
        int keyId = indexed ? node.kids[1] : -1;
        if (!indexed && call.kids[0] >= 0)
        {
            const auto groups = N(call.kids[0]);
            if (groups.kind == MNode::MacroArguments && groups.kids.size() == 1)
            {
                const auto group = N(groups.kids[0]);
                if (group.kind == MNode::MacroArgumentGroup && group.i == '[' && group.kids.size() == 1)
                {
                    const auto argument = N(group.kids[0]);
                    if (argument.kind == MNode::MacroArgument && !argument.kids.empty()) keyId = argument.kids[0];
                }
            }
        }
        if (keyId < 0) { error = "$build 需要单个字符串或名称下标，例如 $build[DEBUG]"; return true; }
        const auto key = N(keyId);
        Memory::String name;
        if (key.kind == MNode::Var) name = key.s;
        else if (key.kind == MNode::Str)
        {
            std::string decoded, problem;
            if (!chali_literal::decodeString(std::string_view(key.s.data(), key.s.size()), decoded, problem))
            { error = Memory::String(problem.c_str()); return true; }
            name = Memory::String(decoded.data(), decoded.size());
        }
        else { error = "$build 下标只接受字符串字面量或标志名称"; return true; }
        if (!chali_build::validBuildOption(name)) { error = "$build 标志名称无效，不支持赋值"; return true; }
        node.kind = MNode::Bool;
        node.i = chali_build::macroEnvironment().flags.count(name) != 0;
        node.s.clear();
    }
    else
    {
        node.kind = MNode::Str;
        node.s = chali_build::targetMacroName(call.s == "cpu");
    }
    node.kids.clear();
    return true;
}

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
 * @brief 为宏生成的枚举载荷绑定保留普通模式绑定的词法名称。
 * @param prefix 同一模式及其载荷读取共享的标识符。
 * @param index 载荷声明序号。
 * @return 随后仍由普通宏卫生规则解析的名称。
 */
Memory::String MacroVariantPayloadName(const Memory::String &prefix, std::size_t index)
{
    return "chali_macro_payload_" + prefix + "_" + Memory::toString(index);
}

/**
 * @brief 根据枚举元数据生成模式、载荷读取或逐载荷克隆的构造表达式。
 * @param id 待处理的内建宏调用。
 * @return 非枚举内建或转换成功时返回真。
 */
bool PrepareMacroVariant(int id)
{
    using Meta = chali_macro::MetadataValue;
    const auto node = N(id);
    if (node.kind != MNode::MacroCall) return true;
    const bool pattern = node.s == "variant_pattern";
    const bool construct = node.s == "variant_clone";
    const bool payload = node.s == "variant_payload";
    if (!(pattern || construct || payload)) return true;
    if (node.kids.size() != 1 || N(node.kids[0]).kids.size() != 1)
        return Fail("$" + node.s + " 需要一组枚举描述符与绑定名称");
    const auto arguments = N(N(node.kids[0]).kids[0]).kids;
    if (arguments.size() != 2) return Fail("$" + node.s + " 的参数数量不符");
    const auto descriptorArgument = N(arguments[0]);
    const auto prefixArgument = N(arguments[1]);
    if (descriptorArgument.kids.empty() || prefixArgument.kids.empty()
        || descriptorArgument.kids[0] < 0 || prefixArgument.kids[0] < 0)
        return Fail("$" + node.s + " 需要枚举描述符与绑定名称");
    const auto descriptor = N(descriptorArgument.kids[0]).compileMetadata;
    const auto prefix = N(prefixArgument.kids[0]);
    if (!descriptor || descriptor->kind != (pattern || construct ? Meta::Variant : Meta::Payload) || prefix.kind != MNode::Var)
        return Fail("$" + node.s + " 的描述符类型或绑定名称无效");
    auto expanded = node;
    expanded.kids.clear();
    if (pattern || construct)
    {
        const auto &variant = descriptor->owner->variants[descriptor->variant];
        expanded.kind = MNode::EnumCtor;
        expanded.s = descriptor->owner->type->name;
        expanded.s2 = variant.name;
        for (std::size_t index = 0; index < variant.payloads.size(); ++index)
        {
            const auto name = MacroVariantPayloadName(prefix.s, index);
            const int binding = M_Var(M_Name(name.c_str()));
            g_nodes[binding].module = node.module;
            g_nodes[binding].syntaxContext = node.syntaxContext;
            auto type = Memory::makeShared<MType>(*variant.payloads[index].type);
            if (construct && type->unresolvedName && !ResolveImportedTypeNames(false, type)) return false;
            const bool copy = type->kind == MType::Int || type->kind == MType::Double
                || type->kind == MType::Bool || type->kind == MType::Char
                || type->name == "std::core::String";
            expanded.kids.push_back(construct && !copy
                ? M_CallOn(M_Member(binding, M_Name("clone")), M_List())
                : binding);
        }
    }
    else
    {
        expanded.kind = MNode::Var;
        expanded.s = MacroVariantPayloadName(prefix.s, descriptor->payload);
        expanded.s2.clear();
    }
    g_nodes[id] = std::move(expanded);
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
 * @brief 判断类型模式是否含单个类型实参通配符。
 * @param type 模式节点。
 * @return 至少有一个通配符时为真。
 */
bool HasMacroTypeWildcard(const std::shared_ptr<MType> &type)
{
    return type && (type->name == "?" || HasMacroTypeWildcard(type->elem) || HasMacroTypeWildcard(type->elem2)
                    || std::any_of(type->genericArguments.begin(), type->genericArguments.end(), HasMacroTypeWildcard));
}

/**
 * @brief 比较已解析的具体类型与带可选通配符的模式。
 * @param actual 具体元数据类型，整数泛型实参为空。
 * @param pattern 已归一化普通名称的模式。
 * @param matches 接收比较结果。
 * @return 模式合法时为真。
 */
bool MatchMacroType(const std::shared_ptr<const MType> &actual, const std::shared_ptr<MType> &pattern, bool &matches)
{
    if (pattern->name == "?")
    {
        matches = static_cast<bool>(actual);
        return true;
    }
    if (!HasMacroTypeWildcard(pattern))
    {
        if (!ResolveAstGenericType(pattern)) return false;
        matches = false;
        if (!actual) return true;
        auto concrete = Memory::makeShared<MType>(*actual);
        if (concrete->unresolvedName && !ResolveImportedTypeNames(false, concrete)) return false;
        if (!ResolveAstGenericType(concrete)) return false;
        // 同一公开 opcode 在早期签名中可表现为句柄，在聚合登记后表现为 Struct。
        // 元数据比较使用声明身份，仍以完整名称隔离用户同名类型和内部 $opcode。
        const bool sameOpcode = concrete->name == pattern->name
            && ((concrete->kind == MType::Struct && pattern->kind == MType::Opcode)
                || (concrete->kind == MType::Opcode && pattern->kind == MType::Struct))
            && (concrete->unresolvedName || pattern->unresolvedName || g_opcodeTypes.count(concrete->name));
        matches = sameOpcode || TypeKey(concrete) == TypeKey(pattern);
        return true;
    }

    Memory::String family, expected;
    Memory::Vector<std::shared_ptr<MType>> patterns;
    if (IsContainer(pattern))
    {
        expected = MacroContainerFamily(*pattern);
        patterns.push_back(pattern->elem);
        if (pattern->elem2) patterns.push_back(pattern->elem2);
    }
    else if (!pattern->genericBase.empty())
    {
        const auto *definition = FindAstGenericTypeDefinition(*pattern, true);
        if (!definition) return false;
        expected = MacroGenericFamily(*definition);
        patterns = pattern->genericArguments;
        if (patterns.size() != definition->params.size()) return Fail("泛型类型模式的实参数量不符：" + expected);
        for (std::size_t index = 0; index < patterns.size(); ++index)
            if (definition->params[index].constant) return Fail("泛型类型模式需要类型实参：" + expected);
    }
    else
        return Fail("类型通配符 ? 需要直接用作泛型类型实参");

    Memory::Vector<std::shared_ptr<const MType>> arguments;
    if (actual)
    {
        const auto concrete = Memory::makeShared<MType>(*actual);
        if (!ReadMacroGenericType(concrete, family)) return false;
        if (family == expected && !ReadMacroGenericType(concrete, family, &arguments)) return false;
    }
    matches = family == expected && arguments.size() == patterns.size();
    for (std::size_t index = 0; index < patterns.size(); ++index)
    {
        bool argumentMatches = false;
        const auto argument = index < arguments.size() ? arguments[index] : nullptr;
        if (!MatchMacroType(argument, patterns[index], argumentMatches)) return false;
        matches = matches && argumentMatches;
    }
    return true;
}

/**
 * @brief 读取遵循普通 char 字面量规则的单个字节。
 * @param node 带引号的字符语法。
 * @param value 接收解码后的字节。
 * @return 字面量能表示一个 char 时返回真。
 */
bool ReadMacroLetChar(const MNode &node, unsigned char &value)
{
    const auto text = UnescapeText(node.s.substr(1, node.s.size() - 2));
    if (text.size() != 1) return Fail("$let 的 char 字面量必须是一个 UTF-8 字节");
    value = static_cast<unsigned char>(text[0]);
    return true;
}

/**

 * @brief 将宏绑定的标量初始化式求值并物化为带类型的字面量。
 * @param id 已归一化的初始化式节点。
 * @param declaredType 显式类型编号，负数表示推导。
 * @return 成功时返回字面量编号。
 */
int EvaluateMacroLet(int id, int declaredType)
{
    const auto type = declaredType >= 0 ? T(declaredType) : nullptr;
    const auto source = N(id);
    if (source.kind == MNode::Str)
    {
        if (!type || IsStandardOpcode(type, "String")) return id;
        auto resolved = Memory::makeShared<MType>(*type);
        if (resolved->unresolvedName && !ResolveImportedTypeNames(false, resolved)) return -1;
        if (resolved->name == "std::core::String") return id;
        Fail("$let 初始化式与显式类型不兼容");
        return -1;
    }
    if (source.kind == MNode::Char)
    {
        unsigned char value;
        if (type && type->kind != MType::Char)
        {
            Fail("$let 初始化式与显式类型不兼容");
            return -1;
        }
        return ReadMacroLetChar(source, value) ? id : -1;
    }
    MConstant value;
    if (!EvaluateConstant(id, &value)) return -1;
    int literalType = declaredType;
    if (type)
    {
        if (type->kind == MType::Int && value.kind == MConstant::Integer)
        {
            const unsigned bits = type->bits;
            const bool negative = !value.isUnsigned && value.integer < 0;
            const auto magnitude = static_cast<std::uint64_t>(value.integer);
            const auto max = (std::uint64_t(1) << (bits - 1)) - 1;
            const bool fits = type->isUnsigned ? !negative && (bits == 64 || magnitude <= (max << 1 | 1)) :
                              negative ? value.integer >= -static_cast<long long>(max) - 1 : magnitude <= max;
            if (!fits)
            {
                Fail("$let 整数初始化式超出显式类型范围");
                return -1;
            }
            value.isUnsigned = type->isUnsigned;
        }
        else if (type->kind == MType::Double && value.kind != MConstant::Boolean)
        {
            if (value.kind == MConstant::Integer)
                value.floating = value.isUnsigned ? double(std::uint64_t(value.integer)) : double(value.integer);
            value.kind = MConstant::Floating;
            if (type->bits == 32) value.floating = static_cast<float>(value.floating);
            if (!std::isfinite(value.floating))
            {
                Fail("$let 浮点初始化式超出显式类型范围");
                return -1;
            }
        }
        else if (type->kind != MType::Bool || value.kind != MConstant::Boolean)
        {
            Fail("$let 初始化式与显式类型不兼容");
            return -1;
        }
    }
    else if (value.kind == MConstant::Integer)
        literalType = M_TypeIntBits(value.bits, value.isUnsigned ? 1 : 0);
    else if (value.kind == MConstant::Floating)
        literalType = M_TypeFloatBits(value.bits);

    auto &literal = g_nodes[id];
    literal.kind = value.kind == MConstant::Integer ? MNode::Int :
                   value.kind == MConstant::Floating ? MNode::Float : MNode::Bool;
    literal.type = literalType;
    literal.i = value.integer;
    literal.d = value.floating;
    literal.unsignedLiteral = value.isUnsigned;
    literal.s.clear();
    literal.s2.clear();
    literal.kids.clear();
    literal.compileMetadata.reset();
    return id;
}

/**
 * @brief 在卫生绑定前处理字段循环、常量分支和编译期诊断，仅构造普通 AST。
 * @param root 已替换泛型类型的独立模板。
 * @param inputs 只读宏实参，条件只允许使用其中的常量表达式。
 * @param error 接收失败原因。
 * @param sourceLocals 普通源码条件所在作用域的声明；宏模板调用时为空。
 * @return 准备成功时返回真。
 */
bool PrepareMacroControlsWithLocals(int root, const std::map<std::string, int> &inputs, std::string &error,
                                   const Memory::Map<Memory::String, int> *sourceLocals)
{
    using Meta = chali_macro::MetadataValue;
    const auto templateContext = root >= 0 ? N(root).syntaxContext : 0;
    Memory::Map<Memory::String, Meta> environment;
    Memory::Map<Memory::String, int> localBindings = sourceLocals ? *sourceLocals : Memory::Map<Memory::String, int>{};
    Memory::Map<Memory::String, int> compileBindings;
    Memory::Vector<Meta> fieldPath;
    Memory::Set<int> statementLists;
    Memory::Set<int> activeConstants;
    std::size_t budget = 1000000;
    std::function<bool(int, std::size_t, bool)> visit;
    visit = [&](int id, std::size_t depth, bool constant) {
        if (id < 0) return true;
        if (depth >= 256 || !budget--) return Fail("宏编译期控制超过深度或规模预算");
        Memory::String environmentError;
        if (ExpandAstMacroEnvironment(id, environmentError)) return environmentError.empty() || Fail(environmentError);
        const auto node = N(id);
        if (node.kind == MNode::MacroTemplate && node.kids.size() == 2) statementLists.insert(node.kids[0]);
        if (node.kind == MNode::Block && node.i >= 0) statementLists.insert(static_cast<int>(node.i));
        if (node.kind == MNode::Case && !node.kids.empty()) statementLists.insert(node.kids[0]);
        if (IsMacroCompileDiagnostic(id))
            return PrepareMacroDiagnostic(id, [&](int child) { return visit(child, depth + 1, true); }, fieldPath, error);
        const auto inheritBindings = [&](int body, const Memory::Map<Memory::String, int> &bindings) {
            if (body < 0 || bindings.empty()) return;
            const int list = N(body).kids[0];
            Memory::Vector<int> children;
            for (const auto &[name, initializer] : bindings)
            {
                const int binding = M_New(MNode::MacroLet, name);
                g_nodes[binding].kids = { initializer };
                g_nodes[binding].syntaxContext = node.syntaxContext;
                children.push_back(binding);
            }
            children.insert(children.end(), N(list).kids.begin(), N(list).kids.end());
            g_nodes[list].kids = std::move(children);
        };
        if (node.kind == MNode::MacroFor)
        {
            if (compileBindings.empty()) return true;
            auto bindings = compileBindings;
            if (node.kids.size() == 3)
                for (int variable : N(node.kids[1]).kids) bindings.erase(N(variable).s);
            else
                bindings.erase(node.s);
            inheritBindings(node.kids[0], bindings);
            return true;
        }
        if ((node.kind == MNode::MacroMatch || node.kind == MNode::MacroRegex)
            && (!compileBindings.empty() || chali_macro::ContainsMacroLet(g_nodes, id)))
        {
            if (node.kind == MNode::MacroMatch)
                for (int branch : N(node.kids[0]).kids) inheritBindings(N(branch).kids[0], compileBindings);
            else
                for (int body : node.kids) inheritBindings(body, compileBindings);
            return true;
        }
        const bool compileScope = node.kind == MNode::MacroTemplate;
        if (node.kind == MNode::MacroLet)
        {
            if (!node.syntaxContext) return Fail("$let 只能在宏模板中声明");
            const int initializer = node.kids[0];
            if (!visit(initializer, depth + 1, true) || EvaluateMacroLet(initializer, node.type) < 0)
            {
                error = "$let 初始化式必须是可赋给声明类型的编译期常量：" + std::string(node.s.data(), node.s.size());
                return false;
            }
            compileBindings[node.s] = initializer;
            g_nodes[id].kind = MNode::MacroTemplate;
            g_nodes[id].kids = { M_List(), -1 };
            return true;
        }
        const bool localScope = node.kind == MNode::Block || node.kind == MNode::Case || node.kind == MNode::FuncDef || node.kind == MNode::MethodDef || node.kind == MNode::GenericLambda || node.kind == MNode::For || node.kind == MNode::Foreach;
        struct RestoreFieldEnvironment
        {
            Memory::Map<Memory::String, Meta> &current;
            Memory::Map<Memory::String, Meta> saved;
            Memory::Map<Memory::String, int> &locals;
            Memory::Map<Memory::String, int> savedLocals;
            Memory::Map<Memory::String, int> &compileBindings;
            Memory::Map<Memory::String, int> savedCompileBindings;
            bool active;
            bool restoreCompile;
            /** @brief 退出普通词法作用域后恢复外层字段迭代绑定。 */
            ~RestoreFieldEnvironment()
            {
                if (restoreCompile) compileBindings = std::move(savedCompileBindings);
                if (active)
                {
                    current = std::move(saved);
                    locals = std::move(savedLocals);
                }
            }
        } restore{ environment, localScope ? environment : Memory::Map<Memory::String, Meta>{}, localBindings, localScope ? localBindings : Memory::Map<Memory::String, int>{},
                   compileBindings, localScope || compileScope ? compileBindings : Memory::Map<Memory::String, int>{}, localScope, localScope || compileScope };
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
        if (node.kind == MNode::Var && node.syntaxContext == templateContext && !node.bindingIdentity && compileBindings.count(node.s))
        {
            g_nodes[id] = N(compileBindings.at(node.s));
            g_nodes[id].sourceLine = node.sourceLine;
            g_nodes[id].sourceColumn = node.sourceColumn;
            return true;
        }
        if (node.kind == MNode::Var && node.syntaxContext && environment.count(node.s))
        {
            SetMacroMetadata(id, environment.at(node.s));
            return true;
        }
        if (constant && node.kind == MNode::Var && (node.syntaxContext || sourceLocals) && !node.bindingIdentity && localBindings.count(node.s))
        {
            const auto declaration = N(localBindings.at(node.s));
            if (declaration.kind != MNode::Declare || declaration.type2 != 1 || declaration.kids.empty() || declaration.kids[0] < 0)
            {
                error = (sourceLocals ? "编译期条件不能读取运行时变量：" : "编译期条件不能读取生成代码中的运行时局部变量：")
                        + std::string(node.s.data(), node.s.size());
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
            std::size_t count = 0;
            Meta::Kind element;
            if (!metadata || !MacroMetadataSequence(*metadata, count, element)) return Fail("编译期循环来源必须是字段、变体、载荷、方法、参数、关联类型、接口或类型实参集合");
            const auto saved = environment;
            const auto savedLocals = localBindings;
            const auto savedCompileBindings = compileBindings;
            const int list = M_List();
            for (std::size_t index = 0; index < count; ++index)
            {
                Meta field = *metadata;
                field.kind = element;
                if (element == Meta::Field)
                    field.field = index;
                else if (element == Meta::Variant)
                    field.variant = index;
                else if (element == Meta::Payload)
                    field.payload = index;
                else if (element == Meta::Method)
                    field.method = index;
                else if (element == Meta::Parameter)
                    field.parameter = index;
                else if (element == Meta::Interface)
                    field.interfaceIndex = index;
                else if (element == Meta::AssociatedType)
                    field.field = index;
                else if (element == Meta::Type)
                    field.type = metadata->owner->typeArguments[index];
                environment = saved;
                localBindings = savedLocals;
                compileBindings = savedCompileBindings;
                compileBindings.erase(node.s);
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
            compileBindings = savedCompileBindings;
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
                if (N(current).kind == MNode::MacroCall && N(current).s == "variant_pattern"
                    && !visit(current, level + 1, constant)) return false;
                const auto pattern = N(current);
                if (payload && pattern.kind == MNode::Var)
                {
                    compileBindings.erase(pattern.s);
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
                compileBindings.erase(node.s);
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
            compileBindings.erase(node.s);
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
        if (!NormalizeMacroMetadata(id, 0) || !PrepareMacroMember(id) || !PrepareMacroVariant(id)) return false;
        if (constant && node.kind == MNode::Bin && (node.s == "==" || node.s == "!=") && node.kids.size() == 2)
        {
            const auto left = N(node.kids[0]).compileMetadata;
            const auto &right = N(node.kids[1]);
            if (right.kind == MNode::TupleTypeItem)
            {
                if (!left || left->kind != Meta::Type || !left->type)
                    return Fail("类型比较左侧需要编译期类型元数据");
                const auto type = T(right.type);
                bool matches = false;
                if (!ResolveImportedTypeNames(false, type) || !MatchMacroType(left->type, type, matches)) return false;
                g_nodes[id].kind = MNode::Bool;
                g_nodes[id].i = matches == (node.s == "==");
                g_nodes[id].kids.clear();
                g_nodes[id].s.clear();
            }
        }
        if (constant && node.kind == MNode::Bin && node.s == "+" && node.kids.size() == 2 && N(node.kids[0]).kind == MNode::Str && N(node.kids[1]).kind == MNode::Str)
        {
            std::string text;
            if (!ReadMacroDiagnosticString(id, text, error)) return false;
            SetMacroMetadataString(id, Memory::String(text.data(), text.size()));
        }
        if (constant && node.kind == MNode::Bin && (node.s == "==" || node.s == "!=") && node.kids.size() == 2
            && N(node.kids[0]).kind == MNode::Char && N(node.kids[1]).kind == MNode::Char)
        {
            unsigned char left, right;
            if (!ReadMacroLetChar(N(node.kids[0]), left) || !ReadMacroLetChar(N(node.kids[1]), right)) return false;
            auto &comparison = g_nodes[id];
            comparison.kind = MNode::Bool;
            comparison.i = (left == right) == (node.s == "==");
            comparison.kids.clear();
            comparison.s.clear();
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
    if (visit(root, 0, sourceLocals != nullptr)) return true;
    if (error.empty()) error = "宏编译期字段或条件准备失败";
    return false;
}


/**
 * @brief 在绑定宏模板前处理编译期控制。
 * @param root 已替换泛型类型的模板。
 * @param inputs 宏实参。
 * @param error 接收失败原因。
 * @return 成功时返回真。
 */
bool PrepareMacroControls(int root, const std::map<std::string, int> &inputs, std::string &error)
{
    return PrepareMacroControlsWithLocals(root, inputs, error, nullptr);
}
