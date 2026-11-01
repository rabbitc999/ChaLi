/** @file chali_aot_library.hpp
 * @brief 已检查标量函数的独立 LLVM 原生单元，与最终程序共用运行时。
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
 * @brief 核对标量单元可达函数，不把未实现的拥有值或全局状态按标量误编译。
 * @param state 当前直接 AST 生成状态。
 * @param function 可达函数。
 * @return 函数及全部节点属于标量范围时为真。
 */
bool validateLibraryFunction(Translator &state, const ast::Function &function)
{
    if (!function.owner.empty() || !libraryScalar(function.result, true)) return state.fail("原生标量库暂不支持方法或拥有值返回：" + function.name), false;
    for (const auto &parameter : function.parameters)
        if (!libraryScalar(parameter.second)) return state.fail("原生标量库暂不支持该参数类型：" + parameter.first), false;
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
        case N::Jump: break;
        case N::Declare:
        case N::Cast:
            if (!libraryScalar(node->declared, node->kind == N::Cast)) return false;
            break;
        case N::Var:
            if (state.globals.count(textOf(node->text))) return false;
            break;
        case N::Assign:
            if (state.globals.count(textOf(node->text)) || (node->kids.size() > 1 && node->kids.front()->kind != N::Var)) return false;
            break;
        case N::Call:
            if (!state.astFunctions.count(textOf(node->text))) return false;
            break;
        default: return false;
        }
        for (const auto &child : node->kids)
            if (!visit(child)) return false;
        return true;
    };
    if (!visit(function.body)) return state.fail("原生标量库含当前未支持的全局状态、宿主、聚合或地址操作：" + function.name), false;
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
    Text out = "define " + abiExtension(entry.resultType, state.target) + resultType + " @" + llvmSymbol(entry.symbol) + "(";
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
    out += ") uwtable {\nentry:\n" + body;
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
    return out + "}\n";
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
    if (request.options.debug || request.options.stepLimit) return state.fail("首批原生标量库暂不支持独立调试或构建期步数预算"), Memory::String();
    Memory::String runtime;
    if (!chali_module::embedded::findGeneratorAssetByPath(state.target.runtime, &runtime)) return state.fail("库单元缺少目标运行时声明"), Memory::String();
    Text out = "; ChaLi native scalar library unit\nsource_filename = \"chali-library\"\ntarget triple = \"" + textOf(state.target.triple) + "\"\ntarget datalayout = \"" + textOf(state.target.dataLayout) + "\"\n";
    out += libraryRuntimeDeclarations(textOf(runtime));
    out += "@chali_return_float = internal thread_local global i8 0\ndeclare void @chali_set_location(ptr, i64, ptr)\n";
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
    out += functions;
    for (const auto &entry : request.abiExports) out += libraryExportThunk(state, entry);
    return Memory::String(out.data(), out.size());
}
