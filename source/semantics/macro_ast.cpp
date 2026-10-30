/**
 * @brief 建立宏参数组，不将空参数组归一化为无参数组。
 * @param names 形参列表节点。
 * @param opening 参数组的左括号字符。
 * @return 参数组节点。
 */
int M_MacroParameters(int names, int opening)
{
    const int id = M_New(MNode::MacroParameters);
    g_nodes[id].kids = g_nodes[names].kids;
    g_nodes[id].i = opening;
    return id;
}

/**
 * @brief 合并参数组而不丢失空组及括号种类。
 * @param groups 已有组结构。
 * @param group 待追加组。
 * @return 多组结构编号。
 */
int M_MacroParameterGroups(int groups, int group)
{
    if (g_nodes[groups].i != 0)
    {
        const int root = M_New(MNode::MacroParameters);
        g_nodes[root].kids = { groups, group };
        return root;
    }
    g_nodes[groups].kids.push_back(group);
    return groups;
}

/**
 * @brief 将宏形参名称保存为占位声明节点。
 * @param name 名称池编号。
 * @param variadic 是否接收本参数组余下的片段。
 * @return 参数节点。
 */
int M_MacroParameter(int name, bool variadic)
{
    return M_New(MNode::MacroParameter, g_names[name], variadic ? 1 : 0);
}

/**
 * @brief 记录显式宏形参类别，不执行或展开实参。
 * @param parameter 已建立的宏形参节点编号。
 * @param category 类别名称池编号。
 * @return 保留引用绑定方式的原形参节点。
 */
int M_MacroCategoryParameter(int parameter, int category)
{
    g_nodes[parameter].s2 = g_names[category];
    return parameter;
}

/**
 * @brief 保存宏形参的引用绑定方式。
 * @param name 名称池编号。
 * @param mode 一表示只读引用，二表示可写引用。
 * @return 参数节点编号。
 */
int M_MacroReferenceParameter(int name, int mode)
{
    const int id = M_MacroParameter(name, false);
    g_nodes[id].type2 = mode;
    return id;
}

/**
 * @brief 保存编译期循环及其片段变量，不创建运行时循环。
 * @param variable 循环变量名称。
 * @param source 变长形参名称。
 * @param body 待逐次展开的模板。
 * @return 循环节点。
 */
int M_MacroFor(int variable, int source, int body)
{
    const int id = M_New(MNode::MacroFor, g_names[variable]);
    g_nodes[id].s2 = g_names[source];
    g_nodes[id].kids = { body };
    return id;
}

/**
 * @brief 保存字段集合查询与逐字段模板。
 * @param variable 字段迭代名称。
 * @param source 编译期集合表达式。
 * @param body 循环模板。
 * @return 新节点编号。
 */
int M_MacroFieldFor(int variable, int source, int body)
{
    const int id = M_New(MNode::MacroFieldFor, g_names[variable]);
    g_nodes[id].kids = { source, body };
    return id;
}

/**
 * @brief 保存等长同步重复所需的变量与来源名称。
 * @param variables 迭代变量列表。
 * @param sources 来源列表。
 * @param body 重复模板。
 * @return 新节点编号。
 */
int M_MacroZip(int variables, int sources, int body)
{
    const int id = M_New(MNode::MacroFor);
    g_nodes[id].kids = { body, variables, sources };
    return id;
}

/**
 * @brief 保存未选分支尚未检查的编译期条件。
 * @param condition 常量条件。
 * @param yes 真分支。
 * @param no 假分支。
 * @return 新节点编号。
 */
int M_MacroCompileIf(int condition, int yes, int no)
{
    const int id = M_New(MNode::MacroCompileIf);
    g_nodes[id].kids = { condition, yes, no };
    return id;
}

/**
 * @brief 保留类型应用结构，不在语法分类时执行模板名称查找或约束检查。
 * @param name 名称池中的类型名称。
 * @param arguments 按顺序保存的类型项。
 * @return 类型描述的池编号。
 */
int M_AstGenericType(int name, int arguments)
{
    auto type = Memory::makeShared<MType>();
    type->kind = MType::Struct;
    type->name = g_names[name];
    type->genericBase = type->name;
    type->lookupModule = g_parseModule;
    for (const int argument : g_nodes[arguments].kids) type->genericArguments.push_back(M_TypeAt(g_nodes[argument].type));
    const auto bare = type->name.rfind("std::lang::", 0) == 0 ? type->name.substr(11) : type->name;
    if ((bare == "List" || bare == "Set" || bare == "Map") && type->genericArguments.size() == (bare == "Map" ? 2u : 1u))
    {
        const int first = g_nodes[g_nodes[arguments].kids[0]].type;
        const int second = bare == "Map" ? g_nodes[g_nodes[arguments].kids[1]].type : -1;
        const int existing = bare == "List" ? M_TypeVec(first) : bare == "Set" ? M_TypeSet(first) :
                                                                                 M_TypeMap(first, second);
        return existing;
    }
    return M_AddType(type);
}

