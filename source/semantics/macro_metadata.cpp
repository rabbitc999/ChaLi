/**
 * @file macro_metadata.cpp
 * @brief 将具体字段及注解包装为编译期只读描述符，仅标量属性进入普通 AST。
 */
Memory::Map<std::uint64_t, Memory::String> g_macroMetadataIdentities;
Memory::Vector<Memory::String> g_macroMetadataDependencies;

/**
 * @brief 为编译期声明计算稳定身份，并检测当前编译中的键冲突。
 * @param key 不含机器地址的规范声明键。
 * @return 稳定身份；冲突时记录编译错误并返回零。
 */
std::uint64_t MacroMetadataIdentity(const Memory::String &key)
{
    const auto identity = mr::stableId(key);
    const auto previous = g_macroMetadataIdentities.find(identity);
    if (previous != g_macroMetadataIdentities.end() && previous->second != key)
        return Fail("编译期元数据身份冲突：" + key), 0;
    g_macroMetadataIdentities.emplace(identity, key);
    return identity;
}

/**
 * @brief 从具体结构体声明读取有序字段及精确注解实参，不依赖布局登记顺序。
 * @param source 查询的已解析或待实例化类型。
 * @return 不可变字段快照；非法类型或实例化失败时为空。
 */
std::shared_ptr<const chali_macro::MetadataFields> ReadMacroFields(const std::shared_ptr<const MType> &source)
{
    if (!source) return Fail("字段元数据需要具体类型"), nullptr;
    auto type = Memory::makeShared<MType>(*source);
    if (!ResolveAstGenericType(type)) return nullptr;
    if (std::find(g_macroMetadataDependencies.begin(), g_macroMetadataDependencies.end(), type->name) != g_macroMetadataDependencies.end())
    {
        Memory::String chain;
        for (const auto &name : g_macroMetadataDependencies) chain += name + " -> ";
        return Fail("字段元数据存在循环依赖：" + chain + type->name), nullptr;
    }
    g_macroMetadataDependencies.push_back(type->name);
    struct RestoreMetadataDependency
    {
        /**
         * @brief 恢复元数据依赖栈，实例化重放或错误退出也不保留半成品。
         */
        ~RestoreMetadataDependency() { g_macroMetadataDependencies.pop_back(); }
    } restore;

    MReflectionBuilder names;
    names.indexDeclarations();
    auto result = Memory::makeShared<chali_macro::MetadataFields>();
    result->name = names.named(type->name);
    result->identity = MacroMetadataIdentity("macro-type:" + names.key(type));
    result->type = type;
    const MNode *declaration = nullptr;
    for (const int root : g_top)
        if (N(root).kind == MNode::StructDecl && N(root).s == type->name)
        {
            if (!ExpandAstMacroMembers(root)) return nullptr;
            declaration = &N(root);
            break;
        }
    if (!declaration || type->kind != MType::Struct)
        return Fail("字段元数据当前只支持已实例化的普通结构体：" + Describe(type)), nullptr;
    for (const int fieldNode : N(declaration->kids[0]).kids)
    {
        const auto &field = N(fieldNode);
        if (field.kind != MNode::Field || field.type2 == 1) continue;
        std::shared_ptr<MType> fieldType;
        const auto registered = g_structs.find(type->name);
        if (registered != g_structs.end())
            for (const auto &actual : registered->second.fields)
                if (actual.name == field.s)
                {
                    fieldType = actual.type;
                    break;
                }
        // 完成登记后复用声明处已绑定的数组维度，不能被查询处的局部同名变量改变。
        if (!fieldType) fieldType = DeclaredFieldType(field);
        if (!fieldType) return nullptr;
        chali_macro::MetadataField item;
        item.name = field.s;
        item.index = result->fields.size();
        item.type = fieldType;
        item.identity = MacroMetadataIdentity("macro-field:" + names.key(type) + "::" + field.s);
        for (const int member : N(declaration->kids[0]).kids)
        {
            const auto &syntax = N(member);
            if (syntax.kind != MNode::Field || syntax.type2 == 1 || syntax.s != field.s) continue;
            for (const int useId : syntax.annotations)
            {
                const auto &use = N(useId);
                if (use.type2 != -2 && (use.type2 < 0 || static_cast<std::size_t>(use.type2) >= g_nodes.size() || N(use.type2).kind != MNode::AnnotationDecl))
                    return Fail("编译期注解元数据尚未完成语义检查：" + field.s), nullptr;

                chali_macro::MetadataAttribute attribute;
                attribute.name = use.type2 == -2 ? Memory::String("std::reflect::Reflect") : AnnotationName(N(use.type2));
                attribute.identity = MacroMetadataIdentity("macro-annotation:" + attribute.name);
                for (std::size_t index = 0; index < use.annotationValues.size(); ++index)
                {
                    const auto &value = use.annotationValues[index];
                    chali_macro::MetadataArgument argument;
                    argument.name = N(N(N(use.type2).kids[0]).kids[index]).s;
                    argument.type = T(value.type);
                    argument.integer = value.integer;
                    argument.floating = value.floating;
                    argument.text = value.text;
                    attribute.arguments.push_back(std::move(argument));
                }
                item.attributes.push_back(std::move(attribute));
            }
        }
        result->fields.push_back(std::move(item));
    }
    return g_error.empty() ? result : nullptr;
}

