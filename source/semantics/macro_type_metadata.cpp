/**
 * @file macro_type_metadata.cpp
 * @brief 读取枚举、方法及接口的编译期声明快照，不引入运行期元数据。
 */
/**
 * @brief 从枚举声明建立不可变的变体及载荷快照，泛型在查询时归一化。
 * @param source 查询的具体枚举类型。
 * @return 成功时返回声明快照，类型或载荷不合法时为空。
 */
std::shared_ptr<const chali_macro::MetadataType> ReadMacroVariants(const std::shared_ptr<const MType> &source)
{
    if (!source) return Fail("变体元数据需要具体枚举类型"), nullptr;
    auto type = Memory::makeShared<MType>(*source);
    if (!ResolveAstGenericType(type)) return nullptr;
    MReflectionBuilder names;
    names.indexDeclarations();
    auto result = Memory::makeShared<chali_macro::MetadataType>();
    result->name = names.named(type->name);
    result->type = type;
    result->identity = MacroMetadataIdentity("macro-type:" + names.key(type));
    for (const int root : g_top)
    {
        const auto declaration = N(root);
        if (declaration.kind != MNode::EnumDecl || declaration.s != type->name) continue;
        const auto variantIds = N(declaration.kids[0]).kids;
        for (const int variantId : variantIds)
        {
            const auto syntax = N(variantId);
            chali_macro::MetadataVariant variant;
            variant.name = syntax.s;
            variant.index = result->variants.size();
            const auto key = "macro-variant:" + names.key(type) + "::" + variant.name;
            variant.identity = MacroMetadataIdentity(key);
            if (!syntax.kids.empty())
            {
                const auto parameterIds = N(syntax.kids[0]).kids;
                for (const int parameterId : parameterIds)
                {
                    const auto parameter = N(parameterId);
                    auto payloadType = Memory::makeShared<MType>(*T(parameter.type));
                    if (!ResolveAstGenericType(payloadType)) return nullptr;
                    chali_macro::MetadataField payload;
                    payload.name = parameter.s;
                    payload.index = variant.payloads.size();
                    payload.identity = MacroMetadataIdentity(key + "::payload:" + payload.name);
                    payload.type = payloadType;
                    variant.payloads.push_back(std::move(payload));
                }
            }
            result->variants.push_back(std::move(variant));
        }
        return g_error.empty() ? result : nullptr;
    }
    return Fail("变体元数据只支持已实例化的枚举：" + Describe(type)), nullptr;
}

/**
 * @brief 查找具体类型的声明，并在读取前完成已有成员宏展开。
 * @param name 已归一化的类型名称。
 * @return 声明节点编号；未找到或展开失败时为负。
 */
int MacroMetadataDeclaration(const Memory::String &name)
{
    const auto roots = g_top;
    for (const int root : roots)
    {
        const auto syntax = N(root);
        if (syntax.s != name || (syntax.kind != MNode::StructDecl && syntax.kind != MNode::EnumDecl && syntax.kind != MNode::InterfaceDecl && syntax.kind != MNode::OpcodeDecl)) continue;
        if (!ExpandAstMacroMembers(root)) return -1;
        return root;
    }
    return -1;
}

/**
 * @brief 从参数签名构建与源声明绑定的只读方法描述符。
 * @param owner 查询对象的快照。
 * @param declaringType 方法原始声明类型。
 * @param name 源方法名称。
 * @param result 返回类型。
 * @param parameters 已解析的普通参数列表。
 * @param isStatic 方法是否为类型级函数。
 * @param constructor 方法是否为构造入口。
 * @param body 方法是否具有实现体。
 * @return 签名有效并追加成功时返回真。
 */
