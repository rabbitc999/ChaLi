/**
 * @file chali_aot_ast.hpp
 * @brief 直接 LLVM 后端的 AST 遍历实现；与标量发射器处于同一私有编译单元。
 */

/**
 * @brief 从 AST 类型取得后端的标量存储类别。
 * @param type 已检查的类型。
 * @return 浮点、字符串或整数句柄类别。
 */
Kind astKind(const ast::TypePtr &type)
{
    if (type && type->kind == ast::Type::Kind::Float) return Kind::Float;
    if (type && type->kind == ast::Type::Kind::String) return Kind::String;
    return Kind::Int;
}

/**
 * @brief 将静态数据初始化表达式编码为共享数据布局器接受的字面量。
 * @param node 常量节点。
 * @param type 对应的存储类型。
 * @return 字面量文本；动态初始化由语言函数体负责。
 */
Memory::String astDataLiteral(const ast::NodePtr &node, const ast::TypePtr &type)
{
    if (!node) return "0";
    if (node->kind == ast::Node::Kind::FloatLit)
    {
        Memory::OutputStringStream out;
        out << std::setprecision(17) << node->floatValue;
        auto text = out.str();
        if (text.find_first_of(".eE") == Memory::String::npos) text += ".0";
        return text;
    }
    if (node->kind == ast::Node::Kind::IntLit || node->kind == ast::Node::Kind::BoolLit)
    {
        auto text = Memory::toString(node->intValue);
        if (type && type->kind == ast::Type::Kind::Float) text += ".0";
        return text;
    }
    if (node->kind == ast::Node::Kind::NullLit) return "0";
    return "0";
}

/**
 * @brief 生成保留映射代次及完整范围检查的 LLVM 地址操作原语。
 * @return 地址视图、上下文失效入口及可内联的物理地址解析函数。
 */
Text astAddressPrimitive()
{
    return R"LLVM(
@chali_address_view = internal thread_local global { i64, i64, ptr, ptr, i64 } zeroinitializer
declare ptr @chali_host_address_view(i64, i64, ptr nocapture)
define void @chali_reset_address_view() {
entry:
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 3)
  ret void
}
define internal ptr @chali_primitive_address(i64 %address, i64 %count) alwaysinline {
entry:
  %revision_pointer = load ptr, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 3)
  %has_revision = icmp ne ptr %revision_pointer, null
  br i1 %has_revision, label %check, label %slow
check:
  %revision = load atomic i64, ptr %revision_pointer acquire, align 8
  %stamp = load i64, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 4)
  %same_revision = icmp eq i64 %revision, %stamp
  %enabled = icmp ne i64 %revision, 0
  %live = and i1 %same_revision, %enabled
  %begin = load i64, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 0)
  %size = load i64, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 1)
  %offset = sub i64 %address, %begin
  %inside = icmp ult i64 %offset, %size
  %remaining = sub i64 %size, %offset
  %fits = icmp ule i64 %count, %remaining
  %range = and i1 %inside, %fits
  %valid = and i1 %live, %range
  br i1 %valid, label %fast, label %slow
fast:
  %base = load ptr, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 2)
  %physical = getelementptr i8, ptr %base, i64 %offset
  ret ptr %physical
slow:
  %resolved = call ptr @chali_host_address_view(i64 %address, i64 %count, ptr @chali_address_view)
  ret ptr %resolved
}
)LLVM";
}

/**
 * @brief 识别只承担地址或标量存储转发的标准包装函数。
 * @param rawLabel 函数的完整标签。
 * @return 属于标准操作包装时返回 true。
 */
bool astPrimitiveWrapper(const Text &rawLabel)
{
    Text label = rawLabel;
    const Text destinationPrefix = "__chali_into$";
    if (label.rfind(destinationPrefix, 0u) == 0u)
    {
        const auto separator = label.find('$', destinationPrefix.size());
        if (separator != Text::npos) label = label.substr(separator + 1u);
    }
    static const char *const prefixes[] = {
        "std::lang::dereference__", "std::lang::barePointer__", "std::lang::pointerFromAddress__",
        "std::lang::__pointerCast__", "std::lang::readAt__", "std::lang::borrowAt__",
        "std::lang::initAt__", "std::lang::takeAt__", "std::lang::memcpy__", "std::lang::memset__",
        "BarePtr__", "std::lang::BarePtr__", "std::lang::__bootList", "std::lang::__bootMap",
        "std::lang::__bootSet", "std::lang::__bootString", "std::lang::__bootAlgorithm"
    };
    for (const auto *prefix : prefixes)
        if (label.rfind(prefix, 0u) == 0u) return true;
    return (label.rfind("RawStorage__", 0u) == 0u || label.rfind("std::lang::RawStorage__", 0u) == 0u)
           && (label.find(".pointer") != Text::npos || label.find(".capacity") != Text::npos
               || label.find(".isInitialized") != Text::npos);
}

