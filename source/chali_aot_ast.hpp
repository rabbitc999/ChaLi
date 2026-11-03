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

#include "chali_aot_numeric_list.hpp"

/**
 * @brief 生成保留映射代次及完整范围检查的 LLVM 地址操作原语。
 * @return 四项线程局部地址视图、上下文失效入口及可内联的物理地址解析函数。
 */
Text astAddressPrimitive()
{
    const auto *disabled = std::getenv("CHALI_NO_AOT_MULTI_ADDRESS_CACHE");
    const unsigned entries = disabled && *disabled ? 1u : 4u;
    const auto *disablePromotion = std::getenv("CHALI_NO_AOT_ADDRESS_PROMOTION");
    const bool promotion = entries > 1u && !(disablePromotion && *disablePromotion);
    Text result = R"LLVM(
@chali_address_view = internal thread_local global { i64, i64, ptr, ptr, i64 } zeroinitializer
@chali_address_view1 = internal thread_local global { i64, i64, ptr, ptr, i64 } zeroinitializer
@chali_address_view2 = internal thread_local global { i64, i64, ptr, ptr, i64 } zeroinitializer
@chali_address_view3 = internal thread_local global { i64, i64, ptr, ptr, i64 } zeroinitializer
@chali_address_next = internal thread_local global i64 0
@chali_address_secondary_hits = internal thread_local global i64 0
declare ptr @chali_host_address_view(i64, i64, ptr nocapture)
define void @chali_reset_address_view() {
entry:
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view1, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view2, i32 0, i32 3)
  store ptr null, ptr getelementptr ({ i64, i64, ptr, ptr, i64 }, ptr @chali_address_view3, i32 0, i32 3)
  store i64 0, ptr @chali_address_next
  store i64 0, ptr @chali_address_secondary_hits
  store ptr null, ptr getelementptr ({ i64, ptr, ptr, i64 }, ptr @chali_list_view, i32 0, i32 2)
  ret void
}
define internal ptr @chali_cached_view_address(ptr %view, i64 %address, i64 %count) alwaysinline {
entry:
  %revision_slot = getelementptr { i64, i64, ptr, ptr, i64 }, ptr %view, i32 0, i32 3
  %revision_pointer = load ptr, ptr %revision_slot
  %has_revision = icmp ne ptr %revision_pointer, null
  br i1 %has_revision, label %check, label %slow
check:
  %revision = load atomic i64, ptr %revision_pointer acquire, align 8
  %stamp_slot = getelementptr { i64, i64, ptr, ptr, i64 }, ptr %view, i32 0, i32 4
  %stamp = load i64, ptr %stamp_slot
  %same_revision = icmp eq i64 %revision, %stamp
  %enabled = icmp ne i64 %revision, 0
  %live = and i1 %same_revision, %enabled
  %begin = load i64, ptr %view
  %size_slot = getelementptr { i64, i64, ptr, ptr, i64 }, ptr %view, i32 0, i32 1
  %size = load i64, ptr %size_slot
  %offset = sub i64 %address, %begin
  %inside = icmp ult i64 %offset, %size
  %remaining = sub i64 %size, %offset
  %fits = icmp ule i64 %count, %remaining
  %range = and i1 %inside, %fits
  %valid = and i1 %live, %range
  br i1 %valid, label %fast, label %slow
fast:
  %base_slot = getelementptr { i64, i64, ptr, ptr, i64 }, ptr %view, i32 0, i32 2
  %base = load ptr, ptr %base_slot
  %physical = getelementptr i8, ptr %base, i64 %offset
  ret ptr %physical
slow:
  ret ptr null
}
)LLVM";
    result += "define internal ptr @chali_primitive_address(i64 %address, i64 %count) alwaysinline {\nentry:\n  br label %lookup0\n";
    for (unsigned index = 0; index < entries; ++index)
    {
        const Text number = Memory::toString(index);
        const Text view = "@chali_address_view" + (index == 0u ? Text() : number);
        const Text next = index + 1u == entries ? "slow" : "lookup" + Memory::toString(index + 1u);
        result += "lookup" + number + ":\n  %cached" + number + " = call ptr @chali_cached_view_address(ptr " + view + ", i64 %address, i64 %count)\n";
        result += "  %found" + number + " = icmp ne ptr %cached" + number + ", null\n  br i1 %found" + number + ", label %hit" + number + ", label %" + next + "\n";
        result += "hit" + number + ":\n";
        if (promotion && index != 0u)
        {
            // 首项热路径不写计数；每三十二次后续命中才摊销交换一个完整视图。
            result += "  %hits" + number + " = load i64, ptr @chali_address_secondary_hits\n";
            result += "  %increment" + number + " = add i64 %hits" + number + ", 1\n";
            result += "  %period" + number + " = and i64 %increment" + number + ", 31\n";
            result += "  store i64 %period" + number + ", ptr @chali_address_secondary_hits\n";
            result += "  %promote" + number + " = icmp eq i64 %period" + number + ", 0\n";
            result += "  br i1 %promote" + number + ", label %swap" + number + ", label %return" + number + "\n";
            result += "swap" + number + ":\n";
            result += "  %old" + number + " = load { i64, i64, ptr, ptr, i64 }, ptr @chali_address_view, align 8\n";
            result += "  %new" + number + " = load { i64, i64, ptr, ptr, i64 }, ptr " + view + ", align 8\n";
            result += "  store { i64, i64, ptr, ptr, i64 } %new" + number + ", ptr @chali_address_view, align 8\n";
            result += "  store { i64, i64, ptr, ptr, i64 } %old" + number + ", ptr " + view + ", align 8\n";
            result += "  br label %return" + number + "\nreturn" + number + ":\n";
        }
        result += "  ret ptr %cached" + number + "\n";
    }
    result += "slow:\n  %target = call ptr @chali_address_target()\n";
    result += "  %resolved = call ptr @chali_host_address_view(i64 %address, i64 %count, ptr %target)\n  ret ptr %resolved\n}\n";
    result += "define internal ptr @chali_address_target() alwaysinline {\nentry:\n";
    if (entries == 1u)
        result += "  ret ptr @chali_address_view\n";
    else
        result += R"LLVM(
  %next = load i64, ptr @chali_address_next
  %first = icmp eq i64 %next, 0
  %second = icmp eq i64 %next, 1
  %third = icmp eq i64 %next, 2
  %tail = select i1 %third, ptr @chali_address_view2, ptr @chali_address_view3
  %rest = select i1 %second, ptr @chali_address_view1, ptr %tail
  %target = select i1 %first, ptr @chali_address_view, ptr %rest
  %following = add i64 %next, 1
  %wrapped = and i64 %following, 3
  store i64 %wrapped, ptr @chali_address_next
  ret ptr %target
)LLVM";
    result += R"LLVM(
}
define internal ptr @chali_data_address(i64 %address, i64 %count, ptr %base, i64 %size) alwaysinline {
entry:
  %present = icmp ne ptr %base, null
  %inside = icmp ult i64 %address, %size
  %remaining = sub i64 %size, %address
  %fits = icmp ule i64 %count, %remaining
  %range = and i1 %inside, %fits
  %valid = and i1 %present, %range
  br i1 %valid, label %fast, label %slow
fast:
  %physical = getelementptr i8, ptr %base, i64 %address
  ret ptr %physical
slow:
  %resolved = call ptr @chali_primitive_address(i64 %address, i64 %count)
  ret ptr %resolved
}
)LLVM";
    return result;
}