bool AppendMacroMethod(chali_macro::MetadataType &owner, const Memory::String &declaringType, const Memory::String &name,
                       const std::shared_ptr<MType> &result, const Memory::Vector<MParam> &parameters, bool isStatic, bool constructor, bool body)
{
    if (!result) return Fail("方法元数据缺少返回类型：" + name);
    MReflectionBuilder names;
    names.indexDeclarations();
    chali_macro::MetadataMethod method;
    method.name = name;
    method.result = result;
    method.index = owner.methods.size();
    method.declaringType = names.named(declaringType);
    method.inherited = declaringType != owner.type->name;
    method.isStatic = isStatic;
    method.isConstructor = constructor;
    method.hasBody = body;
    Memory::String signature = "macro-method:" + names.key(NamedType(declaringType)) + "::" + name + (isStatic ? ":static(" : ":instance(");
    for (const auto &parameter : parameters)
    {
        if (!parameter.type) return Fail("方法元数据缺少参数类型：" + name);
        chali_macro::MetadataField item;
        item.name = parameter.source;
        item.type = parameter.type;
        item.index = method.parameters.size();
        item.variadic = parameter.variadic;
        item.hasDefault = parameter.defaultExpr >= 0;
        signature += names.key(parameter.type) + (parameter.variadic ? "...;" : ";");
        method.parameters.push_back(std::move(item));
    }
    signature += ")->" + names.key(result);
    method.identity = MacroMetadataIdentity(signature);
    for (auto &parameter : method.parameters)
        parameter.identity = MacroMetadataIdentity(signature + ":parameter:" + Memory::toString(parameter.index));
    owner.methods.push_back(std::move(method));
    return g_error.empty();
}

/**
 * @brief 读取接口关系和方法签名，优先复用已登记语义，早期查询回到声明图。
 * @param source 查询的具体类型。
 * @param methods 是否读取方法集合；为假时只读取接口关系。
 * @return 不可变类型快照，非法类型或循环依赖时为空。
 */
