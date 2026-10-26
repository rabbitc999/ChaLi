/**
 * @file abi_export.cpp
 * @brief 编译器导出指令的 C 类型、包装与声明生成，由语义产物分片包含。
 */
/**
 * @brief 选择使用显式导出指令的有效函数，顺序与 LLVM 索引分派一致。
 * @return 按声明顺序排列的函数指针。
 */
Memory::Vector<const MFunctionDecl *> CAbiExportFunctions()
{
    Memory::Vector<const MFunctionDecl *> result;
    for (const auto &function : g_functionDecls)
        if (!function.superseded && !function.exportSymbol.empty()) result.push_back(&function);
    return result;
}
/**
 * @brief 生成与内部逻辑地址调用分离的原生导出元数据。
 * @return 含准确 C 类型的导出声明。
 */
Memory::Vector<::chali_aot::ExternFunction> ChaliAotAbiExports()
{
    Memory::Vector<::chali_aot::ExternFunction> result;
    for (const auto *function : CAbiExportFunctions())
    {
        ::chali_aot::ExternFunction entry;
        entry.label = function->label;
        entry.symbol = function->exportSymbol;
        entry.abiConvention = "C";
        entry.resultType = CAbiType(function->signature.result);
        for (const auto &parameter : function->signature.params) entry.parameterTypes.push_back(CAbiType(parameter.type));
        result.push_back(std::move(entry));
    }
    return result;
}
/**
 * @brief 将标准 C++ 固定位宽类型名转换为可供 C 包含的声明。
 * @param type 已验证的 C ABI 类型。
 * @return 纯 C 拼写。
 */
Memory::String CExportType(const std::shared_ptr<MType> &type)
{
    auto name = CAbiType(type);
    for (std::size_t at = name.find("std::"); at != Memory::String::npos; at = name.find("std::")) name.erase(at, 5);
    return name;
}
/**
 * @brief 生成纯 C 的结构体布局与函数原型，不公开内部六十四位槽约定。
 * @return 导出头文件的声明片段。
 */
Memory::String ChaliAotAbiExportHeader()
{
    Memory::String out;
    Memory::Set<Memory::String> emitted;
    Memory::Function<void(const std::shared_ptr<MType> &)> emit;
    emit = [&](const std::shared_ptr<MType> &input) {
        const auto type = CAbiUnderlying(input);
        if (!type || type->kind != MType::Struct || !emitted.insert(type->name).second) return;
        const auto &definition = g_structs.at(type->name);
        for (const auto &field : definition.fields) emit(field.type);
        const auto name = CExportType(type);
        if (definition.abiPacked) out += "#pragma pack(push, 1)\n";
        out += "typedef struct " + name + " {\n";
        for (std::size_t i = 0; i < definition.fields.size(); ++i)
            out += "    " + CExportType(definition.fields[i].type) + " f" + Memory::toString(i) + ";\n";
        out += "} " + name + ";\n";
        if (definition.abiPacked) out += "#pragma pack(pop)\n";
    };
    for (const auto *function : CAbiExportFunctions())
    {
        emit(function->signature.result);
        for (const auto &parameter : function->signature.params) emit(parameter.type);
        out += "/**\n * @brief 按目标平台 C ABI 调用导出实现。\n";
        for (std::size_t i = 0; i < function->signature.params.size(); ++i)
            out += " * @param p" + Memory::toString(i) + " 按原生类型传入的值。\n";
        out += " * @return 原生返回值；void 函数无返回值。\n */\n";
        out += CExportType(function->signature.result) + " " + function->exportSymbol + "(";
        for (std::size_t i = 0; i < function->signature.params.size(); ++i)
        {
            if (i) out += ", ";
            out += CExportType(function->signature.params[i].type) + " p" + Memory::toString(i);
        }
        if (function->signature.params.empty()) out += "void";
        out += ");\n";
    }
    return out;
}
/**
 * @brief 生成由宿主 C++ 编译器承担平台聚合 ABI 降低的类型化包装。
 * @return 包装实现；原生函数地址不与语言函数值相互转换。
 */
