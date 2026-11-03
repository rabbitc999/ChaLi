/* ---------- 产物 ---------- */
/* AOT 只需要这一份接口：把 `abi` 外部函数交给它时用它的数据结构。 */
#include "chali_aot.hpp"

/**
 * @brief 将诊断消息列表用中文分号连接为一段文本。
 * @param messages 待连接的诊断消息列表。
 * @retval 生成或读取的结果文本。
 */
Memory::String JoinMessages(const Memory::Vector<Memory::String> &messages)
{
    Memory::String text;
    for (std::size_t index = 0; index < messages.size(); ++index)
    {
        if (index != 0u) text += "；";
        text += messages[index];
    }
    return text;
}

/**
 * @brief 将生成内容写入指定产物文件。
 * @param path 目标文件或目录的路径。
 * @param text 待处理的文本。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool WriteFile(const Memory::String &path, const Memory::String &text)
{
    /* 父目录可能还没建（默认产物在 generate/ 下），先补上再写；
     * 目录建不起来时下面打开流自然会失败，错误信息照旧。 */
    const std::filesystem::path target = Memory::u8path(path);
    const std::filesystem::path parent = target.parent_path();
    if (!parent.empty())
    {
        std::error_code created;
        std::filesystem::create_directories(parent, created);
    }
    std::ofstream stream(target, std::ios::binary);
    if (!stream) return Fail("写不了文件：" + path);
    stream.write(text.data(), static_cast<std::streamsize>(text.size()));
    if (!stream) return Fail("写文件失败：" + path);
    return true;
}

/**
 * @brief 将文本编码为合法 C++ 字符串字面量。
 * @param value 待转换、保存或参与计算的值。
 * @retval 生成或读取的结果文本。
 */
Memory::String CxxStringLiteral(const Memory::String &value)
{
    Memory::String out = "\"";
    for (unsigned char ch : value)
    {
        if (ch == '\\' || ch == '"')
        {
            out += '\\';
            out += static_cast<char>(ch);
        }
        else if (ch == '\n')
            out += "\\n";
        else if (ch == '\r')
            out += "\\r";
        else if (ch == '\t')
            out += "\\t";
        else
            out += static_cast<char>(ch);
    }
    return out + '"';
}

/**
 * @brief 生成 C ABI 胶水绑定所需的类型表达式。
 * @param type 类型描述。
 * @retval 生成或读取的结果文本。
 */
Memory::String CAbiBindingType(const std::shared_ptr<MType> &type)
{
    if (const auto reference = CAbiOptionalReference(type))
        return Memory::String("chali_native_marshal::OptionalReference<") + (reference->readOnly ? "const " : "") + CAbiType(reference->elem) + ">";
    if (type && type->kind == MType::Func)
    {
        Memory::String result = "chali_native_marshal::Callback<" + CAbiType(type->elem) + "(";
        for (std::size_t i = 0; i < type->paramTypes.size(); ++i)
        {
            if (i) result += ",";
            result += CAbiType(type->paramTypes[i]);
        }
        return result + ")>";
    }
    const auto underlying = CAbiUnderlying(type);
    if (underlying && underlying->kind == MType::Array)
        return "chali_native_marshal::Array<" + CAbiType(underlying->elem) + ">";
    return CAbiType(type);
}

#include "abi_export.cpp"

/**
 * @brief 判断是否需要将语言逻辑槽封送为平台 C ABI。
 * @param function 已验证的外部函数声明。
 * @return 结果或参数含非标量类型时为真。
 */
bool CAbiNeedsMarshalling(const MFunctionDecl &function)
{
    if (LibraryLanguageAbi(function.abiConvention)) return false;
    const auto scalar = [](const std::shared_ptr<MType> &type) {
        return type && (type->kind == MType::Void || type->kind == MType::Bool || type->kind == MType::Char || type->kind == MType::Int || type->kind == MType::Double);
    };
    if (!scalar(function.signature.result)) return true;
    for (const auto &parameter : function.signature.params)
        if (!scalar(parameter.type)) return true;
    return false;
}

/**
 * @brief 生成外部 ABI 声明对应的宿主胶水源码。
 * @retval 生成或读取的结果文本。
 */
