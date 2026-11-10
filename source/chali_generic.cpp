#include <algorithm>
#include <cctype>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "chali_generic.hpp"
#include "syntax/source_text.hpp"
#include "syntax/constant_expression.hpp"
#include "chali_lambda.hpp"
#include "chali_incremental.hpp"

namespace chali_generic
{

Session session;
namespace { TemplateLoader standardTemplateLoader = nullptr; }

/**
 * @brief 为本次编译设置标准模板恢复入口。
 * @param loader 只读资产回调，空指针禁用。
 */
void setTemplateLoader(TemplateLoader loader) { standardTemplateLoader = loader; }

Memory::Map<Memory::String, Memory::String> genericTypeModules;

Memory::Map<Memory::String, Memory::Set<Memory::String>> nominalTypeNames;

Memory::Set<Memory::String> coreAliasShadows;

/** @brief 用于检测别名与同模块类型声明重名。 */
Memory::Map<Memory::String, std::size_t> nominalTypeDeclarations;

/** @brief 类型实参来源模块的直接导入，仅在当前编译会话中使用。 */
Memory::Map<Memory::String, Memory::Set<Memory::String>> nominalTypeImports;

Memory::Map<Memory::String, Memory::String> constantArguments;

/**
 * @brief 判断字符能否作为类型或模板标识符的首字符。
 * @param value 待检查的字符。
 * @return 字母或下划线返回 true。
 */
bool identStart(char value)
{
    return std::isalpha(static_cast<unsigned char>(value)) || value == '_';
}

/**
 * @brief 判断字符能否继续组成类型或模板标识符。
 * @param value 待检查的字符。
 * @return 字母、数字或下划线返回 true。
 */
bool identPart(char value)
{
    return std::isalnum(static_cast<unsigned char>(value)) || value == '_';
}

/**
 * @brief 移除文本两端的空白并保留内部文本。
 * @param value 原始文本。
 * @return 去除首尾空白的文本；全为空白时返回空串。
 */
Memory::String trim(const Memory::String &value)
{
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == Memory::String::npos) return {};
    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

/**
 * @brief 推进扫描位置至下一个非空白字符。
 * @param text 待扫描的源码。
 * @param at 输入起点并接收跳过空白后的位置。
 */
void skipSpace(const Memory::String &text, std::size_t &at)
{
    while (at < text.size() && std::isspace(static_cast<unsigned char>(text[at]))) ++at;
}

/**
 * @brief 收集模块层整数常量，保留限定名与无歧义的裸名。
 * @param source 预处理后的完整源码。
 */
void collectConstantArguments(const Memory::String &source)
{
    constantArguments.clear();
    const auto mask = chali_lambda::codeMask(source);
    Memory::Map<Memory::String, Memory::Map<Memory::String, Memory::String>> declarations;
    Memory::String module;
    int braces = 0;
    for (std::size_t at = 0; at < mask.size();)
    {
        if (mask[at] == '{')
        {
            ++braces;
            ++at;
            continue;
        }
        if (mask[at] == '}')
        {
            --braces;
            ++at;
            continue;
        }
        if (!identStart(mask[at]))
        {
            ++at;
            continue;
        }
        const auto begin = at;
        while (at < mask.size() && identPart(mask[at])) ++at;
        const auto word = mask.substr(begin, at - begin);
        if (braces) continue;
        if (word == "module")
        {
            skipSpace(mask, at);
            const auto start = at;
            while (at < mask.size() && (identPart(mask[at]) || mask[at] == ':')) ++at;
            module = mask.substr(start, at - start);
            if (module == "__root") module.clear();
            continue;
        }
        if (word != "const") continue;
        skipSpace(mask, at);
        const auto typeStart = at;
        while (at < mask.size() && identPart(mask[at])) ++at;
        const auto type = mask.substr(typeStart, at - typeStart);
        if (type != "int8" && type != "int16" && type != "int32" && type != "int64" && type != "uint8" && type != "uint16" && type != "uint32" && type != "uint64") continue;
        skipSpace(mask, at);
        const auto nameStart = at;
        while (at < mask.size() && identPart(mask[at])) ++at;
        const auto name = mask.substr(nameStart, at - nameStart);
        skipSpace(mask, at);
        if (at >= mask.size() || mask[at] != '=') continue;
        const auto end = mask.find(';', ++at);
        if (end == Memory::String::npos) break;
        declarations[name][module] = trim(source.substr(at, end - at));
        at = end + 1;
    }
    for (const auto &entry : declarations)
    {
        for (const auto &definition : entry.second)
            constantArguments[definition.first.empty() ? entry.first : definition.first + "::" + entry.first] = definition.second;
        if (entry.second.size() == 1) constantArguments[entry.first] = entry.second.begin()->second;
        const auto root = entry.second.find("");
        if (root != entry.second.end()) constantArguments[entry.first] = root->second;
    }
}

/**
 * @brief 检查当前位置是否为一个完整的源码单词。
 * @param text 待扫描的源码。
 * @param at 候选单词起点。
 * @param word 需要匹配的完整单词。
 * @return 拼写匹配且两端不是标识符字符时返回 true。
 */
bool wordAt(const Memory::String &text, std::size_t at, const Memory::String &word)
{
    return text.compare(at, word.size(), word) == 0 && (at == 0 || !identPart(text[at - 1])) && (at + word.size() == text.size() || !identPart(text[at + word.size()]));
}

/**
 * @brief 跳过注释和字面量，查找成对分隔符的结束位置。
 * @param text 完整源码。
 * @param open 起始分隔符的位置。
 * @param left 左分隔符。
 * @param right 右分隔符。
 * @param close 成功时接收匹配的右分隔符位置。
 * @return 嵌套分隔符完整闭合时返回 true。
 */
bool balanced(const Memory::String &text, std::size_t open, char left, char right,
              std::size_t &close)
{
    int depth = 0;
    char quote = 0;
    bool lineComment = false, blockComment = false;
    for (std::size_t at = open; at < text.size(); ++at)
    {
        const char value = text[at];
        if (lineComment)
        {
            if (value == '\n') lineComment = false;
            continue;
        }
        if (blockComment)
        {
            if (value == '*' && at + 1 < text.size() && text[at + 1] == '/')
            {
                blockComment = false;
                ++at;
            }
            continue;
        }
        if (quote != 0)
        {
            if (value == '\\' && at + 1 < text.size())
            {
                ++at;
                continue;
            }
            if (value == quote) quote = 0;
            continue;
        }
        if (value == '/' && at + 1 < text.size() && text[at + 1] == '/')
        {
            lineComment = true;
            ++at;
            continue;
        }
        if (value == '/' && at + 1 < text.size() && text[at + 1] == '*')
        {
            blockComment = true;
            ++at;
            continue;
        }
        if (value == '"' || value == '\'')
        {
            quote = value;
            continue;
        }
        if (value == left)
            ++depth;
        else if (value == right && !(right == '>' && at > 0 && text[at - 1] == '-') && --depth == 0)
        {
            close = at;
            return true;
        }
    }
    return false;
}

/**
 * @brief 仅在最外层逗号处分割泛型形参或实参。
 * @param text 可能包含嵌套类型和括号的参数文本。
 * @return 保留嵌套结构的参数列表；空输入返回空列表。
 */
Memory::Vector<Memory::String> splitTop(const Memory::String &text)
{
    Memory::Vector<Memory::String> result;
    std::size_t begin = 0;
    int angle = 0, round = 0, square = 0;
    for (std::size_t at = 0; at <= text.size(); ++at)
    {
        const char value = at < text.size() ? text[at] : ',';
        if (value == '<')
            ++angle;
        else if (value == '>' && (at == 0 || text[at - 1] != '-'))
            --angle;
        else if (value == '(')
            ++round;
        else if (value == ')')
            --round;
        else if (value == '[')
            ++square;
        else if (value == ']')
            --square;
        else if (value == ',' && angle == 0 && round == 0 && square == 0)
        {
            result.push_back(trim(text.substr(begin, at - begin)));
            begin = at + 1;
        }
    }
    if (result.size() == 1u && result[0].empty()) result.clear();
    return result;
}

/**
 * @brief 解析泛型形参的约束、常量类型和默认实参。
 * @param text 尖括号内部的形参文本。
 * @param result 追加解析成功的形参。
 * @param error 失败时接收重复、格式或默认值顺序诊断。
 * @return 所有形参符合泛型声明规则时返回 true。
 */
bool parseParameters(const Memory::String &text, Memory::Vector<Parameter> &result,
                     Memory::String &error)
{
    for (const Memory::String &item : splitTop(text))
    {
        Parameter parameter;
        Memory::String head = item;
        const std::size_t equal = head.find('=');
        if (equal != Memory::String::npos)
        {
            parameter.hasDefault = true;
            parameter.fallback = trim(head.substr(equal + 1));
            head = trim(head.substr(0, equal));
            if (parameter.fallback.empty())
            {
                error = "泛型参数的默认类型为空";
                return false;
            }
        }
        const std::size_t colon = head.find(':');
        if (colon != Memory::String::npos)
        {
            parameter.constraint = trim(head.substr(colon + 1));
            if (parameter.constraint == "int64")
            {
                parameter.constant = true;
                parameter.constantType = parameter.constraint;
                parameter.constraint.clear();
            }
            head = trim(head.substr(0, colon));
        }
        parameter.name = trim(head);
        if (parameter.name.empty() || !identStart(parameter.name[0]))
        {
            error = "泛型参数名不合法：" + item;
            return false;
        }
        for (char value : parameter.name)
            if (!identPart(value))
            {
                error = "泛型参数名不合法：" + parameter.name;
                return false;
            }
        for (const Parameter &old : result)
            if (old.name == parameter.name)
            {
                error = "重复的泛型参数 " + parameter.name;
                return false;
            }
        result.push_back(parameter);
    }
    if (result.empty())
    {
        error = "泛型声明的 <> 至少需要一个类型参数";
        return false;
    }
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        if (!result[i].hasDefault) continue;
        for (std::size_t at = 0; at < result[i].fallback.size(); ++at)
        {
            for (std::size_t j = i; j < result.size(); ++j)
            {
                if (wordAt(result[i].fallback, at, result[j].name))
                {
                    error = "泛型默认类型只能引用前面的类型参数：" + result[i].name + " 引用了 " + result[j].name;
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * @brief 将源码偏移换算为从零开始的行下标。
 * @param text 完整源码。
 * @param offset 需要定位的字节偏移。
 * @return 该偏移之前的换行数量。
 */
std::size_t lineOf(const Memory::String &text, std::size_t offset)
{
    std::size_t line = 0;
    for (std::size_t at = 0; at < offset; ++at)
        if (text[at] == '\n') ++line;
    return line;
}

/**
 * @brief 识别编译器指令 `#No<接口>Replace` / `#NoRight<接口>` 的范围。
 *
 * 指令本身不带括号，这里按「`#No` + 标识符 +
 * `Replace`」或「`#NoRight` + 标识符」逐段比对，尾后必须是词边界，避免把
 * `#NoAddReplaceX` 当成指令。
 *
 * @param text 代码掩码（注释与字符串已抹白）。
 * @param at 指令起始偏移。
 * @param end 输出：指令结束后的偏移。
 * @return 起点确实是一条运算符开关指令时返回 true。
 */
bool operatorSwitchRange(const Memory::String &text, std::size_t at, std::size_t &end)
{
    if (text.compare(at, 3, "#No") != 0) return false;
    if (at > 0 && identPart(text[at - 1])) return false;
    std::size_t cursor = at + 3u;
    const std::size_t begin = cursor;
    while (cursor < text.size() && identPart(text[cursor])) ++cursor;
    if (cursor == begin) return false;
    if (cursor > begin + 7u && text.compare(cursor - 7u, 7, "Replace") == 0)
    {
        end = cursor;
        return true;
    }
    /* `#NoRightXxx`：以 `#NoRight` 开头且后面还有接口名才算（`#NonCopyable` 不匹配）。 */
    if (text.compare(at, 8, "#NoRight") != 0 || cursor == at + 8u) return false;
    end = cursor;
    return true;
}

/**
 * @brief 保存泛型结构体的运算符替代开关并在原位置保留行号。
 * @param source 待单态化源码。
 * @param switches 接收每个模板的运算符开关文本。
 */
void collectGenericOperatorSwitches(Memory::String &source, Memory::Map<Memory::String, Memory::String> &switches)
{
    auto mask = chali_lambda::codeMask(source);
    Memory::String currentModule;
    const auto updateModule = [&](std::size_t at) {
        if (!wordAt(mask, at, "module")) return;
        auto nameAt = at + 6;
        skipSpace(mask, nameAt);
        const auto end = mask.find(';', nameAt);
        if (end != Memory::String::npos) currentModule = trim(mask.substr(nameAt, end - nameAt));
        if (currentModule == "__root") currentModule.clear();
    };
    int depth = 0;
    for (std::size_t at = 0; at < mask.size(); ++at)
    {
        if (mask[at] == '{') ++depth;
        if (mask[at] == '}') --depth;
        if (!depth) updateModule(at);
        std::size_t switchEnd = 0;
        if (depth != 0 || !operatorSwitchRange(mask, at, switchEnd)) continue;
        std::size_t type = switchEnd;
        skipSpace(mask, type);
        if (!wordAt(mask, type, "struct")) continue;
        type += 6u;
        skipSpace(mask, type);
        const std::size_t begin = type;
        while (type < mask.size() && identPart(mask[type])) ++type;
        const auto name = mask.substr(begin, type - begin);
        skipSpace(mask, type);
        if (name.empty() || type >= mask.size() || mask[type] != '<') continue;
        const auto key = currentModule.empty() ? name : currentModule + "::" + name;
        const auto found = switches.find(key);
        if (found == switches.end())
            switches[key] = source.substr(at, switchEnd - at) + "\n";
        else
            found->second = source.substr(at, switchEnd - at) + "\n" + found->second;
        for (auto i = at; i < switchEnd; ++i)
            if (source[i] != '\n') source[i] = mask[i] = ' ';
        at = switchEnd - 1u;
    }
}

/**
 * @brief 归一化顶层声明中名称后置的泛型参数，正文仍由生成解析器处理。
 * @param source 原地规范化的源码，保留行数以映射诊断。
 */
void normalizeGenericSyntax(Memory::String &source)
{
    struct Edit
    {
        std::size_t begin, end;
        Memory::String text;
    };
    Memory::Vector<Edit> edits;
    std::size_t declaration = 0;
    int braces = 0, rounds = 0;
    bool annotated = false;
    for (std::size_t at = 0; at < source.size(); ++at)
    {
        if (source.compare(at, 2, "//") == 0)
        {
            auto end = source.find('\n', at);
            if (end == Memory::String::npos) break;
            if (trim(source.substr(declaration, at - declaration)).empty()) declaration = end + 1;
            at = end;
            continue;
        }
        if (source.compare(at, 2, "/*") == 0)
        {
            auto end = source.find("*/", at + 2);
            if (end == Memory::String::npos) break;
            if (trim(source.substr(declaration, at - declaration)).empty()) declaration = end + 2;
            at = end + 1;
            continue;
        }
        if (source[at] == '\"' || source[at] == '\'')
        {
            const char quote = source[at++];
            while (at < source.size())
            {
                if (source[at] == '\\')
                {
                    at += 2;
                    continue;
                }
                if (source[at] == quote) break;
                ++at;
            }
            continue;
        }
        // 此处只跳过注解前缀以找泛型声明边界；语法与参数检查仍由 .gram 完成。
        if (braces == 0 && rounds == 0 && source[at] == '@')
        {
            std::size_t end = at + 1;
            const bool definition = wordAt(source, end, "annotation");
            if (definition)
            {
                end += 10;
                skipSpace(source, end);
            }
            while (end < source.size() && (identPart(source[end]) || source[end] == ':')) ++end;
            std::size_t args = end;
            skipSpace(source, args);
            if (args < source.size() && source[args] == '(')
            {
                std::size_t close = 0;
                if (balanced(source, args, '(', ')', close)) end = close + 1;
            }
            declaration = end;
            annotated = !definition;
            at = end - 1;
            continue;
        }
        if (source[at] == '{') ++braces;
        if (source[at] == '}' && --braces == 0)
        {
            declaration = at + 1;
            annotated = false;
        }
        if (braces != 0) continue;
        if (source[at] == '(') ++rounds;
        if (source[at] == ')') --rounds;
        if (source[at] == ';')
        {
            declaration = at + 1;
            rounds = 0;
            annotated = false;
        }
        if (rounds == 0 && !annotated && wordAt(source, at, "type")
            && trim(source.substr(declaration, at - declaration)).empty())
        {
            std::size_t keyword = at + 4u;
            skipSpace(source, keyword);
            if (wordAt(source, keyword, "alias"))
            {
                auto name = keyword + 5u;
                skipSpace(source, name);
                if (name < source.size() && identStart(source[name]))
                {
                    auto end = name + 1u;
                    while (end < source.size() && identPart(source[end])) ++end;
                    auto open = end;
                    skipSpace(source, open);
                    std::size_t close = 0;
                    if (open < source.size() && source[open] == '<' && balanced(source, open, '<', '>', close))
                    {
                        auto parameters = source.substr(open, close - open + 1u);
                        auto head = source.substr(at, end - at);
                        std::replace(parameters.begin(), parameters.end(), '\n', ' ');
                        const auto newlines = std::count(source.begin() + end, source.begin() + close + 1u, '\n');
                        edits.push_back({ at, close + 1u, "__chali_template" + parameters + " " + head });
                        edits.back().text.append(static_cast<std::size_t>(newlines), '\n');
                        at = close;
                    }
                    else if (open < source.size() && source[open] == '=')
                    {
                        edits.push_back({ at, at, "__chali_template<> " });
                        at = end - 1u;
                    }
                    continue;
                }
            }
        }
        // 首版不收集带前置注解的泛型模板，保留原文交给文法拒绝，避免丢失或错绑注解。
        if (source[at] != '<' || rounds != 0 || annotated) continue;
        const std::size_t open = at;
        std::size_t close = 0;
        if (!balanced(source, open, '<', '>', close)) continue;
        std::size_t nameEnd = open;
        while (nameEnd > declaration && std::isspace(static_cast<unsigned char>(source[nameEnd - 1]))) --nameEnd;
        std::size_t name = nameEnd;
        while (name > declaration && identPart(source[name - 1])) --name;
        const auto head = trim(source.substr(declaration, name - declaration));
        std::size_t after = close + 1u;
        skipSpace(source, after);
        const bool nominal = head == "struct" || head == "enum" || head == "interface" || head == "type";
        const bool function = !head.empty() && head.find('=') == Memory::String::npos && after < source.size() && source[after] == '(';
        if (!nominal && !function)
        {
            at = close;
            continue;
        }
        // 内部标记不是语言关键字；用户的 generic 标识符仍是普通名字。
        std::size_t start = declaration;
        skipSpace(source, start);
        edits.push_back(Edit{ start, close + 1u,
                              "__chali_template" + source.substr(open, close - open + 1u) + " " + source.substr(start, nameEnd - start) });
        at = close;
    }
    for (auto edit = edits.rbegin(); edit != edits.rend(); ++edit)
        source.replace(edit->begin, edit->end - edit->begin, edit->text);
}

/**
 * @brief 标记标准容器类型实参的标点，保留普通标识符与比较运算符的词法角色。
 * @param source 已完成单态化的源码；仅插入内部标点，不改变换行。
 * @return 无。
 */
void markContainerTypeArguments(Memory::String &source, bool typeFragment)
{
    Memory::Set<std::size_t> angles;
    for (std::size_t at = 0; at < source.size();)
    {
        if (source.compare(at, 2, "//") == 0)
        {
            auto end = source.find('\n', at);
            if (end == Memory::String::npos) break;
            at = end;
            continue;
        }
        if (source.compare(at, 2, "/*") == 0)
        {
            auto end = source.find("*/", at + 2);
            if (end == Memory::String::npos) break;
            at = end + 2;
            continue;
        }
        if (source[at] == '"' || source[at] == '\'')
        {
            const char quote = source[at++];
            while (at < source.size())
            {
                if (source[at] == '\\')
                {
                    at += 2;
                    continue;
                }
                if (source[at++] == quote) break;
            }
            continue;
        }
        if (!identStart(source[at]))
        {
            ++at;
            continue;
        }
        auto start = at++;
        while (at < source.size() && identPart(source[at])) ++at;
        const auto name = source.substr(start, at - start);
        if (name.empty() || (!typeFragment && !std::isupper(static_cast<unsigned char>(name[0])) && !session.generics.count(name))) continue;
        auto previous = start;
        while (previous && std::isspace(static_cast<unsigned char>(source[previous - 1]))) --previous;
        if (previous && source[previous - 1] == '$') continue;
        auto keyword = previous;
        while (keyword && identPart(source[keyword - 1])) --keyword;
        if (keyword && source[keyword - 1] == '$' && (source.substr(keyword, previous - keyword) == "macro" || source.substr(keyword, previous - keyword) == "replace")) continue;
        auto open = at;
        skipSpace(source, open);
        if (open >= source.size() || source[open] != '<') continue;
        std::size_t close = 0;
        if (!balanced(source, open, '<', '>', close)) continue;
        int rounds = 0, squares = 0;
        bool type = true;
        Memory::Vector<std::size_t> candidate{ open };
        for (auto i = open + 1; i < close; ++i)
        {
            if (source.compare(i, 2, "/*") == 0)
            {
                auto end = source.find("*/", i + 2);
                if (end == Memory::String::npos || end > close)
                {
                    type = false;
                    break;
                }
                i = end + 1;
                continue;
            }
            if (source.compare(i, 2, "//") == 0)
            {
                auto end = source.find('\n', i + 2);
                if (end == Memory::String::npos || end > close)
                {
                    type = false;
                    break;
                }
                i = end;
                continue;
            }
            const char ch = source[i];
            if (ch == '<' || (ch == '>' && source[i - 1] != '-')) candidate.push_back(i);
            if (ch == '(')
                ++rounds;
            else if (ch == ')' && --rounds < 0)
            {
                type = false;
                break;
            }
            if (ch == '[')
                ++squares;
            else if (ch == ']' && --squares < 0)
            {
                type = false;
                break;
            }
            if (ch == ';' || ch == '{' || ch == '}' || ch == '=' || ch == '+' || ch == '/' || ch == '"' || ch == '\''
                || (ch == '&' && i + 1 < close && source[i + 1] == '&')
                || (ch == '|' && i + 1 < close && source[i + 1] == '|'))
            {
                type = false;
                break;
            }
        }
        if (!type || rounds || squares) continue;
        candidate.push_back(close);
        for (auto i : candidate) angles.insert(i);
    }
    for (auto at = angles.rbegin(); at != angles.rend(); ++at) source.insert(*at, 1u, '@');
}

/**
 * @brief 提取泛型声明并保留原始行映射。
 * @param source 读取并移除模板声明的完整源码。
 * @param lines 原始源码的逐行来源。
 * @param generics 接收按限定名索引的模板。
 * @param error 失败时接收模板声明诊断。
 * @return 所有模板成功提取时返回 true。
 */
bool collect(Memory::String &source, const Memory::Vector<preprocess::LineOrigin> &lines,
             Memory::Map<Memory::String, Generic> &generics, Memory::String &error)
{
    Memory::String currentModule;
    for (std::size_t at = 0; at < source.size();)
    {
        if (source.compare(at, 2, "//") == 0)
        {
            const std::size_t end = source.find('\n', at);
            at = end == Memory::String::npos ? source.size() : end + 1;
            continue;
        }
        if (source.compare(at, 2, "/*") == 0)
        {
            const std::size_t end = source.find("*/", at + 2);
            at = end == Memory::String::npos ? source.size() : end + 2;
            continue;
        }
        if (source[at] == '"' || source[at] == '\'')
        {
            const char quote = source[at++];
            while (at < source.size())
            {
                if (source[at] == '\\' && at + 1 < source.size())
                {
                    at += 2;
                    continue;
                }
                if (source[at++] == quote) break;
            }
            continue;
        }
        if (wordAt(source, at, "module"))
        {
            std::size_t nameAt = at + 6u;
            skipSpace(source, nameAt);
            const std::size_t end = source.find(';', nameAt);
            if (end == Memory::String::npos)
            {
                error = "module 声明缺少 ;";
                return false;
            }
            currentModule = trim(source.substr(nameAt, end - nameAt));
            if (currentModule == "__root") currentModule.clear();
            at = end + 1u;
            continue;
        }
        if (!wordAt(source, at, "__chali_template"))
        {
            ++at;
            continue;
        }
        const std::size_t start = at;
        Generic item;
        std::size_t end = 0;
        int cached = 0;
        const auto sourceLine = lineOf(source, start);
        if (standardTemplateLoader && sourceLine < lines.size())
            cached = standardTemplateLoader(currentModule, lines[sourceLine], source, start, end, item, error);
        if (cached < 0) return false;
        if (cached && (end <= start || end > source.size() || item.module != currentModule))
        {
            error = "标准模板资产返回了无效的声明范围或模块";
            return false;
        }
        if (standardTemplateLoader && sourceLine < lines.size()
            && (lines[sourceLine].file == "<内建 range>" || lines[sourceLine].file.compare(0, 13, "<内建 std::") == 0))
        {
            if (cached) ++session.templateHits;
            else ++session.templateMisses;
        }
        if (!cached)
        {
            at += sizeof("__chali_template") - 1u;
            skipSpace(source, at);
            if (at >= source.size() || source[at] != '<')
            {
                error = "泛型声明名称后需要 <...>";
                return false;
            }
            std::size_t paramsEnd = 0;
            if (!balanced(source, at, '<', '>', paramsEnd))
            {
                error = "泛型参数缺少 >";
                return false;
            }
            item.module = currentModule;
            const auto parameters = source.substr(at + 1u, paramsEnd - at - 1u);
            std::size_t decl = paramsEnd + 1;
            skipSpace(source, decl);
            std::size_t nameAt = Memory::String::npos;
            const bool structDecl = wordAt(source, decl, "struct");
            const bool enumDecl = wordAt(source, decl, "enum");
            const bool interfaceDecl = wordAt(source, decl, "interface");
            const bool hostTypeDecl = wordAt(source, decl, "type");
            if (structDecl || enumDecl || interfaceDecl)
            {
                nameAt = decl + (structDecl ? 6u : interfaceDecl ? 9u :
                                                                   4u);
                skipSpace(source, nameAt);
            }
            else if (hostTypeDecl)
            {
                nameAt = decl + 4u;
                skipSpace(source, nameAt);
                if (wordAt(source, nameAt, "alias"))
                {
                    auto candidate = nameAt + 5u;
                    skipSpace(source, candidate);
                    if (candidate < source.size() && identStart(source[candidate])) nameAt = candidate;
                }
            }
            else
            {
                std::size_t open = Memory::String::npos;
                int angleDepth = 0;
                std::size_t signatureBegin = decl;
                // 生命周期契约保留在声明中，但其括号不是泛型函数的参数表。
                while (source.compare(signatureBegin, 7, "#borrow") == 0 || source.compare(signatureBegin, 7, "#static") == 0)
                {
                    std::size_t openDirective = signatureBegin + 7u;
                    skipSpace(source, openDirective);
                    std::size_t closeDirective = 0;
                    if (openDirective >= source.size() || source[openDirective] != '('
                        || !balanced(source, openDirective, '(', ')', closeDirective))
                    {
                        error = "泛型生命周期契约缺少完整参数表";
                        return false;
                    }
                    signatureBegin = closeDirective + 1u;
                    skipSpace(source, signatureBegin);
                }
                // 元组返回类型的括号不是函数参数表。
                if (source[signatureBegin] == '(')
                {
                    std::size_t tupleEnd = 0;
                    if (balanced(source, signatureBegin, '(', ')', tupleEnd)) signatureBegin = tupleEnd + 1u;
                }
                for (std::size_t cursor = signatureBegin; cursor < source.size(); ++cursor)
                {
                    if (source[cursor] == '<')
                        ++angleDepth;
                    else if (source[cursor] == '>' && (cursor == 0 || source[cursor - 1] != '-'))
                        --angleDepth;
                    else if (source[cursor] == '(' && angleDepth == 0)
                    {
                        std::size_t word = cursor;
                        while (word > signatureBegin && std::isspace(static_cast<unsigned char>(source[word - 1]))) --word;
                        const auto end = word;
                        while (word > signatureBegin && identPart(source[word - 1])) --word;
                        if (source.substr(word, end - word) == "fn" || word == end)
                        {
                            std::size_t close = 0;
                            if (balanced(source, cursor, '(', ')', close))
                            {
                                cursor = close;
                                continue;
                            }
                        }
                        open = cursor;
                        break;
                    }
                    else if (source[cursor] == ';' || source[cursor] == '{')
                        break;
                }
                if (open == Memory::String::npos)
                {
                    error = "泛型函数缺少参数表";
                    return false;
                }
                nameAt = open;
                while (nameAt > decl && std::isspace(static_cast<unsigned char>(source[nameAt - 1]))) --nameAt;
                while (nameAt > decl && identPart(source[nameAt - 1])) --nameAt;
            }
            if (nameAt >= source.size() || !identStart(source[nameAt]))
            {
                error = "找不到泛型声明名称";
                return false;
            }
            std::size_t nameEnd = nameAt + 1;
            while (nameEnd < source.size() && identPart(source[nameEnd])) ++nameEnd;
            item.name = source.substr(nameAt, nameEnd - nameAt);
            item.sourceName = item.name;
            auto keyword = decl + 4u;
            skipSpace(source, keyword);
            const bool aliasDecl = hostTypeDecl && keyword != nameAt;
            if ((!aliasDecl || !trim(parameters).empty()) && !parseParameters(parameters, item.params, error)) return false;
            const std::size_t semicolon = source.find(';', nameEnd);
            const std::size_t body = source.find('{', nameEnd);
            const bool hostFunctionDecl = !structDecl && !enumDecl && !interfaceDecl && !hostTypeDecl && semicolon != Memory::String::npos && (body == Memory::String::npos || semicolon < body);
            if (hostTypeDecl || hostFunctionDecl)
            {
                end = semicolon;
                if (end == Memory::String::npos)
                {
                    error = "泛型宿主声明 " + item.name + " 缺少 ;";
                    return false;
                }
                ++end;
            }
            else
            {
                if (body == Memory::String::npos)
                {
                    error = "泛型声明 " + item.name + " 缺少函数体";
                    return false;
                }
                std::size_t bodyEnd = 0;
                if (!balanced(source, body, '{', '}', bodyEnd))
                {
                    error = "泛型声明 " + item.name + " 缺少 }";
                    return false;
                }
                end = bodyEnd + 1;
                skipSpace(source, end);
                if ((structDecl || enumDecl || interfaceDecl) && end < source.size() && source[end] == ';') ++end;
            }
            if (aliasDecl)
            {
                auto equal = nameEnd;
                skipSpace(source, equal);
                if (equal >= semicolon || source[equal] != '='
                    || (item.aliasTarget = trim(source.substr(equal + 1u, semicolon - equal - 1u))).empty())
                {
                    error = "类型别名 " + item.name + " 需要 = 类型;";
                    return false;
                }
            }
            item.declaration = source.substr(decl, end - decl);
            item.nameOffset = nameAt - decl;
            const std::size_t line = lineOf(source, start);
            if (line < lines.size()) item.origin = lines[line];
            const auto declarationLine = lineOf(source, decl);
            const auto declarationLines = static_cast<std::size_t>(std::count(item.declaration.begin(), item.declaration.end(), '\n')) + 1u;
            for (std::size_t index = 0; index < declarationLines; ++index)
                item.declarationOrigins.push_back(declarationLine + index < lines.size() ? lines[declarationLine + index] : item.origin);
            if (!item.declarationOrigins.empty()) item.declarationOrigins[0] = item.origin;
            if (item.origin.file == "<内建 range>" || item.origin.file.compare(0, 13, "<内建 std::") == 0)
                item.sourceTemplate = source.substr(start, end - start);
        }
        else
        {
            item.name = item.sourceName;
            /* 匹配原文保留在只读资产表，当前实例会话不再复制一份模板正文。 */
            Memory::String{}.swap(item.sourceTemplate);
        }
        if (generics.count(item.name))
        {
            if (generics.at(item.name).module == item.module)
            {
                error = "重复的泛型声明 " + item.name;
                return false;
            }
            item.name = "__module_" + item.module + "_" + item.sourceName;
            for (auto &character : item.name)
                if (character == ':') character = '_';
            if (generics.count(item.name))
            {
                error = "重复的泛型声明 " + item.module + "::" + item.sourceName;
                return false;
            }
        }
        generics.emplace(item.name, item);
        for (std::size_t erase = start; erase < end; ++erase)
            if (source[erase] != '\n' && source[erase] != '\r') source[erase] = ' ';
        at = end;
    }
    return true;
}

/**
 * @brief 收集显式及请求派生的接口身份，排除字符串和注释中的伪声明。
 * @param source 已规范化的声明与普通源码。
 * @param implemented 输出具体类型到接口身份集合的映射。
 */
void collectInterfaces(const Memory::String &source,
                       Memory::Map<Memory::String, Memory::Set<Memory::String>> &implemented)
{
    Memory::String text = source;
    for (std::size_t i = 0; i < text.size(); ++i)
    {
        if (text[i] == '"' || text[i] == '\'')
        {
            char q = text[i];
            text[i] = ' ';
            while (++i < text.size())
            {
                char c = text[i];
                text[i] = ' ';
                if (c == '\\' && i + 1 < text.size())
                    text[++i] = ' ';
                else if (c == q)
                    break;
            }
        }
        else if (i + 1 < text.size() && text[i] == '/' && (text[i + 1] == '/' || text[i + 1] == '*'))
        {
            bool line = text[i + 1] == '/';
            text[i++] = ' ';
            text[i] = ' ';
            while (++i < text.size())
            {
                char c = text[i];
                if (line && c == '\n') break;
                if (!line && c == '*' && i + 1 < text.size() && text[i + 1] == '/')
                {
                    text[i++] = ' ';
                    text[i] = ' ';
                    break;
                }
                if (c != '\n') text[i] = ' ';
            }
        }
    }
    Memory::String module;
    for (std::size_t at = 0; at < text.size(); ++at)
    {
        if (wordAt(text, at, "module"))
        {
            at += 6u;
            skipSpace(text, at);
            const auto begin = at;
            while (at < text.size() && (identPart(text[at]) || text[at] == ':')) ++at;
            module = text.substr(begin, at - begin);
            if (module == "__root") module.clear();
        }
        bool structure = wordAt(text, at, "struct"), isInterface = wordAt(text, at, "interface"), enumeration = wordAt(text, at, "enum");
        if (!structure && !isInterface && !enumeration) continue;
        at += structure ? 6 : isInterface ? 9 :
                                            4;
        skipSpace(text, at);
        std::size_t begin = at;
        while (at < text.size() && identPart(text[at])) ++at;
        auto name = text.substr(begin, at - begin);
        if (isInterface)
        {
            const auto qualified = module.empty() ? name : module + "::" + name;
            implemented[name].insert(name);
            implemented[name].insert(qualified);
            implemented[qualified].insert(name);
            implemented[qualified].insert(qualified);
        }
        if (isInterface && name == "ToString")
            implemented["#string-protocol-declarations"].insert(module.empty() ? name : module + "::" + name);
        skipSpace(text, at);
        if (at >= text.size() || text[at] != ':') continue;
        ++at;
        while (at < text.size() && text[at] != '{')
        {
            skipSpace(text, at);
            begin = at;
            while (at < text.size() && (identPart(text[at]) || text[at] == ':')) ++at;
            if (at > begin)
            {
                const auto base = text.substr(begin, at - begin);
                implemented[name].insert(base);
                if (!module.empty())
                {
                    implemented[module + "::" + name].insert(base);
                    if (base.find("::") == Memory::String::npos)
                    {
                        implemented[name].insert(module + "::" + base);
                        implemented[module + "::" + name].insert(module + "::" + base);
                    }
                }
            }
            int nesting = 0;
            while (at < text.size() && text[at] != '{' && !(text[at] == ',' && nesting == 0))
            {
                if (text[at] == '<')
                    ++nesting;
                else if (text[at] == '>')
                    --nesting;
                ++at;
            }
            if (at < text.size() && text[at] == ',') ++at;
        }
    }
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto &entry : implemented)
        {
            // 此项记录协议的声明身份，不是可沿继承边扩展的类型实现集合。
            if (entry.first == "#string-protocol-declarations") continue;
            auto bases = entry.second;
            for (const auto &base : bases)
            {
                auto inherited = implemented.find(base);
                if (inherited != implemented.end())
                    for (const auto &ancestor : inherited->second) changed |= entry.second.insert(ancestor).second;
            }
        }
    }
}

/**
 * @brief 格式化完整的实例标识符，并在泛型实参中缩短无歧义的常用限定名。
 * @param text 待显示的文本。
 * @param typeArgument 文本是泛型实参时为 true。
 * @return 展开的文本。
 */
static Memory::String formatInstances(const Memory::String &text, bool typeArgument)
{
    Memory::String result;
    result.reserve(text.size());
    for (std::size_t at = 0; at < text.size();)
    {
        if (!identPart(text[at]))
        {
            const char character = text[at++];
            result += character;
            if (typeArgument && character == ',' && at < text.size() && text[at] != ' ') result += ' ';
            continue;
        }
        const auto begin = at;
        auto leaf = at;
        do
        {
            while (at < text.size() && identPart(text[at])) ++at;
            if (!typeArgument || text.compare(at, 2, "::") != 0 ||
                at + 2 >= text.size() || !identStart(text[at + 2])) break;
            at += 2;
            leaf = at;
        } while (true);

        const auto name = text.substr(leaf, at - leaf);
        if (leaf != begin)
        {
            const auto prefix = text.substr(begin, leaf - begin);
            bool keepQualifier = true;
            if (prefix == "program::" || prefix == "std::core::" || prefix == "__root::")
            {
                const auto nominal = nominalTypeNames.find(name);
                keepQualifier = nominal != nominalTypeNames.end() &&
                    (nominal->second.size() != 1 || !nominal->second.count(prefix == "__root::" ? name : prefix + name));
            }
            if (keepQualifier) result += prefix;
        }
        const auto instance = session.instanceDisplayNames.find(name);
        if (instance == session.instanceDisplayNames.end())
        {
            result += name;
            continue;
        }
        const auto &display = instance->second;
        const auto scopeEnd = display.rfind("::", display.find('<'));
        const auto scopeSize = scopeEnd == Memory::String::npos ? 0 : scopeEnd + 2;
        if (scopeSize && result.size() >= scopeSize &&
            result.compare(result.size() - scopeSize, scopeSize, display, 0, scopeSize) == 0)
            result.append(display, scopeSize, Memory::String::npos);
        else
            result += display;
    }
    return result;
}

/**
 * @brief 将诊断中的完整泛型实例标识符替换为源码形式。
 * @param message 待显示的诊断消息。
 * @return 保留原有上下文的诊断消息。
 */
Memory::String formatDiagnostic(const Memory::String &message)
{
    if (session.instanceDisplayNames.empty() || message.find("__") == Memory::String::npos) return message;
    return formatInstances(message, false);
}

/**
 * @brief 生成长度受控的实例名，并为诊断保留实参的可读拼写。
 * @param name 模板名称。
 * @param args 已归一化的实际类型参数。
 * @return 保留可读前缀和稳定类型指纹的实例名称。
 */
Memory::String mangle(const Memory::String &name, const Memory::Vector<Memory::String> &args)
{
    Memory::String result = name;
    for (const Memory::String &arg : args)
        result += "__" + chali_generic_name::argumentSymbol(arg);
    if (!args.empty() && !session.instanceDisplayNames.count(result))
    {
        Memory::String display = name;
        const auto definition = session.generics.find(name);
        if (definition != session.generics.end())
        {
            const auto &generic = definition->second;
            const auto &sourceName = generic.sourceName.empty() ? generic.name : generic.sourceName;
            const auto nominal = nominalTypeNames.find(sourceName);
            display = sourceName;
            if (session.ambiguousTemplateNames.count(sourceName) ||
                (nominal != nominalTypeNames.end() && nominal->second.size() > 1u))
                display = (generic.module.empty() ? Memory::String("__root") : generic.module) + "::" + sourceName;
        }
        display += '<';
        const auto argumentsBegin = display.size();
        for (std::size_t index = 0; index < args.size(); ++index)
        {
            if (index) display += ", ";
            display += formatInstances(args[index], true);
            // 递归模板可以在多个实参中重复上一层实例，限制展开后的诊断文本大小。
            if (display.size() - argumentsBegin > 4096u)
            {
                display.resize(argumentsBegin);
                display += "...";
                break;
            }
        }
        session.instanceDisplayNames.emplace(result, display + ">");
    }
    return result;
}

/**
 * @brief 收集用户具名类型，用于保留与核心别名同名的泛型实参。
 * @param source 完整预处理源码。
 */
void collectCoreAliasShadows(const Memory::String &source)
{
    coreAliasShadows.clear();
    nominalTypeNames.clear();
    nominalTypeDeclarations.clear();
    genericTypeModules.clear();
    nominalTypeImports.clear();
    Memory::String module;
    int braces = 0;
    for (std::size_t at = 0; at < source.size();)
    {
        if (source.compare(at, 2, "//") == 0)
        {
            at = source.find('\n', at);
            if (at == Memory::String::npos) break;
            continue;
        }
        if (source.compare(at, 2, "/*") == 0)
        {
            const auto end = source.find("*/", at + 2);
            if (end == Memory::String::npos) break;
            at = end + 2;
            continue;
        }
        if (source[at] == '\"' || source[at] == '\'')
        {
            const auto quote = source[at++];
            while (at < source.size())
            {
                if (source[at] == '\\')
                    at += 2;
                else if (source[at++] == quote)
                    break;
            }
            continue;
        }
        if (!identStart(source[at]))
        {
            if (source[at] == '{') ++braces;
            if (source[at] == '}') --braces;
            ++at;
            continue;
        }
        const auto begin = at++;
        while (at < source.size() && identPart(source[at])) ++at;
        const auto word = source.substr(begin, at - begin);
        if (word == "module")
        {
            skipSpace(source, at);
            const auto name = at;
            while (at < source.size() && (identPart(source[at]) || source[at] == ':')) ++at;
            module = source.substr(name, at - name);
            if (module == "__root") module.clear();
            continue;
        }
        if (word == "import")
        {
            skipSpace(source, at);
            const auto name = at;
            while (at < source.size() && (identPart(source[at]) || source[at] == ':')) ++at;
            nominalTypeImports[module].insert(source.substr(name, at - name));
            continue;
        }
        if (word != "struct" && word != "enum" && word != "type" && word != "interface" && word != "opcode") continue;
        skipSpace(source, at);
        auto name = at;
        if (word == "type" && wordAt(source, at, "alias"))
        {
            auto candidate = at + 5u;
            skipSpace(source, candidate);
            if (candidate < source.size() && identStart(source[candidate])) at = name = candidate;
        }
        while (at < source.size() && identPart(source[at])) ++at;
        const auto bare = source.substr(name, at - name);
        if (word != "opcode" || module != "std::core") coreAliasShadows.insert(bare);
        const auto qualified = module.empty() ? bare : module + "::" + bare;
        nominalTypeNames[bare].insert(qualified);
        if (braces == 0) ++nominalTypeDeclarations[qualified];
        std::size_t open = at;
        skipSpace(source, open);
        if (open < source.size() && source[open] == '<') genericTypeModules[bare] = module;
    }
}

/**
 * @brief 归一化类型拼写并保留被用户声明遮蔽的内建别名。
 * @param value 待比较或生成实例的类型拼写。
 * @param module 可选来源模块；为空指针时保留已解析类型的兼容归一规则。
 * @param error 可选歧义或不可见类型诊断。
 * @return 统一空白、标准库限定名与泛型符号后的类型文本。
 */
Memory::String canonicalTypeImpl(Memory::String value, const Memory::String *module, Memory::String *error)
{
    value = trim(value);
    /* 引用限定符与 & 之间的空白不参与泛型类型身份。 */
    for (std::size_t pos = value.find('&'); pos != Memory::String::npos; pos = value.find('&', pos + 1u))
        while (pos > 0u && std::isspace(static_cast<unsigned char>(value[pos - 1u])))
            value.erase(--pos, 1u);
    /* 内部类型文本也使用可由 0.12.0 文法重新解析的返回类型前置形式。 */
    {
        int paren = 0, angle = 0;
        for (std::size_t at = 0; at < value.size(); ++at)
        {
            if (value[at] == '(')
                ++paren;
            else if (value[at] == ')')
                --paren;
            else if (value[at] == '<')
                ++angle;
            else if (value[at] == '>' && (at == 0 || value[at - 1] != '-'))
                --angle;
            else if (paren == 0 && angle == 0 && at > 0 && wordAt(value, at, "fn"))
            {
                std::size_t open = at + 2u, close = 0;
                skipSpace(value, open);
                if (open < value.size() && value[open] == '(' && balanced(value, open, '(', ')', close) && trim(value.substr(close + 1u)).empty())
                {
                    Memory::String parameters;
                    const auto parts = splitTop(value.substr(open + 1u, close - open - 1u));
                    for (std::size_t index = 0; index < parts.size(); ++index)
                    {
                        if (index) parameters += ",";
                        const auto &part = parts[index];
                        const bool variadic = part.size() >= 3u && part.compare(part.size() - 3u, 3u, "...") == 0;
                        parameters += canonicalTypeImpl(variadic ? part.substr(0, part.size() - 3u) : part, module, error);
                        if (variadic) parameters += "...";
                    }
                    return canonicalTypeImpl(value.substr(0, at), module, error) + " fn(" + parameters + ")";
                }
            }
        }
    }
    /* 复合类型先递归归一叶子，避免别名因引用或元组包装产生不同实例。 */
    if (value.size() > 2u && value.compare(value.size() - 2u, 2u, "[]") == 0)
        return canonicalTypeImpl(value.substr(0, value.size() - 2u), module, error) + "[]";
    for (const Memory::String suffix : { Memory::String(" const&"), Memory::String(" mut&") })
    {
        if (value.size() > suffix.size() && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0)
            return canonicalTypeImpl(value.substr(0, value.size() - suffix.size()), module, error) + suffix;
    }
    if (!value.empty() && value.back() == '&')
        return canonicalTypeImpl(value.substr(0, value.size() - 1u), module, error) + " const&";
    if (value.size() > 2u && value.front() == '(' && value.back() == ')')
    {
        const auto parts = splitTop(value.substr(1, value.size() - 2u));
        if (parts.size() == 1u && (wordAt(trim(parts[0]), 0, "fn") || parts[0].find(" fn(") != Memory::String::npos))
            return "(" + canonicalTypeImpl(parts[0], module, error) + ")";
        if (parts.size() >= 2u)
        {
            Memory::String tuple = "(";
            for (std::size_t index = 0; index < parts.size(); ++index)
            {
                if (index) tuple += ",";
                tuple += canonicalTypeImpl(parts[index], module, error);
            }
            return tuple + ")";
        }
    }
    // 已生成实例的内部名称本身全局唯一；先识别它，避免名义类型补全与限定名前缀剥离来回切换。
    for (const auto &entry : session.made)
    {
        const auto &instance = entry.second;
        if (instance.generic && (value == instance.name || value == instance.generic->module + "::" + instance.name))
            return instance.name;
    }
    const Memory::Map<Memory::String, Memory::String> aliases = {
        { "std::core::String", "std::core::String" },
        { "std::big_number::BigInteger", "std::big_number::BigInteger" },
        { "std::big_number::BigDecimal", "std::big_number::BigDecimal" },
        { "std::core::File", "std::core::File" },
        { "std::core::Process", "std::core::Process" },
        { "std::core::Any", "std::core::Any" },
    };
    const Memory::Map<Memory::String, Memory::String> implicitAliases = {
        { "String", "std::core::String" }, { "BigInteger", "std::big_number::BigInteger" }, { "BigDecimal", "std::big_number::BigDecimal" }, { "File", "std::core::File" }, { "Process", "std::core::Process" }, { "Any", "std::core::Any" }
    };
    const auto implicit = implicitAliases.find(value);
    const auto scopedNominal = nominalTypeNames.find(value);
    if (module && scopedNominal != nominalTypeNames.end())
    {
        const auto local = module->empty() ? value : *module + "::" + value;
        if (scopedNominal->second.count(local))
            return module->empty() && scopedNominal->second.size() > 1u ? "__root::" + value : local;
        if (scopedNominal->second.count(value)) return "__root::" + value;
        Memory::Set<Memory::String> imports{ "std::core" }, candidates;
        if (value == "BigInteger" || value == "BigDecimal") imports.insert("std::big_number");
        const auto imported = nominalTypeImports.find(*module);
        if (imported != nominalTypeImports.end()) imports.insert(imported->second.begin(), imported->second.end());
        for (const auto &owner : imports)
            if (scopedNominal->second.count(owner + "::" + value)) candidates.insert(owner + "::" + value);
        if (candidates.size() == 1) return *candidates.begin();
        if (error) *error = "泛型类型实参 " + value + (candidates.empty() ? " 未在来源模块声明或导入：" : " 在来源模块有歧义，请使用限定名：") + (module->empty() ? "__root" : *module);
        return value;
    }
    if (implicit != implicitAliases.end() && !coreAliasShadows.count(value)) return implicit->second;
    for (const auto &container : { Memory::String("List"), Memory::String("Set"), Memory::String("Map") })
    {
        Memory::String prefix = container + "<";
        if (value.compare(0, prefix.size(), prefix) == 0) value = "std::core::" + value;
        prefix = "std::core::" + prefix;
        if (value.compare(0, prefix.size(), prefix) != 0 || value.back() != '>') continue;
        auto parts = splitTop(value.substr(prefix.size(), value.size() - prefix.size() - 1));
        value = prefix;
        for (std::size_t index = 0; index < parts.size(); ++index)
        {
            if (index) value += ",";
            value += canonicalTypeImpl(parts[index], module, error);
        }
        return value + ">";
    }
    const auto found = aliases.find(value);
    if (found != aliases.end()) return found->second;
    const auto nominal = nominalTypeNames.find(value);
    if (nominal != nominalTypeNames.end() && nominal->second.size() > 1u && nominal->second.count("program::" + value))
        return "program::" + value;
    // 根模块类型在标准库泛型正文中仍须保留根模块身份，避免同名 opcode 抢占实参。
    if (nominal != nominalTypeNames.end() && nominal->second.size() > 1u && nominal->second.count(value)) return "__root::" + value;
    if (nominal != nominalTypeNames.end() && nominal->second.size() == 1u) return *nominal->second.begin();
    for (const auto &generic : genericTypeModules)
    {
        if (!generic.second.empty())
        {
            const auto prefix = generic.second + "::" + generic.first + "__";
            if (value.compare(0, prefix.size(), prefix) == 0) return value.substr(generic.second.size() + 2u);
        }
    }
    for (const auto &nominal : nominalTypeNames)
        if (nominal.second.count(value)) return value;
    /* `std::模块::类型` 一律按**裸类型名**参与改名与类型同一性：同一份
     * `Result<FileStream, IoError>` 不能因为写成了限定形式就变成另一个类型。 */
    if (value.compare(0u, 5u, "std::") == 0u && value.compare(0u, 11u, "std::core::") != 0u)
    {
        const std::size_t last = value.rfind("::");
        if (last != Memory::String::npos && value.find('<', last) == Memory::String::npos)
            return value.substr(last + 2u);
    }
    if (value.compare(0u, 11u, "std::core::") == 0u && value.compare(0u, 16u, "std::core::List<") != 0u && value.compare(0u, 15u, "std::core::Set<") != 0u && value.compare(0u, 15u, "std::core::Map<") != 0u && value.find("::", 11u) == Memory::String::npos)
        return value.substr(11u);
    return value;
}

/**
 * @brief 归一已经解析的类型拼写，保留原有调用接口。
 * @param value 类型文本。
 * @return 不依赖额外作用域的规范文本。
 */
Memory::String canonicalType(Memory::String value)
{
    return canonicalTypeImpl(std::move(value), nullptr, nullptr);
}

/**
 * @brief 归一来源模块中的类型实参，防止其他模块的同名类型捕获它。
 * @param value 类型文本。
 * @param module 原始模块。
 * @param error 歧义或不可见类型诊断。
 * @return 规范类型文本。
 */
Memory::String canonicalTypeInModule(Memory::String value, const Memory::String &module, Memory::String &error)
{
    error.clear();
    return canonicalTypeImpl(std::move(value), &module, &error);
}

/**
 * @brief 按标识符替换类型参数，保留字符串、字符和注释的原始内容。
 * @param source 待替换的声明或类型表达式。
 * @param values 类型参数与实际类型的映射。
 * @param nameOffset 可选的模板声明名称位置。
 * @param name 模板声明的实例名称。
 * @return 完成替换的文本。
 */
Memory::String substituteIdentifiers(const Memory::String &source,
                                     const Memory::Map<Memory::String, Memory::String> &values,
                                     std::size_t nameOffset, const Memory::String &name)
{
    Memory::String result;
    for (std::size_t at = 0; at < source.size();)
    {
        const std::size_t begin = at;
        if (source.compare(at, 2, "//") == 0 || source.compare(at, 2, "/*") == 0)
        {
            const bool line = source[at + 1] == '/';
            const auto end = source.find(line ? "\n" : "*/", at + 2);
            at = end == Memory::String::npos ? source.size() : end + (line ? 1u : 2u);
        }
        else if (source[at] == '"' || source[at] == '\'')
        {
            const char quote = source[at++];
            while (at < source.size())
            {
                if (source[at] == '\\' && at + 1 < source.size())
                {
                    at += 2;
                    continue;
                }
                if (source[at++] == quote) break;
            }
        }
        else if (identStart(source[at]))
        {
            ++at;
            while (at < source.size() && identPart(source[at])) ++at;
            const auto word = source.substr(begin, at - begin);
            const auto found = values.find(word);
            if (found != values.end())
            {
                std::size_t dot = at;
                skipSpace(source, dot);
                if (dot < source.size() && source[dot] == '.')
                {
                    std::size_t member = dot + 1;
                    skipSpace(source, member);
                    const auto start = member;
                    if (member < source.size() && identStart(source[member]))
                    {
                        while (member < source.size() && identPart(source[member])) ++member;
                        result += found->second + "::" + source.substr(start, member - start);
                        result.append(static_cast<std::size_t>(std::count(source.begin() + at, source.begin() + member, '\n')), '\n');
                        at = member;
                        continue;
                    }
                }
            }
            result += begin == nameOffset ? name : found == values.end() ? word :
                                                                           found->second;
            continue;
        }
        else
            ++at;
        result += source.substr(begin, at - begin);
    }
    return result;
}

/**
 * @brief 以实际类型参数生成声明，并保留标准可选值的引用访问器语义。
 * @param expansion 模板及其完整类型实参。
 * @return 已替换参数和声明名称的源码。
 */
Memory::String substitute(const Expansion &expansion)
{
    Memory::Map<Memory::String, Memory::String> values;
    for (std::size_t i = 0; i < expansion.args.size(); ++i)
        values[expansion.generic->params[i].name] = expansion.args[i];
    Memory::String result = substituteIdentifiers(expansion.generic->declaration, values,
                                                  expansion.generic->nameOffset, expansion.name);
    // 引用访问器没有默认空值：成功直接返回载荷，失败路径停止执行。
    if (expansion.generic->name == "Option" || expansion.generic->name == "Result")
    {
        for (const Memory::String method : { Memory::String("unwrap"), Memory::String("unwrapOr"), Memory::String("expect"), Memory::String("error") })
        {
            const std::size_t parameter = method == "error" ? 1u : 0u;
            if (parameter >= expansion.args.size()) continue;
            const auto &reference = expansion.args[parameter];
            if (reference.find("mut&") == Memory::String::npos && reference.find("const&") == Memory::String::npos) continue;
            const auto begin = result.find(" " + method + "(");
            if (begin == Memory::String::npos) continue;
            const auto open = result.find('{', begin);
            std::size_t close = 0;
            if (!balanced(result, open, '{', '}', close)) continue;
            Memory::String body = result.substr(open, close - open + 1);
            const auto replaceAll = [&](const Memory::String &from, const Memory::String &to) {
                for (std::size_t at = 0; (at = body.find(from, at)) != Memory::String::npos; at += to.size()) body.replace(at, from.size(), to);
            };
            replaceAll(reference + " result;", "");
            replaceAll(reference + " result = fallback;", "");
            replaceAll("result = value;", "return value;");
            replaceAll("result = error;", "return error;");
            replaceAll("return result;", method == "unwrapOr" ? "return fallback;" : "std::core::panic(\"不可从空的可选值获取引用\");");
            result.replace(open, close - open + 1, body);
        }
    }
    return expansion.generic->operatorSwitches + result;
}

/**
 * @brief 检查具体类型表达式的嵌套深度，避免归一化默认类型时耗尽调用栈。
 * @param text 类型表达式。
 * @param error 超限时接收诊断。
 * @return 嵌套深度未达到上限时返回 true。
 */
bool checkTypeNesting(const Memory::String &text, Memory::String &error)
{
    std::size_t nesting = 0;
    for (std::size_t i = 0; i < text.size(); ++i)
    {
        const char value = text[i];
        if (value == '<' || value == '(' || value == '[')
        {
            if (++nesting >= 128u)
            {
                error = "泛型类型嵌套超过 128 层，请检查递归类型展开";
                return false;
            }
        }
        else if ((value == '>' && (i == 0 || text[i - 1] != '-')) || value == ')' || value == ']')
        {
            if (nesting) --nesting;
        }
    }
    return true;
}

/** @brief 在递归改写退出时恢复当前别名展开链。 */
struct AliasExpansionScope
{
    Memory::Vector<Memory::String> &chain;
    std::size_t size;

    /** @brief 恢复进入本层前的别名链长度。 */
    ~AliasExpansionScope() { chain.resize(size); }
};

/**
 * @brief 判断裸别名是否出现在声明类型或限定类型成员的位置。
 * @param text 待改写源码。
 * @param begin 完整名称起点。
 * @param after 名称后的首个非空白位置。
 * @return 能按类型解释时返回 true。
 */
bool bareAliasUse(const Memory::String &text, std::size_t begin, std::size_t after)
{
    skipSpace(text, after);
    auto before = begin;
    while (before && std::isspace(static_cast<unsigned char>(text[before - 1u]))) --before;
    if (before && text[before - 1u] == '.') return false;
    if (text.compare(after, 2, "::") == 0) return true;
    auto previous = before;
    while (previous && identPart(text[previous - 1u])) --previous;
    const auto prefix = text.substr(previous, before - previous);
    if (prefix == "impl" || prefix == "new") return true;
    if (after < text.size() && text[after] == '('
        && (prefix.empty() || prefix == "return" || prefix == "yield" || prefix == "move"))
        return true;
    if (after == text.size()) return false;
    if (text.compare(after, 2, "[]") == 0)
    {
        after += 2u;
        skipSpace(text, after);
    }
    if (wordAt(text, after, "const") || wordAt(text, after, "mut"))
    {
        after += wordAt(text, after, "const") ? 5u : 3u;
        skipSpace(text, after);
    }
    if (after < text.size() && text[after] == '&' && text.compare(after, 2, "&&") != 0
        && (prefix.empty() || prefix == "const" || prefix == "static"))
    {
        ++after;
        skipSpace(text, after);
    }
    if (after < text.size() && identStart(text[after])) return true;
    if (before && text[before - 1u] == ':' && (before < 2u || text[before - 2u] != ':')
        && (text[after] == '{' || text[after] == ',' || text[after] == '='))
        return true;
    return false;
}

/**
 * @brief 按已知模板改写泛型使用点并排队等待展开。
 * @param text 需要单态化的声明或表达式文本。
 * @param generics 当前可见的泛型声明。
 * @param implemented 具体类型已实现的接口集合。
 * @param made 复用并补充已创建的模板实例。
 * @param pending 按发现顺序追加待展开的实例名。
 * @param error 失败时接收实参、约束或递归深度诊断。
 * @param depth 当前默认类型展开的递归层数。
 * @param initialModule 类型片段继承的来源模块。
 * @return 所有泛型使用点可完成实例化时返回 true。
 */
bool rewrite(Memory::String &text, const Memory::Map<Memory::String, Generic> &generics,
             const Memory::Map<Memory::String, Memory::Set<Memory::String>> &implemented,
             Memory::Map<Memory::String, Expansion> &made, Memory::Vector<Memory::String> &pending,
             Memory::String &error, std::size_t depth, const Memory::String &initialModule, bool typeFragment, bool recordAliasUses)
{
    if (depth == 0) error.clear();
    if (depth >= 128u)
    {
        error = "泛型类型嵌套超过 128 层，请检查递归默认类型";
        return false;
    }
    Memory::String output;
    Memory::String lookupModule = initialModule;
    auto &aliasChain = session.aliasExpansionChain;
    Memory::Vector<bool> typeGroups;
    bool typeHead = false, interfaceList = false;
    bool typeDefinition = false, typeTarget = false;
    const bool hasAliases = std::any_of(generics.begin(), generics.end(),
                                        [](const auto &entry) { return !entry.second.aliasTarget.empty(); });
    Memory::Map<Memory::String, Memory::String> scopedGenerics;
    for (const auto &entry : generics)
        scopedGenerics[entry.second.module + "::" + (entry.second.sourceName.empty() ? entry.second.name : entry.second.sourceName)] = entry.first;
    for (std::size_t at = 0; at < text.size();)
    {
        if (text.compare(at, 2, "//") == 0)
        {
            const std::size_t end = text.find('\n', at);
            const std::size_t stop = end == Memory::String::npos ? text.size() : end + 1;
            output += text.substr(at, stop - at);
            at = stop;
            continue;
        }
        if (text.compare(at, 2, "/*") == 0)
        {
            const std::size_t end = text.find("*/", at + 2);
            const std::size_t stop = end == Memory::String::npos ? text.size() : end + 2;
            output += text.substr(at, stop - at);
            at = stop;
            continue;
        }
        if (text[at] == '"' || text[at] == '\'')
        {
            const char quote = text[at];
            const std::size_t begin = at++;
            while (at < text.size())
            {
                if (text[at] == '\\' && at + 1 < text.size())
                {
                    at += 2;
                    continue;
                }
                if (text[at++] == quote) break;
            }
            output += text.substr(begin, at - begin);
            continue;
        }
        if (!identStart(text[at]))
        {
            if (typeHead && text[at] == ':' && text.compare(at, 2, "::") != 0
                && (at == 0 || text[at - 1u] != ':'))
                interfaceList = true;
            if (typeDefinition && text[at] == '=') typeTarget = true;
            if (text[at] == '{' || text[at] == ';')
                typeHead = interfaceList = typeDefinition = typeTarget = false;
            if (hasAliases && !typeFragment && text[at] == '(')
            {
                auto head = at;
                while (head && std::isspace(static_cast<unsigned char>(text[head - 1u]))) --head;
                auto word = head;
                while (word && identPart(text[word - 1u])) --word;
                const auto opener = text.substr(word, head - word);
                const bool typed = opener == "fn" || opener == "__typeTag" || opener == "__rawSize" || opener == "__rawAlign";
                std::size_t close = 0;
                const bool tuple = (head == 0 || !identPart(text[head - 1u]))
                    && balanced(text, at, '(', ')', close)
                    && bareAliasUse(text, at, close + 1u);
                typeGroups.push_back(typed || tuple || (!typeGroups.empty() && typeGroups.back()));
            }
            else if (hasAliases && text[at] == ')' && !typeGroups.empty())
                typeGroups.pop_back();
            output += text[at++];
            continue;
        }
        const std::size_t begin = at++;
        while (at < text.size() && identPart(text[at])) ++at;
        const Memory::String name = text.substr(begin, at - begin);
        if (hasAliases && !typeFragment && (name == "struct" || name == "enum" || name == "interface"))
            typeHead = true;
        if (hasAliases && !typeFragment && name == "type")
        {
            auto previous = begin;
            while (previous && std::isspace(static_cast<unsigned char>(text[previous - 1u]))) --previous;
            typeDefinition = previous == 0 || text[previous - 1u] == '{' || text[previous - 1u] == '}' || text[previous - 1u] == ';';
        }
        if (name == "module")
        {
            auto moduleBegin = at;
            skipSpace(text, moduleBegin);
            auto moduleEnd = moduleBegin;
            while (moduleEnd < text.size() && (identPart(text[moduleEnd]) || text[moduleEnd] == ':')) ++moduleEnd;
            lookupModule = text.substr(moduleBegin, moduleEnd - moduleBegin);
            if (lookupModule == "__root") lookupModule.clear();
        }
        if (hasAliases && (name == "module" || name == "import"))
        {
            const auto end = text.find(';', at);
            if (end != Memory::String::npos)
            {
                output += text.substr(begin, end - begin + 1u);
                at = end + 1u;
                continue;
            }
        }
        auto generic = generics.find(name);
        Memory::String qualifier;
        if (begin >= 2 && text.compare(begin - 2, 2, "::") == 0)
        {
            auto qualifierBegin = begin - 2;
            while (qualifierBegin > 0 && (identPart(text[qualifierBegin - 1]) || text[qualifierBegin - 1] == ':')) --qualifierBegin;
            qualifier = text.substr(qualifierBegin, begin - qualifierBegin - 2);
        }
        const auto scope = qualifier.empty() ? lookupModule : qualifier == "__root" ? Memory::String() : qualifier;
        const auto scoped = scopedGenerics.find(scope + "::" + name);
        if (scoped != scopedGenerics.end()) generic = generics.find(scoped->second);
        std::size_t open = at;
        while (open < text.size() && std::isspace(static_cast<unsigned char>(text[open]))) ++open;
        const auto imports = nominalTypeImports.find(lookupModule);
        if (generic != generics.end() && !generic->second.aliasTarget.empty()
            && scoped == scopedGenerics.end())
            generic = generics.end();
        const auto nominal = hasAliases ? nominalTypeNames.find(name) : nominalTypeNames.end();
        const bool localType = nominal != nominalTypeNames.end()
            && nominal->second.count(lookupModule.empty() ? name : lookupModule + "::" + name);
        if (hasAliases && qualifier.empty() && scoped == scopedGenerics.end() && !localType
            && imports != nominalTypeImports.end())
        {
            auto importedAlias = generics.end();
            for (const auto &owner : imports->second)
            {
                const auto found = scopedGenerics.find(owner + "::" + name);
                if (found == scopedGenerics.end()) continue;
                auto candidate = generics.find(found->second);
                if (candidate->second.aliasTarget.empty()) continue;
                if (importedAlias != generics.end() && (typeFragment || interfaceList || typeTarget || bareAliasUse(text, begin, open)
                    || (open < text.size() && text[open] == '<')))
                {
                    error = "类型别名 " + name + " 有歧义，请使用模块限定名";
                    return false;
                }
                importedAlias = candidate;
            }
            if (importedAlias != generics.end()) generic = importedAlias;
        }
        const bool alias = generic != generics.end() && !generic->second.aliasTarget.empty();
        const auto wholeBegin = qualifier.empty() ? begin : begin - qualifier.size() - 2u;
        const bool angles = open < text.size() && text[open] == '<';
        bool typeGroup = false;
        if (alias && !typeGroups.empty() && typeGroups.back())
        {
            auto before = wholeBegin;
            while (before && std::isspace(static_cast<unsigned char>(text[before - 1u]))) --before;
            typeGroup = before && (text[before - 1u] == '(' || text[before - 1u] == ',');
        }
        const bool bareAlias = alias && !angles && (typeFragment || typeGroup || interfaceList || typeTarget || bareAliasUse(text, wholeBegin, open));
        const auto requiredModule = generic == generics.end() ? Memory::String() : generic->second.requiredQualifier.empty() ? Memory::String() : generic->second.requiredQualifier.substr(0, generic->second.requiredQualifier.size() - 2u);
        const bool explicitQualifierMatches = generic == generics.end() || generic->second.requiredQualifier.empty() || (begin >= generic->second.requiredQualifier.size() && text.compare(begin - generic->second.requiredQualifier.size(), generic->second.requiredQualifier.size(), generic->second.requiredQualifier) == 0);
        const bool importedQualifierMatches = qualifier.empty() && !requiredModule.empty()
            && (requiredModule == "std::core" || lookupModule == requiredModule || (imports != nominalTypeImports.end() && imports->second.count(requiredModule)));
        const bool qualifierMatches = explicitQualifierMatches || importedQualifierMatches;
        if (generic == generics.end() || !qualifierMatches || (!angles && !bareAlias))
        {
            output += generic != generics.end() && generic->second.name != name && open < text.size() && text[open] == '(' ? generic->second.name : name;
            continue;
        }
        std::size_t close = 0;
        const auto &declaration = generic->second.declaration;
        const bool functionTemplate = !generic->second.intrinsic && !wordAt(declaration, 0, "struct")
                                      && !wordAt(declaration, 0, "enum") && !wordAt(declaration, 0, "interface")
                                      && !wordAt(declaration, 0, "type");
        const bool closed = bareAlias || balanced(text, open, '<', '>', close);
        if (bareAlias) close = at - 1u;
        // 同名局部值的比较不能跨越语句或函数体寻找模板右括号。
        if (functionTemplate)
        {
            const auto boundary = text.find_first_of(";{}", open + 1);
            if (!closed || (boundary != Memory::String::npos && boundary < close))
            {
                output += name;
                continue;
            }
            auto after = close + 1;
            skipSpace(text, after);
            const auto arguments = text.substr(open + 1, close - open - 1);
            if ((after < text.size() && identStart(text[after]))
                || arguments.find("&&") != Memory::String::npos || arguments.find("||") != Memory::String::npos)
            {
                output += name;
                continue;
            }
        }
        if (!closed)
        {
            error = name + " 的类型实参缺少 >";
            return false;
        }
        // 宏类型模式保留类型应用，不能把通配符作为普通实参单态化。
        bool wildcard = false;
        for (auto mark = text.find('?', open + 1); !bareAlias && mark < close; mark = text.find('?', mark + 1))
        {
            auto before = mark;
            while (before > open && std::isspace(static_cast<unsigned char>(text[before - 1]))) --before;
            auto after = mark + 1;
            skipSpace(text, after);
            if ((text[before - 1] == '<' || text[before - 1] == ',') && (text[after] == '>' || text[after] == ','))
            {
                wildcard = true;
                break;
            }
        }
        if (wildcard)
        {
            output += text.substr(begin, close - begin + 1);
            at = close + 1;
            continue;
        }
        auto args = bareAlias ? Memory::Vector<Memory::String>{} : splitTop(text.substr(open + 1, close - open - 1));
        for (std::size_t index = 0; index < args.size(); ++index)
        {
            auto &arg = args[index];
            if (!checkTypeNesting(arg, error)) return false;
            if (index < generic->second.params.size() && generic->second.params[index].constant) continue;
            if (!rewrite(arg, generics, implemented, made, pending, error, depth + 1u, lookupModule, true, recordAliasUses)) return false;
            arg = lookupModule.empty() ? canonicalType(arg) : canonicalTypeInModule(arg, lookupModule, error);
            if (!error.empty()) return false;
        }
        if (name == "Equal" && generic->second.origin.file == "<内建 Equal>" && args.size() == 1u)
            args.insert(args.begin(), Memory::String("bool"));
        if (args.size() > generic->second.params.size())
        {
            error = name + " 至多接受 " + Memory::toString(generic->second.params.size()) + " 个类型实参";
            return false;
        }
        AliasExpansionScope aliasScope{ aliasChain, aliasChain.size() };
        if (alias)
        {
            const auto &key = generic->second.name;
            if (std::find(aliasChain.begin(), aliasChain.end(), key) != aliasChain.end())
            {
                error = "类型别名形成循环：";
                for (const auto &entry : aliasChain) error += entry + " -> ";
                error += key;
                return false;
            }
            aliasChain.push_back(key);
        }
        Memory::Map<Memory::String, Memory::String> bindings;
        auto constants = constantArguments;
        const auto evaluateArgument = [&](std::size_t index, Memory::String &argument, const Memory::String &module) {
            const auto &parameter = generic->second.params[index];
            if (!parameter.constant) return true;
            Memory::Map<Memory::String, Memory::String> scopedConstants;
            const bool scoped = !module.empty() && !aliasChain.empty();
            if (scoped)
            {
                scopedConstants = constants;
                const auto prefix = module + "::";
                for (const auto &entry : constantArguments)
                {
                    if (entry.first.compare(0, prefix.size(), prefix) != 0
                        || entry.first.find("::", prefix.size()) != Memory::String::npos)
                        continue;
                    const auto bare = entry.first.substr(prefix.size());
                    if (!bindings.count(bare)) scopedConstants[bare] = entry.second;
                }
            }
            Memory::String problem;
            const auto value = chali_syntax::evaluateConstant(argument, scoped ? scopedConstants : constants, problem);
            if (!problem.empty() || value.string)
            {
                error = name + " 的 const 泛型实参 " + parameter.name + " 必须是 int64 编译期常量：" + (problem.empty() ? argument : problem);
                return false;
            }
            argument = Memory::toString(value.number);
            constants[parameter.name] = argument;
            return true;
        };
        for (std::size_t i = 0; i < args.size(); ++i)
        {
            if (!evaluateArgument(i, args[i], lookupModule)) return false;
            bindings[generic->second.params[i].name] = args[i];
        }
        while (args.size() < generic->second.params.size())
        {
            const auto &parameter = generic->second.params[args.size()];
            if (!parameter.hasDefault)
            {
                error = name + " 缺少类型实参 " + parameter.name + "，请显式指定类型；不会默认采用 Any";
                return false;
            }
            auto fallback = substituteIdentifiers(parameter.fallback, bindings);
            if (!checkTypeNesting(fallback, error)) return false;
            if (!parameter.constant && !rewrite(fallback, generics, implemented, made, pending, error,
                                                depth + 1u, generic->second.module, true)) return false;
            if (!parameter.constant) fallback = generic->second.module.empty() ? canonicalType(fallback) : canonicalTypeInModule(fallback, generic->second.module, error);
            if (!error.empty()) return false;
            if (!evaluateArgument(args.size(), fallback, generic->second.module)) return false;
            bindings[parameter.name] = fallback;
            args.push_back(std::move(fallback));
        }
        Memory::Vector<Memory::String> constraints;
        for (const auto &parameter : generic->second.params)
        {
            auto constraint = substituteIdentifiers(parameter.constraint, bindings);
            if (!constraint.empty() && !rewrite(constraint, generics, implemented, made, pending, error, depth + 1u, generic->second.module, true)) return false;
            constraints.push_back(std::move(constraint));
        }
        if (alias)
        {
            auto target = substituteIdentifiers(generic->second.aliasTarget, bindings);
            const auto &module = generic->second.module;
            if (!rewrite(target, generics, implemented, made, pending, error, depth + 1u, module, true)) return false;
            target = canonicalTypeInModule(std::move(target), module, error);
            if (!error.empty()) return false;
            if (std::any_of(constraints.begin(), constraints.end(), [](const auto &value) { return !value.empty(); }))
                session.aliasConstraints.emplace(mangle(generic->second.name, args),
                                                 Expansion{ &generic->second, args, name, std::move(constraints) });
            if (recordAliasUses && session.editorAliasTypes.size() < 4096u)
            {
                auto spelling = text.substr(wholeBegin, close + 1u - wholeBegin);
                spelling.erase(std::remove_if(spelling.begin(), spelling.end(), [](unsigned char ch) { return std::isspace(ch); }), spelling.end());
                session.editorAliasTypes.emplace(std::make_pair(lookupModule, std::move(spelling)), target);
            }
            if (interfaceList && target.find("::") != Memory::String::npos)
            {
                // 接口列表的文法只接收名称，语义层将内部名称还原成已解析的限定名。
                const auto internal = mangle("__chali_alias_interface", { target });
                session.aliasInterfaceNames.emplace(internal, target);
                target = internal;
            }
            if (!qualifier.empty()) output.resize(output.size() - qualifier.size() - 2u);
            auto suffix = close + 1u;
            skipSpace(text, suffix);
            const auto function = target.find(" fn(");
            if (function != Memory::String::npos && target.find('<') > function && target.front() != '('
                && suffix < text.size() && (text[suffix] == '&' || wordAt(text, suffix, "const") || wordAt(text, suffix, "mut")))
                target = "(" + target + ")";
            output += target;
            output.append(static_cast<std::size_t>(std::count(text.begin() + begin, text.begin() + close + 1u, '\n')), '\n');
            at = close + 1u;
            continue;
        }
        const Memory::String specialized = mangle(generic->second.name, args);
        if (generic->second.intrinsic) session.intrinsicArguments[specialized] = args;
        const auto existing = made.find(specialized);
        if (existing != made.end() && (existing->second.generic != &generic->second || existing->second.args != args))
        {
            error = "泛型实例名称冲突：" + name;
            return false;
        }
        if (!generic->second.intrinsic && existing == made.end())
        {
            if (pending.size() >= 1024u)
            {
                error = "泛型实例化超过 1024 个实例，请检查递归类型展开";
                return false;
            }
            made[specialized] = Expansion{ &generic->second, args, specialized, constraints };
            pending.push_back(specialized);
        }
        output += specialized;
        at = close + 1;
    }
    text.swap(output);
    return true;
}

/**
 * @brief 在全部嵌套实例展开后检查约束，包含泛型实例实现的接口及传递继承。
 * @param made 本次编译的实例记录。
 * @param implemented 普通声明与生成声明实现的接口。
 * @param error 失败时接收具体实例的诊断。
 * @param registered 查询文本阶段尚未知晓的普通语义接口登记。
 * @return 所有具体类型实参满足约束时返回 true。
 */
bool validateConstraints(const Memory::Map<Memory::String, Expansion> &made,
                         const Memory::Map<Memory::String, Memory::Set<Memory::String>> &implemented,
                         Memory::String &error, RegisteredInterfaceCheck registered)
{
    for (const auto &item : made)
    {
        const auto &expansion = item.second;
        for (std::size_t i = 0; i < expansion.args.size(); ++i)
        {
            const auto &constraint = expansion.constraints[i];
            if (constraint.empty()) continue;
            // 标准字符串能力由每个值类型的默认 toString 提供，具体方法签名由语义阶段核验。
            const auto declarations = implemented.find("#string-protocol-declarations");
            bool stringCapability = constraint == "std::io::ToString" || (constraint == "ToString" && declarations != implemented.end() && declarations->second.size() == 1 && declarations->second.count("std::io::ToString"));
            const auto protocol = made.find(constraint);
            if (protocol != made.end() && (protocol->second.generic->module == "std::core" || protocol->second.generic->origin.file == "<内建 ImplicitConvert>") && protocol->second.generic->name == "ImplicitConvert" && protocol->second.args.size() == 1 && (protocol->second.args[0] == "String" || protocol->second.args[0] == "std::core::String"))
                stringCapability = true;
            if (stringCapability && expansion.args[i] != "void") continue;
            const auto concrete = implemented.find(expansion.args[i]);
            if (concrete == implemented.end() || !concrete->second.count(constraint))
            {
                if (registered(expansion.args[i], constraint, expansion.generic->module)) continue;
                error = expansion.generic->name + " 的类型实参 " + expansion.args[i] + " 未实现接口 " + constraint;
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief 检查别名定义和默认实参中的递归展开，包括尚未使用的别名。
 * @param generics 当前模块图中的泛型定义和别名。
 * @param error 失败时接收别名声明的诊断。
 * @return 所有别名都可以完成符号展开时返回 true。
 */
bool validateAliasDeclarations(const Memory::Map<Memory::String, Generic> &generics, Memory::String &error)
{
    if (std::none_of(generics.begin(), generics.end(),
                     [](const auto &entry) { return !entry.second.aliasTarget.empty(); }))
        return true;
    auto symbolic = generics;
    for (auto &entry : symbolic)
        for (auto &parameter : entry.second.params)
        {
            parameter.constant = false;
            parameter.constraint.clear();
        }
    for (const auto &entry : symbolic)
    {
        const auto &alias = entry.second;
        if (alias.aliasTarget.empty()) continue;
        const auto name = alias.module.empty() ? alias.sourceName : alias.module + "::" + alias.sourceName;
        const auto declaration = nominalTypeDeclarations.find(name);
        if (declaration != nominalTypeDeclarations.end() && declaration->second > 1u)
        {
            error = "类型别名与类型声明重名：" + name;
            return false;
        }
        Memory::Map<Memory::String, Memory::String> bindings;
        for (std::size_t i = 0; i < alias.params.size(); ++i)
            bindings[alias.params[i].name] = "McAliasParameter" + Memory::toString(i);
        Memory::Map<Memory::String, Expansion> made;
        Memory::Vector<Memory::String> pending;
        Memory::Map<Memory::String, Memory::Set<Memory::String>> implemented;
        session.aliasExpansionChain.push_back(alias.name);
        AliasExpansionScope scope{ session.aliasExpansionChain, 0u };
        for (const auto &parameter : alias.params)
        {
            if (!parameter.hasDefault) continue;
            auto fallback = substituteIdentifiers(parameter.fallback, bindings);
            if (!rewrite(fallback, symbolic, implemented, made, pending, error, 0u, alias.module, true)) return false;
        }
        auto target = substituteIdentifiers(alias.aliasTarget, bindings);
        if (!rewrite(target, symbolic, implemented, made, pending, error, 0u, alias.module, true)) return false;
    }
    return true;
}

/**
 * @brief 为带模块前缀的实例文本追加逐行来源，生成指令映射到模板声明。
 * @param expansion 本次具体实例。
 * @param declaration 包含内层 module 行的实例声明文本。
 * @param origins 接收与输出文本相同数量的行来源。
 * @return 无。
 */
void appendInstantiationOrigins(const Expansion &expansion, const Memory::String &declaration,
                                Memory::Vector<preprocess::LineOrigin> &origins)
{
    const auto &generic = *expansion.generic;
    const auto prefix = generic.operatorSwitches;
    const auto headers = 3u + static_cast<std::size_t>(std::count(prefix.begin(), prefix.end(), '\n'));
    const auto count = 3u + static_cast<std::size_t>(std::count(declaration.begin(), declaration.end(), '\n'));
    for (std::size_t line = 0; line < count; ++line)
    {
        const auto index = line >= headers ? line - headers : generic.declarationOrigins.size();
        origins.push_back(index < generic.declarationOrigins.size() ? generic.declarationOrigins[index] : generic.origin);
    }
}

/**
 * @brief 展开源码中的泛型声明并保留每行原始来源。
 * @param result 待原地规范化的预处理结果。
 * @param error 失败诊断。
 * @return 全部实例与约束有效时返回真。
 */
bool run(preprocess::Result &result, Memory::String &error)
{
    collectConstantArguments(result.text);
    session = Session{};
    collectCoreAliasShadows(result.text);
    Memory::Map<Memory::String, Memory::String> operatorSwitches;
    collectGenericOperatorSwitches(result.text, operatorSwitches);
    // 泛型方法的显式错误类型交给语义层补齐其余参数；跳过注释与字符串。
    for (std::size_t at = 0; at < result.text.size(); ++at)
    {
        if (result.text.compare(at, 2, "//") == 0)
        {
            auto end = result.text.find('\n', at);
            if (end == Memory::String::npos) break;
            at = end;
            continue;
        }
        if (result.text.compare(at, 2, "/*") == 0)
        {
            auto end = result.text.find("*/", at + 2);
            if (end == Memory::String::npos) break;
            at = end + 1;
            continue;
        }
        if (result.text[at] == '"' || result.text[at] == '\'')
        {
            auto quote = result.text[at++];
            while (at < result.text.size())
            {
                if (result.text[at] == '\\')
                {
                    at += 2;
                    continue;
                }
                if (result.text[at] == quote) break;
                ++at;
            }
            continue;
        }
        if (result.text[at] != '.') continue;
        std::size_t begin = at + 1;
        skipSpace(result.text, begin);
        for (const auto &pair : { std::make_pair("mapErr", "__chaliMapErr"), std::make_pair("okOr", "__chaliOkOr"), std::make_pair("okOrElse", "__chaliOkOrElse"), std::make_pair("castTo", "__chaliCastTo") })
        {
            if (!wordAt(result.text, begin, pair.first)) continue;
            auto end = begin + Memory::String(pair.first).size();
            skipSpace(result.text, end);
            if (end < result.text.size() && result.text[end] == '<') result.text.replace(begin, Memory::String(pair.first).size(), pair.second);
        }
    }
    normalizeGenericSyntax(result.text);
    auto &generics = session.generics;
    const auto addBuiltin = [&](const Memory::String &name, Memory::Vector<Parameter> params,
                                const Memory::String &declaration, std::size_t nameOffset) {
        Generic builtin;
        builtin.name = name;
        builtin.params = std::move(params);
        builtin.declaration = declaration;
        builtin.nameOffset = nameOffset;
        builtin.origin.file = "<内建 " + name + ">";
        builtin.origin.line = 1u;
        generics.emplace(name, std::move(builtin));
    };
    const Parameter resultType{ "R", "", "std::core::Any" };
    const Parameter operand{ "T", "", "std::core::Any" };
    /* 名字约定：左边机器（接收者在左）**不带方向前缀**、用全名（`Add` / `Subtract` /
     * `Multiply` / `Divide` / `Modulo` / `Equal` / `NotEqual` / `Less` / `LessOrEqual` /
     * `Greater` / `GreaterOrEqual` / `And` / `Or` / `Xor` / `ShiftLeft` / `ShiftRight`）；
     * 右边机器保留 `Right` 前缀、同样用全名（`RightAdd` / `RightSubtract` / …）。
     * pair 的第二个是**方法名后缀**，仍沿用老缩写（`leftAdd` / `leftSub` / `leftMul` …），
     * `chali_semantics.cpp` 的 `OperatorInterfaceSpecs()` 按方法名配方向，两边必须一致。 */
    const Memory::Vector<std::pair<Memory::String, Memory::String>> operators = {
        { "Add", "Add" },
        { "Subtract", "Sub" },
        { "Multiply", "Mul" },
        { "Divide", "Div" },
        { "Modulo", "Mod" },
        { "Equal", "Equal" },
        { "NotEqual", "NotEqual" },
        { "Less", "Less" },
        { "LessOrEqual", "LessEqual" },
        { "Greater", "Greater" },
        { "GreaterOrEqual", "GreaterEqual" },
        { "And", "BitAnd" },
        { "Or", "BitOr" },
        { "Xor", "BitXor" },
        { "ShiftLeft", "ShiftLeft" },
        { "ShiftRight", "ShiftRight" },
    };
    for (const auto &item : operators)
    {
        for (const Memory::String &side : { Memory::String("Left"), Memory::String("Right") })
        {
            const Memory::String name = (side == "Left") ? item.first : ("Right" + item.first);
            const Memory::String method = (side == "Left" ? "left" : "right") + item.second;
            addBuiltin(name, { resultType, operand },
                       "interface " + name + " { R " + method + "(T value); };", 10u);
        }
    }
    /* 一元运算符：接口方法只有一个参数，就是**操作数本身**（接收者）。
     * 与二元接口的区别是"没有另一个操作数"。 */
    const Memory::Vector<std::pair<Memory::String, Memory::String>> unaryOperators = {
        { "Negate", "negate" },
        { "BitNot", "bitNot" },
        { "Positive", "positive" },
    };
    for (const auto &item : unaryOperators)
    {
        addBuiltin(item.first, { resultType, operand },
                   "interface " + item.first + " { R " + item.second + "(T value); };", 10u);
    }
    addBuiltin("Hash", { Parameter{ "T", "", "std::core::Any" } },
               "interface Hash { uint64 hash(T value); };", 10u);
    addBuiltin("Clone", { Parameter{ "T", "", "std::core::Any" } },
               "interface Clone { T clone(); };", 10u);
    addBuiltin("Copy", { Parameter{ "T", "", "std::core::Any" } },
               "interface Copy { T copy(); };", 10u);
    addBuiltin("ExplicitConvert", { Parameter{ "T", "", "std::core::Any" } },
               "interface ExplicitConvert { T explicitConvert(); };", 10u);
    addBuiltin("ImplicitConvert", { Parameter{ "T", "", "std::core::Any" } },
               "interface ImplicitConvert { T implicitConvert(); };", 10u);
    addBuiltin("Index", { Parameter{ "R", "", "std::core::Any" }, Parameter{ "T", "", "std::core::Any" } },
               "interface Index { R index(T key); };", 10u);
    addBuiltin("Iterator", { Parameter{ "R", "", "std::core::Any" } },
               R"CHALI(interface Iterator {
    Option<R> iterator(int64 index);
};)CHALI",
               10u);
    const auto addPointerConversion = [&](const Memory::String &name, const Memory::String &declaration) {
        addBuiltin(name, { Parameter{ "T", "", "std::core::Any" } }, declaration, declaration.find(name));
        generics[name].module = "std::core";
    };
    addPointerConversion("toReference", R"CHALI(T mut& toReference(Option<T mut&> value) {
    return value.unwrap();
})CHALI");
    const Memory::String toOptional = R"CHALI(Option<T mut&> toOptional(T mut& value) {
    return Option<T mut&>::Value(value);
})CHALI";
    addPointerConversion("toOptional", toOptional);

    /* 标准算法是编译器 intrinsic：这里仅让泛型预处理器吃掉显式 `<T>`，不生成
     * ChaLi 包装函数。限定名约束避免误改用户自己的 `findValue<T>` 等泛型。
     *
     * 算法分两组：`xxxValue` 收 / 返回 `std::core::List<T>`（保持原有的列表语义），
     * `xxxPointer` 收 `T mut&` / `T const&` + 个数、直接在调用者缓冲区上操作
     * （见下面 `module std::algorithms;` 里的那批泛型函数）。 */
    Memory::Map<Memory::String, Generic> algorithmIntrinsics;
    for (const Memory::String &name : { Memory::String("range"), Memory::String("rangeEq") })
    {
        Generic range;
        range.name = name;
        range.params = { Parameter{ "T", "", "std::core::Any" } };
        range.intrinsic = true;
        range.requiredQualifier = "std::core::";
        range.origin.file = "<内建 std::core::" + name + ">";
        range.origin.line = 1u;
        algorithmIntrinsics.emplace(name, std::move(range));
    }
    for (const Memory::String &name : {
             Memory::String("findValue"), Memory::String("sortValue"),
             Memory::String("binarySearchValue"),
             Memory::String("minValue"), Memory::String("maxValue"),
             Memory::String("clampValue"),
             Memory::String("allValue"), Memory::String("anyValue"),
             Memory::String("foldValue"),
             Memory::String("mapValue"), Memory::String("filterValue"),
             Memory::String("reduceValue"),
             Memory::String("sumValue"), Memory::String("countValue") })
    {
        Generic algorithm;
        algorithm.name = name;
        algorithm.params = { Parameter{ "T", "", "std::core::Any" } };
        algorithm.intrinsic = true;
        algorithm.requiredQualifier = "std::algorithms::";
        algorithm.origin.file = "<内建 std::algorithms::" + name + ">";
        algorithm.origin.line = 1u;
        algorithmIntrinsics.emplace(name, std::move(algorithm));
    }

    const Memory::String rangeBuiltinRaw = R"CHALI(
module std::core;

struct __Range : Iterator<int64> {
    int64 start;
    int64 stop;
    int64 step;

    Option<int64> iterator(int64 index) {
        int64 value = self.start + index * self.step;
        if (self.step > 0 && value < self.stop) { return Option<int64>::Value(value); }
        if (self.step < 0 && value > self.stop) { return Option<int64>::Value(value); }
        return Option<int64>::None;
    }
};

__Range range(int64 start, int64 stop) {
    __Range result;
    result.start = start;
    result.stop = stop;
    result.step = 1;
    return result;
}

__Range range(int64 start, int64 stop, int64 step) {
    std::core::assert(step != 0, "range 的步长不能为 0");
    __Range result;
    result.start = start;
    result.stop = stop;
    result.step = step;
    return result;
}

/**
 * @brief 构造包含起点和终点的整数区间。
 * @param start 区间起点。
 * @param stop 包含在区间内的终点。
 * @return 保存独立游标的闭区间值。
 */
__Interval_int64 rangeEq(int64 start, int64 stop) {
    return __interval_int64(start, stop, true);
}

/* fileOpen / fileRead / … 与 processRun / system 已迁到 std/io.chl 与 std/process.chl：
 * 现在要 `import std::io;` / `import std::process;` 才可见。 */


/* 已迁到 std/：见下面的说明注释。 */


module std::algorithms;

/*
 * `choice` / `shuffle` 放这儿不是随手挑的，两条语言约束逼出来的：
 *   ① CHALI **不支持泛型方法**（`T pick<T>(...)` 是语法错；`impl` 只用来实现宿主接口、
 *      不能给 struct 加方法），所以它们没法做成 `Random::choice` 这样的成员；
 *   ② `std::core::Random` 这个名字已经被同名的 **struct** 占着（withSeed / fromEntropy /
 *      nextUInt64 / nextInt / nextFloat64），再开一个同名模块会让
 *      `std::core::Random::withSeed(…)` 被当成"模块函数"而找不到。
 * 放 `std::algorithms` 也顺理成章：`shuffleValue` 本来就是 <algorithm> 里的东西。
 *
 * 两个函数都**不改 Random 的状态**，而是收一个随机数当种子 —— 调用点自己
 * `rng.nextUInt64()` 往前推，这样"取一个 / 洗一副"的顺序仍然完全可复现。
 */

/** 从表里随机取一个元素；空表直接停机（和 `at` 越界一个口径）。 */
T choiceValue<T>(std::core::List<T> items, uint64 seed) {
    std::core::assert(items.len() > 0, "std::algorithms::choiceValue 不能作用在空表上");
    return items.at((int64)(seed % (uint64)items.len()));
}

/**
 * Fisher-Yates 洗牌，返回**打乱后的副本**（原表不动）。
 * 从后往前逐个和前面 `[0, i]` 里的一个随机位置交换；同一个 `seed` 结果可复现。
 */
std::core::List<T> shuffleValue<T>(std::core::List<T> items, uint64 seed) {
    std::core::List<T> result = items;
    uint64 state = seed;
    int64 index = result.len() - 1;
    while (index > 0) {
        state = state * 6364136223846793005 + 1442695040888963407;
        int64 other = (int64)(state % (uint64)(index + 1));
        /* 列表没有 `set` 方法（`set` 还是容器关键字），用下标赋值。 */
        T swap = result[index];
        result[index] = result[other];
        result[other] = swap;
        index = index - 1;
    }
    return result;
}

/**
 * Fisher-Yates 洗牌的**引用形式**：直接在 `data` 里 `count` 个元素上原地打乱。
 * 同一个 `seed` 结果可复现，和 `shuffleValue` 用同一条 LCG。
 */
void shuffleReference<T>(T mut& data, int64 count, uint64 seed) {
    uint64 state = seed;
    int64 index = count - 1;
    while (index > 0) {
        state = state * 6364136223846793005 + 1442695040888963407;
        int64 other = (int64)(state % (uint64)(index + 1));
        T swap = std::core::index(data, index);
        std::core::index(data, index) = std::core::index(data, other);
        std::core::index(data, other) = swap;
        index = index - 1;
    }
}

/** Pair corresponding elements; excess elements in the longer list are ignored. */
std::core::List<(T, U)> zipValue<T, U>(std::core::List<T> left, std::core::List<U> right) {
    std::core::List<(T, U)> result;
    int64 count = left.len();
    if (right.len() < count) { count = right.len(); }
    int64 index = 0;
    while (index < count) {
        T leftValue = left[index];
        U rightValue = right[index];
        result.push((leftValue, rightValue));
        index = index + 1;
    }
    return result;
}

/* ---- 引用形式的算法族 ----
 *
 * 上面这批（`sortValue` / `findValue` / `binarySearchValue` / `foldValue` …）都收 `std::core::List<T>`，
 * `sortValue` 还要复制一份；这里给对照：统一收 `T mut& data, int64 count`，直接在调用者的
 * 缓冲区上读写，不造列表副本、不经过 运行时 容器对象。`&raw[0]` / `raw`（运行时数组会
 * 自动退化）/ `std::memory::Span<T>.data` 都能直接喂进来。
 *
 * 约定和 `std::core::List` 版一致：
 *   * 比较用 `>`（内建数值 / 字符串直接可用；结构体实现 `Greater<bool,T>` 定顺序）；
 *   * 返回下标的找不到时给 -1；返回可选引用的（`minReference` / `maxReference`）空区间给 `None`；
 *   * 调用点先 `import std::algorithms;`，无冲突时可以使用短名并省略类型实参。
 */

/** 原地排序，O(1) 额外空间：`data` 里 `count` 个元素直接排好。
 * 默认是插入排序（稳定）；构建编译器时定义 `SIMPLE_SORT` 改用希尔排序（把间距逐步
 * 缩到 1，同样就地、不额外分配，但**不稳定**）。列表排序 `sortValue` 使用独立的语言稳定归并实现，不受此开关影响。 */
void sortReference<T>(T mut& data, int64 count) {
@@SORT_POINTER_IMPL@@
}

/** 原地反转 `[0, count)`。 */
void reverseReference<T>(T mut& data, int64 count) {
    int64 left = 0;
    int64 right = count - 1;
    while (left < right) {
        T swap = std::core::index(data, left);
        std::core::index(data, left) = std::core::index(data, right);
        std::core::index(data, right) = swap;
        left = left + 1;
        right = right - 1;
    }
}

/** 线性查找，返回下标；找不到返回 -1。 */
int64 findReference<T>(T const& data, int64 count, T value) {
    int64 index = 0;
    while (index < count) {
        if (std::core::index(data, index) == value) { return index; }
        index = index + 1;
    }
    return -1;
}

/** 在按 `>` 升序排好的区间上二分，返回下标；找不到返回 -1。 */
int64 binarySearchReference<T>(T const& data, int64 count, T value) {
    int64 low = 0;
    int64 high = count - 1;
    while (low <= high) {
        int64 middle = low + (high - low) / 2;
        if (std::core::index(data, middle) == value) { return middle; }
        if (std::core::index(data, middle) > value) { high = middle - 1; } else { low = middle + 1; }
    }
    return -1;
}

/** 最小元素的地址；空区间返回 `None`。拿着它可以直接改元素。 */
Option<T mut&> minReference<T>(T mut& data, int64 count) {
    if (count <= 0) { return Option<T mut&>::None; }
    T mut& best = data;
    int64 index = 1;
    while (index < count) {
        if (*best > std::core::index(data, index)) { best = &std::core::index(data, index); }
        index = index + 1;
    }
    return Option<T mut&>::Value(best);
}

/** 最大元素的地址；空区间返回 `None`。 */
Option<T mut&> maxReference<T>(T mut& data, int64 count) {
    if (count <= 0) { return Option<T mut&>::None; }
    T mut& best = data;
    int64 index = 1;
    while (index < count) {
        if (std::core::index(data, index) > *best) { best = &std::core::index(data, index); }
        index = index + 1;
    }
    return Option<T mut&>::Value(best);
}

/** 等于 `value` 的元素个数。 */
int64 countReference<T>(T const& data, int64 count, T value) {
    int64 total = 0;
    int64 index = 0;
    while (index < count) {
        if (std::core::index(data, index) == value) { total = total + 1; }
        index = index + 1;
    }
    return total;
}

/** 求和；空区间直接停机（泛型里没有通用的"零值"可造）。 */
T sumReference<T>(T mut& data, int64 count) {
    std::core::assert(count > 0, "std::algorithms::sumReference 需要至少一个元素");
    T total = std::core::index(data, 0);
    int64 index = 1;
    while (index < count) {
        total = total + std::core::index(data, index);
        index = index + 1;
    }
    return total;
}

module std::core;

/* 已迁到 std/：见下面的说明注释。 */


module std::core;

/* 已迁到 std/：见下面的说明注释。 */


/* 已迁到 std/：见下面的说明注释。 */


module std::core;

int64 __sharedCreate<T>(T value);
T __sharedGet<T>(int64 handle);
void __sharedSet<T>(int64 handle, T value);
T __sharedSwap<T>(int64 handle, T value);

interface Drop { void drop(); };

/* Closeable：把"释放宿主句柄"从"释放结构体自身资源"里分出来。
 *
 * 实现 Closeable 的值在生命周期结束时（出块 / return / `delete` / 停机清理）会自动调用
 * `close()`；用户显式调用过 `close()` 的变量不会再自动调用。Drop 与 Closeable 可以同时
 * 实现：析构顺序是先 `drop()` 再 `close()`。close() 应当对"未打开的句柄"（0）安全，
 * 因为它可能在默认初始化的值上跑。 */
interface Closeable { void close(); };

/*
/* `Text`（字符串工具族）与 `__str*` 宿主签名已迁到 std/text.chl：
 * 现在要 `import std::text;` 才可见。 */

)CHALI";
    /* `SIMPLE_SORT` 是**构建编译器时**的 C++ 宏（和功能裁剪宏一类，整个构建
     * 必须一致定义）：定义了就让排序改用希尔排序，否则用各自的默认实现。两套算法都是纯
     * 文本，这里挑一份塞进引用排序的宏体；运行时 侧 `vecStableSort` 走同一个宏。 */
#ifdef SIMPLE_SORT
    const Memory::String sortReferenceImpl = R"CHALI(    int64 gap = count / 2;
    while (gap > 0) {
        int64 index = gap;
        while (index < count) {
            T key = std::core::index(data, index);
            int64 slot = index;
            while (slot >= gap && std::core::index(data, slot - gap) > key) {
                std::core::index(data, slot) = std::core::index(data, slot - gap);
                slot = slot - gap;
            }
            std::core::index(data, slot) = key;
            index = index + 1;
        }
        gap = gap / 2;
    }
)CHALI";
#else
    const Memory::String sortReferenceImpl = R"CHALI(    int64 index = 1;
    while (index < count) {
        T key = std::core::index(data, index);
        int64 slot = index - 1;
        while (slot >= 0 && std::core::index(data, slot) > key) {
            std::core::index(data, slot + 1) = std::core::index(data, slot);
            slot = slot - 1;
        }
        std::core::index(data, slot + 1) = key;
        index = index + 1;
    }
)CHALI";
#endif
    Memory::String rangeBuiltin = rangeBuiltinRaw;
    // 每种固定宽度整型使用内部状态迭代器；到达闭区间上界后不再递增。
    rangeBuiltin += "\nmodule std::core;\n";
    for (const auto &type : { "int8", "uint8", "int16", "uint16", "int32", "uint32", "int64", "uint64", "char" })
    {
        const Memory::String valueType = type;
        const Memory::String owner = "__Interval_" + valueType;
        rangeBuiltin += "struct " + owner + " : Iterator<" + valueType + "> { " + valueType + " current; " + valueType + " stop; bool inclusive; bool finished; " + "Option<" + valueType + "> iterator(int64 index) { " + "if(self.finished || (!self.inclusive && self.current==self.stop)){self.finished=true;return Option<" + valueType + ">::None;} " + valueType + " value=self.current; if(self.current==self.stop){self.finished=true;} " + "else{self.current=(" + valueType + ")(self.current+1);} return Option<" + valueType + ">::Value(value); } };\n" + owner + " __interval_" + valueType + "(" + valueType + " start," + valueType + " stop,bool inclusive){" + owner + " result;result.current=start;result.stop=stop;result.inclusive=inclusive;result.finished=false;return result;}\n";
    }

    const Memory::String sortReferenceMarker = "@@SORT_POINTER_IMPL@@";
    const std::size_t sortReferenceSlot = rangeBuiltin.find(sortReferenceMarker);
    if (sortReferenceSlot != Memory::String::npos)
        rangeBuiltin.replace(sortReferenceSlot, sortReferenceMarker.size(), sortReferenceImpl);
    rangeBuiltin = "// chali-builtin-range-begin\n" + rangeBuiltin + "\n// chali-builtin-range-end\n";
    result.text += rangeBuiltin;
    preprocess::LineOrigin rangeOrigin;
    rangeOrigin.file = "<内建 range>";
    rangeOrigin.line = 1u;
    const std::size_t rangeLines = static_cast<std::size_t>(
        std::count(rangeBuiltin.begin(), rangeBuiltin.end(), '\n'));
    for (std::size_t line = 0; line < rangeLines; ++line)
    {
        rangeOrigin.line = line + 1;
        result.lineMap.push_back(rangeOrigin);
    }
    Memory::Map<Memory::String, Memory::Set<Memory::String>> noImplementations;
    Memory::Map<Memory::String, Expansion> noExpansions;
    Memory::Vector<Memory::String> noPending;
    if (!rewrite(result.text, algorithmIntrinsics, noImplementations,
                 noExpansions, noPending, error)) return false;
    normalizeGenericSyntax(result.text);
    if (!collect(result.text, result.lineMap, generics, error)) return false;
    if (!validateAliasDeclarations(generics, error)) return false;
    Memory::Set<Memory::String> templateNames;
    for (auto &entry : generics)
    {
        auto &generic = entry.second;
        const auto &sourceName = generic.sourceName.empty() ? generic.name : generic.sourceName;
        if (!templateNames.insert(sourceName).second) session.ambiguousTemplateNames.insert(sourceName);
        const auto key = generic.module.empty() ? generic.sourceName : generic.module + "::" + generic.sourceName;
        const auto switched = operatorSwitches.find(key);
        if (switched != operatorSwitches.end()) generic.operatorSwitches = switched->second;
    }
    auto &implemented = session.implemented;
    collectInterfaces(result.text, implemented);
    auto &made = session.made;
    auto &pending = session.pending;
    if (!rewrite(result.text, generics, implemented, made, pending, error, 0u, {}, false, true)) return false;
    Memory::String generated;
    Memory::Vector<preprocess::LineOrigin> generatedOrigins;
    for (std::size_t index = 0; index < pending.size(); ++index)
    {
        const Expansion expansion = made[pending[index]];
        Memory::String declaration = "module " + (expansion.generic->module.empty() ? Memory::String("__root") : expansion.generic->module) + ";\n" + substitute(expansion);
        if (!rewrite(declaration, generics, implemented, made, pending, error)) return false;
        const Memory::String module = expansion.generic->module.empty() ? Memory::String("__root") : expansion.generic->module;
        generated += "\nmodule " + module + ";\n" + declaration + "\n";
        appendInstantiationOrigins(expansion, declaration, generatedOrigins);
    }
    collectInterfaces(result.text + generated, implemented);
    result.text += generated;
    result.lineMap.insert(result.lineMap.end(), generatedOrigins.begin(), generatedOrigins.end());
    session.emitted = pending.size();
    markContainerTypeArguments(result.text);
    return true;
}

