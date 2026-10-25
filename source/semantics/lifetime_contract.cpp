/* ---------- 显式生命周期契约：#borrow / #static ---------- */
/**
 * @brief 一个函数的显式生命周期契约。
 *
 * 标记 `#static(a)` 后 a 的来源按 'static 处理；`#borrow(out, [in...])` 声明
 * out 借用 in，`out` 写成 `#ret` 时约束函数的返回值，与 Rust 的 `fn f<'a>(x: &'a T) -> &'a T`
 * 同形：调用点只按声明的那几个实参传播来源，不再保守取全部引用实参的最大值。
 */
struct LifetimeContract
{
    Memory::String name;                    ///< 函数名（调用点按它查询）。
    Memory::Set<Memory::String> statics;    ///< `#static(...)` 声明的来源。
    /** @brief `#borrow(out, [in...])`：out 名 → 来源名列表；`#ret` 单独存。 */
    Memory::Map<Memory::String, Memory::Vector<Memory::String>> borrows;
    Memory::Vector<Memory::String> retSources;  ///< `#borrow(#ret, ...)` 的来源名。
    Memory::Set<Memory::String> retSourcesSet;  ///< 同上，供快速查询。
    Memory::Vector<int> retParams;              ///< 返回来源对应的形参下标，供调用点传播。
    Memory::Vector<int> staticParams;           ///< `#static` 形参的下标，调用点要核对实参。
    int paramCount = 0;                         ///< 形参总数，调用点按实参个数挑重载。
    bool hasRet = false;
    /** @brief `#borrow(#ret, ...)` 的来源里有不存在的名字；报过来源错就别再报返回错。 */
    bool retInvalid = false;
    std::size_t line = 0;
    /**
     * @brief 把契约固定下来后建立快速查询集合。
     * @return 无。
     */
    void seal()
    {
        retSourcesSet.clear();
        Memory::Vector<Memory::String> unique;
        for (const auto &source : retSources)
        {
            if (std::find(unique.begin(), unique.end(), source) != unique.end()) continue;
            unique.push_back(source);
        }
        retSources = unique;
        for (const auto &source : retSources) retSourcesSet.insert(source);
    }
};
/** @brief 函数声明节点编号 → 契约。 */
Memory::Map<int, LifetimeContract> g_lifetimeContracts;
/** @brief 函数名 → 带返回契约的重载（调用点按实参个数挑选）。 */
Memory::Map<Memory::String, Memory::Vector<const LifetimeContract *>> g_lifetimeContractsByName;
/** @brief 当前正在降级的函数的契约；无契约时为空。 */
const LifetimeContract *g_lifetimeCurrent = nullptr;

/** @brief 攒下来的契约违规；和借用事实一样，等全部单态化完成后才报。 */
Memory::Vector<std::pair<Memory::String, std::size_t>> g_lifetimeViolations;
/** @brief 已报过的正文；登记与降级会跨轮重放，同一条只留一次。 */
Memory::Set<Memory::String> g_lifetimeReported;
/**
 * @brief 报告一条生命周期诊断并攒下来。
 * @param message 诊断正文。
 * @param line 源码行号。
 * @return 无。
 */
void LifetimeReport(const Memory::String &message, std::size_t line)
{
    /* 位置由诊断队列统一补上，正文里不再重复写一遍行号。 */
    if (!g_lifetimeReported.insert(message).second) return;
    g_lifetimeViolations.push_back({ message, line });
}
/**
 * @brief 把攒下的契约违规交给统一诊断队列。
 * @return 无。
 */
void ReportLifetimeDiagnostics()
{
    for (const auto &item : g_lifetimeViolations) AddDiagnostic(item.first, item.second);
    g_lifetimeViolations.clear();
}

/**
 * @brief 记下函数体里声明过的局部变量名，供契约核对名字。
 * @param name 源名字。
 * @return 无。
 */
void LifetimeNoteLocalName(const Memory::String &name)
{
    chali_lifetime::g_localNames.insert(name);
}

/**
 * @brief 顺着成员、下标、取地址往下找具名存储的根变量节点。
 * @param id 目标表达式编号。
 * @return 根变量节点编号；没有具名根时返回 -1。
 */
int LifetimeRootVarId(int id)
{
    while (id >= 0)
    {
        const auto &node = N(id);
        if (node.kind == MNode::Var) return id;
        const bool descend = node.kind == MNode::Member || node.kind == MNode::Arrow ||
                             node.kind == MNode::Index || node.kind == MNode::Cast || node.kind == MNode::Un;
        if (!descend || node.kids.empty()) break;
        id = node.kids[0];
    }
    return -1;
}