Memory::String AbiGlueSource()
{
    Memory::Map<Memory::String, Memory::Vector<const MFunctionDecl *>> libraries;
    for (const MFunctionDecl &function : g_functionDecls)
        if (!function.abiLibrary.empty() && (!::chali_aot::isDirectLinkLibrary(function.abiLibrary) || CAbiNeedsMarshalling(function)) && !function.superseded)
            libraries[function.abiLibrary].push_back(&function);
    if (libraries.empty() && CAbiExportFunctions().empty()) return {};
    Memory::String definitions;
    Memory::Set<Memory::String> emitted;
    Memory::Function<void(const std::shared_ptr<MType> &)> emitType;
    emitType = [&](const std::shared_ptr<MType> &type) {
        if (!type) return;
        if (const auto reference = CAbiOptionalReference(type))
        {
            emitType(reference->elem);
            return;
        }
        if (type->kind == MType::Ptr || type->kind == MType::Host || type->kind == MType::Array)
        {
            emitType(type->elem);
            return;
        }
        if (type->kind == MType::Func)
        {
            emitType(type->elem);
            for (const auto &parameter : type->paramTypes) emitType(parameter);
            return;
        }
        if (type->kind != MType::Struct || !emitted.insert(type->name).second) return;
        const auto &definition = g_structs.at(type->name);
        for (const auto &field : definition.fields) emitType(field.type);
        const auto name = CAbiStructName(type->name);
        if (definition.abiPacked) definitions += "#pragma pack(push, 1)\n";
        definitions += "struct " + name + " {\n";
        for (std::size_t i = 0; i < definition.fields.size(); ++i)
            definitions += "    " + CAbiType(definition.fields[i].type) + " f" + Memory::toString(i) + ";\n";
        definitions += "};\n";
        if (definition.abiPacked) definitions += "#pragma pack(pop)\n";
        definitions += "namespace chali_native_marshal {\ntemplate <> struct Storage<" + name + "> {\n";
        definitions += "static constexpr std::size_t slots = " + Memory::toString(definition.slotCount) + ";\n";
        definitions += "/**\n * @brief 从语言字段槽读取原生聚合。\n * @param c 当前 C 帧。\n * @param a 逻辑地址。\n * @return 原生聚合副本。\n */\nstatic " + name + " read(Frame c, std::int64_t a) { " + name + " v{};\n";
        for (std::size_t i = 0; i < definition.fields.size(); ++i)
        {
            const auto &field = definition.fields[i];
            definitions += "v.f" + Memory::toString(i) + " = Storage<" + CAbiType(field.type) + ">::read(c, a + " + Memory::toString(field.slot * 8) + ");\n";
        }
        definitions += "return v; }\n/**\n * @brief 逐字段回写原生聚合。\n * @param c 当前 C 帧。\n * @param a 逻辑地址。\n * @param v 原生副本。\n * @return 全部字段写入成功时为 true。\n */\nstatic bool write(Frame c, std::int64_t a, const " + name + " &v) noexcept {\n";
        for (std::size_t i = 0; i < definition.fields.size(); ++i)
        {
            const auto &field = definition.fields[i];
            definitions += "if (!Storage<" + CAbiType(field.type) + ">::write(c, a + " + Memory::toString(field.slot * 8) + ", v.f" + Memory::toString(i) + ")) return false;\n";
        }
        definitions += "return true; }\n};\n}\n";
    };
    for (const auto &library : libraries)
        for (const auto *function : library.second)
        {
            emitType(function->signature.result);
            for (const auto &param : function->signature.params) emitType(param.type);
        }
    for (const auto *function : CAbiExportFunctions()) {
        emitType(function->signature.result);
        for (const auto &parameter : function->signature.params) emitType(parameter.type);
    }
    Memory::String source;
    Memory::Vector<const MFunctionDecl *> functions;
    for (const auto &library : libraries)
        for (const auto *function : library.second) functions.push_back(function);
    std::sort(functions.begin(), functions.end(), [](const auto *left, const auto *right) { return left->label < right->label; });
    for (std::size_t index = 0; index < functions.size(); ++index)
    {
        const auto &function = *functions[index];
        const auto suffix = Memory::toString(index);
        const auto result = CAbiBindingType(function.signature.result);
        const bool returns = CAbiType(function.signature.result) != "void";
        const bool linked = ::chali_aot::isDirectLinkLibrary(function.abiLibrary);
        if (linked)
        {
            source += "extern \"C\" typename chali_native_marshal::Result<" + result + ">::Native chali_linked_" + suffix + "(";
            for (std::size_t arg = 0; arg < function.signature.params.size(); ++arg)
            {
                if (arg) source += ", ";
                source += "decltype(std::declval<chali_native_marshal::Argument<" + CAbiBindingType(function.signature.params[arg].type) + ">&>().get())";
            }
            source += ") __asm__(" + CxxStringLiteral(function.abiSymbol) + ");\n";
            if (!function.abiRelease.empty())
                source += "extern \"C\" void chali_linked_release_" + suffix + "(void *) __asm__(" + CxxStringLiteral(function.abiRelease) + ");\n";
        }
        source += "/**\n * @brief 通过 C 帧封送一次原生调用。\n * @param raw 当前 C 调用帧。\n */\nstatic void chali_abi_call_" + suffix + "(ChaliAbiFrame *raw) {\n";
        source += "char diagnostic[256]; ChaliAbiError error{diagnostic, sizeof(diagnostic), 0, ChaliAbiOk};\n";
        source += "if (chali_abi_check_frame(raw, " + Memory::toString(function.signature.params.size()) + ", &error) != ChaliAbiOk) { chali_abi_fail(raw, diagnostic); return; }\n";
        source += "chali_abi_return_integer(raw, 0);\ntry {\nusing namespace chali_native_marshal; Frame frame{raw}; OwnedReturn owned;\n";
        if (!function.abiRelease.empty())
            source += linked ? "owned.release = &chali_linked_release_" + suffix + ";\n" : "owned.release = reinterpret_cast<void (*)(void *)>(chali_host_abi_symbol(" + CxxStringLiteral(function.abiLibrary) + ", " + CxxStringLiteral(function.abiRelease) + "));\n";
        source += "const auto count = " + (function.abiResultCountIndex < 0 ? Memory::String("std::size_t{1}") :
            "checkedCount(frame.integer(" + Memory::toString(function.abiResultCountIndex) + "), Result<" + result + ">::stride)") + ";\n";
        Memory::String signature = "typename Result<" + result + ">::Native (*) (", arguments;
        for (std::size_t arg = 0; arg < function.signature.params.size(); ++arg)
        {
            const auto type = CAbiBindingType(function.signature.params[arg].type);
            const auto n = Memory::toString(arg);
            if (arg) { signature += ", "; arguments += ", "; }
            signature += "decltype(std::declval<Argument<" + type + ">&>().get())";
            arguments += "a" + n + ".get()";
            source += "Argument<" + type + "> a" + n + "(frame, " + n + ");\n";
        }
        source += "using NativeFunction = " + signature + ");\n";
        source += linked ? "const NativeFunction call = &chali_linked_" + suffix + ";\n" : "static const auto call = reinterpret_cast<NativeFunction>(chali_host_abi_symbol(" + CxxStringLiteral(function.abiLibrary) + ", " + CxxStringLiteral(function.abiSymbol) + "));\n";
        source += (returns ? "const auto returned = " : "") + Memory::String("call(") + arguments + ");\n";
        if (returns && CAbiType(function.signature.result).find('*') != Memory::String::npos)
            source += "owned.pointer = returned;\n";
        for (std::size_t arg = 0; arg < function.signature.params.size(); ++arg)
            source += "a" + Memory::toString(arg) + ".check();\n";
        if (returns) source += "Result<" + result + ">::store(frame, returned, count);\n";
        else source += "(void)count;\n";
        source += "} catch (const std::exception &error) { chali_abi_fail(raw, error.what()); } catch (...) { chali_abi_fail(raw, \"native ABI call failed\"); }\n}\n";
    }
    source += "/**\n * @brief 校验原生符号并原子登记 C 调用入口。\n * @param registry 当前运行时的 C 注册句柄。\n * @param error 可选的有界诊断。\n * @return 稳定 C 状态码。\n */\nextern \"C\" int32_t chali_program_abi_bind(ChaliAbiRegistry *registry, ChaliAbiError *error) {\n";
    source += "if (!registry) return chali_abi_status(error, ChaliAbiInvalidArgument, \"missing registry\");\n";
    for (const auto *function : functions)
    {
        if (::chali_aot::isDirectLinkLibrary(function->abiLibrary)) continue;
        source += "if (!chali_host_abi_lookup(" + CxxStringLiteral(function->abiLibrary) + ", " + CxxStringLiteral(function->abiSymbol) + ", error)) return ChaliAbiHostFailure;\n";
        if (!function->abiRelease.empty())
            source += "if (!chali_host_abi_lookup(" + CxxStringLiteral(function->abiLibrary) + ", " + CxxStringLiteral(function->abiRelease) + ", error)) return ChaliAbiHostFailure;\n";
    }
    if (!functions.empty())
    {
        source += "static const ChaliAbiRecord records[] = {\n";
        for (std::size_t index = 0; index < functions.size(); ++index)
            source += "{" + CxxStringLiteral(functions[index]->label) + ", &chali_abi_call_" + Memory::toString(index) + ", " + Memory::toString(functions[index]->signature.params.size()) + "},\n";
        source += "};\nconst auto abi = chali_abi_local_info(CHALI_ABI_CAP_TRACKED_TEMPORARY);\nconst ChaliAbiModuleDescriptor module{sizeof(ChaliAbiModuleDescriptor), &abi, records, sizeof(records)/sizeof(records[0])};\nreturn chali_abi_registry_register(registry, &module, error);\n";
    }
    else source += "return chali_abi_status(error, ChaliAbiOk, nullptr);\n";
    source += "}\n";
    return "#include \"abi/chali_native_marshal.hpp\"\n#include \"abi/chali_abi_internal.h\"\n" + definitions + source + CAbiExportWrappers();
}

