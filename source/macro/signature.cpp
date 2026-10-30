#include "signature.hpp"

namespace chali_macro
{
/**
 * @brief 验证一个完整的普通标识符。
 * @param text 已去除首尾空白的名称。
 * @return 整个文本均属于该名称时返回真。
 */
static bool signatureIdentifier(const Memory::String &text)
{
    if (text.empty() || !chali_syntax::identStart(text[0])) return false;
    for (const char ch : text)
        if (!chali_syntax::identPart(ch)) return false;
    return true;
}

/**
 * @brief 读取声明头的可编辑结构，保留空组及多组参数的顺序。
 * @param source 声明行。
 * @param signature 接收结构。
 * @param error 接收失败原因。
 * @return 声明头完整有效时返回真。
 */
bool readSignature(const Memory::String &source, Signature &signature, Memory::String &error)
{
    signature = {};
    error.clear();
    std::size_t at = 0;
    chali_syntax::spaces(source, at);
    const auto keyword = source.compare(at, 8, "$replace") == 0 ? Memory::String("$replace") : Memory::String("$macro");
    if (source.compare(at, keyword.size(), keyword) != 0)
    {
        error = "需要宏定义关键字";
        return false;
    }
    signature.replacement = keyword == "$replace";
    at += keyword.size();
    if (at >= source.size() || chali_syntax::identPart(source[at]))
    {
        error = "宏关键字后需要名称";
        return false;
    }
    chali_syntax::spaces(source, at);
    const auto begin = at;
    if (at >= source.size() || !chali_syntax::identStart(source[at]))
    {
        error = "宏名称必须是标识符";
        return false;
    }
    while (at < source.size() && chali_syntax::identPart(source[at])) ++at;
    signature.name = source.substr(begin, at - begin);
    chali_syntax::spaces(source, at);
    if (at < source.size() && source[at] == '<')
    {
        std::size_t end = at;
        if (!chali_syntax::groupArguments(source, at, signature.generics, end) || signature.generics.empty())
        {
            error = "宏类型形参列表不完整";
            return false;
        }
        for (const auto &item : signature.generics)
        {
            const auto colon = item.find(':');
            const auto source = colon == Memory::String::npos ? Memory::String() : chali_syntax::trim(item.substr(colon + 1));
            if (!signatureIdentifier(chali_syntax::trim(item.substr(0, colon))) || (colon != Memory::String::npos && source != "#res" && !signatureIdentifier(source)))
            {
                error = "宏类型参数应写为 T 或 T:参数名";
                return false;
            }
        }
        at = end;
        chali_syntax::spaces(source, at);
    }
    Memory::Set<Memory::String> names;
    while (at < source.size() && (source[at] == '(' || source[at] == '[' || source[at] == '{'))
    {
        SignatureGroup group;
        group.opening = source[at];
        Memory::Vector<Memory::String> parameters;
        std::size_t end = at;
        if (!chali_syntax::groupArguments(source, at, parameters, end))
        {
            error = "宏参数组不完整";
            return false;
        }
        for (std::size_t index = 0; index < parameters.size(); ++index)
        {
            auto parameter = chali_syntax::trim(parameters[index]);
            Memory::String category;
            const auto categoryAt = parameter.find(':');
            if (categoryAt != Memory::String::npos)
            {
                category = chali_syntax::trim(parameter.substr(categoryAt + 1));
                parameter = chali_syntax::trim(parameter.substr(0, categoryAt));
                if (category != "value" && category != "expr" && category != "ident" && category != "type" && category != "stmt" && category != "block" && category != "pattern")
                {
                    error = "不支持的宏形参类别：" + category;
                    return false;
                }
            }
            const bool variadic = parameter.compare(0, 2, "..") == 0;
            if (variadic) parameter = chali_syntax::trim(parameter.substr(parameter.compare(0, 3, "...") == 0 ? 3 : 2));
            Memory::String reference;
            if (parameter.compare(0, 3, "mut") == 0)
            {
                const auto rest = chali_syntax::trim(parameter.substr(3));
                if (!rest.empty() && rest[0] == '&')
                {
                    reference = "mut &";
                    parameter = chali_syntax::trim(rest.substr(1));
                }
            }
            else if (!parameter.empty() && parameter[0] == '&')
            {
                reference = "&";
                parameter = chali_syntax::trim(parameter.substr(1));
            }
            if (!signatureIdentifier(parameter) || !names.insert(parameter).second || (variadic && index + 1 != parameters.size()))
            {
                error = "宏形参必须是互不重复的标识符，变长形参放在组末尾";
                return false;
            }
            if (!category.empty() && !reference.empty() && category != "value")
            {
                error = "引用参数只能标注 value";
                return false;
            }
            if (variadic)
            {
                group.rest = parameter;
                group.restCategory = category;
            }
            else
            {
                group.params.push_back(reference + parameter);
                group.categories.push_back(category);
            }
        }
        signature.groups.push_back(std::move(group));
        at = end;
        chali_syntax::spaces(source, at);
    }
    if (source.compare(at, 2, "->") == 0)
    {
        at += 2;
        chali_syntax::spaces(source, at);
        const auto begin = at;
        while (at < source.size() && chali_syntax::identPart(source[at])) ++at;
        signature.result = source.substr(begin, at - begin);
        if (signature.result != "expr" && signature.result != "stmt" && signature.result != "item")
        {
            error = "不支持的宏结果类别：" + signature.result;
            return false;
        }
        chali_syntax::spaces(source, at);
    }
    if (!signature.replacement && at != source.size())
    {
        error = "宏头不允许额外内容或自动括号标注";
        return false;
    }
    if (signature.replacement && at == source.size())
    {
        error = "单行宏缺少正文";
        return false;
    }
    return true;
}
}  // namespace chali_macro
