/**
 * @file chali_aot_debug.cpp
 * @brief 原生源码行、参数和局部槽的 DWARF 生成。
 */
#include "chali_aot_debug.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <functional>
#include <tuple>

namespace chali_aot
{
namespace
{
/**
 * @brief 按 LLVM 字节转义规则引用 UTF-8 文本。
 * @param text 原始名字或路径。
 * @return 带引号的元数据字符串。
 */
std::string metadataString(const std::string &text)
{
    std::string result = "\"";
    for (const unsigned char byte : text)
        if (byte >= 32u && byte < 127u && byte != '"' && byte != '\\')
            result += static_cast<char>(byte);
        else
        {
            char escaped[4];
            std::snprintf(escaped, sizeof(escaped), "\\%02X", byte);
            result += escaped;
        }
    return result + "\"";
}
/**
 * @brief 将 UTF-8 字符串写为严格 JSON 字符串，不生成或执行目标程序。
 * @param text 原始文本。
 * @return 带引号的 JSON 字符串。
 */
std::string debugJsonString(const std::string &text)
{
    std::string result = "\"";
    for (const unsigned char byte : text)
    {
        if (byte == '"' || byte == '\\')
        {
            result += '\\';
            result += static_cast<char>(byte);
        }
        else if (byte < 32)
        {
            char escaped[7];
            std::snprintf(escaped, sizeof escaped, "\\u%04x", static_cast<unsigned>(byte));
            result += escaped;
        }
        else
            result += static_cast<char>(byte);
    }
    return result + "\"";
}
/**
 * @brief 固定真实源文件的构建期绝对路径，虚拟和不可访问来源保留原身份。
 * @param value 编译器记录的 UTF-8 来源。
 * @return 不依赖调试启动目录的文件名。
 */
std::string debugSourcePath(const std::string &value)
{
    if (value.empty() || value.front() == '<' || value.find("://") != value.npos) return value;
    std::error_code error;
    const auto source = std::filesystem::u8path(value);
    if (!std::filesystem::is_regular_file(source, error)) return value;
    const auto absolute = std::filesystem::absolute(source, error);
    return error ? value : absolute.lexically_normal().u8string();
}
}  // namespace
/**
 * @brief 追加独立的元数据节点。
 * @param value 节点正文。
 * @return 节点编号。
 */
std::size_t NativeDebugInfo::node(const std::string &value)
{
    const auto id = next_++;
    metadata_ += "!" + std::to_string(id) + " = " + value + "\n";
    return id;
}
/**
 * @brief 为原始路径登记 DWARF 文件。
 * @param path UTF-8 路径。
 * @return 对应文件节点。
 */
std::size_t NativeDebugInfo::file(const std::string &path)
{
    const auto found = files_.find(path);
    if (found != files_.end()) return found->second;
    auto source = std::filesystem::u8path(path.empty() ? "<generated>" : debugSourcePath(path));
    const auto id = node("!DIFile(filename: " + metadataString(source.filename().u8string()) + ", directory: " + metadataString(source.parent_path().u8string()) + ")");
    files_.emplace(path, id);
    return id;
}
/**
 * @brief 为真实机器帧添加源位置和变量，不生成解释器状态。
 * @param definition LLVM 函数定义。
 * @param name 源函数名。
 * @param linkage LLVM 链接名字。
 * @param lines 四元式位置。
 * @param variables 源变量的帧地址。
 * @return 带 DWARF 记录的函数定义。
 */
std::string NativeDebugInfo::annotate(const std::string &definition, const std::string &name, const std::string &linkage,
                                      const std::map<std::size_t, DebugLine> &lines, const std::vector<DebugVariable> &variables)
{
    return annotate(definition, name, linkage, lines, variables, {});
}
/**
 * @brief 按实际表示类型、声明位置和词法层级建立原生调试描述。
 * @param definition LLVM 函数定义。
 * @param name 用户函数名。
 * @param linkage 实际符号。
 * @param lines 源码位置及所在作用域。
 * @param variables 源变量和实际帧地址。
 * @param scopes 用户词法作用域树。
 * @return 标注后的 LLVM 函数。
 */
std::string NativeDebugInfo::annotate(const std::string &definition, const std::string &name, const std::string &linkage,
                                      const std::map<std::size_t, DebugLine> &lines, const std::vector<DebugVariable> &variables,
                                      const std::map<std::size_t, DebugScope> &scopes)
{
    DebugLine origin;
    for (const auto &entry : lines)
        if (!entry.second.file.empty() && entry.second.line)
        {
            origin = entry.second;
            break;
        }
    if (const auto declaration = scopes.find(0); declaration != scopes.end() && declaration->second.origin.line) origin = declaration->second.origin;
    if (!scopes.empty())
    {
        const auto function = functions_.size();
        functions_.push_back({ name, linkage, origin, variables, scopes });
        for (const auto &entry : lines)
            if (entry.second.line && !entry.second.file.empty()) positions_.push_back({ function, entry.first, entry.second });
    }
    const auto source = file(origin.file);
    if (!unit_)
    {
        empty_ = node("!{}");
        integer_ = node("!DIBasicType(name: \"int64\", size: 64, encoding: DW_ATE_signed)");
        real_ = node("!DIBasicType(name: \"float64\", size: 64, encoding: DW_ATE_float)");
        unit_ = node("distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !" + std::to_string(source)
                     + ", producer: \"ChaLi\", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)");
    }
    const auto type = node("!DISubroutineType(types: !" + std::to_string(empty_) + ")");
    const auto scope = node("distinct !DISubprogram(name: " + metadataString(name) + ", linkageName: " + metadataString(linkage)
                            + ", scope: !" + std::to_string(source) + ", file: !" + std::to_string(source) + ", line: " + std::to_string(std::max<std::size_t>(1, origin.line))
                            + ", type: !" + std::to_string(type) + ", scopeLine: " + std::to_string(std::max<std::size_t>(1, origin.line))
                            + ", spFlags: DISPFlagDefinition, unit: !" + std::to_string(unit_) + ", retainedNodes: !" + std::to_string(empty_) + ")");
    std::map<std::size_t, std::size_t> lexicalScopes{ { 0, scope } };
    std::function<std::size_t(std::size_t)> lexicalScope = [&](std::size_t id) {
        const auto found = lexicalScopes.find(id);
        if (found != lexicalScopes.end()) return found->second;
        const auto descriptor = scopes.find(id);
        if (descriptor == scopes.end()) return scope;
        const auto parent = lexicalScope(descriptor->second.parent);
        const auto &position = descriptor->second.origin;
        const auto result = node("distinct !DILexicalBlock(scope: !" + std::to_string(parent) + ", file: !" + std::to_string(file(position.file.empty() ? origin.file : position.file))
                                 + ", line: " + std::to_string(std::max<std::size_t>(1, position.line)) + ", column: " + std::to_string(position.column) + ")");
        lexicalScopes.emplace(id, result);
        return result;
    };
    std::map<std::tuple<std::string, std::size_t, std::size_t, std::size_t>, std::size_t> locations;
    const auto location = [&](DebugLine position) {
        if (position.file.empty() || position.line == 0) position = origin;
        const auto key = std::make_tuple(position.file, position.line, position.column, position.scope);
        const auto found = locations.find(key);
        if (found != locations.end()) return found->second;
        auto localScope = lexicalScope(position.scope);
        const auto descriptor = scopes.find(position.scope);
        const auto &scopeFile = descriptor == scopes.end() || descriptor->second.origin.file.empty() ? origin.file : descriptor->second.origin.file;
        if (position.file != scopeFile) localScope = node("!DILexicalBlockFile(scope: !" + std::to_string(localScope) + ", file: !" + std::to_string(file(position.file)) + ", discriminator: 0)");
        const auto id = node("!DILocation(line: " + std::to_string(std::max<std::size_t>(1, position.line)) + ", column: " + std::to_string(position.column) + ", scope: !" + std::to_string(localScope) + ")");
        locations.emplace(key, id);
        return id;
    };
    const auto entryLocation = location(origin);
    // 按父先子后的编号预建词法树，避免大量顺序声明触发深递归。
    for (const auto &entry : scopes) lexicalScope(entry.first);
    std::map<std::size_t, std::string> declarations;
    for (const auto &variable : variables)
    {
        if (variable.name.empty() || variable.name.rfind("__", 0) == 0u) continue;
        auto visible = variable.name.substr(0, variable.name.find('$'));
        auto variableType = variable.floating ? real_ : integer_;
        if (!variable.typeName.empty())
        {
            const auto encoding = variable.encoding.empty() ? (variable.floating ? "DW_ATE_float" : "DW_ATE_signed") : variable.encoding;
            const auto key = variable.typeName + ":" + std::to_string(variable.bits) + ":" + encoding;
            const auto found = types_.find(key);
            if (found != types_.end())
                variableType = found->second;
            else
            {
                variableType = node("!DIBasicType(name: " + metadataString(variable.typeName) + ", size: " + std::to_string(variable.bits) + ", encoding: " + encoding + ")");
                types_.emplace(key, variableType);
            }
        }
        const auto id = node("!DILocalVariable(name: " + metadataString(visible) + (variable.argument ? ", arg: " + std::to_string(variable.argument) : "")
                             + ", scope: !" + std::to_string(lexicalScope(variable.scope)) + ", file: !" + std::to_string(file(variable.file.empty() ? origin.file : variable.file)) + ", line: " + std::to_string(std::max<std::size_t>(1, variable.line ? variable.line : origin.line))
                             + ", type: !" + std::to_string(variableType) + ")");
        const auto declarationLocation = variable.block ? location({ variable.file, variable.line, variable.column, variable.scope }) : entryLocation;
        declarations[variable.block] += "  call void @llvm.dbg.declare(metadata ptr " + variable.pointer + ", metadata !" + std::to_string(id)
                                        + ", metadata !DIExpression()), !dbg !" + std::to_string(declarationLocation) + "\n";
    }
    std::istringstream input(definition);
    std::string line, output;
    // 入口清零与参数保存属于原生序言，不应作为首条源码语句停下。
    auto currentLocation = node("!DILocation(line: 0, column: 0, scope: !" + std::to_string(scope) + ")");
    bool inserted = false;
    while (std::getline(input, line))
    {
        std::string after;
        if (line.rfind("define ", 0) == 0u)
            line.insert(line.rfind('{'), "!dbg !" + std::to_string(scope) + " ");
        if (line.size() > 2u && line[0] == 'q' && line.back() == ':')
        {
            const auto at = static_cast<std::size_t>(std::stoull(line.substr(1)));
            const auto found = lines.find(at);
            if (found != lines.end()) currentLocation = location(found->second);
            if (at) after = declarations[at];
        }
        if (!inserted && line.rfind("  br label %q", 0) == 0u)
        {
            output += declarations[0];
            inserted = true;
        }
        if (line.rfind("  ", 0) == 0u && line.find("!dbg !") == std::string::npos)
            line += ", !dbg !" + std::to_string(currentLocation);
        output += line + "\n";
        output += after;
    }
    return output;
}
/**
 * @brief 汇入调试节点并扩展既有模块版本标记。
 * @param module LLVM 模块。
 */
void NativeDebugInfo::finish(std::string &module)
{
    if (!unit_) return;
    std::string catalog = "{\"version\":1,\"pointerBytes\":8,\"byteOrder\":\"little\",\"functions\":[";
    for (std::size_t index = 0; index < functions_.size(); ++index)
    {
        const auto &function = functions_[index];
        if (index) catalog += ",";
        catalog += "{\"name\":" + debugJsonString(function.name) + ",\"linkage\":" + debugJsonString(function.linkage)
                   + ",\"file\":" + debugJsonString(debugSourcePath(function.origin.file)) + ",\"line\":" + std::to_string(function.origin.line)
                   + ",\"column\":" + std::to_string(function.origin.column) + ",\"scopes\":[";
        bool comma = false;
        for (const auto &entry : function.scopes)
        {
            if (comma) catalog += ",";
            comma = true;
            catalog += "{\"id\":" + std::to_string(entry.first) + ",\"parent\":" + std::to_string(entry.second.parent) + "}";
        }
        catalog += "],\"variables\":[";
        comma = false;
        for (const auto &variable : function.variables)
        {
            if (variable.name.empty() || variable.name.rfind("__", 0) == 0) continue;
            if (comma) catalog += ",";
            comma = true;
            catalog += "{\"name\":" + debugJsonString(variable.name.substr(0, variable.name.find('$')))
                       + ",\"binding\":" + debugJsonString(variable.name) + ",\"type\":" + debugJsonString(variable.typeName)
                       + ",\"scope\":" + std::to_string(variable.scope) + ",\"file\":" + debugJsonString(debugSourcePath(variable.file))
                       + ",\"line\":" + std::to_string(variable.line) + ",\"column\":" + std::to_string(variable.column)
                       + ",\"storage\":" + debugJsonString(variable.storage) + ",\"boxed\":" + (variable.storage.empty() ? "false" : "true")
                       + ",\"readOnly\":" + (variable.readOnly ? "true" : "false") + "}";
        }
        catalog += "]}";
    }
    catalog += "],\"positions\":[";
    std::string addresses;
    for (std::size_t index = 0; index < positions_.size(); ++index)
    {
        const auto &entry = positions_[index];
        if (index)
        {
            catalog += ",";
            addresses += ", ";
        }
        catalog += "{\"function\":" + std::to_string(entry.function) + ",\"file\":" + debugJsonString(debugSourcePath(entry.position.file))
                   + ",\"line\":" + std::to_string(entry.position.line) + ",\"column\":" + std::to_string(entry.position.column)
                   + ",\"scope\":" + std::to_string(entry.position.scope) + "}";
        addresses += "ptr blockaddress(@" + functions_[entry.function].linkage + ", %q" + std::to_string(entry.block) + ")";
    }
    catalog += "]}";
    module += "@chali_debug_catalog = constant [" + std::to_string(catalog.size() + 1) + " x i8] c" + metadataString(catalog + '\0') + "\n";
    module += "@chali_debug_catalog_size = constant i64 " + std::to_string(catalog.size()) + "\n";
    module += "@chali_debug_positions = constant [" + std::to_string(positions_.size()) + " x ptr] " + (positions_.empty() ? "zeroinitializer" : "[" + addresses + "]") + "\n";
    module += "@llvm.used = appending global [3 x ptr] [ptr @chali_debug_catalog, ptr @chali_debug_catalog_size, ptr @chali_debug_positions], section \"llvm.metadata\"\n";
    std::string flags;
    if (module.find("!\"Debug Info Version\"") == std::string::npos)
        flags = "!" + std::to_string(node("!{i32 2, !\"Debug Info Version\", i32 3}"));
    if (module.find("!\"Dwarf Version\"") == std::string::npos)
    {
        if (!flags.empty()) flags += ", ";
        flags += "!" + std::to_string(node("!{i32 2, !\"Dwarf Version\", i32 4}"));
    }
    if (!flags.empty())
    {
        const auto at = module.find("!llvm.module.flags = !{");
        if (at == std::string::npos)
            module += "!llvm.module.flags = !{" + flags + "}\n";
        else
            module.insert(module.find('}', at), ", " + flags);
    }
    const auto units = module.find("!llvm.dbg.cu = !{");
    if (units == std::string::npos)
        module += "!llvm.dbg.cu = !{!" + std::to_string(unit_) + "}\n";
    else
        module.insert(module.find('}', units), ", !" + std::to_string(unit_));
    module += "declare void @llvm.dbg.declare(metadata, metadata, metadata)\n" + metadata_;
}
}  // namespace chali_aot
