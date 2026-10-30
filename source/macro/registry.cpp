#include "registry.hpp"
#include "../chali_ast.hpp"

namespace chali_macro
{
/**
 * @brief 保存当前编译的节点池和卫生解析器。
 * @param nodes AST 节点池。
 * @param resolver 绑定和展开上下文解析器。
 * @param types 当前类型池。
 */
Registry::Registry(Memory::Deque<MNode> &nodes, HygieneResolver &resolver, Memory::Vector<std::shared_ptr<MType>> *types) : nodes_(nodes), resolver_(resolver), types_(types) {}

/**
 * @brief 保存本次编译的受限模板准备器。
 * @param prepare 在卫生绑定前处理具体元数据和编译期分支的回调。
 */
void Registry::setPreparation(std::function<bool(int, const std::map<std::string, int> &, std::string &)> prepare)
{
    prepare_ = std::move(prepare);
}

/**
 * @brief 保存直接导入关系，供宏的定义环境查找使用。
 * @param from 声明所在模块。
 * @param target 被导入模块。
 */
void Registry::addImport(const std::string &from, const std::string &target)
{
    imports_[from].insert(target);
}

/**
 * @brief 登记模块内唯一的宏定义并预先绑定模板。
 * @param definition 定义节点编号。
 * @param scope 定义处词法作用域。
 * @param error 接收诊断，成功时清空。
 * @return 登记成功返回真。
 */
bool Registry::add(int definition, ScopeId scope, std::string &error)
{
    error.clear();
    if (definition < 0 || static_cast<std::size_t>(definition) >= nodes_.size() || nodes_[definition].kind != MNode::MacroDefinition || nodes_[definition].kids.size() != 3)
    {
        error = "宏注册需要有效的定义节点";
        return false;
    }
    const auto node = nodes_[definition];
    const auto key = std::make_pair(std::string(node.module.data(), node.module.size()), std::string(node.s.data(), node.s.size()));
    if (key.second == "line" || key.second == "file" || key.second == "module" || key.second == "concat" || key.second == "stringify" || key.second == "fields" || key.second == "member" || key.second == "count" || key.second == "compile_error" || key.second == "compile_assert")
    {
        error = "宏名称占用了编译期内建名称：" + key.second;
        return false;
    }
    if (!node.s2.empty() && node.s2 != "expr" && node.s2 != "stmt" && node.s2 != "item")
    {
        error = "不支持的宏结果类别：" + std::string(node.s2.data(), node.s2.size());
        return false;
    }
    Definition entry;
    entry.declaration = definition;
    entry.scope = scope;
    entry.context = node.syntaxContext;
    entry.grouped = node.kids[0] >= 0;
    const int generics = node.kids[2];
    if (generics < 0 || static_cast<std::size_t>(generics) >= nodes_.size() || nodes_[generics].kind != MNode::List)
    {
        error = "宏泛型列表结构无效";
        return false;
    }
    entry.generic = !nodes_[generics].kids.empty();
    const auto start = nodes_.size();
    TemplateBinding binder(nodes_, resolver_, types_);
    if (!binder.bind(definition, scope, entry.bound, error, nullptr, nullptr, false)) return false;
    if (entry.grouped)
    {
        Memory::Vector<int> groups = { node.kids[0] };
        if (!nodes_[groups[0]].kids.empty() && nodes_[nodes_[groups[0]].kids[0]].kind == MNode::MacroParameters) groups = nodes_[groups[0]].kids;
        for (const int group : groups)
        {
            Definition::Group signature;
            signature.opening = nodes_[group].i ? static_cast<int>(nodes_[group].i) : '(';
            signature.begin = entry.parameters.size();
            signature.count = nodes_[group].kids.size();
            for (const int parameter : nodes_[group].kids)
            {
                if (signature.variadic)
                {
                    error = "变长宏形参必须位于参数组末尾";
                    return false;
                }
                signature.variadic = nodes_[parameter].i != 0;
                const auto &name = nodes_[parameter].s;
                entry.parameters.push_back(entry.bound.parameters.at(std::string(name.data(), name.size())));
                entry.names.push_back(std::string(name.data(), name.size()));
                entry.modes.push_back(std::max(0, nodes_[parameter].type2));
                const auto category = nodes_[parameter].s2;
                if (!category.empty() && category != "value" && category != "expr" && category != "ident" && category != "type" && category != "stmt" && category != "block" && category != "pattern")
                {
                    nodes_.resize(start);
                    error = "不支持的宏形参类别：" + std::string(category.data(), category.size());
                    return false;
                }
                if (!category.empty() && entry.modes.back() && category != "value")
                {
                    nodes_.resize(start);
                    error = "引用参数只能标注 value";
                    return false;
                }
                if (node.s2 == "item" && (entry.modes.back() || category.empty() || category == "value"))
                {
                    nodes_.resize(start);
                    error = "声明宏的形参必须显式标注语法类别，不能在模块作用域求值或借用运行期实参";
                    return false;
                }
                entry.categories.push_back(category);
            }
            entry.groups.push_back(signature);
        }
    }
    auto &overloads = definitions_[key];
    for (const auto &existing : overloads)
        if (existing.grouped == entry.grouped && existing.groups.size() == entry.groups.size() && std::equal(existing.groups.begin(), existing.groups.end(), entry.groups.begin(), [](const Definition::Group &a, const Definition::Group &b) {
                return a.opening == b.opening && a.count == b.count && a.variadic == b.variadic;
            }))
        {
            nodes_.resize(start);
            error = "宏重复定义：" + key.second;
            return false;
        }
    overloads.push_back(std::move(entry));
    return true;
}

/**
 * @brief 校验宏调用并克隆替换模板，实参保持原始卫生上下文。
 * @param call 调用节点编号。
 * @param error 接收错误，成功时清空。
 * @return 实例化模板编号，失败为负一。
 */
int Registry::instantiate(int call, std::string &error,
                          const std::function<int(int, int, int)> &resolveType,
                          const std::function<bool(int, const std::map<std::string, int> &, std::string &)> &applyTypes)
{
    error.clear();
    if (call < 0 || static_cast<std::size_t>(call) >= nodes_.size() || nodes_[call].kind != MNode::MacroCall || nodes_[call].kids.empty())
    {
        error = "宏展开需要有效的调用节点";
        return -1;
    }
    const auto node = nodes_[call];
    const auto name = std::string(node.s.data(), node.s.size());
    const auto callModule = std::string(node.module.data(), node.module.size());
    const auto key = std::make_pair(callModule, name);
    auto found = definitions_.end();
    /* 限定名调用（`$std::lang::assert`）：按「模块 + 宏名」直接定位。模块路径从最长的
     * 前缀往下试（`std::lang::assert` 先试 `std::lang` + `assert`），第一个命中的算数。 */
    for (std::size_t at = name.rfind("::"); at != std::string::npos && at != 0; at = name.rfind("::", at - 1))
    {
        const auto qualifier = name.substr(0, at);
        /* 限定调用也走可见性：`std::lang` 始终可见，其余必须先 import。 */
        if (qualifier != callModule && qualifier != "std::lang" && imports_[callModule].count(qualifier) == 0) continue;
        const auto candidate = definitions_.find({ qualifier, name.substr(at + 2) });
        if (candidate == definitions_.end()) continue;
        found = candidate;
        break;
    }
    if (found == definitions_.end()) found = definitions_.find(key);
    if (found == definitions_.end())
    {
        auto visible = imports_[key.first];
        visible.insert("std::lang");
        for (const auto &imported : visible)
        {
            const auto candidate = definitions_.find({ imported, key.second });
            if (candidate == definitions_.end()) continue;
            if (found != definitions_.end())
            {
                error = "宏名称存在多个可见定义：" + key.second;
                return -1;
            }
            found = candidate;
        }
    }
    if (found == definitions_.end())
    {
        error = "未定义宏：" + key.second;
        return -1;
    }
    const int group = node.kids[0];
    Memory::Vector<int> groups;
    if (group >= 0)
        groups = nodes_[group].kind == MNode::MacroArguments ? nodes_[group].kids : Memory::Vector<int>{ group };
    const Definition *selected = nullptr;
    bool sameGroup = false;
    for (const auto &candidate : found->second)
        if (candidate.groups.size() == groups.size())
        {
            bool shape = true, arity = true, variadic = false;
            for (std::size_t index = 0; index < groups.size(); ++index)
            {
                const auto &signature = candidate.groups[index];
                const auto &actual = nodes_[groups[index]];
                const int opening = actual.kind == MNode::List ? '(' : static_cast<int>(actual.i);
                shape = shape && signature.opening == opening;
                arity = arity && (signature.variadic ? actual.kids.size() + 1 >= signature.count : actual.kids.size() == signature.count);
                variadic = variadic || signature.variadic;
            }
            sameGroup = sameGroup || shape;
            if (shape && arity)
            {
                if (!selected || !variadic) selected = &candidate;
            }
        }
    if (!selected)
    {
        error = (sameGroup ? "宏实参数量与形参不一致：" : "宏调用的参数组与定义不一致：") + key.second;
        return -1;
    }
    const auto &entry = *selected;
    std::set<int> preparationSeen;
    std::function<bool(int)> needsPreparation = [&](int id) {
        if (id < 0 || !preparationSeen.insert(id).second) return false;
        const auto &syntax = nodes_[id];
        if (syntax.kind == MNode::MacroFieldFor || syntax.kind == MNode::MacroCompileIf || (syntax.kind == MNode::MacroCall && (syntax.s == "count" || syntax.s == "compile_error" || syntax.s == "compile_assert"))) return true;
        for (const int child : syntax.kids)
            if (needsPreparation(child)) return true;
        if (needsPreparation(syntax.guard)) return true;
        const bool indexed = syntax.kind == MNode::Block || syntax.kind == MNode::Case;
        if (indexed && needsPreparation(static_cast<int>(syntax.i))) return true;
        return false;
    };
    const bool compileTime = needsPreparation(nodes_[entry.declaration].kids[1]);
    if ((entry.generic || compileTime) && !resolveType) return -2;
    if (nodes_[entry.declaration].s2 == "item")
    {
        std::string chain = name;
        int frame = node.macroExpansion;
        std::size_t depth = 0;
        while (frame >= 0 && depth++ < 256)
        {
            const auto &origin = nodes_[frame];
            chain += " <- " + std::string(origin.s.data(), origin.s.size());
            if (origin.i == entry.declaration)
            {
                error = "声明宏展开存在循环依赖：" + chain;
                return -1;
            }
            frame = origin.kids.empty() ? -1 : origin.kids[0];
        }
    }

    if (!entry.generic && node.kids.size() > 1)
    {
        error = "非泛型宏不能指定类型实参";
        return -1;
    }
    std::map<BindingId, int> arguments;
    std::map<std::string, int> inputs;
    if (remainingNodes_ == 0)
    {
        error = "宏展开生成的语法节点超过编译预算";
        return -1;
    }
    const auto start = nodes_.size();
    struct Rollback
    {
        Memory::Deque<MNode> &nodes;
        Memory::Vector<std::shared_ptr<MType>> *types;
        std::size_t nodeCount, typeCount;
        bool committed = false;
        /**
         * @brief 实例化失败或被类型请求中断时同步回滚语法池和类型池。
         */
        ~Rollback()
        {
            if (committed) return;
            nodes.resize(nodeCount);
            if (types) types->resize(typeCount);
        }
    } rollback{ nodes_, types_, start, types_ ? types_->size() : 0 };
    for (std::size_t index = 0; index < groups.size(); ++index)
    {
        const auto &signature = entry.groups[index];
        const auto &actuals = nodes_[groups[index]].kids;
        const auto fixed = signature.count - (signature.variadic ? 1 : 0);
        for (std::size_t item = 0; item < fixed; ++item)
            inputs.emplace(entry.names[signature.begin + item], actuals[item]);
        if (signature.variadic)
        {
            MNode rest;
            rest.kind = MNode::MacroVariadic;
            for (std::size_t item = fixed; item < actuals.size(); ++item)
            {
                if (item != fixed) rest.s += ",";
                rest.s += nodes_[actuals[item]].s;
                rest.kids.push_back(actuals[item]);
            }
            inputs.emplace(entry.names[signature.begin + fixed], static_cast<int>(nodes_.size()));
            nodes_.push_back(std::move(rest));
        }
    }
    const auto constrain = [&](int actual, const Memory::String &category) {
        const auto fragment = nodes_[actual];
        const int alternative = category == "ident" ? 1 : category == "type" ? 2 :
                                                      category == "block"    ? 3 :
                                                      category == "stmt"     ? 4 :
                                                      category == "pattern"  ? 5 :
                                                                               0;
        if (fragment.kind != MNode::MacroArgument || fragment.kids.size() != chali_macro::FragmentAlternativeCount || fragment.kids[alternative] < 0) return -1;
        auto constrained = fragment;
        for (std::size_t slot = 0; slot < constrained.kids.size(); ++slot)
            if (slot != static_cast<std::size_t>(alternative) && !(category == "ident" && slot == 0) && !(category == "block" && (slot == 0 || slot == 4)))
                constrained.kids[slot] = -1;
        const int result = static_cast<int>(nodes_.size());
        nodes_.push_back(std::move(constrained));
        return result;
    };
    for (std::size_t index = 0; index < entry.names.size(); ++index)
    {
        const auto &category = entry.categories[index];
        if (category.empty()) continue;
        const int actual = inputs.at(entry.names[index]);
        int constrained = -1;
        if (nodes_[actual].kind == MNode::MacroVariadic)
        {
            auto rest = nodes_[actual];
            bool valid = true;
            for (auto &argument : rest.kids)
            {
                argument = constrain(argument, category);
                if (argument < 0)
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
            {
                constrained = static_cast<int>(nodes_.size());
                nodes_.push_back(std::move(rest));
            }
        }
        else
            constrained = constrain(actual, category);
        if (constrained < 0)
        {
            error = "宏实参与形参类别不匹配：" + entry.names[index] + ": " + std::string(category.data(), category.size());
            return -1;
        }
        inputs[entry.names[index]] = constrained;
    }
    ExpansionSite site;
    site.line = node.expansionLine ? node.expansionLine : node.sourceLine;
    site.column = node.expansionLine ? node.expansionColumn : node.sourceColumn;
    const auto &module = node.expansionLine ? node.expansionModule : node.module;
    site.module.assign(module.data(), module.size());
    MNode frame;
    frame.kind = MNode::MacroExpansionFrame;
    frame.s = node.s;
    frame.module = module;
    frame.sourceLine = site.line;
    frame.sourceColumn = site.column;
    frame.i = entry.declaration;
    frame.kids = { node.macroExpansion };
    site.frame = static_cast<int>(nodes_.size());
    nodes_.push_back(std::move(frame));
    std::map<std::string, int> types;
    if (entry.generic)
    {
        const auto &generics = nodes_[nodes_[entry.declaration].kids[2]].kids;
        const auto explicitCount = node.kids.size() > 1 ? nodes_[node.kids[1]].kids.size() : 0;
        if (explicitCount && explicitCount != generics.size())
        {
            nodes_.resize(start);
            error = "宏显式类型实参数量与泛型形参不一致";
            return -1;
        }
        for (std::size_t index = 0; index < generics.size(); ++index)
        {
            const auto &generic = nodes_[generics[index]];
            const auto source = std::string(generic.s2.data(), generic.s2.size());
            const bool resultSource = source.empty() || source == "#res";
            if (!resultSource && !inputs.count(source))
            {
                nodes_.resize(start);
                error = "宏泛型推导来源不是形参：" + source;
                return -1;
            }
            const int resolved = resolveType(generics[index], resultSource ? -1 : inputs.at(source),
                                             explicitCount ? nodes_[node.kids[1]].kids[index] : -1);
            if (resolved < 0)
            {
                nodes_.resize(start);
                error = "宏泛型类型无法推导：" + std::string(generic.s.data(), generic.s.size());
                return -1;
            }
            types.emplace(std::string(generic.s.data(), generic.s.size()), resolved);
        }
    }
    int preparedDefinition = entry.declaration;
    if (prepare_ && compileTime)
    {
        AstSubstitution copy(nodes_, remainingNodes_, types_);
        preparedDefinition = copy.instantiate(entry.declaration, {}, resolver_.createExpansion(entry.scope, entry.context), error);
        if (preparedDefinition < 0) return -1;
        if (entry.generic && (!applyTypes || !applyTypes(preparedDefinition, types, error))) return -1;
        if (!prepare_(nodes_[preparedDefinition].kids[1], inputs, error)) return -1;
    }
    TemplateBinding binder(nodes_, resolver_, types_);
    BoundTemplate bound;
    if (!binder.bind(preparedDefinition, entry.scope, bound, error, &inputs, &matcher_, true, prepare_))
    {
        nodes_.resize(start);
        return -1;
    }
    if (!normalizeTemplate(bound.root, 0, error))
    {
        nodes_.resize(start);
        return -1;
    }
    const auto bindingCost = nodes_.size() - start;
    if (bindingCost >= remainingNodes_)
    {
        nodes_.resize(start);
        error = "宏展开生成的语法节点超过编译预算";
        return -1;
    }
    for (const auto &input : inputs) arguments.emplace(bound.parameters.at(input.first), input.second);
    arguments.insert(bound.iterations.begin(), bound.iterations.end());
    const auto expansion = resolver_.createExpansion(entry.scope, entry.context);
    std::set<BindingId> valueParameters;
    std::set<BindingId> syntaxParameters;
    std::set<BindingId> statementParameters;
    // 编译期选择器在绑定后被选中分支替换，先从定义中保留其语法参数角色。
    std::set<int> selectorVisited;
    std::function<void(int)> inspectSelectors = [&](int id) {
        if (id < 0 || !selectorVisited.insert(id).second) return;
        const auto &syntax = nodes_[id];
        if (syntax.kind == MNode::MacroMatch || syntax.kind == MNode::MacroRegex)
        {
            const auto found = bound.parameters.find(std::string(syntax.s.data(), syntax.s.size()));
            if (found != bound.parameters.end()) syntaxParameters.insert(found->second);
        }
        for (const int child : syntax.kids) inspectSelectors(child);
    };
    inspectSelectors(entry.declaration);
    std::set<const MType *> inspectedTypes;
    std::function<void(const std::shared_ptr<MType> &)> inspectType = [&](const std::shared_ptr<MType> &type) {
        if (!type || !inspectedTypes.insert(type.get()).second) return;
        if (type->macroBinding) syntaxParameters.insert(type->macroBinding);
        inspectType(type->elem);
        inspectType(type->elem2);
        for (const auto &parameter : type->paramTypes) inspectType(parameter);
        for (const auto &argument : type->genericArguments) inspectType(argument);
    };
    bool exports = false;
    std::set<int> visited;
    std::set<int> textVisited;
    std::function<void(int)> inspectText = [&](int id) {
        if (id < 0 || !textVisited.insert(id).second) return;
        const auto &syntax = nodes_[id];
        if (syntax.bindingIdentity) syntaxParameters.insert(syntax.bindingIdentity);
        if (syntax.nameParameter) syntaxParameters.insert(syntax.nameParameter);
        for (const int child : syntax.kids) inspectText(child);
    };
    std::function<void(int)> inspect = [&](int id) {
        if (id < 0 || !visited.insert(id).second) return;
        const auto syntax = nodes_[id];
        if (syntax.kind == MNode::ExprStmt && syntax.kids.size() == 1 && nodes_[syntax.kids[0]].kind == MNode::MacroHole)
        {
            statementParameters.insert(nodes_[syntax.kids[0]].bindingIdentity);
            return;
        }
        Memory::String called = syntax.s;
        int callArguments = syntax.kids.empty() ? -1 : syntax.kids[0];
        if (syntax.kind == MNode::Call && syntax.s.empty() && syntax.kids.size() == 2)
        {
            const auto &callee = nodes_[syntax.kids[0]];
            called = callee.kind == MNode::EnumCtor ? callee.s + "::" + callee.s2 : callee.s;
            callArguments = syntax.kids[1];
        }
        if (syntax.kind == MNode::Call && callArguments >= 0 && (called == "print" || called == "println" || called == "std::lang::print" || called == "std::lang::println"))
        {
            const auto &arguments = nodes_[callArguments].kids;
            for (std::size_t index = 0; index < arguments.size(); ++index)
            {
                const auto &argument = nodes_[arguments[index]];
                if (index == 0 && argument.kind == MNode::MacroHole)
                    syntaxParameters.insert(argument.bindingIdentity);
                else
                    inspect(arguments[index]);
            }
            return;
        }
        if (syntax.kind == MNode::MacroCall && (syntax.s == "stringify" || syntax.s == "concat"))
        {
            inspectText(id);
            return;
        }
        if (syntax.nameParameter)
        {
            exports = true;
            syntaxParameters.insert(syntax.nameParameter);
        }
        if (types_ && syntax.type >= 0) inspectType(types_->at(syntax.type));
        if (syntax.kind == MNode::MacroHole) valueParameters.insert(syntax.bindingIdentity);
        for (const int child : syntax.kids) inspect(child);
        for (const int annotation : syntax.annotations) inspect(annotation);
        inspect(syntax.guard);
        if (syntax.kind == MNode::Block || syntax.kind == MNode::Case) inspect(static_cast<int>(syntax.i));
    };
    inspect(bound.root);
    std::map<BindingId, int> values;
    Memory::Vector<int> initializers;
    const auto parameterScope = resolver_.createScope(entry.scope);
    const auto cacheValue = [&](const MNode &actual, const std::string &name, int mode) {
        MNode variable;
        variable.kind = MNode::Var;
        variable.s.assign(name.data(), name.size());
        variable.syntaxContext = expansion;
        variable.bindingIdentity = resolver_.declare(name, resolver_.createScope(parameterScope), expansion);
        variable.module = module;
        variable.sourceLine = site.line;
        variable.sourceColumn = site.column;
        variable.macroExpansion = site.frame;
        const int reference = static_cast<int>(nodes_.size());
        nodes_.push_back(variable);
        int expression = actual.kids[0];
        if (mode)
        {
            auto address = variable;
            address.kind = MNode::Un;
            address.s = "&";
            address.bindingIdentity = 0;
            address.kids = { expression };
            expression = static_cast<int>(nodes_.size());
            nodes_.push_back(std::move(address));
        }
        auto declaration = variable;
        declaration.kind = MNode::Declare;
        declaration.i = -1;
        declaration.type2 = 0;
        declaration.s2 = mode == 1 ? "__chali_macro_const_ref" : mode == 2 ? "__chali_macro_mut_ref" :
                                                                             "";
        declaration.kids = { expression };
        const int declared = static_cast<int>(nodes_.size());
        nodes_.push_back(std::move(declaration));
        MNode list;
        list.kind = MNode::List;
        list.kids = { declared };
        const int listed = static_cast<int>(nodes_.size());
        nodes_.push_back(std::move(list));
        auto group = variable;
        group.kind = MNode::Declare;
        group.kids = { listed };
        initializers.push_back(static_cast<int>(nodes_.size()));
        nodes_.push_back(std::move(group));
        return reference;
    };
    for (std::size_t index = 0; types_ && index < entry.names.size(); ++index)
    {
        const auto identity = bound.parameters.at(entry.names[index]);
        const auto &category = entry.categories[index];
        if (!category.empty() && category != "value") continue;
        if (category.empty() && !valueParameters.count(identity) && syntaxParameters.count(identity) && !entry.modes[index]) continue;
        const auto actual = nodes_[inputs.at(entry.names[index])];
        if (actual.kind == MNode::MacroVariadic)
        {
            if (category != "value") continue;
            auto cached = actual;
            std::map<int, int> replacements;
            for (std::size_t slot = 0; slot < cached.kids.size(); ++slot)
            {
                const int source = cached.kids[slot];
                auto argument = nodes_[source];
                argument.kids[0] = cacheValue(argument, entry.names[index] + "_" + std::to_string(slot), entry.modes[index]);
                const int replacement = static_cast<int>(nodes_.size());
                nodes_.push_back(std::move(argument));
                cached.kids[slot] = replacement;
                replacements.emplace(source, replacement);
            }
            const int rest = static_cast<int>(nodes_.size());
            nodes_.push_back(std::move(cached));
            arguments[identity] = rest;
            for (const auto &iteration : bound.iterations)
                if (replacements.count(iteration.second)) arguments[iteration.first] = replacements.at(iteration.second);
            continue;
        }
        if (actual.kind != MNode::MacroArgument || actual.kids.size() != chali_macro::FragmentAlternativeCount || actual.kids[0] < 0) continue;
        if (category.empty() && !valueParameters.count(identity) && statementParameters.count(identity) && actual.kids[3] >= 0 && nodes_[actual.kids[0]].kind == MNode::Loop && nodes_[actual.kids[0]].s == "block" && !entry.modes[index]) continue;
        values[identity] = cacheValue(actual, entry.names[index], entry.modes[index]);
    }
    const auto preparationCost = nodes_.size() - start;
    if (preparationCost >= remainingNodes_)
    {
        error = "宏展开生成的语法节点超过编译预算";
        return -1;
    }
    AstSubstitution substitution(nodes_, remainingNodes_ - preparationCost, types_);
    const int expanded = substitution.instantiate(bound.root, arguments, expansion, error, site, &values);
    if (expanded >= 0 && entry.generic && (!applyTypes || !applyTypes(expanded, types, error)))
    {
        nodes_.resize(start);
        if (error.empty()) error = "宏类型参数未能替换到模板中";
        return -1;
    }
    if (expanded >= 0)
    {
        nodes_[expanded].s2 = nodes_[entry.declaration].s2;
        const auto &result = nodes_[expanded];
        const auto &items = nodes_[result.kids[0]].kids;
        if ((result.s2 == "stmt" && result.kids[1] >= 0) ||
            (result.s2 == "expr" && result.kids[1] < 0 && !(items.size() == 1 && (nodes_[items[0]].kind == MNode::Block || nodes_[items[0]].kind == MNode::If || nodes_[items[0]].kind == MNode::Switch))))
        {
            nodes_.resize(start);
            error = "宏结果与显式类别不匹配";
            return -1;
        }
        if (!initializers.empty())
        {
            auto &statements = nodes_[nodes_[expanded].kids[0]].kids;
            statements.insert(statements.begin(), initializers.begin(), initializers.end());
            nodes_[expanded].i = static_cast<long long>(initializers.size());
            nodes_[expanded].type2 = exports ? 2 : 1;
        }
        remainingNodes_ -= nodes_.size() - start;
        rollback.committed = true;
    }
    else
        nodes_.resize(start);
    return expanded;
}

/**
 * @brief 直接合并选中分支的语句与末尾表达式，不增加运行时块。
 * @param root 模板节点编号。
 * @param depth 当前递归深度。
 * @param error 接收诊断文本。
 * @return 检查成功返回真。
 */
bool Registry::normalizeTemplate(int root, std::size_t depth, std::string &error)
{
    if (root < 0 || depth >= 256 || static_cast<std::size_t>(root) >= nodes_.size() || nodes_[root].kind != MNode::MacroTemplate || nodes_[root].kids.size() != 2)
    {
        error = "宏模板结构无效或过深";
        return false;
    }
    auto &node = nodes_[root];
    const auto parts = nodes_[node.kids[0]].kids;
    Memory::Vector<int> statements;
    for (std::size_t index = 0; index < parts.size(); ++index)
    {
        const int part = parts[index];
        if (nodes_[part].kind != MNode::MacroTemplate)
        {
            statements.push_back(part);
            continue;
        }
        if (!normalizeTemplate(part, depth + 1, error)) return false;
        const auto &selected = nodes_[part];
        const auto &items = nodes_[selected.kids[0]].kids;
        statements.insert(statements.end(), items.begin(), items.end());
        if (selected.kids[1] >= 0)
        {
            if (index + 1 == parts.size() && node.kids[1] < 0)
                node.kids[1] = selected.kids[1];
            else
            {
                const int expression = selected.kids[1];
                nodes_[expression].checkedExpression = true;
                MNode statement;
                statement.kind = MNode::ExprStmt;
                statement.kids = { expression };
                statement.sourceLine = nodes_[expression].sourceLine;
                statement.sourceColumn = nodes_[expression].sourceColumn;
                statement.module = nodes_[expression].module;
                statements.push_back(static_cast<int>(nodes_.size()));
                nodes_.push_back(std::move(statement));
            }
        }
    }
    nodes_[node.kids[0]].kids = std::move(statements);
    return true;
}
}  // namespace chali_macro