/**
 * @brief 将模块限定类型名与类型项组成同一通用类型应用。
 * @param name 限定类型名称表达式。
 * @param arguments 类型项列表。
 * @return 类型描述的池编号。
 */
int M_QualifiedAstGenericType(int name, int arguments)
{
    const auto &node = g_nodes[name];
    const auto qualified = node.kind == MNode::EnumCtor ? node.s + "::" + node.s2 : node.s;
    return M_AstGenericType(M_Name(qualified.c_str()), arguments);
}

/**
 * @brief 建立尚未实例化类型的成员引用，供常规语义查找与构造器规则处理。
 * @param type 类型描述编号。
 * @param member 成员名称编号。
 * @return 限定引用节点。
 */
int M_AstTypeQualifiedRef(int type, int member)
{
    const int id = M_New(MNode::TypeQualifiedRef);
    g_nodes[id].type = type;
    g_nodes[id].s2 = g_names[member];
    return id;
}

/**
 * @brief 保存宏体语句及可选表达式尾项。
 * @param statements 模板语句列表。
 * @param expression 末尾表达式，负值表示不存在。
 * @return 模板节点。
 */
int M_MacroTemplate(int statements, int expression)
{
    const int id = M_New(MNode::MacroTemplate);
    g_nodes[id].kids = { statements, expression };
    return id;
}

/**
 * @brief 保存语法分析后的宏定义，展开留到语义分析。
 * @param name 宏名称池编号。
 * @param parameters 参数组节点，负值表示无参数组。
 * @param body 已解析宏体。
 * @param generics 宏泛型参数节点列表。
 * @return 定义节点。
 */
int M_MacroDefinition(int name, int parameters, int body, int generics)
{
    const int id = M_New(MNode::MacroDefinition, g_names[name]);
    g_nodes[id].kids = { parameters, body, generics };
    return id;
}

/**
 * @brief 保存宏调用实参结构，禁止提前拼接实参文本。
 * @param name 含美元前缀的名称池编号。
 * @param arguments 实参列表，负值表示未提供参数组。
 * @return 调用节点。
 */
int M_MacroCall(int name, int arguments)
{
    const int id = M_New(MNode::MacroCall, g_names[name].substr(1));
    g_nodes[id].kids = { arguments };
    const auto raw = g_rawMacroNames.find(name);
    if (raw != g_rawMacroNames.end())
    {
        const auto &source = chali_macro::sourceFragments.at(raw->second);
        g_nodes[id].i = static_cast<long long>(raw->second + 1);
        g_nodes[id].kids.clear();
        g_nodes[id].sourceLine = source.line;
        g_nodes[id].sourceColumn = source.column;
    }
    return id;
}

/**
 * @brief 保存泛型推导来源，空来源表示外部期望类型。
 * @param name 泛型参数名。
 * @param source 推导来源参数名，-1 表示隐式结果上下文，-2 表示显式 #res。
 * @return 泛型参数节点。
 */
int M_MacroGeneric(int name, int source)
{
    const int id = M_New(MNode::MacroGeneric, g_names[name]);
    if (source >= 0) g_nodes[id].s2 = g_names[source];
    else if (source == -2) g_nodes[id].s2 = "#res";
    return id;
}

/**
 * @brief 为宏调用记录显式类型参数。
 * @param name 带美元前缀的宏名。
 * @param types 显式类型列表。
 * @param arguments 实参列表，负值表示无参数组。
 * @return 调用节点。
 */
int M_MacroTypedCall(int name, int types, int arguments)
{
    const int id = M_MacroCall(name, arguments);
    if (g_nodes[id].i == 0) g_nodes[id].kids.push_back(types);
    return id;
}

/**
 * @brief 建立文法确认的实参组，原文分类引用与表达式都保留节点身份。
 * @param arguments 实参项列表。
 * @param opening 左括号字符。
 * @return 实参组编号。
 */
int M_MacroCallGroup(int arguments, int opening)
{
    const int id = M_New(MNode::MacroArgumentGroup, "", opening);
    g_nodes[id].kids = g_nodes[arguments].kids;
    return id;
}

/**
 * @brief 保留所有文法读取的参数组，捕获过原文的调用在语义阶段统一分类。
 * @param name 宏名称。
 * @param groups 实参组列表。
 * @param types 显式类型项列表，负值表示没有。
 * @return 调用节点。
 */
int M_MacroGroupedCall(int name, int groups, int types)
{
    const int id = M_MacroCall(name, -1);
    if (g_nodes[id].i != 0) return id;
    const int arguments = M_New(MNode::MacroArguments);
    g_nodes[arguments].kids = g_nodes[groups].kids;
    g_nodes[id].kids = { arguments };
    if (types >= 0) g_nodes[id].kids.push_back(types);
    return id;
}

