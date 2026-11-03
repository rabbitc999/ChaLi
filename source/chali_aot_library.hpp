/** @file chali_aot_library.hpp
 * @brief 已检查语言函数的独立 LLVM 原生单元，与最终程序共用运行时。
 */
#pragma once

/**
 * @brief 判断首批独立单元能够按值传递的准确标量类型。
 * @param type 已检查的 AST 类型。
 * @param allowVoid 是否允许无返回值。
 * @return 类型属于当前原生标量约定时为真。
 */
bool libraryScalar(const ast::TypePtr &type, bool allowVoid = false)
{
    if (!type) return false;
    using K = ast::Type::Kind;
    return (allowVoid && type->kind == K::Void) || type->kind == K::Bool
           || ((type->kind == K::Int || type->kind == K::Uint) && (type->bits == 8 || type->bits == 16 || type->bits == 32 || type->bits == 64))
           || (type->kind == K::Float && (type->bits == 32 || type->bits == 64));
}
/**
 * @brief 判断仅包含扁平标量字段的记录或枚举逻辑引用。
 * @param type 已检查且已展开字段布局的 AST 类型。
 * @return 不传递拥有状态、可复用消费者共享内存时为真。
 */
bool libraryBorrow(const ast::TypePtr &type)
{
    if (!type || type->kind != ast::Type::Kind::Ref || !type->element || type->element->kind != ast::Type::Kind::Struct) return false;
    for (const auto &field : type->element->fields)
        if (!libraryScalar(field.second)) return false;
    return !type->element->fields.empty();
}
/**
 * @brief 判断独立单元按逻辑地址传递且不含拥有字段的记录或枚举值。
 * @param type 已展开的 AST 类型。
 * @return 全部存储字段为基础标量时为真。
 */
bool libraryPlainValue(const ast::TypePtr &type)
{
    if (!type || type->kind != ast::Type::Kind::Struct || type->fields.empty()) return false;
    for (const auto &field : type->fields)
        if (!libraryScalar(field.second)) return false;
    return true;
}
/**
 * @brief 核对独立单元可达函数，不把未实现的拥有值或全局状态按标量误编译。
 * @param state 当前直接 AST 生成状态。
 * @param function 可达函数。
 * @return 函数及全部节点属于标量范围时为真。
 */