/**
 * @brief 将前端 C++ ABI 类型规范化为 AOT 接口使用的 C ABI 类型描述。
 * @param abiType `CAbiType` 给出的名字（`std::int64_t` / `double` / `const char *` …）。
 * @return C ABI 类型拼写，由 LLVM 降低器转换成 LLVM 类型；未知类型按 long long 兜底。
 */
static Memory::String AotCType(const Memory::String &abiType)
{
    if (abiType.empty()) return "long long";
    if (abiType == "void") return "void";
    if (abiType == "bool") return "bool";
    if (abiType == "float" || abiType == "double") return abiType;
    if (abiType == "const char *" || abiType == "char *") return "const char *";
    const bool unsigned_ = abiType.find("uint") != Memory::String::npos;
    if (abiType.find("int8_t") != Memory::String::npos) return unsigned_ ? "unsigned char" : "signed char";
    if (abiType.find("int16_t") != Memory::String::npos) return unsigned_ ? "unsigned short" : "short";
    if (abiType.find("int32_t") != Memory::String::npos) return unsigned_ ? "unsigned int" : "int";
    if (abiType.find("int64_t") != Memory::String::npos) return unsigned_ ? "unsigned long long" : "long long";
    return "long long";
}

/**
 * @brief 收集外部函数的符号、ABI 签名及链接库，供 LLVM 降低器生成声明和调用。
 *
 * 这是"AOT 支持外部 C 调用"的源头：VM 那条路在运行期 `dlopen` 这些符号，
 * AOT 将它们转换为 LLVM 函数声明与链接参数，产物直接调用外部符号。
 * @return 按声明顺序排列的外部函数清单。
 */