/**
 * @brief 判断变量是否在最外层作用域（全局）：全局来源天然是 'static。
 * @param name 变量名。
 * @return 是全局变量时为 true。
 */
bool LifetimeIsGlobalName(const Memory::String &name)
{
    if (g_scope.empty()) return false;
    const auto entry = g_scope[0].find(name);
    return entry != g_scope[0].end();
}

/**
 * @brief 判断名字是否被当前函数声明为 'static。
 * @param name 来源名。
 * @return 声明为 'static 时为 true。
 */
bool LifetimeIsStaticName(const Memory::String &name)
{
    if (g_lifetimeCurrent && g_lifetimeCurrent->statics.count(name)) return true;
    return LifetimeIsGlobalName(name);
}

/**
 * @brief 查询函数的返回契约对应的形参下标。
 * @param symbol 被调函数的符号名。
 * @param argumentCount 调用点的实参个数。
 * @return 带契约时返回形参下标表；否则返回空指针。
 */
const LifetimeContract *FindLifetimeContract(const Memory::String &symbol, std::size_t argumentCount);
const Memory::Vector<int> *LifetimeRetParams(const Memory::String &symbol, std::size_t argumentCount)
{
    const LifetimeContract *contract = FindLifetimeContract(symbol, argumentCount);
    if (!contract || !contract->hasRet || contract->retParams.empty()) return nullptr;
    return &contract->retParams;
}

const Memory::Vector<int> *LifetimeStaticParams(const Memory::String &symbol, std::size_t argumentCount);
/**
 * @brief 取契约对象，按实参个数在同名重载里挑一个。
 * @param symbol 被调函数的符号名。
 * @param argumentCount 调用点的实参个数。
 * @return 命中的契约；没有时返回空指针。
 */
const LifetimeContract *FindLifetimeContract(const Memory::String &symbol, std::size_t argumentCount)
{
    if (symbol.empty()) return nullptr;
    const auto found = g_lifetimeContractsByName.find(symbol);
    if (found != g_lifetimeContractsByName.end())
    {
        for (const LifetimeContract *contract : found->second)
            if (contract->paramCount == static_cast<int>(argumentCount)) return contract;
        return nullptr;
    }
    /* 支持模块限定名：去掉 `模块::` 前缀再查一次。 */
    const auto bar = symbol.rfind("::");
    if (bar == Memory::String::npos) return nullptr;
    return FindLifetimeContract(symbol.substr(bar + 2), argumentCount);
}

/**
 * @brief 查询函数要求哪些形参必须是 'static。
 * @param symbol 被调函数的符号名。
 * @param argumentCount 调用点的实参个数。
 * @return 需要 'static 的形参下标表；没有时返回空指针。
 */
const Memory::Vector<int> *LifetimeStaticParams(const Memory::String &symbol, std::size_t argumentCount)
{
    const LifetimeContract *contract = FindLifetimeContract(symbol, argumentCount);
    if (!contract || contract->staticParams.empty()) return nullptr;
    return &contract->staticParams;
}

/**
 * @brief 核对调用点给 `#static` 形参的实参真是 'static 来源。
 *
 * 同一个调用节点可能被求值多次（先当语句、再当值），诊断只报一次。
 *
 * @param callId 调用表达式编号。
 * @param symbol 被调函数的符号名。
 * @param arguments 实参编号列表。
 * @param line 源码行号。
 * @return 无。
 */
/** @brief 已经核对过的调用，避免同一节点重复报同一条诊断。 */
Memory::Set<int> g_lifetimeCheckedCalls;
void CheckLifetimeStaticArguments(int callId, const Memory::String &symbol,
                                  const Memory::Vector<int> &arguments, std::size_t line)
{
    if (!g_lifetimeCheckedCalls.insert(callId).second) return;
    const Memory::Vector<int> *indices = LifetimeStaticParams(symbol, arguments.size());
    if (!indices) return;
    Memory::Vector<Memory::String> offenders;
    for (const int index : *indices)
    {
        if (index < 0 || static_cast<std::size_t>(index) >= arguments.size()) continue;
        if (BorrowSourceDepth(arguments[static_cast<std::size_t>(index)]) == 0u) continue;
        offenders.push_back(NameOfExpr(arguments[static_cast<std::size_t>(index)]));
    }
    if (offenders.empty()) return;
    Memory::String listed;
    for (const auto &name : offenders)
    {
        if (!listed.empty()) listed += "、";
        listed += name;
    }
    LifetimeReport("[CHL1214] " + symbol + " 要求 'static 实参，但 " + listed +
                       " 是局部或形参来源；只能传全局变量或用 'static 存储",
                   line);
}

