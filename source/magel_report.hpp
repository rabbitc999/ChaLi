#pragma once
/* magel_report.hpp —— 报错呈现（驱动 / 语言服务共用的一层），中英日三语。
 *
 * 三件事：
 *   ① **Token 显示名**：解析器给的语法错误里写的是内部名（`SEMI` / `IMPL` /
 *      `VARIABLE`…），用户看不懂。这里给每个 Token 配一份「名字(写法)」
 *      （`分号(;)` / `semicolon(;)` / `セミコロン(;)`），打印前换掉。
 *      表与 `magel_token.gram` 一一对应；文法加了终结符要同步改这里。
 *      替换按**整词**做：只认全大写的标识符片段，且必须整段命中表，
 *      所以源码路径、类型名（`std::lang::List`）里的大小写都不会被误伤。
 *      这份是**名字表**不是**消息表**，所以就地三语、不进 `i18n/messages.tsv`
 *      （那边按整条中文消息做键，装不了逐词替换）。
 *   ② **语法错误骨架的三语重建**：`magel_parser.cpp` 是生成文件，文案写死中文，
 *      改不动。这里按中文骨架把「期望哪些 / 实际是什么」拆出来，再按当前语言
 *      重新拼一遍——英文是 `expected one of A / B, found C` 的语序，直接
 *      逐词替换中文串是翻不出来的。拆不动（碰上没见过的文案）就退回①。
 *   ③ **源码片段 + 波浪号**：定位到第几行第几列之后，回显那一行的原文，
 *      并按出错位置的**长度**画波浪号。画的时候按显示宽度算（中文占 2 列、
 *      制表符按 4 列制表位展开），否则列会歪。
 */