/**
 * @brief 保存宏原文片段的内部引用，禁止将其作为普通语言表达式解释。
 * @param marker 名称池中的片段引用。
 * @return 片段节点。
 */
int M_MacroFragmentReference(int marker)
{
    return M_New(MNode::MacroArgument, g_names[marker]);
}

/**
 * @brief 创建语法匹配的一个候选分支。
 * @param category 类别名称，负值表示兜底。
 * @param body 分支模板。
 * @return 分支节点。
 */
int M_MacroBranch(int category, int body)
{
    const int id = M_New(MNode::MacroBranch, category < 0 ? Memory::String() : g_names[category]);
    g_nodes[id].kids = { body };
    return id;
}

/**
 * @brief 将候选分支按源代码顺序加入列表。
 * @param branch 当前候选。
 * @param rest 后续候选列表。
 * @return 合并后的列表。
 */
int M_MacroBranches(int branch, int rest)
{
    const int id = M_List1(branch);
    g_nodes[id].kids.insert(g_nodes[id].kids.end(), g_nodes[rest].kids.begin(), g_nodes[rest].kids.end());
    return id;
}

/**
 * @brief 创建编译期语法匹配节点。
 * @param parameter 宏形参名称。
 * @param branches 已按顺序排列的分支。
 * @return 匹配节点。
 */
int M_MacroMatch(int parameter, int branches)
{
    const int id = M_New(MNode::MacroMatch, g_names[parameter]);
    g_nodes[id].kids = { branches };
    return id;
}

/**
 * @brief 保存正则分支语法，不在解析阶段执行或展开分支。
 * @param parameter 待匹配参数名。
 * @param pattern 正则字符串的名称池编号。
 * @param body 匹配成功时的模板。
 * @param otherwise 不匹配时的可选模板。
 * @return 正则分支节点。
 */
int M_MacroRegex(int parameter, int pattern, int body, int otherwise)
{
    const int id = M_New(MNode::MacroRegex, g_names[parameter]);
    g_nodes[id].s2 = g_names[pattern];
    g_nodes[id].kids = { body, otherwise };
    return id;
}

/**
 * @brief 构造完整的泛型闭包语法树，供语法检查与模板阶段保留结构信息。
 * @param resultType 返回类型编号，负数表示按具体实现推导。
 * @param generics 类型参数列表。
 * @param params 调用参数列表。
 * @param body 完整函数体。
 * @param groups 显式捕获分组，负数表示没有分组。
 * @param capture 默认捕获方式。
 * @return 泛型闭包语法节点。
 */
int M_GenericClosureSyntax(int resultType, int generics, int params, int body, int groups, int capture)
{
    const int id = M_New(MNode::GenericLambda);
    g_nodes[id].type = resultType;
    g_nodes[id].type2 = -1;
    g_nodes[id].i = capture;
    g_nodes[id].kids = { generics, params, body };
    if (groups >= 0)
    {
        const int captures = M_List();
        for (const int group : g_nodes[groups].kids)
            for (const int name : g_nodes[group].kids)
            {
                const int reference = M_Var(name);
                const int entry = M_List1(reference);
                g_nodes[entry].i = g_nodes[group].i;
                M_ListAdd(captures, entry);
            }
        g_nodes[id].guard = captures;
    }
    return id;
}

/**
 * @brief 构造带显式输出类别的宏定义，保留原有形参与模板结构。
 * @param name 宏名称。
 * @param parameters 形参组。
 * @param body 模板节点。
 * @param generics 泛型参数。
 * @param category 结果类别名称池编号；负一表示声明结果。
 * @return 已标注的宏定义节点。
 */
int M_MacroResultDefinition(int name, int parameters, int body, int generics, int category)
{
    const int id = M_MacroDefinition(name, parameters, body, generics);
    g_nodes[id].s2 = category < 0 ? Memory::String("item") : g_names[category];
    if (category < 0)
        for (const int child : g_nodes[g_nodes[body].kids[0]].kids)
        {
            const auto &declaration = g_nodes[child];
            if (declaration.kind != MNode::FuncDef && declaration.kind != MNode::StructDecl && declaration.kind != MNode::EnumDecl && declaration.kind != MNode::InterfaceDecl && declaration.kind != MNode::MacroCall)
                g_parseDiagnostics.push_back({ "声明宏只能生成函数、结构体、枚举、接口或声明宏调用", declaration.sourceLine, declaration.sourceColumn });
        }
    return id;
}

/**
 * @brief 构造带语法角色的 match 模式片段。
 * @param patterns 模式列表节点。
 * @return 包装节点编号；解包和载荷绑定由语义展开完成。
 */
int M_MacroPatternFragment(int patterns)
{
    const int id = M_New(MNode::MacroPatternFragment);
    g_nodes[id].kids = { patterns };
    return id;
}