/**
 * @brief 生成基于活跃堆视图的字符串描述符读取，不把数据区字面量解释为描述符。
 * @return 仅在完整三十二字节视图和缓冲标记匹配时读取字段的 LLVM 内核。
 */
Text astStringStoragePrimitive()
{
    const auto *disabled = std::getenv("CHALI_NO_AOT_MULTI_ADDRESS_CACHE");
    const unsigned entries = disabled && *disabled ? 1u : 4u;
    // 宿主数据区视图始终从逻辑零开始；堆分配视图使用实际分配首。
    Text result = R"LLVM(
define internal ptr @chali_cached_string_header(ptr %view, i64 %address) alwaysinline {
entry:
  %begin = load i64, ptr %view
  %heap = icmp ne i64 %begin, 0
  br i1 %heap, label %check, label %miss
check:
  %header = call ptr @chali_cached_view_address(ptr %view, i64 %address, i64 32)
  ret ptr %header
miss:
  ret ptr null
}
define internal i64 @chali_primitive_string_storage(i64 %operation, i64 %address) alwaysinline {
entry:
  br label %lookup0
)LLVM";
    for (unsigned index = 0; index < entries; ++index)
    {
        const Text number = Memory::toString(index);
        const Text view = "@chali_address_view" + (index == 0u ? Text() : number);
        const Text next = index + 1u == entries ? "slow" : "lookup" + Memory::toString(index + 1u);
        result += "lookup" + number + ":\n  %header" + number + " = call ptr @chali_cached_string_header(ptr " + view + ", i64 %address)\n";
        result += "  %found" + number + " = icmp ne ptr %header" + number + ", null\n  br i1 %found" + number + ", label %hit" + number + ", label %" + next + "\n";
        result += "hit" + number + ":\n  br label %inspect\n";
    }
    result += "inspect:\n  %header = phi ptr ";
    for (unsigned index = 0; index < entries; ++index)
    {
        const Text number = Memory::toString(index);
        if (index != 0u) result += ", ";
        result += "[ %header" + number + ", %hit" + number + " ]";
    }
    result += R"LLVM(
  %marker = load i64, ptr %header, align 1
  %buffered = icmp eq i64 %marker, -1
  br i1 %buffered, label %select, label %slow
select:
  switch i64 %operation, label %slow [ i64 0, label %length
                                     i64 1, label %data
                                     i64 2, label %capacity
                                     i64 3, label %kind ]
length:
  %length_slot = getelementptr i8, ptr %header, i64 8
  br label %read
data:
  %data_slot = getelementptr i8, ptr %header, i64 24
  br label %read
capacity:
  %capacity_slot = getelementptr i8, ptr %header, i64 16
  br label %read
read:
  %slot = phi ptr [ %length_slot, %length ], [ %data_slot, %data ], [ %capacity_slot, %capacity ]
  %value = load i64, ptr %slot, align 1
  %valid = icmp sge i64 %value, 0
  br i1 %valid, label %success, label %slow
success:
  ret i64 %value
kind:
  ret i64 1
slow:
  %target = call ptr @chali_address_target()
  %fallback = call i64 @chali_host_string_storage_view(i64 %operation, i64 %address, ptr %target)
  ret i64 %fallback
}
)LLVM";
    return result;
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
        if (!request.options.libraryModule.empty()) target.llvmName = "f_" + state.identifier(target.label, "libfn");
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
    std::vector<std::string> pending;
    if (request.options.libraryModule.empty()) pending = { "main", "__chali_library_init", "__chali_library_finish" };
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
    std::size_t debugEntryEnd = 0;  ///< 初始入口标签末尾，只有尚无正文的函数入口可以跳过发布。
    bool terminated = false;
    bool snapshotData = false;  ///< 无不透明调用的循环可在本函数帧内保存数据区视图。
    Value dataBase, dataSize;   ///< 只在当前帧内有效的基址和完整字节数。
    std::map<std::size_t, DebugLine> debugLines;
    std::map<std::size_t, DebugScope> debugScopes;
    std::map<Text, DebugVariable> debugDeclarations;
    std::map<Text, Text> debugAvailability;
    std::size_t debugScope = 0;
    /** @brief 真实绑定的逻辑存储及其源码有效区间，镜像只在调试路径刷新。 */
    struct DebugBox
    {
        Memory::String storage;
        ast::TypePtr valueType;
        std::size_t scope = 0;
    };
    std::map<Text, DebugBox> debugBoxes;
    bool refreshingDebugBindings = false;
    std::size_t debugBlocks = 1;
    std::size_t profileIndex = 0;  ///< 当前函数中计数位置的稳定遍历编号。
    ast::NodePtr lastLocation;     ///< 合成控制流节点缺少位置时，沿用本函数最近的源码位置。

    /** @brief 与运行时 value_text 同格式的只读逻辑布局，不含地址或用户回调。 */
    struct LayoutShape
    {
        char kind = 'X';
        std::string name;
        std::size_t offset = 0, count = 0, width = 8;
        std::vector<LayoutShape> children;
    };

    /**
     * @brief 编码一个布局节点，格式与运行时 value_text::encode 一致。
     * @param shape 待编码布局。
     * @return 长度前缀文本。
     */
    static std::string encodeLayoutShape(const LayoutShape &shape)
    {
        const auto number = [](std::size_t value) { return std::to_string(value) + ":"; };
        std::string out(1, shape.kind);
        out += number(shape.name.size()) + shape.name + number(shape.offset) + number(shape.count) + number(shape.width) + number(0)
               + number(shape.children.size());
        for (const auto &child : shape.children) out += encodeLayoutShape(child);
        return out;
    }

    /**
     * @brief 从检查后的类型推导可解码布局；未知或过深的部分保留为不可解码节点。
     * @param type 已检查类型。
     * @param depth 当前嵌套深度。
     * @param remaining 当前布局尚可生成的节点数；空指针表示建立独立预算。
     * @return 只读布局节点。
     */
    LayoutShape layoutOf(const ast::TypePtr &type, std::size_t depth = 0, std::size_t *remaining = nullptr)
    {
        LayoutShape out;
        std::size_t budget = 4096;
        if (!remaining) remaining = &budget;
        if (!type) return out;
        if (depth >= 32 || !*remaining) { out.name = "超过布局观察预算"; return out; }
        --*remaining;
        const Memory::String prefix = "debug-layout:";
        if (type->meta.rfind(prefix, 0) == 0)
        {
            const auto encoded = type->meta.substr(prefix.size());
            stackvm::value_text::Shape shape;
            std::size_t at = 0, remaining = 4096;
            if (stackvm::value_text::decode(encoded, at, shape, remaining) && at == encoded.size())
                return debugShape(shape);
            return out;
        }
        using K = ast::Type::Kind;
        out.name = textOf(type->name);
        switch (type->kind)
        {
        case K::Bool:
            out.kind = 'b';
            out.width = 1;
            break;
        case K::Int:
        case K::Uint:
            out.kind = type->kind == K::Int ? 'i' : 'u';
            out.width = static_cast<std::size_t>(std::max(1, type->bits / 8));
            break;
        case K::Float:
            out.kind = 'f';
            out.width = 8;  // 数值帧以 double 保存 f32 与 f64。
            break;
        case K::String:
            out.kind = 's';
            out.width = 8;
            break;
        case K::Ref:
            out.kind = 'R';
            out.width = 8;
            if (type->element) out.children.push_back(layoutOf(type->element, depth + 1, remaining));
            break;
        case K::Array:
            out.kind = 'a';
            out.width = type->element && type->element->kind == K::Uint && type->element->bits == 8 ? 1 : 8;
            out.children.push_back(layoutOf(type->element, depth + 1, remaining));
            break;
        case K::Struct:
        {
            // 枚举在运行期是带 __tag 与 __vN_载荷 槽的普通记录；变体名由前端登记在类型上。
            const bool enumeration = !type->variants.empty();
            out.width = type->fields.size() * 8;
            if (enumeration)
            {
                out.kind = 'N';
                for (std::size_t variant = 0; variant < type->variants.size(); ++variant)
                {
                    LayoutShape node;
                    node.kind = 'V';
                    node.name = textOf(type->variants[variant].first);
                    node.count = variant;
                    const auto prefix = "__v" + std::to_string(variant) + "_";
                    for (std::size_t index = 0; index < type->fields.size(); ++index)
                    {
                        const auto field = textOf(type->fields[index].first);
                        if (field.rfind(prefix, 0) != 0) continue;
                        auto child = layoutOf(type->fields[index].second, depth + 1, remaining);
                        child.name = field.substr(prefix.size());
                        child.offset = index * 8;
                        node.children.push_back(std::move(child));
                    }
                    out.children.push_back(std::move(node));
                }
                break;
            }
            out.kind = 'S';
            for (std::size_t index = 0; index < type->fields.size(); ++index)
            {
                auto child = layoutOf(type->fields[index].second, depth + 1, remaining);
                child.name = textOf(type->fields[index].first);
                child.offset = index * 8;
                out.children.push_back(std::move(child));
            }
            break;
        }
        default: break;  // 枚举与容器需要运行时身份，留到后续布局扩展。
        }
        return out;
    }

    /**
     * @brief 将已验证的只读值布局转换为调试目录节点，忽略格式化回调。
     * @param shape 容器及元素的静态布局。
     * @return 不含用户方法的调试节点。
     */
    static LayoutShape debugShape(const stackvm::value_text::Shape &shape)
    {
        LayoutShape out;
        out.kind = shape.kind;
        out.name = textOf(shape.name);
        out.offset = shape.offset;
        out.count = shape.count;
        out.width = shape.width;
        for (const auto &child : shape.children) out.children.push_back(debugShape(child));
        return out;
    }

    /**
     * @brief 编码变量的逻辑布局。
     * @param type 已检查类型。
     * @return 布局文本；空类型返回空文本。
     */
    std::string encodedLayout(const ast::TypePtr &type)
    {
        if (!type) return {};
        return encodeLayoutShape(layoutOf(type));
    }

    /**
     * @brief 从检查后的基础类型描述实际帧表示，复合逻辑句柄不伪装成机器指针。
     * @param variable 待补齐的变量描述。
     * @param valueType 检查后的类型。
     * @return 无。
     */
    void debugType(DebugVariable &variable, const ast::TypePtr &valueType)
    {
        if (!valueType) return;
        using K = ast::Type::Kind;
        variable.logicalAddress = valueType->kind == K::Struct || valueType->kind == K::Array || valueType->kind == K::String
                                  || (valueType->kind == K::Ref && valueType->element
                                      && (valueType->element->kind == K::Struct || valueType->element->kind == K::Array
                                          || valueType->element->kind == K::String || valueType->element->kind == K::Bool
                                          || valueType->element->kind == K::Int || valueType->element->kind == K::Uint
                                          || valueType->element->kind == K::Float));
        variable.layout = state.debugInfo.registerLayout(encodedLayout(valueType));
        variable.bits = 64;
        variable.encoding = "DW_ATE_unsigned";
        switch (valueType->kind)
        {
        case K::Bool:
            variable.typeName = "bool";
            variable.bits = 8;
            variable.encoding = "DW_ATE_boolean";
            break;
        case K::Int:
            variable.typeName = "int" + std::to_string(valueType->bits);
            variable.bits = valueType->bits;
            variable.encoding = "DW_ATE_signed";
            break;
        case K::Uint:
            variable.typeName = valueType->name == "char" ? "char" : "uint" + std::to_string(valueType->bits);
            variable.bits = valueType->bits;
            if (valueType->name == "char") variable.encoding = "DW_ATE_unsigned_char";
            break;
        case K::Float:
            variable.typeName = "float" + std::to_string(valueType->bits);
            variable.encoding = "DW_ATE_float";  // 前端数值帧当前以 double 保存 f32 与 f64。
            break;
        case K::String: variable.typeName = "String"; break;
        case K::Struct: variable.typeName = textOf(valueType->name); break;
        case K::Ref: variable.typeName = "ChaLi reference"; break;
        case K::Array: variable.typeName = "ChaLi array"; break;
        default: variable.typeName = "ChaLi handle"; break;
        }
        if (valueType->meta.rfind("debug-layout:", 0) == 0) variable.typeName = layoutOf(valueType).name;
    }

    /**
     * @brief 打开用户块或声明后的有效区间，合成清理块沿用父区间。
     * @param source 最接近的用户源码节点。
     * @return 无。
     */
    void openDebugScope(const ast::NodePtr &source)
    {
        const auto position = source && source->sourceLine ? source : lastLocation;
        const auto id = debugScopes.size() + 1u;
        DebugLine origin;
        if (position) origin = { textOf(position->sourceFile), position->sourceLine, position->sourceColumn, id };
        debugScopes.emplace(id, DebugScope{ debugScope, origin });
        debugScope = id;
    }
    /**
     * @brief 将共享宏来源登记一次，并给当前指令保存原定义位置。
     * @param source 指令的源码节点。
     * @return 当前词法区间和展开来源。
     */
    DebugLine debugLocation(const ast::NodePtr &source)
    {
        DebugLine result{ textOf(source->sourceFile), source->sourceLine, source->sourceColumn, debugScope };
        if (!source->debugMacro || !source->debugMacro->frame) return result;
        const auto &origin = *source->debugMacro;
        result.macro = origin.frame->id;
        result.definitionFile = textOf(origin.definition.file);
        result.definitionLine = origin.definition.line;
        result.definitionColumn = origin.definition.column;
        const auto position = [](const ast::DeclarationSource &value) { return DebugLine{ textOf(value.file), value.line, value.column }; };
        auto frame = origin.frame;
        while (frame)
        {
            DebugMacroFrame descriptor;
            descriptor.id = frame->id;
            descriptor.parent = frame->parent ? frame->parent->id : -1;
            descriptor.name = textOf(frame->name);
            descriptor.call = position(frame->call);
            descriptor.invocation = position(frame->invocation);
            descriptor.definition = position(frame->definition);
            state.debugInfo.recordMacro(std::move(descriptor));
            frame = frame->parent;
        }
        return result;
    }

    /**
     * @brief 在源码停点之前从真实绑定刷新帧镜像，不执行用户方法或创建语言对象。
     * @return 无；发布构建不调用此路径。
     */
    void refreshDebugBindings()
    {
        if (refreshingDebugBindings || terminated || debugBoxes.empty()) return;
        refreshingDebugBindings = true;
        for (const auto &entry : debugBoxes)
        {
            auto scope = debugScope;
            while (scope && scope != entry.second.scope) scope = debugScopes.at(scope).parent;
            if (scope != entry.second.scope) continue;
            const auto address = convert(read(symbol(entry.second.storage)), "i64");
            const auto test = instruction("i1", "icmp ne i64 " + address.text + ", 0");
            const auto valid = blockName(), done = blockName();
            body += "  br i1 " + test.text + ", label %" + valid + ", label %" + done + "\n";
            terminated = true;
            enter(valid);
            const auto value = expression(ast::Node::loadFrom(ast::Node::var(entry.second.storage), entry.second.valueType));
            write(symbol(Memory::String(entry.first.c_str())), value);
            body += "  br label %" + done + "\n";
            terminated = true;
            enter(done);
        }
        refreshingDebugBindings = false;
    }

    /**
     * @brief 在已经求值的 AST 操作旁记录计数，不重新计算用户实参。
     * @param event 操作类别。
     * @param node 当前操作节点。
     * @param bytes 已求值的请求字节数，零表示不适用。
     * @return 无。
     */
    void profile(const Text &event, const ast::NodePtr &node, Value bytes = { "i64", "0" })
    {
        if (!state.profileOwnership) return;
        const auto source = node && node->sourceLine ? node : lastLocation;
        const Text site = event + "\t" + textOf(function.label) + "\t" + (source ? textOf(source->sourceFile) + ":" + std::to_string(source->sourceLine) : Text("<generated>:0")) + "\tast=" + std::to_string(profileIndex++);
        const auto index = state.profileSites.emplace(site, state.profileSites.size()).first->second;
        call("chali_backend_profile_event", "void", { { "ptr", "@backend_profile_site" + std::to_string(index) }, convert(bytes, "i64") });
    }

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
        bool loop = false, memoryAccess = false, stable = true;
        Memory::Set<Memory::String> seenLabels;
        std::function<void(const ast::NodePtr &)> visit = [&](const ast::NodePtr &node) {
            if (!node) return;
            using N = ast::Node::Kind;
            loop = loop || node->kind == N::While || node->kind == N::DoWhile;
            if (node->kind == N::Label) seenLabels.insert(node->text);
            if (node->kind == N::Jump && seenLabels.count(node->text)) loop = true;
            memoryAccess = memoryAccess || node->kind == N::LoadInt || node->kind == N::Index ||
                           node->kind == N::Member || node->kind == N::StoreAt;
            switch (node->kind)
            {
            case N::Call:
                // 引用检查不会改变上下文；panic 不返回，不存在快照后的继续执行。
                if (node->text != "__std_reference" && node->text != "__std_panic") stable = false;
                break;
            case N::Alloc: case N::Free: case N::Drop: case N::CallPointer: case N::MethodCall:
            case N::Lambda: case N::MemoryCopy: case N::MemorySet: case N::DebugStorageLifetime: case N::Read: case N::Write:
            case N::Print: case N::Putc: stable = false; break;
            default: break;
            }
            if (node->kind == ast::Node::Kind::Declare) environment[node->text] = node->declared;
            if (node->kind == ast::Node::Kind::Call || node->kind == ast::Node::Kind::CallPointer) argumentCapacity = std::max(argumentCapacity, node->kids.size() + 1u);
            for (const auto &child : node->kids) visit(child);
        };
        visit(syntax.body);
        if (state.request.options.debug)
            for (const auto &parameter : syntax.parameters)
            {
                const auto origin = syntax.parameterSources.find(parameter.first);
                if (origin != syntax.parameterSources.end() && !origin->second.storage.empty())
                    debugBoxes.emplace(textOf(parameter.first), DebugBox{ origin->second.storage, parameter.second, 0u });
            }
        const bool inlineBody = !function.exported && !state.addressedFunctions.count(textOf(function.label)) &&
                                state.astInlineFunctions.count(textOf(function.label));
        snapshotData = state.request.options.libraryModule.empty() && state.dataSnapshots && state.cacheAddresses && state.rawIntrinsics && state.referencePrimitives && !state.stepChecks && !state.request.options.debug && !inlineBody && loop && memoryAccess && stable;
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
                const bool publish = body.size() > debugEntryEnd;
                if (!refreshingDebugBindings) refreshDebugBindings();
                // 镜像刷新和汇合也会形成空块；只有函数最初的空入口才没有需要重新发布的状态。
                if (publish) body += "  call void @chali_debug_publish_memory()\n";
                const auto index = debugBlocks++;
                enter("q" + std::to_string(index));
                debugLines[index] = debugLocation(source);
            }
            const auto file = state.sourceFiles.emplace(textOf(source->sourceFile), state.sourceFiles.size()).first;
            if (!state.request.options.libraryModule.empty())
                body += "  call void @chali_set_location(ptr @source_file" + std::to_string(file->second) + ", i64 " + std::to_string(source->sourceLine) + ", ptr @" + textOf(function.llvmName) + ")\n";
            else
            {
                body += "  store ptr @" + textOf(function.llvmName) + ", ptr @chali_source_function\n";
                body += "  store ptr @source_file" + std::to_string(file->second) + ", ptr @chali_source_file\n";
                body += "  store i64 " + std::to_string(source->sourceLine) + ", ptr @chali_source_line\n";
            }
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
        const bool cache = state.request.options.libraryModule.empty() && state.cacheAddresses && !state.stepChecks && !state.request.options.debug;
        const auto pointer = snapshotData ? call("chali_data_address", "ptr",
            { convert(address, "i64"), { "i64", byte ? "1" : "8" }, dataBase, dataSize }) :
            call(cache ? "chali_primitive_address" : "chali_host_address", "ptr",
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
            if (!state.unitDataSymbol.empty())
            {
                const auto base = instruction("i64", "load i64, ptr @" + textOf(state.unitDataSymbol));
                return instruction("i64", "add i64 " + base.text + ", " + std::to_string(state.astData.at(node.get())));
            }
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
            if (node->text == "__std_move_begin" && node->kids.size() == 2)
            {
                const auto address = expression(node->kids[0]);
                const auto bytes = expression(node->kids[1]);
                location(node);
                profile("call:" + textOf(node->text), node);
                call("chali_host_storage_begin", "void", { convert(address, "i64"), convert(bytes, "i64") });
                return { "i64", "0" };
            }
            if ((node->text == "__std_move_live" || node->text == "__std_move_require" || node->text == "__std_move_end") && node->kids.size() == 3)
            {
                const auto address = expression(node->kids[0]);
                const auto bytes = expression(node->kids[1]);
                Value typeId;
                const auto &tag = node->kids[2];
                const auto symbol = tag->kind == N::IntLit ? state.request.syntax->nativeTypeSymbols.find(tag->intValue) : state.request.syntax->nativeTypeSymbols.end();
                if (symbol != state.request.syntax->nativeTypeSymbols.end())
                    typeId = instruction("i64", "load i64, ptr @" + llvmSymbol(symbol->second));
                else
                    typeId = expression(tag);
                location(node);
                profile("call:" + textOf(node->text), node);
                const auto operation = node->text == "__std_move_live" ? 0 : node->text == "__std_move_require" ? 1 :
                                                                                                                  2;
                return call("chali_host_storage_lifetime", "i64", { { "i64", std::to_string(operation) }, convert(address, "i64"), convert(bytes, "i64"), convert(typeId, "i64") });
            }
            // 独立语言单元不持有宿主名称表，借用 ABI 始终使用共享运行时的同一非空检查。
            if (!state.request.options.libraryModule.empty() && node->text == "__std_reference" && node->kids.size() == 1)
            {
                const auto value = expression(node->kids.front());
                location(node);
                profile("call:" + textOf(node->text), node);
                return call("chali_require_reference", "i64", { convert(value, "i64") });
            }
            const bool indirect = node->kind == N::CallPointer;
            Value pointer{ "i64", "0" };
            if (indirect) pointer = expression(node->kids[0]);
            std::vector<Value> values;
            for (std::size_t index = indirect ? 1u : 0u; index < node->kids.size(); ++index) values.push_back(expression(node->kids[index]));
            location(node);
            profile(indirect ? Text("indirect_call") : "call:" + textOf(node->text), node);
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
        case N::Alloc:
        {
            const auto bytes = convert(expression(node->kids[0]), "i64");
            location(node);
            profile("allocation_request", node, bytes);
            return call("chali_alloc", "i64", { bytes });
        }
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
        case N::DebugStorageLifetime:
        {
            if (state.request.options.debug && node->intValue == 2)
            {
                const auto found = debugAvailability.find(textOf(node->kids[0]->text));
                if (found != debugAvailability.end()) body += "  store volatile i64 0, ptr " + found->second + "\n";
                return { "i64", "0" };
            }
            if (state.request.options.debug)
                call("chali_debug_storage_lifetime", "void", { { "i64", std::to_string(node->intValue) },
                     convert(expression(node->kids[0]), "i64"), convert(expression(node->kids[1]), "i64"),
                     convert(expression(node->kids[2]), "i64") });
            return { "i64", "0" };
        }
        case N::MemoryCopy:
        case N::MemorySet:
        {
            const auto destination = expression(node->kids[0]);
            const auto source = expression(node->kids[1]);
            const auto bytes = expression(node->kids[2]);
            location(node);
            const bool copy = node->kind == N::MemoryCopy;
            profile(copy ? "memcpy" : "memset", node, bytes);
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
            const auto saved = debugScope;
            const bool lexical = state.request.options.debug && (node->lexicalScope || node == syntax.body);
            if (state.request.options.debug && node->sourceLine) lastLocation = node;
            if (lexical) openDebugScope(node);
            for (const auto &child : node->kids) statement(child);
            if (lexical) debugScope = saved;
            return;
        }
        if (node->kind == N::Label)
        {
            enter("ast_label_" + textOf(state.use(node->text)));
            location(node, true);
            return;
        }
        if (terminated) enter(blockName());
        if (state.request.options.debug && node->kind == N::Declare && (node->text.rfind("__", 0) != 0 || (node->debugMacro && node->debugMacro->privateBinding)))
        {
            openDebugScope(node);
            DebugVariable descriptor;
            descriptor.scope = debugScope;
            auto position = node->sourceLine ? node : lastLocation;
            const auto origin = node->debugBindingSource ? node->debugBindingSource.get() :
                                node->debugMacro && node->debugMacro->frame ? &node->debugMacro->frame->call : nullptr;
            if (!node->sourceLine && origin && origin->line)
            {
                position = ast::Node::i(0);
                position->sourceFile = origin->file;
                position->sourceLine = origin->line;
                position->sourceColumn = origin->column;
                position->debugMacro = node->debugMacro;
            }
            if (position)
            {
                descriptor.file = textOf(position->sourceFile);
                descriptor.line = position->sourceLine;
                descriptor.column = position->sourceColumn;
            }
            if (position) location(position);
            descriptor.block = debugBlocks - 1u;
            debugType(descriptor, node->declared);
            if (!node->debugTypeName.empty()) descriptor.typeName = textOf(node->debugTypeName);
            descriptor.readOnly = node->debugReadOnly;
            descriptor.stringOwner = node->debugStringOwner;
            descriptor.storage = textOf(node->debugStorage);
            if (node->debugMacro && node->debugMacro->privateBinding && node->debugMacro->frame) descriptor.macro = node->debugMacro->frame->id;
            if (!node->debugStorage.empty()) debugBoxes[textOf(node->text)] = { node->debugStorage, node->declared, debugScope };
            debugDeclarations[textOf(node->text)] = std::move(descriptor);
        }
        const bool observer = node->kind == N::ExprStmt && !node->kids.empty() && node->kids[0]->kind == N::DebugStorageLifetime;
        if (!observer && (node->kind != N::Declare || !state.request.options.debug)) location(node);
        switch (node->kind)
        {
        case N::Declare:
        {
            const auto available = debugAvailability.find(textOf(node->text));
            if (available != debugAvailability.end()) body += "  store volatile i64 1, ptr " + available->second + "\n";
            return;
        }
        case N::ExprStmt:
            if (!node->kids.empty()) expression(node->kids[0]);
            return;
        case N::Assign:
        {
            const auto value = expression(node->kids.back());
            const auto target = node->kids.size() > 1 && node->kids.front()->kind == N::Var ? node->kids.front()->text : node->text;
            if (target.rfind("__chali_move_source", 0) == 0) profile("aggregate_move", node);
            if (target.rfind("__chali_array_copy_source", 0) == 0) profile("array_copy", node);
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
            if (state.request.options.debug && node->debugLifetimeWrite)
                call("chali_debug_storage_lifetime", "void", { { "i64", "0" }, convert(at, "i64"),
                     { "i64", width.bits == 8 ? "1" : "8" }, { "i64", "0" } });
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
        case N::Free:
        {
            const auto value = convert(expression(node->kids[0]), "i64");
            profile("cleanup", node);
            call("chali_owned_drop", "void", { value });
            return;
        }
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
        if (state.request.options.debug && syntax.declaration.line)
            debugScopes.emplace(0u, DebugScope{ 0u, { textOf(syntax.declaration.file), syntax.declaration.line, syntax.declaration.column, 0u } });
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
        signature += "uwtable";
        // 显式节中的局部函数不能由 ThinLTO 提升；独立库尚不提供该调试代码区登记，使用普通代码节。
        if (state.request.options.libraryModule.empty()) signature += " section \"chali_code\"";
        signature += " {\nentry:\n";
        signature += "  %indirect_arguments = alloca [" + std::to_string(argumentCapacity) + " x i64]\n  %native_argument_types = alloca [" + std::to_string(argumentCapacity) + " x i8]\n";
        signature += "  %raw_memory_output = alloca ptr\n  %raw_memory_input = alloca ptr\n  %collection_result_kind = alloca i8\n";
        if (snapshotData) signature += "  %data_snapshot_size = alloca i64\n";
        for (const auto &name : locals)
        {
            const Memory::String token(name.c_str());
            signature += "  " + pointer(token) + " = alloca " + type(token) + "\n  store " + type(token) + " " + (type(token) == "double" ? "0.0" : "0") + ", ptr " + pointer(token) + "\n";
            if (type(token) == "i64") signature += "  " + tagPointer(token) + " = alloca i8\n  store i8 0, ptr " + tagPointer(token) + "\n";
        }
        for (std::size_t index = 0; index < function.parameters.size(); ++index) signature += "  store " + type(function.parameters[index]) + " %p" + std::to_string(index) + ", ptr " + pointer(function.parameters[index]) + "\n";
        if (snapshotData)
        {
            dataBase = call("chali_host_data_snapshot", "ptr", { { "ptr", "%data_snapshot_size" } });
            dataSize = instruction("i64", "load i64, ptr %data_snapshot_size");
        }
        if (state.request.options.debug)
        {
            std::vector<ast::NodePtr> pending{ syntax.body };
            while (!pending.empty())
            {
                const auto node = pending.back();
                pending.pop_back();
                if (!node) continue;
                if (node->kind == ast::Node::Kind::DebugStorageLifetime && node->intValue == 2)
                    debugAvailability.emplace(textOf(node->kids[0]->text), "%debug_available_" + std::to_string(debugAvailability.size()));
                pending.insert(pending.end(), node->kids.begin(), node->kids.end());
            }
            for (const auto &entry : debugAvailability)
                signature += "  " + entry.second + " = alloca i64\n  store volatile i64 1, ptr " + entry.second + "\n";
            signature += "  %debug_context = alloca [3 x i64]\n";
            signature += "  call void @chali_debug_capture_context(ptr %debug_context)\n";
            // 私有观察槽需在优化调试中跨调用保留，避免身份仅存于已被覆盖的寄存器。
            for (std::size_t index = 0; index < 3; ++index)
            {
                const auto suffix = std::to_string(index);
                signature += "  %debug_context_" + suffix + " = alloca i64\n";
                signature += "  %debug_context_source_" + suffix + " = getelementptr [3 x i64], ptr %debug_context, i64 0, i64 " + suffix + "\n";
                signature += "  %debug_context_value_" + suffix + " = load i64, ptr %debug_context_source_" + suffix + "\n";
                signature += "  store volatile i64 %debug_context_value_" + suffix + ", ptr %debug_context_" + suffix + "\n";
            }
            signature += "  br label %q0\n";
            body = "q0:\n";
            debugEntryEnd = body.size();
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
            DebugVariable variable;
            const auto declaration = debugDeclarations.find(textOf(entry.first));
            if (declaration != debugDeclarations.end())
                variable = declaration->second;
            else if (const auto declared = environment.find(entry.first); declared != environment.end())
                debugType(variable, declared->second);
            variable.name = textOf(entry.first);
            if (variable.macro >= 0) variable.dwarfName = "__chali_macro_debug_" + std::to_string(variable.macro) + "_" + std::to_string(variable.scope) + "_" + std::to_string(variables.size());
            variable.pointer = pointer(entry.second);
            variable.floating = type(entry.second) == "double";
            variable.nativeStorage = type(entry.second);
            variable.argument = parameter == function.parameters.end() ? 0u : static_cast<std::size_t>(parameter - function.parameters.begin() + 1u);
            if (variable.argument)
            {
                const auto origin = syntax.parameterSources.find(entry.first);
                if (origin != syntax.parameterSources.end())
                {
                    variable.file = textOf(origin->second.file);
                    variable.line = origin->second.line;
                    variable.column = origin->second.column;
                    variable.readOnly = origin->second.readOnly;
                    variable.storage = textOf(origin->second.storage);
                    if (!origin->second.typeName.empty()) variable.typeName = textOf(origin->second.typeName);
                }
            }
            if (!variable.storage.empty())
            {
                const auto storage = symbol(Memory::String(variable.storage.c_str()));
                if (locals.count(textOf(storage)) && type(storage) == "i64")
                {
                    variable.storageDwarf = "__chali_storage_" + std::to_string(variables.size());
                    variable.storageBytes = variable.typeName == "char" || variable.typeName == "uint8" ? 1u : 8u;
                    auto hidden = variable;
                    hidden.name = hidden.dwarfName = variable.storageDwarf;
                    hidden.pointer = pointer(storage);
                    hidden.typeName = "uint64";
                    hidden.bits = 64;
                    hidden.encoding = "DW_ATE_unsigned";
                    hidden.floating = false;
                    hidden.argument = 0;
                    hidden.internal = true;
                    hidden.storage.clear();
                    hidden.nativeStorage = "i64";
                    variables.push_back(std::move(hidden));
                }
            }
            const auto availability = debugAvailability.find(variable.storage.empty() ? variable.name : variable.storage);
            if (availability != debugAvailability.end())
            {
                variable.availabilityDwarf = "__chali_available_" + std::to_string(variables.size());
                auto hidden = variable;
                hidden.name = hidden.dwarfName = variable.availabilityDwarf;
                hidden.pointer = availability->second;
                hidden.typeName = "uint64";
                hidden.bits = 64;
                hidden.encoding = "DW_ATE_unsigned";
                hidden.floating = false;
                hidden.argument = 0;
                hidden.internal = true;
                hidden.storage.clear();
                hidden.nativeStorage = "i64";
                variables.push_back(std::move(hidden));
            }
            variables.push_back(std::move(variable));
        }
        for (std::size_t index = 0; index < 3; ++index)
        {
            DebugVariable context;
            context.name = context.dwarfName = std::string("__chali_context_") + (index == 0 ? "heap" : index == 1 ? "data" : "objects");
            context.pointer = "%debug_context_" + std::to_string(index);
            context.typeName = "uint64";
            context.encoding = "DW_ATE_unsigned";
            context.nativeStorage = "i64";
            context.file = textOf(syntax.declaration.file);
            context.line = syntax.declaration.line;
            context.internal = true;
            variables.push_back(std::move(context));
        }
        return state.debugInfo.annotate(definition, textOf(syntax.debugName.empty() ? function.label : syntax.debugName), textOf(function.llvmName), debugLines, variables, debugScopes);
    }
};