Memory::Vector<::chali_aot::ExternFunction> ChaliAotAbiExterns()
{
    Memory::Vector<::chali_aot::ExternFunction> out;
    for (const MFunctionDecl &function : g_functionDecls)
    {
        if (function.abiLibrary.empty() || function.superseded) continue;
        ::chali_aot::ExternFunction entry;
        entry.abiConvention = function.abiConvention.empty() ? "C" : function.abiConvention;
        entry.label = function.label;
        entry.symbol = function.abiSymbol.empty() ? function.label : function.abiSymbol;
        entry.library = function.abiLibrary;
        entry.requiresMarshalling = CAbiNeedsMarshalling(function);
        const bool languageAbi = LibraryLanguageAbi(function.abiConvention);
        entry.borrowedResult = languageAbi && LibraryBorrowParameter(function.signature.result);
        entry.resultType = AotCType(languageAbi ? LibraryNativeType(function.signature.result) : CAbiType(function.signature.result));
        for (const auto &parameter : function.signature.params)
            entry.parameterTypes.push_back(AotCType((LibraryLanguageAbi(function.abiConvention)) ? LibraryNativeType(parameter.type) : CAbiType(parameter.type)));
        out.push_back(std::move(entry));
    }
    return out;
}

/* 把上面这个函数挂给 AOT：它自己是独立模块，不能反向 include 前端，
 * 所以由前端在静态初始化时注入一次取 abi 声明的办法。 */