/**
 * @brief 按调用依赖自底向上计算展开规模，排除递归环、间接调用及其调用者。
 * @param state 已登记函数及可达集合的 LLVM 生成状态。
 * @note 只分析 AST，不生成四元式；重复调用按实际次数累计成本，避免短函数链展开失控。
 */
void analyzeAstInlining(Translator &state)
{
    if (!state.inlineHints || state.stepChecks) return;
    constexpr std::size_t shortBudget = 48, primitiveBudget = 256, overflow = primitiveBudget + 1;
    std::map<Text, std::size_t> remaining, ownCost, expandedCost;
    std::map<Text, std::vector<Text>> calls, callers;
    for (const auto &entry : state.astFunctions)
    {
        if (!state.reachable.count(entry.first)) continue;
        auto &dependencies = remaining[entry.first];
        auto &cost = ownCost[entry.first];
        std::vector<ast::NodePtr> pending{ entry.second->body };
        while (!pending.empty())
        {
            const auto node = pending.back();
            pending.pop_back();
            if (!node) continue;
            using N = ast::Node::Kind;
            if (node->kind != N::Block && node->kind != N::Declare && node->kind != N::Label)
                cost = std::min(overflow, cost + 1u);
            if (node->kind == N::CallPointer) ++dependencies;  // 无法证明间接调用不回到当前函数。
            if (node->kind == N::Call)
            {
                const auto callee = textOf(node->text);
                if (state.astFunctions.count(callee))
                {
                    calls[entry.first].push_back(callee);
                    callers[callee].push_back(entry.first);
                    ++dependencies;
                }
                else
                    cost = std::min(overflow, cost + 8u);  // 原生边界也占用展开预算。
            }
            for (const auto &child : node->kids) pending.push_back(child);
        }
    }
    std::vector<Text> ready;
    for (const auto &entry : remaining)
        if (!entry.second) ready.push_back(entry.first);
    while (!ready.empty())
    {
        const auto label = ready.back();
        ready.pop_back();
        auto cost = ownCost.at(label);
        for (const auto &callee : calls[label]) cost = std::min(overflow, cost + expandedCost.at(callee));
        expandedCost[label] = cost;
        if (cost <= shortBudget || (state.rawIntrinsics && astPrimitiveWrapper(label) && cost <= primitiveBudget))
            state.astInlineFunctions.insert(label);
        for (const auto &caller : callers[label])
            if (--remaining.at(caller) == 0u) ready.push_back(caller);
    }
}

/**
 * @brief 直接从已检查 AST 建立签名、可达函数、局部身份及静态数据，不生成四元式。
 * @param request 当前可写请求，接收数据映像与布局。
 * @param state LLVM 生成状态。
 */
