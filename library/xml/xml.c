/** @file xml.c @brief 基于 Expat 的 XML C 桥接，缓冲仅使用 C 分配接口。 */
#include <expat.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT (16u * 1024u * 1024u)
#define MAX_OUTPUT (64u * 1024u * 1024u)
#define MAX_DEPTH 128u

/** @brief 独占、以零结束的有界输出缓冲。 */
typedef struct Buffer
{
    char *data;
    size_t length, capacity;
    const char *failure;
} Buffer;
/** @brief 解析状态与不需要动态分配的固定深度子节点标记。 */
typedef struct Reader
{
    XML_Parser parser;
    Buffer output;
    unsigned char children[MAX_DEPTH];
    size_t depth;
    const char *failure;
} Reader;

/**
 * @brief 增长输出并追加原始字节，失败保留已有缓冲。
 * @param output 输出状态。
 * @param text 字节。
 * @param length 字节数。
 * @return 成功为一。
 */
static int append(Buffer *output, const char *text, size_t length)
{
    size_t needed, capacity;
    char *grown;
    if (output->failure) return 0;
    if (length > MAX_OUTPUT - output->length)
    {
        output->failure = "XML output limit exceeded";
        return 0;
    }
    needed = output->length + length + 1;
    if (needed > output->capacity)
    {
        capacity = output->capacity ? output->capacity : 256;
        while (capacity < needed)
            capacity = capacity > (MAX_OUTPUT + 1u) / 2 ? MAX_OUTPUT + 1u : capacity * 2;
        grown = (char *)realloc(output->data, capacity);
        if (!grown)
        {
            output->failure = "XML output allocation failed";
            return 0;
        }
        output->data = grown;
        output->capacity = capacity;
    }
    if (length) memcpy(output->data + output->length, text, length);
    output->length += length;
    output->data[output->length] = 0;
    return 1;
}
/**
 * @brief 追加以零结束的固定文本。
 * @param output 输出。
 * @param text 文本。
 * @return 成功为一。
 */
static int append_text(Buffer *output, const char *text) { return append(output, text, strlen(text)); }
/**
 * @brief 生成与原桥接一致的 JSON 字符串转义。
 * @param output 输出。
 * @param text UTF-8 字节。
 * @param length 字节数。
 * @return 成功为一。
 */
static int quote(Buffer *output, const char *text, size_t length)
{
    static const char hex[] = "0123456789abcdef";
    size_t index;
    if (!append_text(output, "\"")) return 0;
    for (index = 0; index < length; ++index)
    {
        const unsigned char byte = (unsigned char)text[index];
        if ((byte == '"' || byte == '\\') && !append_text(output, "\\")) return 0;
        if (byte < 32)
        {
            char escaped[6] = {'\\', 'u', '0', '0', hex[byte >> 4], hex[byte & 15]};
            if (!append(output, escaped, sizeof(escaped))) return 0;
        }
        else if (!append(output, text + index, 1)) return 0;
    }
    return append_text(output, "\"");
}
/**
 * @brief 停止解析并保留首次错误。
 * @param reader 解析状态。
 * @param message 稳定错误文本。
 */
static void fail(Reader *reader, const char *message)
{
    if (reader->failure) return;
    reader->failure = message;
    XML_StopParser(reader->parser, XML_FALSE);
}
/**
 * @brief 在父元素中追加下一项的分隔符。
 * @param reader 解析状态。
 */
static void separate(Reader *reader)
{
    if (!reader->depth) return;
    if (reader->children[reader->depth - 1]) append_text(&reader->output, ",");
    reader->children[reader->depth - 1] = 1;
}
/**
 * @brief 记录元素名称、保序属性与子项数组。
 * @param context 解析状态。
 * @param name 元素名。
 * @param attributes 名称和值交替的空终止数组。
 */
static void XMLCALL start(void *context, const XML_Char *name, const XML_Char **attributes)
{
    Reader *reader = context;
    const XML_Char **item;
    if (reader->failure) return;
    if (reader->depth == MAX_DEPTH)
    {
        fail(reader, "XML nesting or output limit exceeded");
        return;
    }
    separate(reader);
    append_text(&reader->output, "{\"name\":");
    quote(&reader->output, name, strlen(name));
    append_text(&reader->output, ",\"attributes\":{");
    for (item = attributes; *item && !reader->output.failure; item += 2)
    {
        if (item != attributes) append_text(&reader->output, ",");
        quote(&reader->output, item[0], strlen(item[0]));
        append_text(&reader->output, ":");
        quote(&reader->output, item[1], strlen(item[1]));
    }
    append_text(&reader->output, "},\"children\":[");
    if (reader->output.failure) fail(reader, reader->output.failure);
    else reader->children[reader->depth++] = 0;
}
/**
 * @brief 结束当前元素。
 * @param context 解析状态。
 * @param name 已匹配的元素名。
 */