#include "allocator/magel_allocator.hpp"
#include "magel_i18n.hpp"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace magel_report
{

/**
 * @brief 一个 Token 名在三种语言下的写法。
 */
struct TokenName
{
    const char *zh;
    const char *en;
    const char *ja;
};

/**
 * @brief 按当前显示语言挑一份写法。
 * @param name 三语名字。
 * @return 当前语言的文本。
 */
inline const char *pick(const TokenName &name)
{
    switch (::magel_i18n::language())
    {
    case ::magel_i18n::Language::En: return name.en;
    case ::magel_i18n::Language::Ja: return name.ja;
    default: return name.zh;
    }
}

/**
 * @brief 内部 Token 名 → 三语显示名。
 * @return 只读表。
 */
inline const std::map<std::string, TokenName> &tokenDisplayNames()
{
    static const std::map<std::string, TokenName> table = {
        /* 类别 Token（没有固定写法，只给名字）。 */
        { "STRING", { "字符串", "string", "文字列" } },
        { "NUMBER", { "数字", "number", "数値" } },
        { "VARIABLE", { "标识符", "identifier", "識別子" } },
        { "FLIT", { "浮点字面量", "float literal", "浮動小数点リテラル" } },
        { "HEXLIT", { "十六进制字面量", "hex literal", "16進リテラル" } },
        { "BINLIT", { "二进制字面量", "binary literal", "2進リテラル" } },
        { "CHRLIT", { "字符字面量", "character literal", "文字リテラル" } },
        { "TOKEN_END", { "文件结尾", "end of file", "ファイル終端" } },
        { "INVALID", { "无效符号", "invalid symbol", "無効な記号" } },
        /* 注解与关键字。 */
        { "ANNOTATION", { "注解声明(@annotation)", "annotation(@annotation)", "注釈(@annotation)" } },
        { "AT", { "at(@)", "at(@)", "アットマーク(@)" } },
        { "STRUCT", { "结构体(struct)", "structure(struct)", "構造体(struct)" } },
        { "ENUM", { "枚举(enum)", "enum(enum)", "列挙(enum)" } },
        { "INT8", { "整数类型(int8)", "integer type(int8)", "整数型(int8)" } },
        { "INT16", { "整数类型(int16)", "integer type(int16)", "整数型(int16)" } },
        { "INT32", { "整数类型(int32)", "integer type(int32)", "整数型(int32)" } },
        { "INT64", { "整数类型(int64)", "integer type(int64)", "整数型(int64)" } },
        { "UINT8", { "整数类型(uint8)", "integer type(uint8)", "整数型(uint8)" } },
        { "UINT16", { "整数类型(uint16)", "integer type(uint16)", "整数型(uint16)" } },
        { "UINT32", { "整数类型(uint32)", "integer type(uint32)", "整数型(uint32)" } },
        { "UINT64", { "整数类型(uint64)", "integer type(uint64)", "整数型(uint64)" } },
        { "FLOAT32", { "浮点类型(float32)", "float type(float32)", "浮動小数点型(float32)" } },
        { "FLOAT64", { "浮点类型(float64)", "float type(float64)", "浮動小数点型(float64)" } },
        { "VOID", { "空类型(void)", "void type(void)", "void 型(void)" } },
        { "BOOL", { "布尔类型(bool)", "boolean type(bool)", "真偽型(bool)" } },
        { "CHAR_T", { "字符类型(char)", "character type(char)", "文字型(char)" } },
        { "TRUE", { "真(true)", "true(true)", "真(true)" } },
        { "FALSE", { "假(false)", "false(false)", "偽(false)" } },
        { "CONST", { "常量(const)", "constant(const)", "定数(const)" } },
        { "MUT", { "可变(mut)", "mutable(mut)", "可変(mut)" } },
        { "NEW", { "新建(new)", "new(new)", "生成(new)" } },
        { "DELETE", { "删除(delete)", "delete(delete)", "削除(delete)" } },
        { "DROP", { "丢弃(drop)", "drop(drop)", "破棄(drop)" } },
        { "STATIC", { "静态(static)", "static(static)", "静的(static)" } },
        { "FN", { "函数(fn)", "function(fn)", "関数(fn)" } },
        { "AUTO", { "自动类型(auto)", "auto type(auto)", "型推論(auto)" } },
        { "COPY", { "拷贝(copy)", "copy(copy)", "コピー(copy)" } },
        { "MOVE", { "移动(move)", "move(move)", "ムーブ(move)" } },
        { "INLINE", { "内联(inline)", "inline(inline)", "インライン(inline)" } },
        { "IMPL", { "实现(impl)", "impl(impl)", "実装(impl)" } },
        { "TYPE_KW", { "类型别名(type)", "type alias(type)", "型別名(type)" } },
        { "ABI_KW", { "外部绑定(abi)", "abi binding(abi)", "ABI バインド(abi)" } },
        { "MODULE", { "模块(module)", "module(module)", "モジュール(module)" } },
        { "IMPORT", { "导入(import)", "import(import)", "インポート(import)" } },
        { "CAST_KW", { "类型转换(cast)", "cast(cast)", "型変換(cast)" } },
        { "SIZEOF", { "取大小(sizeof)", "sizeof(sizeof)", "サイズ取得(sizeof)" } },
        { "IF", { "条件(if)", "if(if)", "条件分岐(if)" } },
        { "ELIF", { "否则条件(elif)", "else-if(elif)", "別条件(elif)" } },
        { "ELSE", { "否则(else)", "else(else)", "それ以外(else)" } },
        { "WHILE", { "当型循环(while)", "while loop(while)", "while ループ(while)" } },
        { "FOR", { "计数循环(for)", "for loop(for)", "for ループ(for)" } },
        { "FOREACH", { "遍历(foreach)", "foreach(foreach)", "反復(foreach)" } },
        { "IN", { "在(in)", "in(in)", "内含(in)" } },
        { "LOOP", { "循环(loop)", "loop(loop)", "ループ(loop)" } },
        { "RETURN", { "返回(return)", "return(return)", "復帰(return)" } },
        { "MATCH", { "匹配(match)", "match(match)", "マッチ(match)" } },
        { "DO", { "执行(do)", "do(do)", "実行(do)" } },
        { "BREAK", { "跳出(break)", "break(break)", "中断(break)" } },
        { "CONTINUE", { "继续(continue)", "continue(continue)", "継続(continue)" } },
        { "WILDCARD", { "通配符(_)", "wildcard(_)", "ワイルドカード(_)" } },
        { "INTERFACE", { "接口(interface)", "interface(interface)", "インターフェース(interface)" } },
        { "TYPE_TAG", { "内部类型标签(__typeTag)", "internal type tag(__typeTag)", "内部型タグ(__typeTag)" } },
        /* 运算符与标点。 */
        { "INCREMENT", { "自增(++)", "increment(++)", "インクリメント(++)" } },
        { "DECREMENT", { "自减(--)", "decrement(--)", "デクリメント(--)" } },
        { "PLUS", { "加号(+)", "plus(+)", "加算(+)" } },
        { "MINUS", { "减号(-)", "minus(-)", "減算(-)" } },
        { "STAR", { "星号(*)", "asterisk(*)", "アスタリスク(*)" } },
        { "SLASH", { "除号(/)", "slash(/)", "除算(/)" } },
        { "PERCENT", { "取余(%)", "percent(%)", "剰余(%)" } },
        { "ASSIGN", { "赋值(=)", "assignment(=)", "代入(=)" } },
        { "ADD_ASSIGN", { "加赋值(+=)", "add-assign(+=)", "加算代入(+=)" } },
        { "SUB_ASSIGN", { "减赋值(-=)", "subtract-assign(-=)", "減算代入(-=)" } },
        { "MUL_ASSIGN", { "乘赋值(*=)", "multiply-assign(*=)", "乗算代入(*=)" } },
        { "DIV_ASSIGN", { "除赋值(/=)", "divide-assign(/=)", "除算代入(/=)" } },
        { "MOD_ASSIGN", { "取余赋值(%=)", "remainder-assign(%=)", "剰余代入(%=)" } },
        { "AND_ASSIGN", { "按位与赋值(&=)", "bitwise-and-assign(&=)", "ビット積代入(&=)" } },
        { "OR_ASSIGN", { "按位或赋值(|=)", "bitwise-or-assign(|=)", "ビット和代入(|=)" } },
        { "XOR_ASSIGN", { "按位异或赋值(^=)", "bitwise-xor-assign(^=)", "ビット排他代入(^=)" } },
        { "SHL_ASSIGN", { "左移赋值(<<=)", "left-shift-assign(<<=)", "左シフト代入(<<=)" } },
        { "SHR_ASSIGN", { "右移赋值(>>=)", "right-shift-assign(>>=)", "右シフト代入(>>=)" } },
        { "EQ", { "等于(==)", "equal(==)", "等価(==)" } },
        { "NE", { "不等于(!=)", "not-equal(!=)", "不等価(!=)" } },
        { "LE", { "小于等于(<=)", "less-or-equal(<=)", "以下(<=)" } },
        { "GE", { "大于等于(>=)", "greater-or-equal(>=)", "以上(>=)" } },
        { "LT", { "小于(<)", "less-than(<)", "小なり(<)" } },
        { "GT", { "大于(>)", "greater-than(>)", "大なり(>)" } },
        { "AND", { "逻辑与(&&)", "logical-and(&&)", "論理積(&&)" } },
        { "OR", { "逻辑或(||)", "logical-or(||)", "論理和(||)" } },
        { "NOT", { "逻辑非(!)", "logical-not(!)", "論理否定(!)" } },
        { "ARROW", { "箭头(->)", "arrow(->)", "アロー(->)" } },
        { "ELLIPSIS", { "省略号(...)", "ellipsis(...)", "省略記号(...)" } },
        { "DOT", { "点(.)", "dot(.)", "ドット(.)" } },
        { "LPAREN", { "左圆括号(()", "left parenthesis(()", "左括弧(()" } },
        { "RPAREN", { "右圆括号())", "right parenthesis())", "右括弧())" } },
        { "LBRACE", { "左花括号({)", "left brace({)", "左波括弧({)" } },
        { "RBRACE", { "右花括号(})", "right brace(})", "右波括弧(})" } },
        { "LBRACKET", { "左方括号([)", "left bracket([)", "左角括弧([)" } },
        { "RBRACKET", { "右方括号(])", "right bracket(])", "右角括弧(])" } },
        { "SEMI", { "分号(;)", "semicolon(;)", "セミコロン(;)" } },
        { "COMMA", { "逗号(,)", "comma(,)", "カンマ(,)" } },
        { "COLON", { "冒号(:)", "colon(:)", "コロン(:)" } },
        { "SCOPE", { "作用域(::)", "scope(::)", "スコープ(::)" } },
        { "AMP", { "按位与(&)", "bitwise-and(&)", "ビット積(&)" } },
        { "PIPE", { "按位或(|)", "bitwise-or(|)", "ビット和(|)" } },
        { "CARET", { "按位异或(^)", "bitwise-xor(^)", "ビット排他(^)" } },
        { "TILDE", { "按位取反(~)", "bitwise-not(~)", "ビット反転(~)" } },
        { "SHL", { "左移(<<)", "left-shift(<<)", "左シフト(<<)" } },
        { "SHR", { "右移(>>)", "right-shift(>>)", "右シフト(>>)" } },
        { "FATARROW", { "胖箭头(=>)", "fat-arrow(=>)", "ファットアロー(=>)" } },
        { "QUESTION", { "问号(?)", "question(?)", "疑問符(?)" } },
    };
    return table;
}

/**
 * @brief 查一个内部 Token 名在当前语言下的写法。
 * @param internal 内部名（如 `SEMI`）。
 * @return 显示名；表里没有就返回原样。
 */
inline std::string tokenDisplayName(const std::string &internal)
{
    const auto found = tokenDisplayNames().find(internal);
    return (found != tokenDisplayNames().end()) ? pick(found->second) : internal;
}

/**
 * @brief 把消息里的内部 Token 名换成当前语言下的「名字(写法)」。
 * @param message 解析器 / 语义检查给出的原始消息。
 * @return 换过名字的消息；没有命中就原样返回。
 */
inline Memory::String humanizeMessage(const Memory::String &message)
{
    const std::string text(message.data(), message.size());
    const std::map<std::string, TokenName> &table = tokenDisplayNames();
    std::string out;
    out.reserve(text.size());
    for (std::size_t at = 0u; at < text.size();)
    {
        const unsigned char lead = static_cast<unsigned char>(text[at]);
        /* 只从 ASCII 大写字母起词：UTF-8 的中文首字节 >= 0x80，不会误判。 */
        if (lead < 'A' || lead > 'Z')
        {
            out.push_back(text[at]);
            ++at;
            continue;
        }
        std::size_t end = at;
        while (end < text.size())
        {
            const unsigned char c = static_cast<unsigned char>(text[end]);
            if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
            {
                ++end;
                continue;
            }
            break;
        }
        const std::string word = text.substr(at, end - at);
        const auto found = table.find(word);
        out += (found != table.end()) ? pick(found->second) : word;
        at = end;
    }
    return Memory::String(out.data(), out.size());
}

/**
 * @brief 剥掉解析器消息自带的「语法错误：」前缀。
 *
 * 驱动打印时已经写了「语法错误：<文件> 第 X 行第 Y 列：」，消息里再来一次就成了
 * 「语法错误：…：语法错误：期望…」。
 * @param message 原始消息。
 * @return 剥掉前缀后的消息（没有该前缀就原样返回）。
 */
inline Memory::String stripSyntaxPrefix(const Memory::String &message)
{
    /* 解析器是生成文件、文案写死中文；这里三种前缀都认，将来换语言也剥得掉。 */
    for (const char *prefix : { "语法错误：", "Syntax error: ", "構文エラー：" })
    {
        const std::size_t length = std::strlen(prefix);
        if (message.size() < length) continue;
        if (message.compare(0u, length, prefix) == 0) return message.substr(length);
    }
    return message;
}

/**
 * @brief 按当前语言把「期望 A / B 之一，实际是 C」重拼一遍。
 *
 * 语序是跟着语言走的（英文的 `one of` 在前面、中文的「之一」在后面），所以整句重建，
 * 不做逐词替换。
 * @param expected 期望的 Token 显示名（可能只列了前几个）。
 * @param total 解析器统计到的期望总数（不超过 `expected` 大小时按后者算）。
 * @param actual 实际看到的 Token 显示名。
 * @return 一条完整说明。
 */
inline std::string composeExpectation(const std::vector<std::string> &expected,
                                      std::size_t total, const std::string &actual)
{
    if (expected.empty()) return std::string();
    const bool multiple = (total > 1u) || (expected.size() > 1u);
    const bool truncated = total > expected.size();
    const std::string count = truncated ? std::to_string(total) : std::string();
    std::string listed;
    for (std::size_t at = 0u; at < expected.size(); ++at)
    {
        listed += (at == 0u) ? std::string() : " / ";
        listed += expected[at];
    }
    switch (::magel_i18n::language())
    {
    case ::magel_i18n::Language::En:
    {
        std::string out = "expected ";
        if (multiple) out += "one of ";
        out += listed;
        if (truncated) out += " (" + count + " candidates)";
        out += ", found " + actual;
        return out;
    }
    case ::magel_i18n::Language::Ja:
    {
        std::string out = listed;
        if (truncated) out += " 他 " + count + " 件";
        /* 期望太多只列前几个时，中文是「等 N 个之一」，日语放前面更顺。 */
        out += multiple ? "のいずれかを期待しましたが、実際は " : "を期待しましたが、実際は ";
        out += actual + " です";
        return out;
    }
    default:
    {
        std::string out = "期望 " + listed;
        /* 与生成的解析器文案保持一致：截断时「个」后不空格，不截断时「 之一」带空格。 */
        if (truncated)
            out += " 等 " + count + " 个之一";
        else if (multiple)
            out += " 之一";
        out += "，实际是 " + actual;
        return out;
    }
    }
}

/**
 * @brief 把解析器的语法错误消息翻成当前语言。
 *
 * 走两步：先剥「语法错误：」，再按中文骨架拆出「期望 / 实际」重建；拆不出来
 * （遇到没见过的文案）就只换 Token 名，至少保证不显示内部名。
 * @param message 解析器给的原始消息。
 * @return 当前语言的说明。
 */
inline Memory::String localizeSyntaxMessage(const Memory::String &message)
{
    const Memory::String body = stripSyntaxPrefix(message);
    const std::string text(body.data(), body.size());
    /* 解析器在无可用动作时的兜底文案（`kExpectedFallback`）。 */
    if (text == "该状态下无可用动作")
    {
        const char *fallback = "该状态下无可用动作";
        switch (::magel_i18n::language())
        {
        case ::magel_i18n::Language::En: fallback = "no valid action in this state"; break;
        case ::magel_i18n::Language::Ja: fallback = "この状態では有効な動作がありません"; break;
        default: break;
        }
        return Memory::String(fallback);
    }
    const std::string expectPrefix = "期望 ";
    const std::string foundSeparator = "，实际是 ";
    if (text.compare(0u, expectPrefix.size(), expectPrefix) != 0u) return humanizeMessage(body);
    const auto split = text.rfind(foundSeparator);
    if (split == std::string::npos) return humanizeMessage(body);
    std::string expected = text.substr(expectPrefix.size(),
                                       split - expectPrefix.size());
    const std::string actual = text.substr(split + foundSeparator.size());

    /* 「之一」：只有一个期望时解析器不写；截断时紧贴「个」写，不截断时前面带空格。 */
    for (const char *oneOf : { " 之一", "之一" })
    {
        const std::size_t length = std::strlen(oneOf);
        if (expected.size() >= length && expected.compare(expected.size() - length, length, oneOf) == 0u)
        {
            expected.erase(expected.size() - length);
            break;
        }
    }
    /* 「 等 N 个」：解析器只列前 4 个，总数另报。 */
    std::size_t total = 0u;
    const std::string morePrefix = " 等 ";
    const std::string moreSuffix = " 个";
    const auto more = expected.find(morePrefix);
    if (more != std::string::npos)
    {
        const auto digits = expected.find_first_not_of("0123456789", more + morePrefix.size());
        if (digits != std::string::npos && digits > more + morePrefix.size() && expected.compare(digits, moreSuffix.size(), moreSuffix) == 0u)
        {
            total = static_cast<std::size_t>(
                std::stoull(expected.substr(more + morePrefix.size(),
                                            digits - (more + morePrefix.size()))));
            expected = expected.substr(0u, more) + expected.substr(digits + moreSuffix.size());
        }
    }

    std::vector<std::string> names;
    for (std::size_t at = 0u;;)
    {
        const auto next = expected.find(" / ", at);
        const std::string one = (next == std::string::npos) ? expected.substr(at) : expected.substr(at, next - at);
        if (one.empty()) break;
        names.push_back(tokenDisplayName(one));
        if (next == std::string::npos) break;
        at = next + 3u;
    }
    if (names.empty()) return humanizeMessage(body);
    if (total < names.size()) total = names.size();
    const std::string composed =
        composeExpectation(names, total, tokenDisplayName(actual));
    return Memory::String(composed.data(), composed.size());
}

/**
 * @brief 一个字符的显示宽度（中文等宽字符算 2）。
 * @param text UTF-8 文本。
 * @return 列数。
 */
inline std::size_t displayWidth(const std::string &text)
{
    std::size_t width = 0u;
    for (std::size_t at = 0u; at < text.size();)
    {
        const unsigned char lead = static_cast<unsigned char>(text[at]);
        std::size_t length = 1u;
        if (lead >= 0xF0u)
            length = 4u;
        else if (lead >= 0xE0u)
            length = 3u;
        else if (lead >= 0xC0u)
            length = 2u;
        if (length >= 2u)
            width += 2u; /* 非 ASCII 一律按双宽算。 */
        else if (lead >= 0x20u && lead < 0x7Fu)
            width += 1u;
        else if (lead == '\t')
            width += 4u;
        at += length;
    }
    return width;
}

/**
 * @brief 把制表符按 4 列制表位展开成空格（画波浪号前先铺平，否则列会歪）。
 */
inline std::string expandTabs(const std::string &text)
{
    std::string out;
    out.reserve(text.size());
    std::size_t column = 0u;
    for (const char character : text)
    {
        if (character != '\t')
        {
            out.push_back(character);
            column += displayWidth(std::string(1, character));
            continue;
        }
        const std::size_t pad = 4u - (column % 4u);
        out.append(pad, ' ');
        column += pad;
    }
    return out;
}

/**
 * @brief `renderSnippet` 的 length 取这个值：**整行都算重点**。
 *
 * 语义诊断经常只知道"第几行"（拿不到精确列），这时只标一格反而像指着别处，
 * 所以整行铺满波浪号，告诉用户"这一行整句都在报错范围内"。
 */
inline constexpr std::size_t kSnippetWholeLine = static_cast<std::size_t>(-1);

inline bool isWordByte(unsigned char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c >= 0x80u;
}

/**
 * @brief 自动定位时，把落点间接到「这一行第一个词」上。
 *
 * 语义诊断给的是**节点被创建时**的列，而节点是归约完才造的 —— 语句级的节点
 * 落点就是它在行的最后那个 token（`;` / `}`）。指着分号说"这里错了"等于没说，
 * 所以落到实处是标点 / 空白时，改指本行第一个词（语句开头），比指成分号好认。
 * @param raw 未经 Tab 展开的整行原文。
 * @param start 原先的 0 基落点。
 * @return 调整后的 0 基落点。
 */
inline std::size_t anchorColumn(const std::string &raw, std::size_t start)
{
    if (start < raw.size() && isWordByte(static_cast<unsigned char>(raw[start]))) return start;
    for (std::size_t at = 0u; at < raw.size(); ++at)
        if (isWordByte(static_cast<unsigned char>(raw[at]))) return at;
    return start;
}

/**
 * @brief 从 raw 的 start 位置起，取出那个「词」的长度。
 *
 * 报错有时只给得出起始列（比如降级期诊断），把整条标识符画出来比只画一格好认；
 * 指到空白 / 运算符上时退成一个字符。
 * @param raw 未经 Tab 展开的整行原文。
 * @param start 0 基起始下标。
 * @return 词的长度，至少 1。
 */
inline std::size_t wordExtentAt(const std::string &raw, std::size_t start)
{
    std::size_t end = start;
    while (end < raw.size() && isWordByte(static_cast<unsigned char>(raw[end]))) ++end;
    return end > start ? end - start : 1u;
}

/**
 * @brief 读一个源文件的全部行并缓存（同一轮里报好几处错时只读一次）。
 * @param file 源文件路径。
 * @return 按行存好的文本；读不到就是空表。
 */
inline const std::vector<std::string> &cachedFileLines(const Memory::String &file)
{
    static std::map<std::string, std::vector<std::string>> cache;
    if (file.empty()) return cache[std::string()];
    const std::string path(file.data(), file.size());
    auto found = cache.find(path);
    if (found == cache.end())
    {
        std::vector<std::string> lines;
        std::ifstream input(path);
        std::string one;
        while (std::getline(input, one))
        {
            if (!one.empty() && one.back() == '\r') one.pop_back();
            lines.push_back(one);
        }
        found = cache.emplace(path, std::move(lines)).first;
    }
    return found->second;
}

/**
 * @brief 这一行能不能从磁盘上读回来 —— 决定**要不要补"第 N 行"**。
 * 合成文件（`<内建 lang>` 之类）与前处理行映射失效时读不到，这时给行号等于指着空气。
 * @param file 源文件路径。
 * @param line 行号（1 基）。
 * @return 读得到返回 true。
 */
inline bool hasSourceLine(const Memory::String &file, std::size_t line)
{
    return line != 0u && line <= cachedFileLines(file).size();
}

/**
 * @brief 自动定位用的实际落点列号（1 基）—— 调整过的，供**打印 "第 N 列"** 用。
 *
 * 调整必须在这里做而不能只在画图时做：否则文案写"第 15 列"、波浪号却画在"第 6 列"
 * 上，人看到的两处说的不是一回事（编辑器里的划波浪线也按列号走）。
 * @param file 源文件路径。
 * @param line 行号（1 基）。
 * @param column 原始落点列号（1 基）。
 * @return 调整后的列号；拿不到源文时原样返回。
 */
inline std::size_t anchoredColumn(const Memory::String &file, std::size_t line, std::size_t column)
{
    if (column == 0u) return 0u;
    const std::vector<std::string> &lines = cachedFileLines(file);
    if (line == 0u || line > lines.size()) return column;
    return anchorColumn(lines[line - 1u], column - 1u) + 1u;
}

/**
 * @brief 回显出错那一行并在下面画波浪号。
 *
 * 形如（行号栏右对齐，与行号位数对齐）：
 * ```
 *    |
 *  2 | extern impl Value : Some = binding;
 *    |        ~~~~
 * ```
 * @param file 源文件路径（读不到就返回空串，调用方跳过）。
 * @param line 行号（1 基）。
 * @param column 列号（1 基）；0 表示不知道列，从头画起。
 * @param length 出错位置的字符数；0 = 按列的那个「词」自动定；`kSnippetWholeLine` = 整行。
 * @return 三行文本；拿不到源文时返回空串。
 */
inline Memory::String renderSnippet(const Memory::String &file, std::size_t line,
                                    std::size_t column, std::size_t length = 0u)
{
    if (file.empty() || line == 0u) return Memory::String();
    const std::vector<std::string> &lines = cachedFileLines(file);
    if (line > lines.size()) return Memory::String();
    const std::string &raw = lines[line - 1u];
    const std::string shown = expandTabs(raw);

    const bool autoWidth = (length == 0u);
    std::size_t start = (column > 0u) ? (column - 1u) : 0u;
    /* 只有"自动定位置"（length == 0）才挪落点：语法错误给的是精确的 token 与长度，
     * 它点在分号 / 右花括号上是有意义的（错的就是那儿），不能被这句改写。 */
    if (autoWidth && column > 0u) start = anchorColumn(raw, start);
    const std::size_t indent =
        displayWidth(expandTabs(start < raw.size() ? raw.substr(0u, start) : raw));
    std::size_t take = length;
    if (take == 0u)
        take = wordExtentAt(raw, start);
    else if (take == kSnippetWholeLine)
        take = raw.size();
    const std::string marked =
        (start < raw.size()) ? raw.substr(start, take) : std::string();
    std::size_t width = displayWidth(expandTabs(marked));
    if (width == 0u) width = 1u;

    const std::string number = std::to_string(line);
    const std::string bar(number.size() + 2u, ' '); /* 行号栏：与下面 "|" 对齐。 */
    Memory::String out;
    out += Memory::String((bar + "|\n").data(), (bar + "|\n").size());
    out += Memory::String((" " + number + " | " + shown + "\n").data(),
                          (" " + number + " | " + shown + "\n").size());
    const std::string underline = bar + "| " + std::string(indent, ' ') + std::string(width, '~');
    out += Memory::String((underline + "\n").data(), (underline + "\n").size());
    return out;
}

}  // namespace magel_report