/**
 * @brief 递归收集表达式里引用来源的根名字。
 *
 * 全局与 `#static` 来源统一记为 `$static`；新分配的引用记为 `$fresh`。
 * 没有契约的调用保守地取全部引用实参的来源，有契约的只取声明的那几个。
 *
 * @param id 表达式编号。
 * @param names 收集到的来源名集合。
 * @param guard 递归层数上限，防止深表达式爆栈。
 * @return 无。
 */
void CollectLifetimeOrigins(int id, Memory::Set<Memory::String> &names, int guard)
{
    if (id < 0 || guard > 12) return;
    const auto node = N(id);
    if (node.kind == MNode::Var)
    {
        if (!HasBorrowedValue(RawExprType(id))) return;
        if (LifetimeIsStaticName(node.s)) { names.insert("$static"); return; }
        names.insert(node.s);
        return;
    }
    if (node.kind == MNode::NewPtr) { names.insert("$fresh"); return; }
    if (node.kind == MNode::Un && node.s == "&" && !node.kids.empty())
    {
        CollectLifetimeOrigins(node.kids[0], names, guard + 1);
        return;
    }
    if (node.kind == MNode::Cast && !node.kids.empty())
    {
        CollectLifetimeOrigins(node.kids[0], names, guard + 1);
        return;
    }
    if ((node.kind == MNode::Member || node.kind == MNode::Arrow || node.kind == MNode::Index) && !node.kids.empty())
    {
        CollectLifetimeOrigins(node.kids[0], names, guard + 1);
        return;
    }
    if (node.kind == MNode::Call && node.kids.size() >= 2u)
    {
        const auto callee = N(node.kids[0]);
        const auto arguments = N(node.kids[1]).kids;
        if (const Memory::Vector<int> *indices = LifetimeRetParams(CallSymbol(callee), arguments.size()))
        {
            for (const int index : *indices)
                if (index >= 0 && static_cast<std::size_t>(index) < arguments.size())
                    CollectLifetimeOrigins(arguments[static_cast<std::size_t>(index)], names, guard + 1);
            return;
        }
        for (const int argument : arguments)
            if (HasBorrowedValue(RawExprType(argument)))
                CollectLifetimeOrigins(argument, names, guard + 1);
        return;
    }
}

/**
 * @brief 拼出契约要求的来源清单文本，供诊断直接给出可抄的写法。
 * @param sources 来源名列表。
 * @return `#borrow(#ret, [a, b])` 形式的文本。
 */
Memory::String LifetimeSourceList(const Memory::Vector<Memory::String> &sources)
{
    Memory::String text = "[";
    for (std::size_t index = 0; index < sources.size(); ++index)
    {
        if (index != 0u) text += ", ";
        text += sources[index];
    }
    return text + "]";
}
/**
 * @brief 拼出 `#borrow(out, [来源])` 形式的契约文本，供诊断直接给出可抄的写法。
 * @param target out 名；写成 `#ret` 时指返回值。
 * @param sources 来源名列表。
 * @return 可抄的契约文本。
 */
Memory::String LifetimeContractText(const Memory::String &target, const Memory::Vector<Memory::String> &sources)
{
    return "#borrow(" + target + ", " + LifetimeSourceList(sources) + ")";
}

/**
 * @brief 核对一条 return 的引用来源是否落在契约声明内。
 * @param value 返回值表达式编号。
 * @param line 源码行号。
 * @return 无。
 */
void CheckLifetimeReturn(int value, std::size_t line)
{
    /* 契约的来源本身写错了时只报来源那一条：返回检查的建议里会带着不存在的名字。 */
    if (!g_lifetimeCurrent || !g_lifetimeCurrent->hasRet || g_lifetimeCurrent->retInvalid || value < 0) return;
    Memory::Set<Memory::String> origins;
    CollectLifetimeOrigins(value, origins, 0);
    Memory::Vector<Memory::String> undeclared;
    for (const auto &origin : origins)
    {
        if (origin == "$static" || origin == "$fresh") continue;
        if (g_lifetimeCurrent->retSourcesSet.count(origin)) continue;
        undeclared.push_back(origin);
    }
    if (undeclared.empty()) return;
    Memory::String listed;
    for (std::size_t index = 0; index < undeclared.size(); ++index)
    {
        if (index != 0u) listed += "、";
        listed += undeclared[index];
    }
    Memory::Vector<Memory::String> all = g_lifetimeCurrent->retSources;
    for (const auto &name : undeclared) all.push_back(name);
    LifetimeReport("[CHL1211] 返回值借用了未声明的来源 " + listed + "；请声明 " +
                       LifetimeContractText(chali_lifetime::RetName(), all) +
                       "，或把该来源标为 #static（它若是局部变量则不能返回）",
                   line);
}