/**
 * @brief 为不能按标量直接调用的 ABI 声明生成统一宿主封送绑定。
 * @return 存在非标量 ABI 类型时返回全部绑定源码，否则返回空串。
 */
Memory::String ChaliAotAbiGlue()
{
    if (!CAbiExportFunctions().empty()) return AbiGlueSource();
    for (const auto &function : g_functionDecls)
    {
        if (function.abiLibrary.empty() || function.superseded) continue;
        if (CAbiNeedsMarshalling(function)) return AbiGlueSource();
    }
    return {};
}

static const bool kChaliAotRegistered = (::chali_aot::setAbiExternProvider(&ChaliAotAbiExterns),
                                         ::chali_aot::setAbiGlueProvider(&ChaliAotAbiGlue),
                                         ::chali_aot::setAbiExportProvider(&ChaliAotAbiExports),
                                         ::chali_aot::setAbiExportHeaderProvider(&ChaliAotAbiExportHeader), true);

/**
 * @brief 写出编译产生的四元式、数据与二进制产物。
 * @param prefix 生成产物文件的路径前缀。
 * @param result 接收处理结果的对象。
 * @param binaryPath 二进制产物的文件路径。
 * @param layoutPath 数据布局描述文件的路径。
 * @param parsed 已完成组装的程序产物。
 * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
 */
bool WriteArtifacts(const Memory::String &prefix, const ast::CodegenResult &result,
                    Memory::String *binaryPath, Memory::String *layoutPath, stackvm::QuadFile *parsed)
{
    *binaryPath = prefix + ".data.bin";
    *layoutPath = prefix + ".layout";
    Memory::String layout;
    for (const Memory::String &line : result.exportDataLayout()) layout += line + "\n";
    /* 仅在程序导入 C ABI 符号时生成可供 VM 宿主编入的绑定胶水；
     * 没有绑定时删除同前缀旧文件，避免误用其他程序的符号声明。 */
    const Memory::String abiGlue = AbiGlueSource();
    if (abiGlue.empty())
    {
        std::error_code removed;
        std::filesystem::remove(std::filesystem::path(prefix + ".abi.cpp"), removed);
    }
    else if (!WriteFile(prefix + ".abi.cpp", abiGlue))
        return false;
    *parsed = stackvm::parseQuads(result.quads, prefix + ".quad");
    const auto quadBytes = stackvm::encodeQuadBinary(*parsed);
    Memory::Vector<stackvm::LayoutRow> rows;
    if (quadBytes.empty() || !stackvm::decodeDataLayout(layout, rows)) return false;
    // 文本转储与 C 数组都是重复数据；清理旧版本留下的同前缀产物。
    for (const char *suffix : { ".data", ".data.h" })
    {
        std::error_code removed;
        std::filesystem::remove(std::filesystem::path(prefix + suffix), removed);
    }
    return WriteFile(prefix + ".quad", quadBytes) && WriteFile(*binaryPath, result.exportDataBinary()) && WriteFile(*layoutPath, stackvm::encodeDataLayout(rows));
}

/**
 * @brief 每次编译前清空降级期的状态（同一个进程里可以编译多份源文件）。
 */