std::shared_ptr<const chali_macro::MetadataType> ReadMacroTypeMembers(const std::shared_ptr<const MType> &source, bool methods)
{
    if (!source) return Fail("类型元数据需要具体类型"), nullptr;
    auto type = Memory::makeShared<MType>(*source);
    if (!ResolveAstGenericType(type)) return nullptr;
    if (std::find(g_macroMetadataDependencies.begin(), g_macroMetadataDependencies.end(), type->name) != g_macroMetadataDependencies.end())
        return Fail("类型成员元数据存在循环依赖：" + type->name), nullptr;
    g_macroMetadataDependencies.push_back(type->name);
    struct RestoreTypeMetadataDependency
    {
        /**
         * @brief 在所有返回路径恢复当前查询的类型依赖栈。
         */
        ~RestoreTypeMetadataDependency() { g_macroMetadataDependencies.pop_back(); }
    } restore;
    const int declarationId = MacroMetadataDeclaration(type->name);
    if (declarationId < 0)
        return g_error.empty() ? (Fail("类型成员元数据需要已实例化的结构体、枚举或接口：" + Describe(type)), nullptr) : nullptr;
    const auto declaration = N(declarationId);
    MReflectionBuilder names;
    names.indexDeclarations();
    auto result = Memory::makeShared<chali_macro::MetadataType>();
    result->name = names.named(type->name);
    result->type = type;
    result->identity = MacroMetadataIdentity("macro-type:" + names.key(type));
    Memory::Set<Memory::String> seen;
    Memory::Set<Memory::String> active;
    std::function<bool(const Memory::String &, bool, std::size_t)> addInterface;
    addInterface = [&](const Memory::String &name, bool direct, std::size_t depth) {
        if (depth >= 256 || active.count(name)) return Fail("接口元数据继承存在循环或超过深度预算：" + name);
        if (seen.count(name))
        {
            if (direct)
                for (auto &item : result->interfaces)
                    if (item.type->name == name) item.direct = true;
            return true;
        }
        const int id = MacroMetadataDeclaration(name);
        if (id < 0 || N(id).kind != MNode::InterfaceDecl) return Fail("接口元数据来源不是接口：" + name);
        const auto syntax = N(id);
        names.indexDeclarations();
        seen.insert(name);
        active.insert(name);
        chali_macro::MetadataInterface item;
        const auto interfaceType = NamedType(name);
        item.type = interfaceType;
        item.identity = MacroMetadataIdentity("macro-type:" + names.key(interfaceType));
        item.index = result->interfaces.size();
        item.direct = direct;
        result->interfaces.push_back(std::move(item));
        if (syntax.i >= 0)
        {
            const auto bases = N(static_cast<int>(syntax.i)).kids;
            for (const int base : bases)
            {
                const auto baseName = g_names[static_cast<std::size_t>(base)];
                if (!addInterface(baseName, false, depth + 1)) return false;
            }
        }
        active.erase(name);
        return true;
    };
    if (declaration.i >= 0)
    {
        const auto bases = N(static_cast<int>(declaration.i)).kids;
        for (const int base : bases)
        {
            const auto baseName = g_names[static_cast<std::size_t>(base)];
            if (!addInterface(baseName, true, 0)) return nullptr;
        }
    }
    if (!methods) return g_error.empty() ? result : nullptr;

    const auto interfaceMethods = g_interfaces.find(type->name);
    if (interfaceMethods != g_interfaces.end())
    {
        for (const auto &method : interfaceMethods->second)
            if (!AppendMacroMethod(*result, method.owner, method.name, method.result, method.bodyParams,
                                   method.isStatic, false, method.bodyId >= 0)) return nullptr;
        return result;
    }
    const auto structure = g_structs.find(type->name);
    if (structure != g_structs.end() && !structure->second.methods.empty())
    {
        for (const auto &method : structure->second.methods)
            if (!method.superseded && !method.implicitDeclaration && !AppendMacroMethod(*result, method.declarationOwner.empty() ? type->name : method.declarationOwner, method.source == "__ctor" ? result->name : method.source, method.result, method.params, method.isStatic, method.isConstructor, method.bodyId >= 0)) return nullptr;
        return result;
    }
    // 声明展开期尚未建立方法表，仅读取已解析的声明；不执行方法体。
    const auto readDeclared = [&](int id, const Memory::String &owner) {
        const auto syntax = N(id);
        if (syntax.kind != MNode::MethodDef) return true;
        Memory::Vector<MParam> parameters;
        for (const int parameterId : N(syntax.kids[0]).kids)
        {
            const auto parameter = N(parameterId);
            parameters.push_back(MParam{ parameter.s, Memory::String(), T(parameter.type), parameter.i != 0,
                                         parameter.kids.empty() ? -1 : parameter.kids[0] });
        }
        return AppendMacroMethod(*result, owner, syntax.s, T(syntax.type), parameters, syntax.type2 == 1,
                                 syntax.type2 == 2 || (syntax.type2 == 1 && syntax.s == "create"), syntax.kids.size() > 1 && syntax.kids[1] >= 0);
    };
    const std::size_t slot = declaration.kind == MNode::EnumDecl || declaration.kind == MNode::OpcodeDecl ? 1 : 0;
    if (slot < declaration.kids.size())
        for (const int member : N(declaration.kids[slot]).kids)
            if (!readDeclared(member, type->name)) return nullptr;
    if (!result->interfaces.empty())
    {
        for (const auto &base : result->interfaces)
        {
            const int id = MacroMetadataDeclaration(base.type->name);
            if (id < 0) return nullptr;
            const auto members = N(N(id).kids[0]).kids;
            for (const int member : members)
            {
                if (declaration.kind != MNode::InterfaceDecl && N(member).kind == MNode::MethodDef
                    && (N(member).kids.size() < 2 || N(member).kids[1] < 0)) continue;
                const auto before = result->methods.size();
                if (!readDeclared(member, base.type->name)) return nullptr;
                if (result->methods.size() == before) continue;
                const auto &candidate = result->methods.back();
                for (std::size_t index = 0; index + 1 < result->methods.size(); ++index)
                {
                    const auto &existing = result->methods[index];
                    if (existing.name != candidate.name || existing.isStatic != candidate.isStatic
                        || existing.parameters.size() != candidate.parameters.size()) continue;
                    bool same = true;
                    for (std::size_t parameter = 0; parameter < existing.parameters.size(); ++parameter)
                        if (!SameType(Memory::makeShared<MType>(*existing.parameters[parameter].type),
                                      Memory::makeShared<MType>(*candidate.parameters[parameter].type))) same = false;
                    if (same)
                    {
                        result->methods.pop_back();
                        break;
                    }
                }
            }
        }
    }
    return g_error.empty() ? result : nullptr;
}