void prepareAst(Request &request, Translator &state)
{
    const auto &program = *request.syntax;
    for (const auto &type : program.types) state.astTypes[type->name] = type;
    for (const auto &entry : program.variables)
    {
        state.astTypes[entry.first] = entry.second;
        state.globals[textOf(entry.first)] = "g_" + decimal(static_cast<long long>(state.globals.size())) + "_" + sanitize(entry.first);
        state.kinds[textOf(entry.first)] = astKind(entry.second);
    }
    for (const auto &entry : program.constants)
    {
        state.astTypes[entry.first] = entry.second;
        state.globals[textOf(entry.first)] = "g_" + decimal(static_cast<long long>(state.globals.size())) + "_" + sanitize(entry.first);
        state.kinds[textOf(entry.first)] = astKind(entry.second);
    }
    for (const auto &external : request.externs)
    {
        state.externsByLabel[textOf(external.label)] = &external;
        state.astTypes[external.label] = external.resultType == "double" || external.resultType == "float" ? ast::Type::f64() : ast::Type::i64();
    }
    for (const auto &function : program.functions)
    {
        if (state.functionByLabel.count(textOf(function.quadLabel()))) continue;
        Function target;
        target.label = function.quadLabel();
        target.llvmName = "f_" + state.use(target.label);
        if (target.label == "main" || target.label == "__chali_library_init" || target.label == "__chali_library_finish") target.llvmName = "f_" + target.label;
        target.returnsFloat = astKind(function.result) == Kind::Float;
        target.returnsString = astKind(function.result) == Kind::String;
        target.exported = std::find(request.options.exports.begin(), request.options.exports.end(), target.label) != request.options.exports.end();
        const bool typedExport = std::any_of(request.abiExports.begin(), request.abiExports.end(), [&](const auto &entry) { return entry.label == target.label; });
        if (typedExport)
        {
            target.exported = true;
            target.llvmName = "__chali_impl_" + sanitize(target.label);
        }
        else if (target.exported)
            target.llvmName = "f_" + sanitize(target.label.rfind("program::", 0u) == 0u ? target.label.substr(9u) : target.label);
        state.astFunctions[textOf(target.label)] = &function;
        state.astTypes[target.label] = function.result;
        state.astTypes[function.name] = function.result;
        auto &symbols = state.astSymbols[textOf(target.label)];
        const auto declare = [&](const Memory::String &name, const ast::TypePtr &type) {
            const auto identity = "__ast_" + Memory::toString(state.functions.size()) + "_" + name;
            symbols[name] = identity;
            state.kinds[textOf(identity)] = astKind(type);
        };
        if (!function.owner.empty())
        {
            declare("self", ast::Type::i64());
            target.parameters.push_back(symbols["self"]);
        }
        for (const auto &parameter : function.parameters)
        {
            declare(parameter.first, parameter.second);
            target.parameters.push_back(symbols[parameter.first]);
        }
        std::function<void(const ast::NodePtr &)> collect = [&](const ast::NodePtr &node) {
            if (!node) return;
            if (node->kind == ast::Node::Kind::Declare) declare(node->text, node->declared);
            for (const auto &child : node->kids) collect(child);
        };
        collect(function.body);
        state.functionByLabel[textOf(target.label)] = state.functions.size();
        state.functions.push_back(std::move(target));
    }
    if (!request.options.shared && !state.functionByLabel.count("main")) state.fail("AOT：找不到入口 main");
    std::vector<std::string> pending{ "main", "__chali_library_init", "__chali_library_finish" };
    for (const auto &name : request.options.exports) pending.push_back(textOf(name));
    for (const auto &entry : request.abiExports) pending.push_back(textOf(entry.label));
    while (!pending.empty())
    {
        const auto label = pending.back();
        pending.pop_back();
        if (!state.reachable.insert(label).second) continue;
        const auto function = state.astFunctions.find(label);
        if (function == state.astFunctions.end()) continue;
        std::function<void(const ast::NodePtr &)> visit = [&](const ast::NodePtr &node) {
            if (!node) return;
            if (node->kind == ast::Node::Kind::Call) pending.push_back(textOf(node->text));
            if (node->kind == ast::Node::Kind::StringLit && state.astFunctions.count(textOf(node->text)))
            {
                state.addressedFunctions.insert(textOf(node->text));
                pending.push_back(textOf(node->text));
            }
            for (const auto &child : node->kids) visit(child);
        };
        visit(function->second->body);
    }
    analyzeAstInlining(state);
    ast::CodegenResult data;
    std::map<std::string, Memory::String> pool;
    std::map<const ast::Node *, Memory::String> names;
    std::function<void(const ast::NodePtr &)> collectData = [&](const ast::NodePtr &node) {
        if (!node) return;
        using N = ast::Node::Kind;
        if (node->kind == N::StringLit || node->kind == N::ByteArrayLit || node->kind == N::ArrayNew || node->kind == N::StructNew || node->kind == N::EnumNew)
        {
            const bool string = node->kind == N::StringLit;
            const auto key = std::to_string(static_cast<int>(node->kind)) + ":" + textOf(node->text) + ":" + textOf(node->typeName);
            Memory::String name;
            if (string || node->kind == N::ByteArrayLit)
            {
                const auto found = pool.find(key);
                if (found != pool.end()) name = found->second;
            }
            if (name.empty())
            {
                name = string && !node->typeName.empty() ? node->typeName : "ast_data_" + Memory::toString(data.entries.size());
                auto type = string ? ast::Type::stringType() : node->declared;
                Memory::String body;
                if (string)
                    body = "\"" + node->text + "\"";
                else if (node->kind == N::ByteArrayLit)
                {
                    body = "[";
                    for (unsigned char byte : node->text)
                    {
                        if (body.size() > 1u) body += ",";
                        body += Memory::toString(byte);
                    }
                    body += "]";
                }
                else
                {
                    if (!type)
                    {
                        const auto found = state.astTypes.find(node->typeName.empty() ? node->text : node->typeName);
                        type = found == state.astTypes.end() ? ast::Type::array(ast::Type::i64(), node->text) : found->second;
                    }
                    body = node->kind == N::StructNew ? "{" : node->kind == N::EnumNew ? node->text + "(" :
                                                                                         "[";
                    for (std::size_t index = 0; index < node->kids.size(); ++index)
                    {
                        if (index) body += ",";
                        const auto slotType = type && node->kind == N::StructNew && index < type->fields.size() ? type->fields[index].second : type ? type->element :
                                                                                                                                                      nullptr;
                        body += astDataLiteral(node->kids[index], slotType);
                    }
                    body += node->kind == N::StructNew ? "}" : node->kind == N::EnumNew ? ")" :
                                                                                          "]";
                }
                data.entries.push_back({ name, type ? type : ast::Type::i64(), body });
                pool[key] = name;
            }
            names[node.get()] = name;
        }
        for (const auto &child : node->kids) collectData(child);
    };
    for (const auto &entry : state.astFunctions)
        if (state.reachable.count(entry.first)) collectData(entry.second->body);
    request.dataBytes = data.exportDataBinary();
    request.dataLayout.clear();
    for (const auto &line : data.exportDataLayout()) request.dataLayout += line + "\n";
    loadLayout(state);
    for (const auto &entry : names)
    {
        state.astData[entry.first] = state.dataOffsets.at(textOf(entry.second));
        if (entry.first->kind == ast::Node::Kind::StringLit && state.addressedFunctions.count(textOf(entry.first->text)))
            state.functionAddresses[state.astData[entry.first]] = entry.first->text;
    }
}