Memory::String CAbiExportWrappers()
{
    const auto functions = CAbiExportFunctions();
    if (functions.empty()) return {};
    Memory::String out = "extern \"C\" std::int64_t chali_export_dispatch(std::int64_t, const std::int64_t *);\nextern \"C\" void chali_host_export_apply(void (*)(stackvm::NativeCallContext *, void *), void *);\n";
    out += "/** @brief 清理本次导出参数的结构体封送副本。 */\nstruct __chali_export_arguments { chali_glue::Context &context; Memory::Vector<std::int64_t> addresses;\n/** @brief 清理本帧参数，不清理语言函数内部产生的返回借用。 */\n~__chali_export_arguments() { for (auto address : addresses) context.releaseMemory(address, true); } };\n";
    for (std::size_t index = 0; index < functions.size(); ++index)
    {
        const auto &function = *functions[index];
        const auto suffix = Memory::toString(index);
        const auto frame = "__chali_export_frame_" + suffix;
        const bool returns = function.signature.result->kind != MType::Void;
        out += "/** @brief 当前导出调用的原生参数与返回值，重入时独立存储。 */\nstruct " + frame + " {\n";
        for (std::size_t i = 0; i < function.signature.params.size(); ++i)
            out += CAbiType(function.signature.params[i].type) + " p" + Memory::toString(i) + ";\n";
        if (returns) out += CAbiType(function.signature.result) + " result{};\n";
        out += "};\n/**\n * @brief 编解码一次原生导出调用。\n * @param context 当前上下文。\n * @param payload 本次调用独立的参数帧。\n */\nstatic void __chali_export_marshal_" + suffix + "(stackvm::NativeCallContext *context, void *payload) {\n";
        out += "__chali_export_arguments owned{*context, {}};\nauto &frame = *static_cast<" + frame + " *>(payload);\nstd::int64_t arguments[" + Memory::toString(std::max<std::size_t>(1, function.signature.params.size())) + "]{};\n";
        for (std::size_t i = 0; i < function.signature.params.size(); ++i)
        {
            const auto n = Memory::toString(i);
            out += "auto v" + n + " = chali_glue::Codec<" + CAbiType(function.signature.params[i].type) + ">::encode(*context, frame.p" + n + ");\n";
            if (CAbiUnderlying(function.signature.params[i].type)->kind == MType::Struct)
                out += "owned.addresses.push_back(v" + n + ".asInt());\n";
            out += "if (v" + n + ".isFloat()) { const double real = v" + n + ".asFloat(); std::memcpy(&arguments[" + n + "], &real, 8); } else arguments[" + n + "] = v" + n + ".asInt();\n";
        }
        out += "const auto bits = chali_export_dispatch(" + suffix + ", arguments);\n";
        if (returns)
        {
            if (CAbiUnderlying(function.signature.result)->kind == MType::Double)
                out += "double real; std::memcpy(&real, &bits, 8); auto value = stackvm::NativeValue::fromFloat(real);\n";
            else
                out += "auto value = stackvm::NativeValue::fromInt(bits);\n";
            out += "frame.result = chali_glue::Codec<" + CAbiType(function.signature.result) + ">::decode(*context, value);\n";
        }
        else
            out += "(void)bits;\n";
        out += "}\n#ifdef _WIN32\n#define CHALI_C_EXPORT __declspec(dllexport)\n#else\n#define CHALI_C_EXPORT __attribute__((visibility(\"default\")))\n#endif\n";
        out += "/**\n * @brief 以声明的原生 C 符号调用语言实现。\n";
        for (std::size_t i = 0; i < function.signature.params.size(); ++i) out += " * @param p" + Memory::toString(i) + " 原生参数。\n";
        out += " * @return 已解码的原生结果。\n */\nextern \"C\" CHALI_C_EXPORT " + CAbiType(function.signature.result) + " " + function.exportSymbol + "(";
        for (std::size_t i = 0; i < function.signature.params.size(); ++i)
        {
            if (i) out += ", ";
            out += CAbiType(function.signature.params[i].type) + " p" + Memory::toString(i);
        }
        out += ") {\n" + frame + " frame{};\n";
        for (std::size_t i = 0; i < function.signature.params.size(); ++i)
        {
            const auto n = Memory::toString(i);
            out += "frame.p" + n + " = p" + n + ";\n";
        }
        out += "chali_host_export_apply(&__chali_export_marshal_" + suffix + ", &frame);\n";
        if (returns) out += "return frame.result;\n";
        out += "}\n#undef CHALI_C_EXPORT\n";
    }
    return out;
}
