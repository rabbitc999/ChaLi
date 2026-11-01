#pragma once
#include "chali_diagnostics.hpp"
#include "ast/ast.hpp"
#include <functional>
#include <regex>
#include <sstream>

/** @brief 可配置警告；只检查已通过语义检查的程序，不参与所有权优化。 */
namespace chali_warnings
{
/** @brief 稳定规则目录。 */
struct Rule
{
    const char *id, *name;
    bool enabled;
};
inline constexpr Rule rules[] = { { "CHL1601", "overlapping-pattern", true },
                                  { "CHL2101", "type-naming", false },
                                  { "CHL2102", "function-naming", false },
                                  { "CHL2201", "unused-local", false } };
/** @brief 一次编译的警告配置，不共享可变状态。 */
struct Options
{
    Memory::Map<Memory::String, Memory::String> levels;
    bool errors = false;
    /**
     * @brief 配置规则或规则组，拒绝未知名称。
     * @param name 稳定代码、规则名、naming 或 all。
     * @param level allow、warn 或 error。
     * @return 名称有效时为 true。
     */
    bool set(const Memory::String &name, const Memory::String &level)
    {
        bool found = false;
        for (const auto &rule : rules)
            if (name == "all" || name == rule.id || name == rule.name || (name == "naming" && (Memory::String(rule.id) == "CHL2101" || Memory::String(rule.id) == "CHL2102")))
            {
                levels[rule.id] = level;
                found = true;
            }
        return found;
    }
    /**
     * @brief 查询最终级别。
     * @param id 规则代码。
     * @return allow、warn 或 error。
     */
    Memory::String level(const Memory::String &id) const
    {
        const auto found = levels.find(id);
        Memory::String result = "allow";
        if (found != levels.end())
            result = found->second;
        else
            for (const auto &rule : rules)
                if (id == rule.id) result = rule.enabled ? "warn" : "allow";
        return errors && result == "warn" ? Memory::String("error") : result;
    }
};
/**
 * @brief 取得真正的行注释，跳过字符串及块注释。
 * @param body 原始源码。
 * @return 每一行的行注释文本，无注释时为空。
 */
inline Memory::Vector<Memory::String> comments(const Memory::String &body)
{
    Memory::Vector<Memory::String> out(1);
    bool block = false;
    char quote = 0;
    for (std::size_t i = 0; i < body.size(); ++i)
    {
        const char c = body[i], next = i + 1 < body.size() ? body[i + 1] : 0;
        if (c == '\n')
        {
            out.emplace_back();
            quote = 0;
            continue;
        }
        if (block)
        {
            if (c == '*' && next == '/')
            {
                block = false;
                ++i;
            }
            continue;
        }
        if (quote)
        {
            if (c == '\\' && next)
                ++i;
            else if (c == quote)
                quote = 0;
            continue;
        }
        if (c == '"' || c == '\'')
        {
            quote = c;
            continue;
        }
        if (c == '/' && next == '*')
        {
            block = true;
            ++i;
            continue;
        }
        if (c == '/' && next == '/')
        {
            const auto end = body.find('\n', i);
            out.back() = body.substr(i + 2, end == body.npos ? body.size() - i - 2 : end - i - 2);
            i = end == body.npos ? body.size() - 1 : end - 1;
        }
    }
    return out;
}
/**
 * @brief 检查精确的局部抑制指令，不能借字符串内容抑制。
 * @param comment 行注释。
 * @param directive 指令名称。
 * @param id 稳定代码。
 * @return 指令明确匹配时为 true。
 */
inline bool suppressed(const Memory::String &comment, const char *directive, const Memory::String &id)
{
    std::istringstream input(std::string(comment.data(), comment.size()));
    std::string prefix, command, rule, extra;
    input >> prefix >> command >> rule;
    return prefix == "chali:" && command == directive && (rule == "all" || rule == std::string(id.data(), id.size())) && !(input >> extra);
}
/**
 * @brief 绑定原始源码、应用配置和局部抑制，并生成不删除代码的修复。
 * @param item 待输出警告。
 * @param options 当前配置。
 * @param body 对应原始源码。
 * @return 允许输出时为 true。
 */
inline bool prepare(chali_diagnostics::Diagnostic &item, const Options &options, const Memory::String &body)
{
    if (item.id.empty()) item.id = chali_diagnostics::code(item.message);
    const auto level = options.level(item.id);
    if (level == "allow" || !item.line) return false;
    const auto notes = comments(body);
    if (item.line > notes.size()) return false;
    if (suppressed(notes[item.line - 1], "allow", item.id) || (item.line > 1 && suppressed(notes[item.line - 2], "allow-next-line", item.id))) return false;
    item.warning = level != "error";
    std::istringstream lines(std::string(body.data(), body.size()));
    std::string line;
    for (std::size_t at = 0; at < item.line; ++at) std::getline(lines, line);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    item.hasSource = true;
    item.sourceText.assign(line.data(), line.size());
    if (item.id == "CHL2101" || item.id == "CHL2102")
    {
        const auto separator = item.message.rfind("：");
        const auto name = separator == item.message.npos ? Memory::String() : item.message.substr(separator + Memory::String("：").size());
        const auto at = name.empty() ? item.sourceText.npos : item.sourceText.find(name);
        if (at == item.sourceText.npos) return false;
        item.column = at + 1;
        item.endColumn = at + name.size() + 1;
    }
    const auto indent = line.find_first_not_of(" \t");
    const auto eol = body.find("\r\n") == body.npos ? "\n" : "\r\n";
    item.fixText = Memory::String(line.data(), indent == line.npos ? 0 : indent) + "// chali: allow-next-line " + item.id + eol;
    return true;
}
/**
 * @brief 收集保守的命名及未使用局部变量规则。
 * @param program 已检查 AST。
 * @return 原始位置上的候选警告，尚未应用配置。
 */
inline Memory::Vector<chali_diagnostics::Diagnostic> collect(const ast::Program *program)
{
    Memory::Vector<chali_diagnostics::Diagnostic> out;
    if (!program) return out;
    for (const auto &function : program->functions)
    {
        Memory::Map<Memory::String, const ast::Node *> declarations;
        Memory::Map<Memory::String, std::size_t> uses;
        std::function<void(const ast::NodePtr &, bool)> visit = [&](const ast::NodePtr &node, bool write) {
            if (!node) return;
            if (node->kind == ast::Node::Kind::Declare) declarations[node->text] = node.get();
            if (node->kind == ast::Node::Kind::Var && !write) ++uses[node->text];
            for (std::size_t i = 0; i < node->kids.size(); ++i)
                visit(node->kids[i], node->kind == ast::Node::Kind::Assign && node->text.empty() && i == 0);
        };
        visit(function.body, false);
        for (const auto &entry : declarations)
        {
            const auto &node = *entry.second;
            if (uses[entry.first] || entry.first.empty() || entry.first[0] == '_' || !node.sourceLine || node.sourceFile.empty() || node.sourceFile[0] == '<' || node.text.find("::") != node.text.npos) continue;
            chali_diagnostics::Diagnostic d;
            d.message = "局部变量未被读取；保留初始化和清理行为。";
            d.file = node.sourceFile;
            d.line = node.sourceLine;
            d.id = "CHL2201";
            out.push_back(std::move(d));
        }
    }
    return out;
}
}  // namespace chali_warnings