/**
 * @brief 从 AST 直接产生 LLVM 基本块和 SSA 表达式，复用已验证的标量 ABI 发射器。
 */
struct AstBuilder : Builder
{
    Memory::Map<Memory::String, ast::TypePtr> environment;
    const ast::Function &syntax;
    std::size_t expressions = 0, blocks = 0, argumentCapacity = 1;
    bool terminated = false;
    std::map<std::size_t, DebugLine> debugLines;
    std::size_t debugBlocks = 1;
    ast::NodePtr lastLocation;  ///< 合成控制流节点缺少位置时，沿用本函数最近的源码位置。

    /**
     * @brief 为当前 AST 函数登记独立局部存储和表达式类型环境。
     * @param translation 本次 LLVM 模块状态。
     * @param current 当前函数签名。
     */
    AstBuilder(Translator &translation, const Function &current) : Builder(translation, current), environment(translation.astTypes), syntax(*translation.astFunctions.at(textOf(current.label)))
    {
        for (const auto &entry : state.astSymbols.at(textOf(function.label))) locals.insert(textOf(entry.second));
        for (const auto &parameter : syntax.parameters) environment[parameter.first] = parameter.second;
        if (!syntax.owner.empty()) environment["self"] = environment.count(syntax.owner) ? environment[syntax.owner] : ast::Type::i64();
        std::function<void(const ast::NodePtr &)> visit = [&](const ast::NodePtr &node) {
            if (!node) return;
            if (node->kind == ast::Node::Kind::Declare) environment[node->text] = node->declared;
            if (node->kind == ast::Node::Kind::Call || node->kind == ast::Node::Kind::CallPointer) argumentCapacity = std::max(argumentCapacity, node->kids.size() + 1u);
            for (const auto &child : node->kids) visit(child);
        };
        visit(syntax.body);
    }

    /**
     * @brief 取得当前函数内名字的独立身份，未声明为局部的名字引用全局。
     * @param name AST 中的变量名。
     * @return LLVM 存储符号的稳定身份。
     */
    Memory::String symbol(const Memory::String &name)
    {
        const auto &symbols = state.astSymbols.at(textOf(function.label));
        const auto found = symbols.find(name);
        return found == symbols.end() ? name : found->second;
    }

    /**
     * @brief 将一个表达式绑定到 SSA 操作数，不创建中间指令表或内存槽。
     * @param value LLVM 操作数及动态类别。
     * @return 仅供本发射器引用的 SSA 句柄。
     */
    Memory::String valueToken(Value value)
    {
        const auto name = "__llvm_value_" + Memory::toString(expressions++);
        state.kinds[textOf(name)] = value.type == "double" ? Kind::Float : Kind::Int;
        astValues[textOf(name)] = std::move(value);
        return name;
    }

    /**
     * @brief 创建当前函数内不重复的基本块名。
     * @return LLVM 标签名字。
     */
    Text blockName() { return "ast_block_" + std::to_string(blocks++); }

    /**
     * @brief 结束当前基本块并进入指定块，保留已终止路径的正确控制流。
     * @param name 目标块标签。
     */
    void enter(const Text &name)
    {
        if (!terminated) body += "  br label %" + name + "\n";
        body += name + ":\n";
        terminated = false;
        checkedAddresses.clear();
    }

    /**
     * @brief 对当前 AST 节点记录原生源码位置及可选安全点。
     * @param node 当前节点。
     * @param controlPoint 合成循环或标签没有源码行时仍须生成安全点。
     */
    void location(const ast::NodePtr &node, bool controlPoint = false)
    {
        const auto source = node && node->sourceLine ? node : controlPoint && state.stepChecks ? lastLocation : nullptr;
        if (source)
        {
            lastLocation = source;
            if (state.request.options.debug)
            {
                const auto index = debugBlocks++;
                enter("q" + std::to_string(index));
                debugLines[index] = { textOf(source->sourceFile), source->sourceLine };
            }
            const auto file = state.sourceFiles.emplace(textOf(source->sourceFile), state.sourceFiles.size()).first;
            body += "  store ptr @" + textOf(function.llvmName) + ", ptr @chali_source_function\n";
            body += "  store ptr @source_file" + std::to_string(file->second) + ", ptr @chali_source_file\n";
            body += "  store i64 " + std::to_string(source->sourceLine) + ", ptr @chali_source_line\n";
        }
        if (state.stepChecks && (source || controlPoint)) call("chali_host_tick", "void", { { "i64", "1" } });
    }