bool validateLibraryFunction(Translator &state, const ast::Function &function)
{
    if (!function.owner.empty() || (!libraryScalar(function.result, true) && !libraryPlainValue(function.result) && !libraryBorrow(function.result))) return state.fail("原生库尚不支持该方法或拥有值返回：" + function.name), false;
    for (const auto &parameter : function.parameters)
        if (!libraryScalar(parameter.second) && !libraryBorrow(parameter.second) && !libraryPlainValue(parameter.second)) return state.fail("原生库暂不支持该参数类型：" + parameter.first), false;
    auto environment = state.astTypes;
    std::set<Text> borrowed;
    for (const auto &parameter : function.parameters)
    {
        environment[parameter.first] = parameter.second;
        if (libraryBorrow(parameter.second) || libraryPlainValue(parameter.second)) borrowed.insert(textOf(parameter.first));
    }
    std::function<bool(const ast::NodePtr &)> addressOrigin = [&](const ast::NodePtr &node) {
        if (!node) return false;
        using N = ast::Node::Kind;
        if (node->kind == N::Alloc) return true;
        if (node->kind == N::StructNew) return true;
        if (node->kind == N::Call)
        {
            const auto own = state.astFunctions.find(textOf(node->text));
            if (own != state.astFunctions.end() && (libraryPlainValue(own->second->result) || libraryBorrow(own->second->result))) return true;
            const auto external = state.externsByLabel.find(textOf(node->text));
            if (external != state.externsByLabel.end() && (external->second->abiConvention == "ChaLiValue" || external->second->borrowedResult)) return true;
        }
        if (node->kind == N::Var) return borrowed.count(textOf(node->text)) != 0;
        if (node->kind == N::Cast || node->kind == N::Offset || node->kind == N::Member || node->kind == N::Index)
            return !node->kids.empty() && addressOrigin(node->kids.front());
        if (node->kind == N::Sequence) return !node->kids.empty() && addressOrigin(node->kids.back());
        if (node->kind == N::Call && node->text == "__std_reference" && node->kids.size() == 1) return addressOrigin(node->kids.front());
        if (node->kind == N::Binary && (node->text == "+" || node->text == "-"))
            return !node->kids.empty() && addressOrigin(node->kids.front());
        return false;
    };
    Text unsupported;
    std::function<bool(const ast::NodePtr &)> visit = [&](const ast::NodePtr &node) {
        if (!node) return true;
        using N = ast::Node::Kind;
        switch (node->kind)
        {
        case N::IntLit:
        case N::FloatLit:
        case N::BoolLit:
        case N::Binary:
        case N::Unary:
        case N::Sequence:
        case N::If:
        case N::While:
        case N::DoWhile:
        case N::Return:
        case N::ExprStmt:
        case N::Block:
        case N::Label:
        case N::Jump:
        case N::Member:
        case N::Index:
        case N::Offset:
        case N::LoadInt:
        case N::StoreAt:
        case N::CheckPointer:
        case N::Alloc:
        case N::Free:
        case N::Drop:
        case N::MemoryCopy:
        case N::MemorySet:
        case N::DebugStorageLifetime: break;
        case N::StructNew:
            if (!libraryPlainValue(ast::inferType(node, environment))) return false;
            break;
        case N::Declare:
            environment[node->text] = node->declared;
            if (!libraryScalar(node->declared) && !libraryBorrow(node->declared) && !libraryPlainValue(node->declared))
            {
                unsupported = "局部类型 " + (node->declared ? textOf(node->declared->describe()) : Text("无类型"));
                return false;
            }
            break;
        case N::Cast:
            if (!libraryScalar(node->declared, true))
            {
                const auto source = node->kids.empty() ? ast::TypePtr() : ast::inferType(node->kids.front(), environment);
                const auto sourceRecord = source && source->kind == ast::Type::Kind::Ref ? source->element : source;
                const auto targetRecord = node->declared && node->declared->kind == ast::Type::Kind::Ref ? node->declared->element : node->declared;
                if (!sourceRecord || !targetRecord || sourceRecord->kind != ast::Type::Kind::Struct || targetRecord->kind != ast::Type::Kind::Struct
                    || sourceRecord->name != targetRecord->name)
                {
                    if (targetRecord && targetRecord->kind == ast::Type::Kind::Struct
                        && !node->kids.empty() && addressOrigin(node->kids.front())) break;
                    unsupported = "转换 " + (source ? textOf(source->describe()) : Text("无类型")) + " -> " + (node->declared ? textOf(node->declared->describe()) : Text("无类型"));
                    return false;
                }
            }
            break;
        case N::Var:
            if (state.globals.count(textOf(node->text)))
            {
                unsupported = "全局 " + textOf(node->text);
                return false;
            }
            break;
        case N::Assign:
        {
            const auto target = node->kids.size() > 1 && node->kids.front()->kind == N::Var ? node->kids.front()->text : node->text;
            const auto found = environment.find(target);
            if (found != environment.end() && (libraryBorrow(found->second) || libraryPlainValue(found->second)))
            {
                const auto value = node->kids.empty() ? ast::NodePtr() : node->kids.back();
                if (value && value->kind == N::IntLit && value->intValue == 0)
                    borrowed.erase(textOf(target));
                else
                {
                    if (!addressOrigin(value))
                    {
                        unsupported = "记录引用来源 " + textOf(target);
                        return false;
                    }
                    borrowed.insert(textOf(target));
                }
            }
            if (state.globals.count(textOf(node->text)))
            {
                unsupported = "全局 " + textOf(node->text);
                return false;
            }
            if (node->kids.size() > 1 && node->kids.front()->kind != N::Var && node->kids.front()->kind != N::Member
                && node->kids.front()->kind != N::Index && node->kids.front()->kind != N::Offset) return false;
            break;
        }
        case N::Call:
            if (node->text == "__std_move_begin" && node->kids.size() == 2) break;
            if (node->text == "__std_move_live" || node->text == "__std_move_require" || node->text == "__std_move_end")
            {
                if (node->kids.size() != 3 || node->kids[2]->kind != N::IntLit || !state.request.syntax->nativeTypeSymbols.count(node->kids[2]->intValue))
                {
                    unsupported = "未登记的跨单元移动类型";
                    return false;
                }
                break;
            }
            if (node->text == "__std_reference" && node->kids.size() == 1) break;
            if (!state.astFunctions.count(textOf(node->text)))
            {
                const auto external = state.externsByLabel.find(textOf(node->text));
                if (external == state.externsByLabel.end())
                {
                    unsupported = "调用 " + textOf(node->text);
                    return false;
                }
                const auto &entry = *external->second;
                if ((entry.abiConvention != "C" && entry.abiConvention != "ChaLiBorrow" && entry.abiConvention != "ChaLiValue") || !isDirectLinkLibrary(entry.library) || entry.requiresMarshalling) return false;
                if (abiType(entry.resultType).empty() || abiType(entry.resultType) == "ptr") return false;
                for (const auto &parameter : entry.parameterTypes)
                    if (abiType(parameter).empty() || abiType(parameter) == "ptr" || abiType(parameter) == "void") return false;
            }
            break;
        default: unsupported = "节点 " + std::to_string(static_cast<int>(node->kind)); return false;
        }
        for (const auto &child : node->kids)
            if (!visit(child)) return false;
        return true;
    };
    if (!visit(function.body)) return state.fail("原生库含当前未支持的全局状态、宿主、聚合或地址操作：" + function.name + " " + Memory::String(unsupported.c_str())), false;
    return true;
}
/**
 * @brief 为运行时定义提取相同调用约定的声明，独立单元不复制运行时实现。
 * @param runtime 当前目标的固定 LLVM 运行时资产。
 * @return 不含函数体、初始化器及属性组引用的外部声明。
 */
