/**
 * @file derive_json.cpp
 * @brief 校验受限 JSON 派生及字段注解，供普通编解码方法生成器共用。
 */

/**
 * @brief 判断具体结构体或枚举是否请求 JSON 派生。
 * @param name 实际类型名。
 * @return 存在派生请求时返回真。
 */
bool HasJsonDerivation(const Memory::String &name)
{
    for (const auto id : g_top)
        if ((N(id).kind == MNode::StructDecl || N(id).kind == MNode::EnumDecl) && N(id).s == name)
            return std::find(N(id).derives.begin(), N(id).derives.end(), "Json") != N(id).derives.end();
    return false;
}

/**
 * @brief 读取标准 JSON 接口经模块消歧后的名字。
 * @return 未导入标准 JSON 模块时返回空串。
 */
Memory::String DerivedJsonInterfaceName()
{
    for (const auto id : g_top)
        if (N(id).kind == MNode::InterfaceDecl && N(id).module == "std::json" && BareTypeName(N(id).s) == "Json") return N(id).s;
    return {};
}

/** @brief 已校验的单个结构体字段 JSON 规则。 */
struct MDerivedJsonField
{
    Memory::String name;  ///< 编码键名，缺省为原始字段名。
    bool skip = false;    ///< 编解码均忽略该字段，解码保留默认构造值。
};

/**
 * @brief 提取已经通过普通注解检查的 JSON 字段规则。
 * @param field 字段声明。
 * @param rule 接收实际键名及忽略标记。
 * @return 注解唯一且名称可表示时返回真。
 */
bool DerivedJsonFieldRule(const MNode &field, MDerivedJsonField &rule)
{
    rule.name = field.s;
    bool found = false;
    for (const auto id : field.annotations)
    {
        const auto &use = N(id);
        if (use.type2 < 0 || AnnotationName(N(use.type2)) != "std::json::Field") continue;
        if (found) return AnnotationFail(use, "JSON 字段只能使用一次 std::json::Field");
        found = true;
        if (use.annotationValues.size() != 2) return AnnotationFail(use, "JSON 字段注解需要已校验的名称与忽略参数");
        if (!use.annotationValues[0].text.empty()) rule.name = use.annotationValues[0].text;
        rule.skip = use.annotationValues[1].integer != 0;
        for (const unsigned char byte : rule.name)
            if (byte < 32 || byte == 127) return AnnotationFail(use, "JSON 字段名称不能包含零字节或 ASCII 控制字节");
    }
    return true;
}

/**
 * @brief 在声明索引中查找实际字段规则，未派生类型保持既有字段名。
 * @param metadata 具体声明索引。
 * @param owner 实际结构体名。
 * @param field 原始字段名。
 * @return 派生注解规则；普通反射类型不采用派生专用注解。
 */
MDerivedJsonField DerivedJsonFieldFor(MReflectionBuilder &metadata, const Memory::String &owner, const Memory::String &field)
{
    MDerivedJsonField rule{ field, false };
    if (!HasJsonDerivation(owner)) return rule;
    const auto found = metadata.declarations.find(owner);
    if (found == metadata.declarations.end()) return rule;
    for (const auto id : N(found->second->kids[0]).kids)
        if (N(id).kind == MNode::Field && N(id).s == field)
        {
            DerivedJsonFieldRule(N(id), rule);
            break;
        }
    return rule;
}

/**
 * @brief 递归检查 JSON 派生支持的类型及拥有关系，循环容器按访问集终止。
 * @param type 具体字段类型。
 * @param visited 本轮已检查的类型身份。
 * @return 类型、所有嵌套字段和重命名均有效时返回真。
 */
bool ValidateDerivedJsonType(const std::shared_ptr<MType> &type, Memory::Set<Memory::String> &visited)
{
    if (!type) return Fail("#derive(Json) 需要完整具体类型");
    if (type->kind == MType::Int || type->kind == MType::Double || type->kind == MType::Bool || type->kind == MType::Char || IsStandardOpcode(type, "String")) return true;
    const auto key = TypeKey(type);
    if (!visited.insert(key).second) return true;
    if (IsContainer(type))
    {
        if (type->name == "map")
        {
            if (!IsStandardOpcode(type->elem, "String")) return Fail("#derive(Json) 的 Map 键必须是标准 String");
            return ValidateDerivedJsonType(type->elem2, visited);
        }
        return ValidateDerivedJsonType(type->elem, visited);
    }
    if (type->kind != MType::Struct || HasNonCopyableValue(type)) return Fail("#derive(Json) 不支持引用、函数、动态接口、数组或不可复制资源：" + Describe(type));
    const auto bare = BareTypeName(type->name);
    const bool builtin = bare.rfind("Option__", 0) == 0 || bare.rfind("Result__", 0) == 0;
    if (!builtin && !HasJsonDerivation(type->name)) return Fail("#derive(Json) 的嵌套类型必须派生 Json：" + Describe(type));
    if (const auto enumeration = g_enums.find(type->name); enumeration != g_enums.end())
    {
        for (const auto &variant : enumeration->second.variants)
            for (const auto &payload : variant.payloads)
                if (!ValidateDerivedJsonType(payload.type, visited)) return false;
        return true;
    }
    const auto definition = FindStruct(type->name);
    if (!definition) return Fail("#derive(Json) 找不到结构体布局：" + type->name);
    MReflectionBuilder metadata;
    metadata.indexDeclarations();
    Memory::Set<Memory::String> names;
    for (const auto &field : definition->fields)
    {
        if (!ValidateDerivedJsonType(field.type, visited)) return false;
        const auto rule = DerivedJsonFieldFor(metadata, type->name, field.name);
        if (!g_error.empty()) return false;
        if (!names.insert(rule.name).second) return Fail("#derive(Json) 字段重命名发生冲突：" + type->name + "." + rule.name);
    }
    return true;
}