    /**
     * @brief 将标量转换成语言条件。
     * @param value 已求值表达式。
     * @return LLVM i1 条件。
     */
    Value condition(Value value)
    {
        value = convert(value, value.type == "double" ? "double" : "i64");
        return instruction("i1", Text(value.type == "double" ? "fcmp une double " : "icmp ne i64 ") + value.text + (value.type == "double" ? ", 0.0" : ", 0"));
    }

    /**
     * @brief 在共享运行时已验证的字节范围中装入或写入标量。
     * @param address 逻辑地址。
     * @param type 语言存储类型。
     * @param stored 非空时写入该值，空时读取。
     * @return 读取结果；写入时返回零。
     */
    Value memory(Value address, const ast::TypePtr &type, const Value *stored = nullptr)
    {
        const bool real = astKind(type) == Kind::Float;
        const bool byte = type && type->kind == ast::Type::Kind::Uint && type->bits == 8;
        const Text storage = real ? "double" : byte ? "i8" :
                                                      "i64";
        const bool cache = state.cacheAddresses && !state.stepChecks && !state.request.options.debug;
        const auto pointer = call(cache ? "chali_primitive_address" : "chali_host_address", "ptr",
                                  { convert(address, "i64"), { "i64", byte ? "1" : "8" } });
        if (stored)
        {
            const auto value = convert(*stored, storage, byte);
            body += "  store " + storage + " " + value.text + ", ptr " + pointer.text + ", align 1\n";
            return { "i64", "0" };
        }
        return convert(instruction(storage, "load " + storage + ", ptr " + pointer.text + ", align 1"), real ? "double" : "i64", byte);
    }

    /**
     * @brief 直接求取成员、下标或步长节点的逻辑地址。
     * @param node AST 左值节点。
     * @return 地址值。
     */
    Value address(const ast::NodePtr &node)
    {
        const auto base = convert(expression(node->kids[0]), "i64");
        if (node->kind == ast::Node::Kind::Member)
        {
            const auto type = ast::inferType(node->kids[0], environment);
            std::size_t index = 0;
            for (; index < type->fields.size(); ++index)
                if (type->fields[index].first == node->text) break;
            return instruction("i64", "add i64 " + base.text + ", " + std::to_string(index * 8u));
        }
        const auto index = convert(expression(node->kids[1]), "i64");
        const auto type = ast::inferType(node, environment);
        const Text width = node->kind == ast::Node::Kind::Offset ? textOf(node->text) : type && type->kind == ast::Type::Kind::Uint && type->bits == 8 ? "1" :
                                                                                                                                                         "8";
        const auto offset = instruction("i64", "mul i64 " + index.text + ", " + width);
        return instruction("i64", "add i64 " + base.text + ", " + offset.text);
    }