/**
 * @brief 核对一次保存引用的写入是否来自契约声明的来源。
 * @param target 被写入的目标表达式编号。
 * @param value 写入的值表达式编号。
 * @param line 源码行号。
 * @return 无。
 */
void CheckLifetimeStore(int target, int value, std::size_t line)
{
    if (!g_lifetimeCurrent || target < 0) return;
    /* 契约里写的是源码名，降级改名（重名时加 $序号）后的名字对不上，一律按源码名查。 */
    const int rootId = LifetimeRootVarId(target);
    if (rootId < 0 || !BorrowRootVariable(target)) return;
    const Memory::String targetName = N(rootId).s;
    const auto declared = g_lifetimeCurrent->borrows.find(targetName);
    if (declared == g_lifetimeCurrent->borrows.end()) return;
    Memory::Set<Memory::String> origins;
    CollectLifetimeOrigins(value, origins, 0);
    Memory::Vector<Memory::String> undeclared;
    for (const auto &origin : origins)
    {
        if (origin == "$static" || origin == "$fresh") continue;
        if (std::find(declared->second.begin(), declared->second.end(), origin) != declared->second.end()) continue;
        undeclared.push_back(origin);
    }
    if (undeclared.empty()) return;
    Memory::String listed;
    for (std::size_t index = 0; index < undeclared.size(); ++index)
    {
        if (index != 0u) listed += "、";
        listed += undeclared[index];
    }
    Memory::Vector<Memory::String> all = declared->second;
    for (const auto &name : undeclared) all.push_back(name);
    LifetimeReport("[CHL1212] " + targetName + " 按契约只借用 " + LifetimeSourceList(declared->second) +
                       "，却保存了来自 " + listed + " 的引用；请改契约 " +
                       LifetimeContractText(targetName, all) + "，或改从已声明的来源取引用",
                   line);
}

/**
 * @brief 核对契约里的名字都真实存在。
 * @param contract 契约。
 * @param line 报告位置。
 * @return 无。
 */
void ValidateLifetimeNames(const LifetimeContract &contract)
{
    /* 作用域编号：0 = 全局，1 = 形参，2 及以上 = 越来越深的局部块。 */
    Memory::Map<Memory::String, std::size_t> home;
    for (std::size_t index = 0; index < g_scope.size(); ++index)
        for (const auto &entry : g_scope[index]) home[entry.first] = index;
    /* 函数体的局部作用域此时已经出栈，那些名字在降级时逐个攒到了这里。 */
    for (const auto &name : chali_lifetime::g_localNames) home.insert({ name, 2u });
    Memory::Vector<Memory::String> missing, localStatics;
    for (const auto &name : contract.statics)
    {
        const auto found = home.find(name);
        if (found == home.end()) { missing.push_back(name); continue; }
        if (found->second >= 2u) localStatics.push_back(name);
    }
    for (const auto &entry : contract.borrows)
    {
        if (!home.count(entry.first)) missing.push_back(entry.first);
        for (const auto &source : entry.second)
            if (!home.count(source)) missing.push_back(source);
    }
    for (const auto &source : contract.retSources)
        if (!home.count(source)) missing.push_back(source);
    Memory::String listed;
    for (const auto &name : missing)
    {
        if (!listed.empty()) listed += "、";
        listed += name;
    }
    if (!listed.empty())
        LifetimeReport("[CHL1210] 生命周期契约引用了不存在的名字 " + listed + "（只能是本函数的形参或局部变量）", contract.line);
    if (!localStatics.empty())
    {
        Memory::String names;
        for (const auto &name : localStatics)
        {
            if (!names.empty()) names += "、";
            names += name;
        }
        LifetimeReport("[CHL1213] #static(" + names + ") 标的是局部变量；局部存储出块就没了，只有全局或形参才可能是 'static",
                       contract.line);
    }
}

/**
 * @brief 从文法挂上来的指令列表建立契约。
 * @param directives 指令列表节点编号。
 * @param name 函数名。
 * @param line 报告位置。
 * @return 建立好的契约。
 */
void PublishLifetimeContractName(int functionId, const Memory::String &name);

