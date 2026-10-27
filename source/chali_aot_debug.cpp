/**
 * @file chali_aot_debug.cpp
 * @brief 原生源码行、参数和局部槽的 DWARF 生成。
 */
#include "chali_aot_debug.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <sstream>

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
    auto source = std::filesystem::u8path(path.empty() ? "<generated>" : path);
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
    DebugLine origin;
    for (const auto &entry : lines)
        if (!entry.second.file.empty() && entry.second.line)
        {
            origin = entry.second;
            break;
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
    std::map<std::pair<std::string, std::size_t>, std::size_t> locations;
    const auto location = [&](DebugLine position) {
        if (position.file.empty() || position.line == 0) position = origin;
        const auto key = std::make_pair(position.file, position.line);
        const auto found = locations.find(key);
        if (found != locations.end()) return found->second;
        const auto localScope = position.file == origin.file ? scope : node("!DILexicalBlockFile(scope: !" + std::to_string(scope) + ", file: !" + std::to_string(file(position.file)) + ", discriminator: 0)");
        const auto id = node("!DILocation(line: " + std::to_string(std::max<std::size_t>(1, position.line)) + ", column: 1, scope: !" + std::to_string(localScope) + ")");
        locations.emplace(key, id);
        return id;
    };
    const auto entryLocation = location(origin);
    std::string declarations;
    for (const auto &variable : variables)
    {
        if (variable.name.empty() || variable.name.rfind("__", 0) == 0u) continue;
        auto visible = variable.name.substr(0, variable.name.find('$'));
        const auto id = node("!DILocalVariable(name: " + metadataString(visible) + (variable.argument ? ", arg: " + std::to_string(variable.argument) : "")
                             + ", scope: !" + std::to_string(scope) + ", file: !" + std::to_string(source) + ", line: " + std::to_string(std::max<std::size_t>(1, origin.line))
                             + ", type: !" + std::to_string(variable.floating ? real_ : integer_) + ")");
        declarations += "  call void @llvm.dbg.declare(metadata ptr " + variable.pointer + ", metadata !" + std::to_string(id)
                        + ", metadata !DIExpression()), !dbg !" + std::to_string(entryLocation) + "\n";
    }
    std::istringstream input(definition);
    std::string line, output;
    // 入口清零与参数保存属于原生序言，不应作为首条源码语句停下。
    auto currentLocation = node("!DILocation(line: 0, column: 0, scope: !" + std::to_string(scope) + ")");
    bool inserted = false;
    while (std::getline(input, line))
    {
        if (line.rfind("define ", 0) == 0u)
            line.insert(line.rfind('{'), "!dbg !" + std::to_string(scope) + " ");
        if (line.size() > 2u && line[0] == 'q' && line.back() == ':')
        {
            const auto at = static_cast<std::size_t>(std::stoull(line.substr(1)));
            const auto found = lines.find(at);
            if (found != lines.end()) currentLocation = location(found->second);
        }
        if (!inserted && line.rfind("  br label %q", 0) == 0u)
        {
            output += declarations;
            inserted = true;
        }
        if (line.rfind("  ", 0) == 0u && line.find("!dbg !") == std::string::npos)
            line += ", !dbg !" + std::to_string(currentLocation);
        output += line + "\n";
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
        if (at == std::string::npos) module += "!llvm.module.flags = !{" + flags + "}\n";
        else module.insert(module.find('}', at), ", " + flags);
    }
    const auto units = module.find("!llvm.dbg.cu = !{");
    if (units == std::string::npos) module += "!llvm.dbg.cu = !{!" + std::to_string(unit_) + "}\n";
    else module.insert(module.find('}', units), ", !" + std::to_string(unit_));
    module += "declare void @llvm.dbg.declare(metadata, metadata, metadata)\n" + metadata_;
}
}  // namespace chali_aot