    /**
     * @brief 直接遍历表达式并生成 SSA 值，调用实参在构造调用之前全部求值。
     * @param node 表达式节点。
     * @return 对应的 LLVM 操作数。
     */
    Value expression(const ast::NodePtr &node)
    {
        if (!node) return { "i64", "0" };
        if (terminated) enter(blockName());
        using N = ast::Node::Kind;
        const auto type = ast::inferType(node, environment);
        switch (node->kind)
        {
        case N::IntLit:
        case N::BoolLit: return { "i64", std::to_string(node->intValue) };
        case N::NullLit: return { "i64", "0" };
        case N::FloatLit:
        {
            std::uint64_t bits;
            std::memcpy(&bits, &node->floatValue, 8);
            char hex[32];
            std::snprintf(hex, sizeof(hex), "0x%016llX", static_cast<unsigned long long>(bits));
            return { "double", hex };
        }
        case N::Var: return read(symbol(node->text));
        case N::StringLit:
        case N::ByteArrayLit:
        case N::ArrayNew:
        case N::StructNew:
        case N::EnumNew:
            return { "i64", std::to_string(state.astData.at(node.get())) };
        case N::Sequence:
            for (std::size_t index = 0; index + 1 < node->kids.size(); ++index) statement(node->kids[index]);
            return node->kids.empty() ? Value{ "i64", "0" } : expression(node->kids.back());
        case N::Binary:
        {
            const auto leftType = ast::inferType(node->kids[0], environment);
            const auto rightType = ast::inferType(node->kids[1], environment);
            auto left = expression(node->kids[0]);
            if (node->text == "&&" || node->text == "||")
            {
                const bool conjunction = node->text == "&&";
                const auto test = condition(left);
                const auto shortcut = blockName(), rhs = blockName(), done = blockName();
                body += "  br i1 " + test.text + ", label %" + (conjunction ? rhs : shortcut) + ", label %" + (conjunction ? shortcut : rhs) + "\n";
                terminated = true;
                enter(shortcut);
                body += "  br label %" + done + "\n";
                terminated = true;
                enter(rhs);
                const auto right = condition(expression(node->kids[1]));
                const auto rhsExit = blockName();
                enter(rhsExit);
                body += "  br label %" + done + "\n";
                terminated = true;
                enter(done);
                return convert(instruction("i1", "phi i1 [ " + Text(conjunction ? "false" : "true") + ", %" + shortcut + " ], [ " + right.text + ", %" + rhsExit + " ]"), "i64");
            }
            auto right = expression(node->kids[1]);
            auto operandType = leftType;
            if (leftType->kind == ast::Type::Kind::Float || rightType->kind == ast::Type::Kind::Float)
            {
                operandType = ast::Type::make(ast::Type::Kind::Float, std::max(leftType->kind == ast::Type::Kind::Float ? leftType->bits : 0, rightType->kind == ast::Type::Kind::Float ? rightType->bits : 0));
                left = convert(left, "double", leftType->kind == ast::Type::Kind::Uint);
                right = convert(right, "double", rightType->kind == ast::Type::Kind::Uint);
            }
            else if ((leftType->kind == ast::Type::Kind::Int || leftType->kind == ast::Type::Kind::Uint) && (rightType->kind == ast::Type::Kind::Int || rightType->kind == ast::Type::Kind::Uint))
                operandType = ast::Type::make(leftType->kind == ast::Type::Kind::Uint || rightType->kind == ast::Type::Kind::Uint ? ast::Type::Kind::Uint : ast::Type::Kind::Int, std::max(leftType->bits, rightType->bits));
            const auto output = valueToken({ astKind(type) == Kind::Float ? "double" : "i64", "0" });
            location(node);
            if (!arithmetic(node->text, operandType->quadType(), valueToken(left), valueToken(right), output)) state.fail("AST LLVM：不支持的运算 " + node->text);
            return read(output);
        }
        case N::Unary:
        {
            const auto operand = expression(node->kids[0]);
            if (node->text == "!") return convert(instruction("i1", "xor i1 " + condition(operand).text + ", true"), "i64");
            if (node->text == "+") return operand;
            const auto output = valueToken({ operand.type, operand.type == "double" ? "0.0" : "0" });
            arithmetic(node->text == "-" ? Memory::String("neg") : Memory::String("not"), ast::inferType(node->kids[0], environment)->quadType(), valueToken(operand), "0", output);
            return read(output);
        }
        case N::Cast:
        {
            const auto target = node->declared ? node->declared : ast::Type::i64();
            const auto sourceType = ast::inferType(node->kids[0], environment);
            const bool unsignedValue = target->kind == ast::Type::Kind::Float ? sourceType->kind == ast::Type::Kind::Uint : target->kind == ast::Type::Kind::Uint;
            const auto value = expression(node->kids[0]);
            const bool floating = target->kind == ast::Type::Kind::Float;
            const auto width = widthOf(target->quadType());
            const auto narrowed = convert(value, floating ? target->bits == 32 ? "float" : "double" : "i" + std::to_string(width.bits), unsignedValue);
            return convert(narrowed, floating ? "double" : "i64", unsignedValue);
        }
        case N::Call:
        case N::CallPointer:
        {
            const bool indirect = node->kind == N::CallPointer;
            Value pointer{ "i64", "0" };
            if (indirect) pointer = expression(node->kids[0]);
            std::vector<Value> values;
            for (std::size_t index = indirect ? 1u : 0u; index < node->kids.size(); ++index) values.push_back(expression(node->kids[index]));
            location(node);
            arguments.clear();
            for (std::size_t index = 0; index < values.size(); ++index) arguments.push_back({ index < node->names.size() ? node->names[index] : Memory::String("arg"), values[index] });
            const auto output = valueToken({ astKind(type) == Kind::Float ? "double" : "i64", astKind(type) == Kind::Float ? "0.0" : "0" });
            if (!indirect)
            {
                pendingFormatValid = node->text == "__std_print" && !node->kids.empty() && node->kids[0]->kind == N::StringLit;
                if (pendingFormatValid) pendingFormat = textOf(node->kids[0]->text);
                emitCall(node->text, output);
            }
            else
            {
                for (std::size_t index = 0; index < values.size(); ++index)
                {
                    auto value = values[index];
                    if (value.type == "double") value = instruction("i64", "bitcast double " + value.text + " to i64");
                    const auto slot = instruction("ptr", "getelementptr i64, ptr %indirect_arguments, i64 " + std::to_string(index));
                    body += "  store i64 " + convert(value, "i64").text + ", ptr " + slot.text + "\n";
                }
                auto result = call("chali_invoke", "i64", { convert(pointer, "i64"), { "i64", std::to_string(values.size()) }, { "ptr", "%indirect_arguments" } });
                result.floating = call("chali_result_float", "i8", {}).text;
                write(output, result);
            }
            arguments.clear();
            pendingFormatValid = false;
            return read(output);
        }
        case N::LoadInt:
        {
            const auto at = expression(node->kids[0]);
            location(node);
            return memory(at, type);
        }
        case N::Index:
        case N::Member:
        {
            const auto at = address(node);
            location(node);
            return memory(at, type);
        }
        case N::Offset: return address(node);
        case N::Alloc: location(node); return call("chali_alloc", "i64", { convert(expression(node->kids[0]), "i64") });
        case N::CheckIndex:
        case N::CheckPointer:
        {
            const auto index = expression(node->kids[0]);
            const auto limit = expression(node->kids[1]);
            location(node);
            if (state.request.options.checked)
            {
                std::vector<Value> values{ convert(index, "i64"), convert(limit, "i64") };
                if (node->kind == N::CheckPointer) values.push_back({ "i64", std::to_string(node->intValue) });
                call(node->kind == N::CheckPointer ? "chali_host_check_pointer" : "chali_check", "void", values);
            }
            return index;
        }
        case N::MemoryCopy:
        case N::MemorySet:
        {
            const auto destination = expression(node->kids[0]);
            const auto source = expression(node->kids[1]);
            const auto bytes = expression(node->kids[2]);
            location(node);
            const bool copy = node->kind == N::MemoryCopy;
            call("chali_host_memory_prepare", "void", { { "i64", copy ? "7" : "8" }, convert(destination, "i64"), convert(source, "i64"), convert(bytes, "i64"), { "ptr", "%raw_memory_output" }, { "ptr", "%raw_memory_input" } });
            const auto output = instruction("ptr", "load ptr, ptr %raw_memory_output");
            if (copy)
                call("llvm.memcpy.p0.p0.i64", "void", { output, instruction("ptr", "load ptr, ptr %raw_memory_input"), convert(bytes, "i64"), { "i1", "false" } });
            else
                call("llvm.memset.p0.i64", "void", { output, convert(source, "i8"), convert(bytes, "i64"), { "i1", "false" } });
            return { "i64", "0" };
        }
        default: state.fail("AST LLVM：不支持的表达式节点 " + Memory::toString(static_cast<int>(node->kind))); return { "i64", "0" };
        }
    }