LifetimeContract BuildLifetimeContract(int directives, const Memory::String &name, std::size_t line)
{
    LifetimeContract contract;
    contract.name = name;
    contract.line = line;
    const auto list = N(directives);
    /* 指令自己带位置时用指令的位置：契约写错该指向那行 #borrow / #static。 */
    if (list.sourceLine != 0u) contract.line = list.sourceLine;
    for (const int directiveId : list.kids)
    {
        const auto directive = N(directiveId);
        if (directive.s == "#static")
        {
            for (const int nameId : directive.kids)
                contract.statics.insert(g_names[static_cast<std::size_t>(nameId)]);
            continue;
        }
        if (directive.s != "#borrow") continue;
        const Memory::String target = g_names[static_cast<std::size_t>(directive.i)];
        Memory::Vector<Memory::String> sources;
        for (const int sourceId : directive.kids)
            sources.push_back(g_names[static_cast<std::size_t>(sourceId)]);
        if (target == chali_lifetime::RetName())
        {
            contract.hasRet = true;
            for (const auto &source : sources) contract.retSources.push_back(source);
        }
        else
        {
            Memory::Vector<Memory::String> &slot = contract.borrows[target];
            for (const auto &source : sources) slot.push_back(source);
        }
    }
    return contract;
}

/**
 * @brief 在函数体分析前登记契约，并把它按名字公开给调用点。
 * @param functionId 函数声明节点编号。
 * @param name 函数名。
 * @param params 形参列表（按声明顺序）。
 * @param line 报告位置。
 * @return 无。
 */
void LoadLifetimeContract(int functionId, const Memory::String &name,
                          const Memory::Vector<MParam> &params, std::size_t line)
{
    const auto found = chali_lifetime::g_directiveNodes.find(functionId);
    if (found == chali_lifetime::g_directiveNodes.end()) return;
    LifetimeContract contract = BuildLifetimeContract(found->second, name, line);
    contract.paramCount = static_cast<int>(params.size());
    for (std::size_t i = 0; i < params.size(); ++i)
        if (contract.statics.count(params[i].source)) contract.staticParams.push_back(static_cast<int>(i));
    for (const auto &source : contract.retSources)
    {
        int index = -1;
        for (std::size_t i = 0; i < params.size(); ++i)
            if (params[i].source == source) { index = static_cast<int>(i); break; }
        if (index < 0)
        {
            LifetimeReport("[CHL1210] #borrow(#ret, ...) 的来源 " + source + " 不是本函数的形参，返回值只能借用形参", line);
            contract.retInvalid = true;
            continue;
        }
        contract.retParams.push_back(index);
    }
    const auto stored = g_lifetimeContracts.insert({ functionId, contract }).first;
    stored->second.seal();
    PublishLifetimeContractName(functionId, name);
}

/**
 * @brief 把已有契约再公开到一个名字下（模块限定名与裸名各一份）。
 * @param functionId 函数声明节点编号。
 * @param name 调用点会用到的符号名。
 * @return 无。
 */
void PublishLifetimeContractName(int functionId, const Memory::String &name)
{
    const auto found = g_lifetimeContracts.find(functionId);
    if (found == g_lifetimeContracts.end()) return;
    auto &list = g_lifetimeContractsByName[name];
    if (std::find(list.begin(), list.end(), &found->second) == list.end()) list.push_back(&found->second);
}

/**
 * @brief 进入函数体时切到该函数的契约。
 * @param functionId 函数声明节点编号。
 * @return 无。
 */
void EnterLifetimeContract(int functionId)
{
    const auto found = g_lifetimeContracts.find(functionId);
    g_lifetimeCurrent = found == g_lifetimeContracts.end() ? nullptr : &found->second;
}

/**
 * @brief 函数体降级结束时核对契约里的名字；作用域此时还在。
 * @return 无。
 */
void ValidateLifetimeCurrent()
{
    if (g_lifetimeCurrent) ValidateLifetimeNames(*g_lifetimeCurrent);
}

/**
 * @brief 保存并切换当前契约上下文的守卫，离开函数体时恢复。
 */
struct LifetimeFunctionGuard
{
    const LifetimeContract *previous;
    Memory::Set<Memory::String> previousNames;
    /**
     * @brief 进入函数体。
     * @param functionId 函数声明节点编号。
     */
    explicit LifetimeFunctionGuard(int functionId) : previous(g_lifetimeCurrent), previousNames(chali_lifetime::g_localNames)
    {
        chali_lifetime::g_localNames.clear();
        EnterLifetimeContract(functionId);
    }
    /**
     * @brief 恢复进入前的契约上下文与局部变量名。
     */
    ~LifetimeFunctionGuard()
    {
        g_lifetimeCurrent = previous;
        chali_lifetime::g_localNames = previousNames;
    }
};
/** @brief 下一个要降级的函数的声明节点编号；-1 表示没有契约。 */
int g_lifetimePendingFunction = -1;