/**
 * @brief 将语义推导出的具体类型展开成普通声明；错误由调用方报告。
 * @param name 模板名称。
 * @param args 推导出的完整类型实参。
 * @param generated 接收本次新增声明。
 * @param error 失败时接收诊断。
 * @param origins 可选输出，按生成文本行序追加原始来源。
 * @return 类型实参和约束有效且展开成功时返回 true。
 */
bool instantiate(const Memory::String &name, const Memory::Vector<Memory::String> &args,
                 Memory::String &generated, Memory::String &error,
                 Memory::Vector<preprocess::LineOrigin> *origins)
{
    Memory::String request = name + "<";
    for (std::size_t index = 0; index < args.size(); ++index)
    {
        if (index) request += ",";
        request += args[index];
    }
    request += ">";
    if (!rewrite(request, session.generics, session.implemented, session.made, session.pending, error)) return false;
    while (session.emitted < session.pending.size())
    {
        const Expansion expansion = session.made.at(session.pending[session.emitted++]);
        auto declaration = "module " + (expansion.generic->module.empty() ? Memory::String("__root") : expansion.generic->module) + ";\n" + substitute(expansion);
        if (!rewrite(declaration, session.generics, session.implemented, session.made, session.pending, error)) return false;
        generated += "\nmodule " + (expansion.generic->module.empty() ? Memory::String("__root") : expansion.generic->module)
                     + ";\n" + declaration + "\n";
        if (origins) appendInstantiationOrigins(expansion, declaration, *origins);
    }
    collectInterfaces(generated, session.implemented);
    markContainerTypeArguments(generated);
    chali_lambda::markClosureTypeArguments(generated);
    return true;
}

/**
 * @brief 在 AST 已登记成员宏接口后，核对泛型约束。
 * @param registered 查询宿主及接口的普通语义登记。
 * @param error 失败时接收具体实参与约束诊断。
 * @return 约束全部满足时返回真。
 */
bool validateRegisteredConstraints(RegisteredInterfaceCheck registered, Memory::String &error)
{
    return validateConstraints(session.made, session.implemented, error, registered)
           && validateConstraints(session.aliasConstraints, session.implemented, error, registered);
}

#include "generic/standard_templates.cpp"

}  // namespace chali_generic