/**
 * @brief 识别字段查询内建名称，宏入口本身不需要导入运行期反射模块。
 * @param id 当前节点。
 * @return 节点是字段元数据入口时返回真。
 */
bool IsMacroMetadataSource(int id)
{
    return id >= 0 && N(id).kind == MNode::MacroCall && (N(id).s == "fields" || N(id).s == "std::lang::fields");
}

/**
 * @brief 将语法节点替换为只供编译期操作的有类型描述符，保留来源链。
 * @param id 待替换节点。
 * @param value 已校验的不可变元数据视图。
 * @return 无。
 */
void SetMacroMetadata(int id, chali_macro::MetadataValue value)
{
    auto &node = g_nodes[id];
    node.kind = MNode::MacroMetadata;
    node.compileMetadata = Memory::makeShared<chali_macro::MetadataValue>(std::move(value));
    node.kids.clear();
    node.s.clear();
    node.s2.clear();
    node.type = -1;
}

/**
 * @brief 将元数据整数属性直接写为精确的整数常量。
 * @param id 待替换节点。
 * @param value 常量位模式。
 * @param unsignedValue 是否保留 uint64 身份。
 * @return 无。
 */
void SetMacroMetadataInteger(int id, std::uint64_t value, bool unsignedValue = false)
{
    auto &node = g_nodes[id];
    node.kind = MNode::Int;
    node.i = ConstantBits(value);
    node.unsignedLiteral = unsignedValue;
    node.type = M_TypeIntBits(64, unsignedValue ? 1 : 0);
    node.s.clear();
    node.s2.clear();
    node.kids.clear();
    node.compileMetadata.reset();
}

/**
 * @brief 将已解码的名称写为普通字符串字面量，控制字符只转义一次。
 * @param id 待替换节点。
 * @param value 原始 UTF-8 字节。
 * @return 无。
 */
void SetMacroMetadataString(int id, const Memory::String &value)
{
    auto &node = g_nodes[id];
    const auto quoted = MReflectionQueries::quote(value);
    node.kind = MNode::Str;
    node.s = quoted.substr(1, quoted.size() - 2);
    node.s2.clear();
    node.type = -1;
    node.kids.clear();
    node.compileMetadata.reset();
}

/**
 * @brief 在受限常量索引中先归一化嵌套元数据属性，不执行运行期表达式。
 * @param id 索引表达式。
 * @param depth 当前递归深度。
 * @return 元数据与常量子表达式均合法时返回真。
 */
bool NormalizeMacroMetadataIndex(int id, std::size_t depth)
{
    if (id < 0 || depth >= 256) return Fail("编译期元数据索引超过深度预算");
    const auto children = N(id).kids;
    for (const int child : children)
        if (!NormalizeMacroMetadataIndex(child, depth + 1)) return false;
    return NormalizeMacroMetadata(id, depth);
}

/**
 * @brief 折叠字段集合、字段、类型、注解与注解实参的类型化查询链。
 * @param id 当前语法节点。
 * @param depth 元数据查询链深度。
 * @return 查询合法或当前节点不是元数据操作时返回真。
 */