    /**
     * @brief 将已完成所有权展开的语句直接映射为 LLVM 控制流。
     * @param node 语句节点。
     */
    void statement(const ast::NodePtr &node)
    {
        if (!node) return;
        using N = ast::Node::Kind;
        if (node->kind == N::Block)
        {
            for (const auto &child : node->kids) statement(child);
            return;
        }
        if (node->kind == N::Label)
        {
            enter("ast_label_" + textOf(state.use(node->text)));
            location(node, true);
            return;
        }
        if (terminated) enter(blockName());
        location(node);
        switch (node->kind)
        {
        case N::Declare: return;
        case N::ExprStmt:
            if (!node->kids.empty()) expression(node->kids[0]);
            return;
        case N::Assign:
        {
            const auto value = expression(node->kids.back());
            if (node->kids.size() == 1)
                write(symbol(node->text), value);
            else if (node->kids[0]->kind == N::Var)
                write(symbol(node->kids[0]->text), value);
            else
                memory(address(node->kids[0]), ast::inferType(node->kids[0], environment), &value);
            return;
        }
        case N::StoreAt:
        {
            const auto value = expression(node->kids[1]);
            const auto at = expression(node->kids[0]);
            const auto width = widthOf(node->text);
            memory(at, ast::Type::make(width.float_ ? ast::Type::Kind::Float : width.unsigned_ ? ast::Type::Kind::Uint :
                                                                                                 ast::Type::Kind::Int,
                                       width.bits),
                   &value);
            return;
        }
        case N::If:
        {
            const auto test = condition(expression(node->kids[0]));
            const auto yes = blockName(), no = blockName(), done = blockName();
            body += "  br i1 " + test.text + ", label %" + yes + ", label %" + no + "\n";
            terminated = true;
            enter(yes);
            if (node->kids.size() > 1) statement(node->kids[1]);
            if (!terminated)
            {
                body += "  br label %" + done + "\n";
                terminated = true;
            }
            enter(no);
            if (node->kids.size() > 2) statement(node->kids[2]);
            if (!terminated)
            {
                body += "  br label %" + done + "\n";
                terminated = true;
            }
            enter(done);
            return;
        }
        case N::While:
        case N::DoWhile:
        {
            const auto testBlock = blockName(), loop = blockName(), done = blockName();
            if (node->kind == N::While)
            {
                enter(testBlock);
                location(node, true);
                const auto test = condition(expression(node->kids[0]));
                body += "  br i1 " + test.text + ", label %" + loop + ", label %" + done + "\n";
                terminated = true;
                enter(loop);
                statement(node->kids[1]);
                if (!terminated)
                {
                    body += "  br label %" + testBlock + "\n";
                    terminated = true;
                }
            }
            else
            {
                enter(loop);
                location(node, true);
                statement(node->kids[0]);
                enter(testBlock);
                const auto test = condition(expression(node->kids[1]));
                body += "  br i1 " + test.text + ", label %" + loop + ", label %" + done + "\n";
                terminated = true;
            }
            enter(done);
            return;
        }
        case N::Jump:
            body += "  br label %ast_label_" + textOf(state.use(node->text)) + "\n";
            terminated = true;
            return;
        case N::Return:
        {
            auto value = expression(node->kids.empty() ? nullptr : node->kids[0]);
            const bool floating = function.returnsFloat;
            value = convert(value, floating ? "double" : "i64");
            body += "  store i8 " + Text(floating ? "1" : value.floating.empty() ? "0" :
                                                                                   value.floating)
                    + ", ptr @chali_return_float\n  ret " + value.type + " " + value.text + "\n";
            terminated = true;
            return;
        }
        case N::Drop:
        case N::Free: call("chali_owned_drop", "void", { convert(expression(node->kids[0]), "i64") }); return;
        case N::Putc: call("chali_put_char", "void", { convert(expression(node->kids[0]), "i64") }); return;
        default: expression(node); return;
        }
    }

