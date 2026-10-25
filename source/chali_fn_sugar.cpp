/* chali_fn_sugar.cpp —— fn 前缀与返回类型后置写法的文本归一化。
 *
 * 本趟扫描在泛型单态化之前运行，只做等价改写，不引入新的文法产生式，
 * 也不产生 AST 节点：改写后的文本仍由既有产生式解析。
 */
#include "chali_fn_sugar.hpp"

#include <algorithm>
#include <cctype>

#include "chali_generic.hpp"
#include "chali_lambda.hpp"

namespace chali_fn_sugar
{
namespace
{
using chali_generic::balanced;
using chali_generic::identPart;
using chali_generic::identStart;
using chali_generic::skipSpace;
using chali_generic::splitTop;
using chali_generic::trim;
using chali_generic::wordAt;

/**
 * @brief 一次文本替换。
 */
struct Edit
{
    std::size_t begin;
    std::size_t end;
    Memory::String text;
};

// 前向声明：`fn(...) -> fn(...)` 的嵌套需要两者互相调用。
bool rewriteSignature(const Memory::String &mask, std::size_t at, Memory::String &out, std::size_t &end);

/**
 * @brief 判断单词是否为 lambda 的捕获标记。
 * @param word 待判断的单词。
 * @return const、mut、copy 或 move 返回 true。
 */
bool captureWord(const Memory::String &word)
{
    return word == "const" || word == "mut" || word == "copy" || word == "move";
}

/**
 * @brief 读取一个完整标识符。
 * @param text 待扫描的源码。
 * @param at 输入起点，成功时推进到标识符之后。
 * @param word 成功时接收标识符拼写。
 * @return 当前位置是标识符首字符时返回 true。
 */
bool readWord(const Memory::String &text, std::size_t &at, Memory::String &word)
{
    if (at >= text.size() || !identStart(text[at])) return false;
    auto end = at;
    while (end < text.size() && identPart(text[end])) ++end;
    word = text.substr(at, end - at);
    at = end;
    return true;
}

/**
 * @brief 类型拼写作为返回类型需要包围时补上括号。
 * @param type 已读取的类型文本。
 * @return 已整体带括号或无需包围时返回原文，否则返回带括号的拼写。
 */
Memory::String bracket(const Memory::String &type)
{
    if (type.empty()) return type;
    if (type.front() == '(')
    {
        std::size_t close = 0;
        if (balanced(type, 0, '(', ')', close) && close + 1u == type.size()) return type;
    }
    bool plain = true;
    for (std::size_t at = 0; at < type.size(); ++at)
    {
        if (std::isspace(static_cast<unsigned char>(type[at]))) plain = false;
        if (wordAt(type, at, "fn")) plain = false;
    }
    return plain ? type : "(" + type + ")";
}

/**
 * @brief 删除类型后面的形参名，保留引用与数组等后缀。
 * @param text 单个形参的原文。
 * @return 只剩类型时返回原文，否则返回去掉形参名的文本。
 */
Memory::String stripParameterName(const Memory::String &text)
{
    auto end = text.size();
    while (end != 0 && std::isspace(static_cast<unsigned char>(text[end - 1u]))) --end;
    if (end == 0) return text;
    auto start = end;
    while (start != 0 && !std::isspace(static_cast<unsigned char>(text[start - 1u]))) --start;
    if (start == 0) return text;
    const auto last = text.substr(start, end - start);
    for (const char value : last)
        if (!identPart(value)) return text;
    return trim(text.substr(0, start));
}

/**
 * @brief 去掉函数类型形参表里的形参名。
 * @param params 含外层括号的形参表原文。
 * @return 只保留类型的形参表；函数类型的形参按既有规则不写名称。
 */
Memory::String stripParameterNames(const Memory::String &params)
{
    if (params.size() < 2u) return params;
    const auto inner = params.substr(1u, params.size() - 2u);
    if (trim(inner).empty()) return params;
    Memory::String out = "(";
    bool first = true;
    for (const auto &item : chali_generic::splitTop(inner))
    {
        if (!first) out += ", ";
        first = false;
        out += stripParameterName(trim(item));
    }
    return out + ")";
}

/**
 * @brief 去掉文本中前置函数类型 `T fn(形参)` 的形参名。
 * @param text 类型原文，例如 `T fn(T value)`。
 * @return 形参表只剩类型的文本。
 */
Memory::String stripNamesInside(const Memory::String &text)
{
    Memory::String out = text;
    for (std::size_t at = 0; at + 2u <= out.size(); ++at)
    {
        if (!wordAt(out, at, "fn")) continue;
        std::size_t open = at + 2u;
        skipSpace(out, open);
        if (open >= out.size() || out[open] != '(') continue;
        std::size_t close = 0;
        if (!balanced(out, open, '(', ')', close)) continue;
        const auto params = stripParameterNames(out.substr(open, close + 1u - open));
        out.replace(open, close + 1u - open, params);
        at = open + params.size() - 1u;
    }
    return out;
}

/**
 * @brief 读取一个类型表达式，含限定名、容器实参、数组、引用与其中的函数类型。
 * @param mask 遮蔽注释与字面量后的源码。
 * @param at 输入起点，成功时推进到类型末尾。
 * @param type 成功时接收去除首尾空白的类型拼写。
 * @return 读取到非空类型时返回 true。
 */
bool scanType(const Memory::String &mask, std::size_t &at, Memory::String &type)
{
    const std::size_t begin = at;
    Memory::String word;
    if (!readWord(mask, at, word)) return false;
    for (;;)
    {
        std::size_t scan = at;
        skipSpace(mask, scan);
        if (scan >= mask.size()) break;
        const char value = mask[scan];
        if (value == ':' && scan + 1u < mask.size() && mask[scan + 1u] == ':')
        {
            std::size_t after = scan + 2u;
            skipSpace(mask, after);
            Memory::String next;
            if (!readWord(mask, after, next)) break;
            at = after;
            continue;
        }
        if (value == '<')
        {
            std::size_t close = 0;
            if (!balanced(mask, scan, '<', '>', close)) break;
            at = close + 1u;
            continue;
        }
        if (value == '[')
        {
            std::size_t close = 0;
            if (!balanced(mask, scan, '[', ']', close)) break;
            at = close + 1u;
            continue;
        }
        if (value == '&')
        {
            at = scan + 1u;
            continue;
        }
        // 引用限定词与函数类型跟在类型之后；其余标识符是声明的变量名，不是类型的一部分。
        if (scan > at && (wordAt(mask, scan, "mut") || wordAt(mask, scan, "const")))
        {
            std::size_t after = scan + (wordAt(mask, scan, "mut") ? 3u : 5u);
            skipSpace(mask, after);
            if (after < mask.size() && mask[after] == '&')
            {
                at = after + 1u;
                continue;
            }
            break;
        }
        if (scan > at && wordAt(mask, scan, "fn"))
        {
            std::size_t after = scan + 2u;
            skipSpace(mask, after);
            std::size_t close = 0;
            if (after < mask.size() && mask[after] == '(' && balanced(mask, after, '(', ')', close))
            {
                at = close + 1u;
                continue;
            }
        }
        break;
    }
    type = trim(mask.substr(begin, at - begin));
    return !type.empty();
}

/**
 * @brief 读取返回类型，最内层的后置函数类型先完成归一化。
 * @param mask 遮蔽注释与字面量后的源码。
 * @param at 输入起点，成功时推进到返回类型末尾。
 * @param type 成功时接收可用的返回类型拼写。
 * @return 返回类型完整时返回 true。
 */
bool scanReturn(const Memory::String &mask, std::size_t &at, Memory::String &type)
{
    skipSpace(mask, at);
    if (at >= mask.size()) return false;
    if (wordAt(mask, at, "fn"))
    {
        Memory::String inner;
        std::size_t innerEnd = 0;
        if (!rewriteSignature(mask, at, inner, innerEnd)) return false;
        type = bracket(inner);
        at = innerEnd;
        return true;
    }
    if (mask[at] == '(')
    {
        std::size_t close = 0;
        if (!balanced(mask, at, '(', ')', close)) return false;
        // 括号里既可能是后置写法，也可能是既有的 `T fn(形参)`；后者同样去掉形参名。
        type = "(" + stripNamesInside(mask.substr(at + 1u, close - at - 1u)) + ")";
        at = close + 1u;
        return true;
    }
    return scanType(mask, at, type);
}

/**
 * @brief 把 `fn(参数) -> 返回类型` 改写成既有的 `返回类型 fn(参数)`。
 * @param mask 遮蔽注释与字面量后的源码。
 * @param at fn 关键字的位置。
 * @param out 成功时接收改写后的函数类型。
 * @param end 成功时接收改写区间的结束位置。
 * @return 当前位置是函数类型写法时返回 true。
 */
bool rewriteSignature(const Memory::String &mask, std::size_t at, Memory::String &out, std::size_t &end)
{
    std::size_t open = at + 2u;
    skipSpace(mask, open);
    if (open >= mask.size() || mask[open] != '(') return false;
    std::size_t paramsEnd = 0;
    if (!balanced(mask, open, '(', ')', paramsEnd)) return false;
    const auto params = stripParameterNames(mask.substr(open, paramsEnd + 1u - open));
    std::size_t arrow = paramsEnd + 1u;
    skipSpace(mask, arrow);
    if (arrow + 1u >= mask.size() || mask[arrow] != '-' || mask[arrow + 1u] != '>') return false;
    std::size_t retAt = arrow + 2u;
    Memory::String result;
    if (!scanReturn(mask, retAt, result)) return false;
    out = bracket(result) + " fn" + params;
    end = retAt;
    return true;
}

/**
 * @brief 把 `fn 名<类型参数>(参数) -> 返回类型` 改写成既有的前置写法。
 * @param mask 遮蔽注释与字面量后的源码。
 * @param at fn 关键字的位置。
 * @param out 成功时接收改写后的声明头。
 * @param end 成功时接收改写区间的结束位置。
 * @return 当前位置是具名声明写法时返回 true。
 */
bool rewriteDeclaration(const Memory::String &mask, std::size_t at, Memory::String &out, std::size_t &end)
{
    std::size_t scan = at + 2u;
    skipSpace(mask, scan);
    Memory::String name;
    if (!readWord(mask, scan, name)) return false;
    Memory::String types;
    std::size_t cursor = scan;
    skipSpace(mask, cursor);
    if (cursor < mask.size() && mask[cursor] == '<')
    {
        std::size_t close = 0;
        if (!balanced(mask, cursor, '<', '>', close)) return false;
        types = mask.substr(cursor, close + 1u - cursor);
        cursor = close + 1u;
    }
    skipSpace(mask, cursor);
    if (cursor >= mask.size() || mask[cursor] != '(') return false;
    std::size_t paramsEnd = 0;
    if (!balanced(mask, cursor, '(', ')', paramsEnd)) return false;
    const auto params = mask.substr(cursor, paramsEnd + 1u - cursor);
    const auto head = name + types + params;
    std::size_t arrow = paramsEnd + 1u;
    skipSpace(mask, arrow);
    if (arrow + 1u < mask.size() && mask[arrow] == '-' && mask[arrow + 1u] == '>')
    {
        std::size_t retAt = arrow + 2u;
        Memory::String result;
        if (!scanReturn(mask, retAt, result)) return false;
        out = bracket(result) + " " + head;
        end = retAt;
        return true;
    }
    out = "void " + head;
    end = paramsEnd + 1u;
    return true;
}

}  // namespace

void normalize(Memory::String &source)
{
    const auto mask = chali_lambda::codeMask(source);
    Memory::Vector<Edit> edits;
    for (std::size_t at = 0; at + 2u <= mask.size(); ++at)
    {
        if (!wordAt(mask, at, "fn")) continue;
        std::size_t next = at + 2u;
        skipSpace(mask, next);
        if (next >= mask.size()) continue;
        // `fn<类型参数>` 是泛型闭包头，交给既有的闭包通道处理。
        if (mask[next] == '<') continue;
        if (identStart(mask[next]))
        {
            Memory::String word;
            std::size_t probe = next;
            readWord(mask, probe, word);
            if (captureWord(word)) continue;
            Memory::String text;
            std::size_t stop = 0;
            if (rewriteDeclaration(mask, at, text, stop) && stop > at)
            {
                edits.push_back(Edit{ at, stop, text });
                at = stop - 1u;
            }
            continue;
        }
        if (mask[next] == '(')
        {
            Memory::String text;
            std::size_t stop = 0;
            if (rewriteSignature(mask, at, text, stop) && stop > at)
            {
                // `fn(参数) -> 返回类型 { 函数体 }` 是 0.12.0 已移除的 lambda 箭头返回类型，保持拒绝。
                std::size_t after = stop;
                skipSpace(mask, after);
                if (after >= mask.size() || mask[after] != '{')
                {
                    edits.push_back(Edit{ at, stop, text });
                    at = stop - 1u;
                }
            }
        }
    }
    for (auto edit = edits.rbegin(); edit != edits.rend(); ++edit)
    {
        Memory::String text = edit->text;
        text.append(static_cast<std::size_t>(
                        std::count(source.begin() + static_cast<std::ptrdiff_t>(edit->begin),
                                   source.begin() + static_cast<std::ptrdiff_t>(edit->end), '\n')),
                    '\n');
        source.replace(edit->begin, edit->end - edit->begin, text);
    }
}

}  // namespace chali_fn_sugar