bool NormalizeMacroMetadata(int id, std::size_t depth)
{
    if (id < 0) return true;
    if (depth >= 256) return Fail("编译期元数据查询超过深度预算");
    const auto original = N(id);
    if (IsMacroMetadataSource(id))
    {
        if (original.kids.size() != 2 || N(original.kids[1]).kids.size() != 1)
            return Fail("$fields<T>() 需要一个显式类型实参");
        if (original.kids[0] < 0 || N(original.kids[0]).kids.size() != 1 || !N(N(original.kids[0]).kids[0]).kids.empty())
            return Fail("$fields<T>() 不接受值实参");
        const auto type = T(N(N(original.kids[1]).kids[0]).type);
        auto fields = ReadMacroFields(type);
        if (!fields) return false;
        chali_macro::MetadataValue value;
        value.kind = chali_macro::MetadataValue::Fields;
        value.owner = fields;
        SetMacroMetadata(id, std::move(value));
        return true;
    }
    if ((original.kind != MNode::Member && original.kind != MNode::Index) || original.kids.empty()) return true;
    if (!NormalizeMacroMetadata(original.kids[0], depth + 1)) return false;
    const auto &parent = N(original.kids[0]);
    if (parent.kind != MNode::MacroMetadata) return true;
    if (!parent.compileMetadata) return Fail("编译期元数据描述符缺少类型信息");
    auto value = *parent.compileMetadata;
    using Meta = chali_macro::MetadataValue;
    const auto &fields = value.owner->fields;
    if (original.kind == MNode::Index)
    {
        if (original.kids.size() != 2 || !NormalizeMacroMetadataIndex(original.kids[1], depth + 1)) return false;
        MConstant index;
        if (!EvaluateConstant(original.kids[1], &index)) return false;
        if (index.kind != MConstant::Integer || index.integer < 0) return Fail("编译期元数据索引必须为非负整数常量");
        const auto selected = static_cast<std::uint64_t>(index.integer);
        if (value.kind == Meta::Fields)
        {
            if (selected >= fields.size()) return Fail("编译期字段索引越界");
            value.kind = Meta::Field;
            value.field = static_cast<std::size_t>(selected);
        }
        else if (value.kind == Meta::Attributes)
        {
            if (selected >= fields[value.field].attributes.size()) return Fail("编译期注解索引越界");
            value.kind = Meta::Attribute;
            value.attribute = static_cast<std::size_t>(selected);
        }
        else if (value.kind == Meta::Arguments)
        {
            if (selected >= fields[value.field].attributes[value.attribute].arguments.size()) return Fail("编译期注解实参索引越界");
            value.kind = Meta::Argument;
            value.argument = static_cast<std::size_t>(selected);
        }
        else
            return Fail("该编译期元数据类别不支持下标");
        SetMacroMetadata(id, std::move(value));
        return true;
    }
    const auto &property = original.s;
    if (value.kind == Meta::Fields)
    {
        if (property == "count")
            SetMacroMetadataInteger(id, fields.size());
        else if (property == "name")
            SetMacroMetadataString(id, value.owner->name);
        else if (property == "id")
            SetMacroMetadataInteger(id, value.owner->identity, true);
        else
            return Fail("字段集合没有此编译期属性：" + property);
        return true;
    }
    if (value.kind == Meta::Field)
    {
        const auto &field = fields[value.field];
        if (property == "name")
            SetMacroMetadataString(id, field.name);
        else if (property == "index")
            SetMacroMetadataInteger(id, field.index);
        else if (property == "id")
            SetMacroMetadataInteger(id, field.identity, true);
        else if (property == "type")
        {
            value.kind = Meta::Type;
            value.type = field.type;
            SetMacroMetadata(id, std::move(value));
        }
        else if (property == "attributes")
        {
            value.kind = Meta::Attributes;
            SetMacroMetadata(id, std::move(value));
        }
        else
            return Fail("字段没有此编译期属性：" + property);
        return true;
    }
    if (value.kind == Meta::Type)
    {
        MReflectionBuilder names;
        names.indexDeclarations();
        auto type = Memory::makeShared<MType>(*value.type);
        if (property == "name")
            SetMacroMetadataString(id, EditorTypeText(type));
        else if (property == "id")
            SetMacroMetadataInteger(id, MacroMetadataIdentity("macro-type:" + names.key(type)), true);
        else if (property == "fields")
        {
            value.owner = ReadMacroFields(value.type);
            if (!value.owner) return false;
            value.kind = Meta::Fields;
            SetMacroMetadata(id, std::move(value));
        }
        else
            return Fail("类型没有此编译期属性：" + property);
        return true;
    }
    const auto &attributes = fields[value.field].attributes;
    if (value.kind == Meta::Attributes)
    {
        if (property != "count") return Fail("注解集合只有 count 编译期属性");
        SetMacroMetadataInteger(id, attributes.size());
        return true;
    }
    const auto &attribute = attributes[value.attribute];
    if (value.kind == Meta::Attribute)
    {
        if (property == "name")
            SetMacroMetadataString(id, attribute.name);
        else if (property == "id")
            SetMacroMetadataInteger(id, attribute.identity, true);
        else if (property == "arguments")
        {
            value.kind = Meta::Arguments;
            SetMacroMetadata(id, std::move(value));
        }
        else
            return Fail("注解没有此编译期属性：" + property);
        return true;
    }
    if (value.kind == Meta::Arguments)
    {
        if (property != "count") return Fail("注解实参集合只有 count 编译期属性");
        SetMacroMetadataInteger(id, attribute.arguments.size());
        return true;
    }
    const auto &argument = attribute.arguments[value.argument];
    if (property == "name")
        SetMacroMetadataString(id, argument.name);
    else if (property == "type")
    {
        value.kind = Meta::Type;
        value.type = argument.type;
        SetMacroMetadata(id, std::move(value));
    }
    else if (property == "value")
    {
        if (IsStandardOpcode(Memory::makeShared<MType>(*argument.type), "String"))
            SetMacroMetadataString(id, argument.text);
        else
        {
            SetMacroMetadataInteger(id, static_cast<std::uint64_t>(argument.integer));
            auto &node = g_nodes[id];
            node.type = M_AddType(Memory::makeShared<MType>(*argument.type));
            node.unsignedLiteral = argument.type->kind == MType::Int && argument.type->isUnsigned;
            if (argument.type->kind == MType::Double)
            {
                node.kind = MNode::Float;
                node.d = argument.floating;
            }
            else if (argument.type->kind == MType::Bool)
                node.kind = MNode::Bool;
        }
    }
    else
        return Fail("注解实参没有此编译期属性：" + property);
    return true;
}