static void XMLCALL end(void *context, const XML_Char *name)
{
    Reader *reader = context;
    (void)name;
    if (reader->failure) return;
    append_text(&reader->output, "]}");
    if (reader->depth) --reader->depth;
    if (reader->output.failure) fail(reader, reader->output.failure);
}
/**
 * @brief 按 Expat 原回调顺序记录混合内容。
 * @param context 解析状态。
 * @param bytes 文本。
 * @param length 文本字节数。
 */
static void XMLCALL characters(void *context, const XML_Char *bytes, int length)
{
    Reader *reader = context;
    if (reader->failure || !reader->depth || length <= 0) return;
    separate(reader);
    quote(&reader->output, bytes, (size_t)length);
    if (reader->output.failure) fail(reader, reader->output.failure);
}
/**
 * @brief 与原实现一样拒绝文档类型声明和外部实体。
 * @param context 解析状态。
 * @param name 声明名。
 * @param system_id 系统标识。
 * @param public_id 公共标识。
 * @param internal 是否有内部子集。
 */
static void XMLCALL doctype(void *context, const XML_Char *name, const XML_Char *system_id,
                           const XML_Char *public_id, int internal)
{
    (void)name; (void)system_id; (void)public_id; (void)internal;
    fail(context, "DOCTYPE is disabled");
}
/**
 * @brief 追加无符号行列号。
 * @param output 输出。
 * @param value 数值。
 */
static void append_number(Buffer *output, uintmax_t value)
{
    char text[32];
    int length = snprintf(text, sizeof(text), "%" PRIuMAX, value);
    if (length > 0 && (size_t)length < sizeof(text)) append(output, text, (size_t)length);
}
/**
 * @brief 生成拥有型错误结果。
 * @param reason 错误原因。
 * @param line 一起始行号。
 * @param column 一起始列号。
 * @return 可由 chali_xml_free 释放的文本，分配失败为空。
 */
static char *error_result(const char *reason, uintmax_t line, uintmax_t column)
{
    Buffer output = {0};
    append_text(&output, "{\"error\":");
    quote(&output, reason, strlen(reason));
    append_text(&output, ",\"line\":");
    append_number(&output, line);
    append_text(&output, ",\"column\":");
    append_number(&output, column);
    append_text(&output, "}");
    if (!output.failure) return output.data;
    free(output.data);
    return NULL;
}
/**
 * @brief 使用 Expat C 接口解析 XML，保持既有传输 JSON 和行列诊断。
 * @param text UTF-8 输入。
 * @param length 输入字节数。
 * @return 独占结果，内存耗尽时为空。
 */
char *chali_xml_parse(const char *text, int64_t length)
{
    Reader reader = {0};
    char *result;
    int valid;
    if (length < 0 || (uint64_t)length > MAX_INPUT)
        return error_result("XML input limit exceeded", 1, 1);
    if (!text && length) return error_result("invalid XML", 1, 1);
    reader.parser = XML_ParserCreate("UTF-8");
    if (!reader.parser) return NULL;
    XML_SetUserData(reader.parser, &reader);
    XML_SetElementHandler(reader.parser, start, end);
    XML_SetCharacterDataHandler(reader.parser, characters);
    XML_SetStartDoctypeDeclHandler(reader.parser, doctype);
    valid = XML_Parse(reader.parser, text ? text : "", (int)length, XML_TRUE) == XML_STATUS_OK;
    if (valid && !reader.failure && !reader.output.failure)
    {
        result = reader.output.data;
        reader.output.data = NULL;
    }
    else
    {
        const char *reason = reader.failure;
        if (!reason) reason = XML_ErrorString(XML_GetErrorCode(reader.parser));
        result = error_result(reason ? reason : "invalid XML", XML_GetCurrentLineNumber(reader.parser),
                              XML_GetCurrentColumnNumber(reader.parser) + 1);
    }
    free(reader.output.data);
    XML_ParserFree(reader.parser);
    return result;
}
/**
 * @brief 使用配对的 C free 释放结果。
 * @param text 返回的文本或空指针。
 */
void chali_xml_free(char *text) { free(text); }