Text libraryRuntimeDeclarations(const Text &runtime)
{
    std::istringstream input(runtime);
    Text line, result;
    std::set<Text> seen;
    while (std::getline(input, line))
    {
        const bool definition = line.rfind("define ", 0) == 0;
        if (!definition && line.rfind("declare ", 0) != 0) continue;
        const auto name = line.find('@'), start = line.find('(', name);
        if (name == Text::npos || start == Text::npos) continue;
        unsigned depth = 1;
        std::size_t end = start + 1;
        for (; end < line.size() && depth; ++end)
        {
            if (line[end] == '(') ++depth;
            if (line[end] == ')') --depth;
        }
        if (depth) throw std::runtime_error("库单元的运行时声明跨行或括号不完整");
        const auto symbol = line.substr(name + 1, start - name - 1);
        if (!seen.insert(symbol).second) continue;
        Text declaration = line.substr(definition ? 7 : 8, end - (definition ? 7 : 8));
        for (const char *linkage : { "internal ", "private ", "linkonce_odr " })
        {
            const auto at = declaration.find(linkage);
            if (at != Text::npos && at < declaration.find('@')) declaration.erase(at, std::strlen(linkage));
        }
        result += "declare " + declaration + "\n";
    }
    return result;
}
/**
 * @brief 为一个标量语言导出生成准确 C ABI 的 LLVM 转换入口。
 * @param state 已检查类型和目标约定。
 * @param entry 已规范化的公开原生签名。
 * @return 包含位宽及浮点转换的函数定义。
 */
Text libraryExportThunk(Translator &state, const ExternFunction &entry)
{
    const auto found = state.functionByLabel.find(textOf(entry.label));
    if (found == state.functionByLabel.end()) throw std::runtime_error("库导出函数缺少已检查实现");
    const auto &function = state.functions[found->second];
    const Text resultType = abiType(entry.resultType);
    // 语言包入口在最终单元内链接；显式原生别名保留公开可见性，避免把私有 ABI 入口当成动态导出保活。
    const bool native = !entry.nativeAlias.empty() && entry.nativeAlias == entry.symbol;
    Text out = Text("define ") + (native ? "" : "hidden ") + abiExtension(entry.resultType, state.target) + resultType + " @" + llvmSymbol(entry.symbol) + "(";
    Text arguments, body;
    for (std::size_t i = 0; i < entry.parameterTypes.size(); ++i)
    {
        const auto type = abiType(entry.parameterTypes[i]);
        const auto suffix = std::to_string(i), argument = "%p" + suffix;
        if (i)
        {
            out += ", ";
            arguments += ", ";
        }
        out += type + " " + abiExtension(entry.parameterTypes[i], state.target) + argument;
        Text value = argument;
        if (type == "float")
        {
            value = "%v" + suffix;
            body += "  " + value + " = fpext float " + argument + " to double\n";
        }
        else if (type != "double" && type != "i64")
        {
            value = "%v" + suffix;
            const bool unsigned_ = type == "i1" || entry.parameterTypes[i].find("unsigned") != Memory::String::npos;
            body += "  " + value + " = " + Text(unsigned_ ? "zext " : "sext ") + type + " " + argument + " to i64\n";
        }
        arguments += Text(type == "float" || type == "double" ? "double " : "i64 ") + value;
    }
    out += ") uwtable {\nentry:\n";
    if (state.request.options.debug) out += "  br label %q1\nq1:\n";
    out += body;
    const Text internalType = function.returnsFloat ? "double" : "i64";
    out += "  %value = call " + internalType + " @" + textOf(function.llvmName) + "(" + arguments + ")\n";
    if (resultType == "void")
        out += "  ret void\n";
    else if (resultType == internalType)
        out += "  ret " + resultType + " %value\n";
    else
    {
        const Text conversion = resultType == "float" ? "fptrunc double %value to float" : resultType == "i1" ? "icmp ne i64 %value, 0" :
                                                                                                                "trunc i64 %value to " + resultType;
        out += "  %converted = " + conversion + "\n  ret " + resultType + " %converted\n";
    }
    out += "}\n";
    if (state.request.options.debug)
        return state.debugInfo.annotate(out, textOf(function.label) + " [ABI]", entry.symbol.c_str(),
                                        { { 1, { "<native-library-entry>", 1, 1 } } }, {});
    return out;
}
/**
 * @brief 从已检查 AST 生成无入口和无运行时副本的首批标量库单元。
 * @param request 包含明确模块身份和公开签名的请求。
 * @param state 可达性和语义类型已准备好的生成状态。
 * @return 可单独编译的 LLVM 模块；错误时返回空文本。
 */