void ResetState()
{
    g_debugMacroFrames.clear();
    g_debugMacroLocations.clear();
    chali_editor::compilerAnalysis.resetPass();
    g_diagnostics.clear();
    g_warnings.clear();
    g_borrowFacts.clear();
    g_borrowFlows.clear();
    g_borrowFlowCapture = nullptr;
    g_effectConflicts.clear();
    g_effectAliases.clear();
    g_inferenceConcreteTypes.clear();
    g_inferenceConcreteCount = 0;
    g_error.clear();
    g_errorLine = 0;
    g_errorColumn = 0;
    g_constantBindings.clear();
    g_addressTakenNames.clear();
    g_functionAddressTakenNames = nullptr;
    g_referenceStorage.clear();
    g_parameterReferenceOwners.clear();
    g_pendingParameterStorage.clear();
    g_parameterValueStorage.clear();
    for (std::size_t index = 0; index < g_nodes.size(); ++index)
        if (N(static_cast<int>(index)).kind == MNode::MacroCall)
            CollectMacroArgumentReferenceNames(static_cast<int>(index), g_addressTakenNames);
    for (const auto &node : g_nodes)
        if (node.kind == MNode::Un && node.s == "&" && !node.kids.empty())
        {
            const auto &operand = N(node.kids[0]);
            if (operand.kind == MNode::Var) g_addressTakenNames.insert(operand.s);
            if (operand.kind == MNode::EnumCtor) g_addressTakenNames.insert(operand.s2);
        }
    /* `move x` 也要取 x 的地址；容器 / 字符串 / 标量的引用槽在声明处就准备好，
     * 否则 `move list` 会在降级时因为变量没有稳定存储而被拒绝。 */
    for (const auto &node : g_nodes)
        if (node.kind == MNode::Call && !node.kids.empty() && StandardCallName(N(node.kids[0])) == "__move")
            for (const int argument : N(node.kids[1]).kids)
            {
                const auto &operand = N(argument);
                if (operand.kind == MNode::Var) g_addressTakenNames.insert(operand.s);
            }
    /* 捕获在函数体降级时才解析；提前为 lambda 中的变量准备稳定引用槽。 */
    std::function<void(int)> prepareCaptureStorage = [&](int id) {
        if (id < 0) return;
        const auto node = N(id);
        if (node.kind == MNode::Var) g_addressTakenNames.insert(node.s);
        if (node.kind == MNode::Block) prepareCaptureStorage(static_cast<int>(node.i));
        for (const int child : node.kids) prepareCaptureStorage(child);
    };
    for (const auto &node : g_nodes)
        if (node.kind == MNode::FuncDef && node.s.rfind("__chali_lambda_", 0) == 0)
            for (const int child : node.kids) prepareCaptureStorage(child);
    // 带值退出在作用域绑定后才选择内部变量移动，提前准备其可能使用的稳定引用槽。
    std::function<void(int)> prepareBreakStorage = [&](int id) {
        if (id < 0) return;
        const auto &node = N(id);
        if (node.kind == MNode::Var) g_addressTakenNames.insert(node.s);
        if (node.kind == MNode::Tuple)
            for (const int child : node.kids) prepareBreakStorage(child);
    };
    for (const auto &node : g_nodes)
        if (node.kind == MNode::Break && node.type2 != 1 && !node.kids.empty()) prepareBreakStorage(node.kids[0]);
    for (const auto &specification : chali_lambda::captureSpecifications)
        for (const auto &capture : specification.second) g_addressTakenNames.insert(capture.first);
    /* 迭代器入口隐式借用或转移容器，必须在局部声明降级前准备引用槽。 */
    for (const auto &node : g_nodes)
    {
        if (node.kind != MNode::Call || node.kids.empty()) continue;
        const auto &callee = N(node.kids[0]);
        if (callee.kind != MNode::Member || callee.kids.empty() || (callee.s != "iter" && callee.s != "iterMut" && callee.s != "intoIter")) continue;
        const auto &receiver = N(callee.kids[0]);
        if (receiver.kind == MNode::Var) g_addressTakenNames.insert(receiver.s);
        if (receiver.kind == MNode::EnumCtor) g_addressTakenNames.insert(receiver.s2);
    }
    /* `map.getOption(k)` 在类型推导期改写成 `__mapGetOptionImpl(&m, k)`（见
     * `RewriteMapGetOption`）：那个 `&m` 是推导时才造出来的，这里扫不到，
     * 所以接收者的引用槽必须提前准备好，否则降级时会报"取不了地址"。 */
    for (const auto &node : g_nodes)
    {
        if (node.kind != MNode::Call || node.kids.empty()) continue;
        const auto &callee = N(node.kids[0]);
        if (callee.kind != MNode::Member || callee.kids.empty() || callee.s != "getOption") continue;
        const auto &receiver = N(callee.kids[0]);
        if (receiver.kind == MNode::Var) g_addressTakenNames.insert(receiver.s);
        if (receiver.kind == MNode::EnumCtor) g_addressTakenNames.insert(receiver.s2);
    }
    g_structs.clear();
    g_tupleElements.clear();
    g_staticFields.clear();
    g_enums.clear();
    g_funcs.clear();
    g_operators.clear();
    g_conversions.clear();
    g_indexes.clear();
    g_iterators.clear();
    g_hashes.clear();
    g_mapKeyPolicies.clear();
    g_collectionConstructors.clear();
    g_structType.clear();
    g_loweringProgram = nullptr;
    g_scope.clear();
    g_globalInit.clear();
    g_usedNames.clear();
    g_owner.clear();
    g_currentModule.clear();
    g_localSerial = 0;
    /* 接口 / 类型号 / 装箱的上下文（同一个进程里编译第二份源文件时不能继承） */
    g_interfaces.clear();
    g_interfaceBases.clear();
    g_interfaceOrder.clear();
    g_usedInterfaceMethods.clear();
    g_typeIdIndex.clear();
    g_debugTypeIds.clear();
    g_nextTypeId = 1;
    g_expectedObjGet = nullptr;
    RegisterBuiltinTypeIds(); /* 内建占 1..9、10..32 预留：`__typeTag(int)` 永远是 1 */
    g_functionIndex.clear();
    g_moduleImports.clear();
    g_inferenceFunctions.clear();
    g_functionDecls.clear();
    g_lambdaCaptures.clear();
    g_lambdaAutoReturns.clear();
    g_lambdaPrepared.clear();
    g_hostTypes.clear();
    g_opcodeTypes.clear();
    g_opcodeValueTypes.clear();
    g_hostInterfaceImpls.clear();
    g_globalNames.clear();
    /* "用到才生成"的运行时库开关也要归位（同一个进程里编译第二份源文件时不能继承） */
    g_usesNewArray = false;
    g_usesStructCopy = false;
    g_usesLanguageMove = true;  // 自动清理须区分已经移动或显式销毁的聚合。
    for (const auto &node : g_nodes)
    {
        if (node.kind != MNode::Call || node.kids.empty()) continue;
        const auto &callee = N(node.kids[0]);
        if ((callee.kind == MNode::EnumCtor && callee.s == "std::lang" && callee.s2 == "__move") || (callee.kind == MNode::Var && callee.s == "__move")) g_usesLanguageMove = true;
    }
    g_needsAggregateRelease.clear();
    g_needsCaptureDestructor.clear();
    g_needsInterfaceBox.clear();
    g_exhaustiveMatches.clear();
    g_needsAggregateCopy.clear();
    g_expectedFuncType = nullptr;
    g_expectedNewType = nullptr;
    /* 循环的跳转目标栈按降级过程自然清空；这里再兜一次底，
     * 免得上一次编译在中途跳出（报错提前返回）留下的残状态影响下一次。 */
    g_blockStack.clear();
    g_blockSerial = 0;
    g_ownedScopes.clear();
    g_ownedHome.clear();
    // 泛型重放会复用内部名字，不能让上一轮的数组清理布局污染新一轮聚合。
    g_arrayElementTypes.clear();
    g_arrayFixedCounts.clear();
    g_resourceCloseStates.clear();
    g_resourceCloseAliases.clear();
    g_scope.push_back(Memory::Map<Memory::String, MVar>()); /* 0 号 = 全局作用域 */
    /* 运行时辅助函数的内部名字先占住，免得用户程序撞名 */
    for (const char *name : { "__chali_p", "__chali_ip", "__chali_fr", "__chali_k", "__chali_d", "__chali_code",
                              "__chali_num", "__chali_flt", "__chali_addr", "__chali_tgt", "__chali_src", "__chali_n",
                              "__chali_val", "__chali_l", "__chali_r" })
        g_usedNames.insert(name);
}
