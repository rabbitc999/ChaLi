#include <expat.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace
{
constexpr std::size_t maxInput = 16u * 1024u * 1024u;
constexpr std::size_t maxOutput = 64u * 1024u * 1024u;
constexpr std::size_t maxDepth = 128;

struct Reader
{
    XML_Parser parser;
    std::string output;
    std::vector<bool> children;
    const char *failure = nullptr;
};

/**
 * @brief 向传输缓冲追加带 JSON 转义的 UTF-8 字符串。
 * @param out 输出缓冲。
 * @param text 待转义的字符串。
 */
void quote(std::string &out, std::string_view text)
{
    static constexpr char hex[] = "0123456789abcdef";
    out += '"';
    for (const unsigned char byte : text)
    {
        if (byte == '"' || byte == '\\') out += '\\';
        if (byte < 32)
        {
            out += "\\u00";
            out += hex[byte >> 4];
            out += hex[byte & 15];
        }
        else out += static_cast<char>(byte);
    }
    out += '"';
}

/**
 * @brief 终止当前解析并记录稳定错误文本。
 * @param reader 当前解析状态。
 * @param message 错误原因。
 */
void fail(Reader &reader, const char *message) noexcept
{
    reader.failure = message;
    XML_StopParser(reader.parser, XML_FALSE);
}

/**
 * @brief 追加元素或文本子项的分隔符。
 * @param reader 当前解析状态。
 */
void separate(Reader &reader)
{
    if (reader.children.empty()) return;
    if (reader.children.back()) reader.output += ',';
    reader.children.back() = true;
}

/**
 * @brief 记录元素名、属性及子项的起始位置。
 * @param context 解析器用户数据。
 * @param name 元素名。
 * @param attributes 以空指针结尾的名称和值对。
 */
void XMLCALL start(void *context, const XML_Char *name, const XML_Char **attributes) noexcept
{
    auto &reader = *static_cast<Reader *>(context);
    if (reader.children.size() == maxDepth || reader.output.size() > maxOutput)
    {
        fail(reader, "XML nesting or output limit exceeded");
        return;
    }
    try
    {
        separate(reader);
        reader.output += "{\"name\":";
        quote(reader.output, name);
        reader.output += ",\"attributes\":{";
        for (const auto **item = attributes; *item; item += 2)
        {
            if (item != attributes) reader.output += ',';
            quote(reader.output, item[0]);
            reader.output += ':';
            quote(reader.output, item[1]);
        }
        reader.output += "},\"children\":[";
        reader.children.push_back(false);
    }
    catch (...) { fail(reader, "XML output allocation failed"); }
}

/**
 * @brief 结束当前元素。
 * @param context 解析器用户数据。
 * @param name 已由解析器核对的元素名。
 */
void XMLCALL end(void *context, const XML_Char *name) noexcept
{
    (void)name;
    auto &reader = *static_cast<Reader *>(context);
    try
    {
        reader.output += "]}";
        reader.children.pop_back();
    }
    catch (...) { fail(reader, "XML output allocation failed"); }
}

/**
 * @brief 按原顺序记录文本，保留元素间的混合内容。
 * @param context 解析器用户数据。
 * @param bytes 文本字节。
 * @param length 文本字节数。
 */
void XMLCALL characters(void *context, const XML_Char *bytes, int length) noexcept
{
    auto &reader = *static_cast<Reader *>(context);
    if (reader.children.empty() || !length) return;
    if (reader.output.size() > maxOutput)
    {
        fail(reader, "XML output limit exceeded");
        return;
    }
    try
    {
        separate(reader);
        quote(reader.output, {bytes, static_cast<std::size_t>(length)});
    }
    catch (...) { fail(reader, "XML output allocation failed"); }
}

/**
 * @brief 拒绝文档类型声明，解析器不读取外部实体。
 * @param context 解析器用户数据。
 */
void XMLCALL doctype(void *context, const XML_Char *, const XML_Char *, const XML_Char *, int) noexcept
{
    fail(*static_cast<Reader *>(context), "DOCTYPE is disabled");
}

/**
 * @brief 复制结果到可经 C ABI 交付的缓冲区。
 * @param text 解析结果或错误文本。
 * @return 由 chali_xml_free 释放的字符串。
 */
char *copy_result(const std::string &text) noexcept
{
    auto *result = static_cast<char *>(std::malloc(text.size() + 1));
    if (result) std::memcpy(result, text.c_str(), text.size() + 1);
    return result;
}
}

/**
 * @brief 解析 UTF-8 XML 并返回根节点或行列错误。
 * @param text 输入缓冲。
 * @param length 输入字节数。
 * @return 拥有的传输文本，内存耗尽时可返回空指针。
 */
extern "C" char *chali_xml_parse(const char *text, int64_t length) noexcept
{
    if (length < 0 || static_cast<uint64_t>(length) > maxInput)
        return copy_result("{\"error\":\"XML input limit exceeded\",\"line\":1,\"column\":1}");
    auto parser = XML_ParserCreate("UTF-8");
    if (!parser) return nullptr;
    Reader reader{parser, {}, {}};
    XML_SetUserData(parser, &reader);
    XML_SetElementHandler(parser, start, end);
    XML_SetCharacterDataHandler(parser, characters);
    XML_SetStartDoctypeDeclHandler(parser, doctype);
    const bool valid = XML_Parse(parser, text, static_cast<int>(length), XML_TRUE) == XML_STATUS_OK;
    char *result = nullptr;
    try
    {
        if (valid && reader.output.size() <= maxOutput)
            result = copy_result(reader.output);
        else
        {
            std::string error = "{\"error\":";
            const char *reason = reader.failure;
            if (!reason) reason = valid ? "XML output limit exceeded" : XML_ErrorString(XML_GetErrorCode(parser));
            quote(error, reason ? reason : "invalid XML");
            error += ",\"line\":" + std::to_string(XML_GetCurrentLineNumber(parser));
            error += ",\"column\":" + std::to_string(XML_GetCurrentColumnNumber(parser) + 1) + "}";
            result = copy_result(error);
        }
    }
    catch (...) {}
    XML_ParserFree(parser);
    return result;
}

/**
 * @brief 释放由解析函数交付的缓冲区。
 * @param text 解析结果指针。
 */
extern "C" void chali_xml_free(char *text) noexcept
{
    std::free(text);
}