    /**
     * @brief 生成当前 AST 函数；只有形参和具名声明分配帧存储。
     * @return LLVM 函数定义。
     */
    Text generateAst()
    {
        Text signature = "define ";
        const bool typedExport = std::any_of(state.request.abiExports.begin(), state.request.abiExports.end(), [&](const auto &entry) { return entry.label == function.label; });
        if (state.target.windows && function.exported && !typedExport) signature += "dllexport ";
        if (!function.exported) signature += "internal ";
        signature += Text(function.returnsFloat ? "double" : "i64") + " @" + textOf(function.llvmName) + "(";
        for (std::size_t index = 0; index < function.parameters.size(); ++index)
        {
            if (index) signature += ", ";
            signature += type(function.parameters[index]) + " %p" + std::to_string(index);
        }
        signature += ") ";
        if (!function.exported && !state.addressedFunctions.count(textOf(function.label))
            && state.astInlineFunctions.count(textOf(function.label))) signature += "alwaysinline ";
        signature += "uwtable section \"chali_code\" {\nentry:\n";
        signature += "  %indirect_arguments = alloca [" + std::to_string(argumentCapacity) + " x i64]\n  %native_argument_types = alloca [" + std::to_string(argumentCapacity) + " x i8]\n";
        signature += "  %raw_memory_output = alloca ptr\n  %raw_memory_input = alloca ptr\n  %collection_result_kind = alloca i8\n";
        for (const auto &name : locals)
        {
            const Memory::String token(name.c_str());
            signature += "  " + pointer(token) + " = alloca " + type(token) + "\n  store " + type(token) + " " + (type(token) == "double" ? "0.0" : "0") + ", ptr " + pointer(token) + "\n";
            if (type(token) == "i64") signature += "  " + tagPointer(token) + " = alloca i8\n  store i8 0, ptr " + tagPointer(token) + "\n";
        }
        for (std::size_t index = 0; index < function.parameters.size(); ++index) signature += "  store " + type(function.parameters[index]) + " %p" + std::to_string(index) + ", ptr " + pointer(function.parameters[index]) + "\n";
        if (state.request.options.debug)
        {
            signature += "  br label %q0\n";
            body = "q0:\n";
        }
        if (function.label == "main" || function.label == "__chali_library_init")
            for (const auto &constant : state.request.syntax->constants)
                write(constant.first, read(constant.second->meta.empty() ? Memory::String("0") : constant.second->meta));
        statement(syntax.body);
        if (!terminated) body += "  store i8 0, ptr @chali_return_float\n  ret " + Text(function.returnsFloat ? "double 0.0" : "i64 0") + "\n";
        const auto definition = signature + body + "}\n";
        if (!state.request.options.debug) return definition;
        std::vector<DebugVariable> variables;
        for (const auto &entry : state.astSymbols.at(textOf(function.label)))
        {
            const auto parameter = std::find(function.parameters.begin(), function.parameters.end(), entry.second);
            variables.push_back({ textOf(entry.first), pointer(entry.second), type(entry.second) == "double", parameter == function.parameters.end() ? 0u : static_cast<std::size_t>(parameter - function.parameters.begin() + 1u) });
        }
        return state.debugInfo.annotate(definition, textOf(function.label), textOf(function.llvmName), debugLines, variables);
    }
};
