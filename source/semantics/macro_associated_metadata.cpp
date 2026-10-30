/**
 * @file macro_associated_metadata.cpp
 * @brief 从声明图读取关联类型，早期宏查询不依赖尚未登记的接口表。
 */
/**
 * @brief 从具体声明中查找关联定义，不把接口占位误认为具体类型。
 * @param owner 查询的声明名称。
 * @param name 关联类型名称。
 * @return 具体类型；仅有要求或未找到时为空。
 */
std::shared_ptr<MType> MacroAssociatedDefinition(const Memory::String &owner, const Memory::String &name)
{
    const int declaration = MacroMetadataDeclaration(owner);
    if (declaration < 0) return nullptr;
    const auto node = N(declaration);
    const auto slot = node.kind == MNode::EnumDecl ? 1u : 0u;
    if (node.kids.size() <= slot) return nullptr;
    for (const int member : N(node.kids[slot]).kids)
        if (N(member).kind == MNode::AssociatedDef && N(member).s == name) return T(N(member).type);
    return nullptr;
}

/**
 * @brief 归一化关联投影及其组成类型，检测递归而不执行用户代码。
 * @param source 类型或包含关联投影的复合类型。
 * @param implementation 接口要求的实际实现者。
 * @param depth 当前投影深度。
 * @return 归一化类型；循环或超出预算时为空并报告错误。
 */
std::shared_ptr<MType> MacroResolveAssociated(const std::shared_ptr<const MType> &source,
                                              const Memory::String &implementation, unsigned depth = 0)
{
    if (!source) return nullptr;
    if (depth >= 64) return Fail("关联类型元数据存在循环或超过深度预算"), nullptr;
    if (source->kind == MType::Struct)
    {
        const auto split = source->name.rfind("::");
        if (split != Memory::String::npos)
        {
            const auto owner = source->name.substr(0, split), name = source->name.substr(split + 2);
            const int declaration = MacroMetadataDeclaration(owner);
            const bool requirement = declaration >= 0 && N(declaration).kind == MNode::InterfaceDecl;
            const auto actual = MacroAssociatedDefinition(requirement && !implementation.empty() ? implementation : owner, name);
            if (actual) return MacroResolveAssociated(actual, implementation, depth + 1);
        }
    }
    auto type = Memory::makeShared<MType>(*source);
    if (!ResolveAstGenericType(type)) return nullptr;
    if (source->elem && !(type->elem = MacroResolveAssociated(source->elem, implementation, depth + 1))) return nullptr;
    if (source->elem2 && !(type->elem2 = MacroResolveAssociated(source->elem2, implementation, depth + 1))) return nullptr;
    for (auto &parameter : type->paramTypes)
        if (!(parameter = MacroResolveAssociated(parameter, implementation, depth + 1))) return nullptr;
    return type;
}

/**
 * @brief 按自身声明及基接口顺序读取关联类型要求和具体绑定。
 * @param source 被查询的具体类型或接口。
 * @param implementation 绑定接口要求的实际实现者；接口自身保留未绑定要求。
 * @return 只读关联类型快照，非法来源或循环时为空。
 */
std::shared_ptr<const chali_macro::MetadataType> ReadMacroAssociatedTypes(const std::shared_ptr<const MType> &source,
                                                                          const std::shared_ptr<const MType> &implementation)
{
    auto type = MacroResolveAssociated(source, implementation ? implementation->name : Memory::String{});
    if (!type) return nullptr;
    MReflectionBuilder names;
    names.indexDeclarations();
    auto result = Memory::makeShared<chali_macro::MetadataType>();
    result->type = type;
    result->name = names.named(type->name);
    result->identity = MacroMetadataIdentity("macro-type:" + names.key(type));
    Memory::Set<Memory::String> seen, active;
    std::function<bool(const Memory::String &, unsigned)> append = [&](const Memory::String &owner, unsigned depth) {
        if (depth >= 256 || !active.insert(owner).second) return Fail("关联类型元数据继承存在循环或超过深度预算");
        const int declaration = MacroMetadataDeclaration(owner);
        if (declaration < 0) return Fail("关联类型元数据需要类型声明：" + owner);
        const auto node = N(declaration);
        const auto slot = node.kind == MNode::EnumDecl ? 1u : 0u;
        if (node.kids.size() > slot)
            for (const int member : N(node.kids[slot]).kids)
            {
                const auto field = N(member);
                if ((field.kind != MNode::AssociatedDecl && field.kind != MNode::AssociatedDef) || !seen.insert(field.s).second) continue;
                chali_macro::MetadataField item;
                item.name = field.s;
                item.index = result->fields.size();
                item.declaringType = names.named(owner);
                item.identity = MacroMetadataIdentity("macro-associated:" + names.key(NamedType(owner)) + "::" + field.s);
                auto actual = implementation ? MacroAssociatedDefinition(implementation->name, field.s) : nullptr;
                if (!actual && field.kind == MNode::AssociatedDef) actual = T(field.type);
                if (actual && !(item.type = MacroResolveAssociated(actual, implementation ? implementation->name : owner))) return false;
                result->fields.push_back(std::move(item));
            }
        if (node.i >= 0)
            for (const int base : N(static_cast<int>(node.i)).kids)
                if (!append(g_names[base], depth + 1)) return false;
        active.erase(owner);
        return true;
    };
    return append(type->name, 0) && g_error.empty() ? result : nullptr;
}

/**
 * @brief 收集当前方法体的直接语法效果，局部函数和闭包的效果不算入外层。
 * @param id 方法体或子节点。
 * @param method 接收只读效果标记。
 * @param budget 剩余遍历节点数。
 * @param depth 当前语法深度。
 * @return 遍历未超出预算时返回真。
 */
bool ReadMacroMethodEffects(int id, chali_macro::MetadataMethod &method, std::size_t &budget, unsigned depth = 0)
{
    if (id < 0) return true;
    if (depth >= 256 || !budget--) return Fail("方法效果元数据超过语法预算");
    const auto node = N(id);
    if (node.kind == MNode::Lambda || node.kind == MNode::GenericLambda || node.kind == MNode::FuncDef || node.kind == MNode::MethodDef) return true;
    method.usesUnsafe = method.usesUnsafe || node.unsafeScope || node.unsafeRequired;
    method.hasPropagation = method.hasPropagation || node.kind == MNode::Try;
    for (const int child : node.kids)
        if (!ReadMacroMethodEffects(child, method, budget, depth + 1)) return false;
    return node.kind != MNode::Block || ReadMacroMethodEffects(static_cast<int>(node.i), method, budget, depth + 1);
}