Memory::String generateLibraryLLVM(Request &request, Translator &state)
{
    if (!request.syntax || request.abiExports.empty()) return state.fail("原生库需要至少一个公开标量函数"), Memory::String();
    if (request.options.stepLimit) return state.fail("独立原生库暂不支持构建期步数预算"), Memory::String();
    Memory::String runtime;
    if (!chali_module::embedded::findGeneratorAssetByPath(state.target.runtime, &runtime)) return state.fail("库单元缺少目标运行时声明"), Memory::String();
    const auto identity = chali_digest::hex(chali_digest::sha256(std::string_view(request.options.libraryModule.data(), request.options.libraryModule.size())));
    Text out = "; ChaLi native scalar library unit\nsource_filename = \"chali-library-" + identity + "\"\ntarget triple = \"" + textOf(state.target.triple) + "\"\ntarget datalayout = \"" + textOf(state.target.dataLayout) + "\"\n";
    if (!state.unitDataSymbol.empty()) out += "@" + textOf(state.unitDataSymbol) + " = external constant i64\n";
    for (const auto &type : request.syntax->nativeTypeSymbols) out += "@" + llvmSymbol(type.second) + " = external constant i64\n";
    out += libraryRuntimeDeclarations(textOf(runtime));
    if (request.options.debug) out += "declare void @chali_debug_publish_memory()\ndeclare void @chali_debug_capture_context(ptr)\ndeclare void @chali_debug_storage_lifetime(i64, i64, i64, i64)\n";
    if (state.profileOwnership) out += "declare void @chali_backend_profile_event(ptr, i64)\n";
    out += "@chali_return_float = internal thread_local global i8 0\ndeclare void @chali_set_location(ptr, i64, ptr)\n";
    if (out.find("@chali_host_check_pointer(") == Text::npos) out += "declare void @chali_host_check_pointer(i64, i64, i64)\n";
    if (out.find("@chali_host_address(") == Text::npos) out += "declare ptr @chali_host_address(i64, i64)\n";
    out += "declare void @chali_host_storage_begin(i64, i64)\n";
    out += "declare i64 @chali_host_storage_lifetime(i64, i64, i64, i64)\n";
    if (out.find("@chali_host_copy_words(") == Text::npos) out += "declare i64 @chali_host_copy_words(i64, i64, i64)\n";
    if (out.find("@chali_host_memory_prepare(") == Text::npos) out += "declare void @chali_host_memory_prepare(i64, i64, i64, i64, ptr, ptr)\n";
    Text functions;
    for (auto &function : state.functions)
    {
        if (!state.reachable.count(textOf(function.label))) continue;
        const auto found = state.astFunctions.find(textOf(function.label));
        if (found == state.astFunctions.end()) continue;
        if (!validateLibraryFunction(state, *found->second)) return {};
        function.exported = false;
        functions += AstBuilder(state, function).generateAst();
    }
    out += profileSiteDeclarations(state);
    for (const auto &source : state.sourceFiles)
    {
        Text bytes;
        for (const unsigned char byte : source.first)
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            bytes += escaped;
        }
        out += "@source_file" + std::to_string(source.second) + " = private constant [" + std::to_string(source.first.size() + 1) + " x i8] c\"" + bytes + "\\00\"\n";
    }
    std::map<Text, Text> declarations;
    for (const auto &entry : request.externs)
    {
        if (!state.usedExternLabels.count(textOf(entry.label))) continue;
        const auto declaration = linkedCDeclaration(entry, state.target);
        const auto inserted = declarations.emplace(textOf(entry.symbol), declaration);
        if (!inserted.second && inserted.first->second != declaration) return state.fail("库单元外部符号 ABI 冲突：" + entry.symbol), Memory::String();
        if (inserted.second) out += declaration;
    }
    out += functions;
    for (const auto &entry : request.abiExports)
    {
        out += libraryExportThunk(state, entry);
        if (!entry.nativeAlias.empty() && entry.nativeAlias != entry.symbol)
        {
            auto native = entry;
            native.symbol = entry.nativeAlias;
            out += libraryExportThunk(state, native);
        }
    }
    if (request.options.debug)
        state.debugInfo.finish(out, "chali_debug_" + identity, "[]", textOf(request.options.libraryModule));
    return Memory::String(out.data(), out.size());
}
