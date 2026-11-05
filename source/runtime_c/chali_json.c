/**
 * @file chali_json.c
 * @brief 运行时 JSON 的独立 C 服务；编译器 JSON 保持原实现。
 */
#include "chali_json.h"
#include "chali_json_internal.h"
#include "chali_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

/**
 * @brief 创建未初始化的 C 拥有值。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_new(void) { return calloc(1, sizeof(ChaliJson)); }
/**
 * @brief 释放文档并保留可重复关闭的记录。
 * @param value 拥有值、输入标量或待释放存储。
 */
void chali_json_close(ChaliJson *value)
{
    if (value)
    {
        yyjson_mut_doc_free(value->document);
        value->document = NULL;
        value->root = NULL;
    }
}
/**
 * @brief 释放记录及其文档。
 * @param value 拥有值、输入标量或待释放存储。
 */
void chali_json_free(ChaliJson *value)
{
    chali_json_close(value);
    free(value);
}
/**
 * @brief 查询记录是否持有有效 JSON 根节点。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int chali_json_valid(const ChaliJson *value) { return value && value->root; }
/**
 * @brief 借用记录中的解析错误文本。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
const char *chali_json_error(const ChaliJson *value) { return value ? value->error : ""; }

/**
 * @brief 复制解析树并保持首次键顺序、末次键值及有符号整数范围。
 * @param doc 接收节点的 C 文档。
 * @param source 只读源节点或记录。
 * @return 由目标文档拥有的节点，失败为空。
 */
static yyjson_mut_val *convert(yyjson_mut_doc *doc, yyjson_val *source)
{
    yyjson_mut_val *out;
    if (yyjson_is_obj(source))
    {
        size_t index, max;
        yyjson_val *key, *value;
        out = yyjson_mut_obj(doc);
        if (!out) return NULL;
        yyjson_obj_foreach(source, index, max, key, value)
        {
            yyjson_mut_val *name = yyjson_mut_strncpy(doc, yyjson_get_str(key), yyjson_get_len(key));
            yyjson_mut_val *item = convert(doc, value);
            if (!name || !item) return NULL;
            if (!yyjson_mut_obj_replace(out, name, item) && !yyjson_mut_obj_add(out, name, item)) return NULL;
        }
        return out;
    }
    if (yyjson_is_arr(source))
    {
        size_t index, max;
        yyjson_val *item;
        out = yyjson_mut_arr(doc);
        if (!out) return NULL;
        yyjson_arr_foreach(source, index, max, item)
        {
            yyjson_mut_val *value = convert(doc, item);
            if (!value || !yyjson_mut_arr_append(out, value)) return NULL;
        }
        return out;
    }
    if (yyjson_is_uint(source) && yyjson_get_uint(source) > INT64_MAX)
        return yyjson_mut_real(doc, (double)yyjson_get_uint(source));
    if (yyjson_is_raw(source))
    {
        size_t n = yyjson_get_len(source);
        char *number = malloc(n + 1);
        double parsed;
        if (!number) return NULL;
        memcpy(number, yyjson_get_raw(source), n);
        number[n] = 0;
        parsed = strtod(number, NULL);
        free(number);
        return yyjson_mut_real(doc, parsed);
    }
    return yyjson_val_mut_copy(doc, source);
}

/* 独立检查转义代理对，允许原实现透传的非 UTF-8 原始字节。 */
/**
 * @brief 读取恰好四位十六进制转义。
 * @param text 输入字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @param at 起始字节位置。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
static int hex4(const char *text, size_t length, size_t at, unsigned *value)
{
    unsigned out = 0;
    size_t i;
    if (at > length || length - at < 4) return 0;
    for (i = 0; i < 4; i++)
    {
        unsigned char ch = (unsigned char)text[at + i];
        unsigned digit;
        if (ch >= '0' && ch <= '9')
            digit = ch - '0';
        else if (ch >= 'a' && ch <= 'f')
            digit = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            digit = ch - 'A' + 10;
        else
            return 0;
        out = out * 16 + digit;
    }
    *value = out;
    return 1;
}
/**
 * @brief 核对控制字节、转义及高代理对。
 * @param text 输入字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
static int valid_surrogates(const char *text, size_t length)
{
    size_t i;
    int quoted = 0;
    for (i = 0; i < length; i++)
    {
        if (text[i] == '"')
            quoted = !quoted;
        else if (quoted && (unsigned char)text[i] < 32)
            return 0;
        else if (quoted && text[i] == '\\')
        {
            unsigned code, low;
            if (++i == length) return 0;
            if (text[i] != 'u')
            {
                if (!strchr("\"\\/bfnrt", text[i])) return 0;
                continue;
            }
            if (!hex4(text, length, i + 1, &code)) return 0;
            i += 4;

            if (code >= 0xd800 && code <= 0xdbff)
            {
                if (length - i < 7 || text[i + 1] != '\\' || text[i + 2] != 'u' || !hex4(text, length, i + 3, &low) || low < 0xdc00 || low > 0xdfff) return 0;
                i += 6;
            }
        }
    }
    return 1;
}
/* 原解析器把独立低代理转成三字节原始 UTF-8；保留现有字节协议。 */
/**
 * @brief 保留原实现对独立低代理的三字节 UTF-8 输出。
 * @param text 输入字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @param normalized_length 接收规范化后的字节数。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
static char *normalize_low_surrogates(const char *text, size_t length, size_t *normalized_length)
{
    char *out = malloc(length + 1);
    size_t i, j = 0;
    int quoted = 0;
    if (!out) return NULL;
    for (i = 0; i < length; i++)
    {
        char ch = text[i];
        if (ch == '"') quoted = !quoted;
        if (quoted && ch == '\\' && i + 1 < length)
        {
            unsigned code;
            if (text[i + 1] == 'u' && hex4(text, length, i + 2, &code))
            {
                if (code >= 0xdc00 && code <= 0xdfff)
                {
                    out[j++] = (char)(0xe0 | (code >> 12));
                    out[j++] = (char)(0x80 | ((code >> 6) & 63));
                    out[j++] = (char)(0x80 | (code & 63));
                    i += 5;
                    continue;
                }
                if (code >= 0xd800 && code <= 0xdbff)
                {
                    memcpy(out + j, text + i, 12);
                    j += 12;
                    i += 11;
                    continue;
                }
            }
            out[j++] = ch;
            out[j++] = text[++i];
            continue;
        }
        out[j++] = ch;
    }
    out[j] = 0;
    *normalized_length = j;
    return out;
}
/**
 * @brief 解析独立 JSON 文档并保持原有数值及重复键协议。
 * @param text 输入字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_parse(const char *text, size_t length)
{
    ChaliJson *out = chali_json_new();
    yyjson_doc *parsed;
    yyjson_read_err error;
    char *normalized;
    size_t normalized_length;
    if (!out) return NULL;
    if (length == SIZE_MAX)
    {
        chali_json_free(out);
        return NULL;
    }
    if ((!text && length) || !valid_surrogates(text, length))
    {
        snprintf(out->error, sizeof(out->error), "JSON: invalid Unicode escape");
        return out;
    }
    normalized = normalize_low_surrogates(text ? text : "", length, &normalized_length);
    if (!normalized)
    {
        chali_json_free(out);
        return NULL;
    }
    parsed = yyjson_read_opts(normalized, normalized_length, YYJSON_READ_ALLOW_BOM | YYJSON_READ_ALLOW_INVALID_UNICODE | YYJSON_READ_BIGNUM_AS_RAW, NULL, &error);
    free(normalized);
    if (!parsed)
    {
        size_t i, line = 1, column = 1;
        for (i = 0; i < error.pos && i < length; i++)
        {
            if (text[i] == '\n')
            {
                line++;
                column = 1;
            }
            else
                column++;
        }
        snprintf(out->error, sizeof(out->error), "%zu:%zu: %s [json-parse]", line, column, error.msg ? error.msg : "parse failed");
        return out;
    }
    out->document = yyjson_mut_doc_new(NULL);
    if (out->document) out->root = convert(out->document, yyjson_doc_get_root(parsed));
    yyjson_doc_free(parsed);
    if (!out->root)
    {
        chali_json_free(out);
        return NULL;
    }
    yyjson_mut_doc_set_root(out->document, out->root);
    return out;
}
/**
 * @brief 创建不依赖原文档的节点快照。
 * @param value 拥有值、输入标量或待释放存储。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
static ChaliJson *copy_node(const yyjson_mut_val *value)
{
    ChaliJson *out = chali_json_new();
    if (!out) return NULL;
    if (!value) return out;
    out->document = yyjson_mut_doc_new(NULL);
    if (out->document) out->root = yyjson_mut_val_mut_copy(out->document, value);
    if (!out->root)
    {
        chali_json_free(out);
        return NULL;
    }
    yyjson_mut_doc_set_root(out->document, out->root);
    return out;
}
/**
 * @brief 深复制完整值和错误，副本独立于原记录的关闭。
 * @param value 拥有值、输入标量或待释放存储。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_clone(const ChaliJson *value)
{
    ChaliJson *out = copy_node(value ? value->root : NULL);
    if (out && value) memcpy(out->error, value->error, sizeof(out->error));
    return out;
}
/**
 * @brief 读取零空值、一布尔、二整数、三浮点、四字符串、五数组、六对象类型。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int chali_json_kind(const ChaliJson *value)
{
    yyjson_mut_val *node = value ? value->root : NULL;
    if (yyjson_mut_is_bool(node)) return 1;
    if (yyjson_mut_is_int(node)) return 2;
    if (yyjson_mut_is_real(node)) return 3;
    if (yyjson_mut_is_str(node)) return 4;
    if (yyjson_mut_is_arr(node)) return 5;
    if (yyjson_mut_is_obj(node)) return 6;
    return 0;
}
/**
 * @brief 读取数组元素或对象成员个数。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
size_t chali_json_size(const ChaliJson *value)
{
    if (!value) return 0;
    return yyjson_mut_is_arr(value->root) ? yyjson_mut_arr_size(value->root) : yyjson_mut_obj_size(value->root);
}
/**
 * @brief 按原有整数与可精确整型浮点的转换规则读取值。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int64_t chali_json_integer(const ChaliJson *value)
{
    double number;
    if (!value) return 0;
    if (yyjson_mut_is_int(value->root)) return yyjson_mut_get_sint(value->root);
    number = yyjson_mut_get_real(value->root);
    if (isfinite(number) && floor(number) == number && number >= -9223372036854775808.0 && number < 9223372036854775808.0) return (int64_t)number;
    return 0;
}
/**
 * @brief 读取整数或浮点近似值。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
double chali_json_real(const ChaliJson *value) { return value ? yyjson_mut_get_num(value->root) : 0; }
/**
 * @brief 读取布尔值。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int chali_json_boolean(const ChaliJson *value) { return value && yyjson_mut_get_bool(value->root); }
/**
 * @brief 借用字符串内容及明确字节长度。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
const char *chali_json_text(const ChaliJson *value, size_t *length)
{
    yyjson_mut_val *node = value ? value->root : NULL;
    if (length) *length = yyjson_mut_is_str(node) ? yyjson_mut_get_len(node) : 0;
    return yyjson_mut_get_str(node);
}
/**
 * @brief 按插入顺序借用对象键，保留嵌入零字节。
 * @param value 拥有值、输入标量或待释放存储。
 * @param index 从零起的元素或成员下标。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
const char *chali_json_key(const ChaliJson *value, size_t index, size_t *length)
{
    yyjson_mut_obj_iter iterator;
    yyjson_mut_val *key = NULL;
    if (length) *length = 0;
    if (!value || !yyjson_mut_obj_iter_init(value->root, &iterator)) return NULL;
    while ((key = yyjson_mut_obj_iter_next(&iterator)))
    {
        if (!index--) break;
    }
    if (length && key) *length = yyjson_mut_get_len(key);
    return yyjson_mut_get_str(key);
}
/**
 * @brief 为下标对应子节点创建独立快照，越界返回无值记录。
 * @param value 拥有值、输入标量或待释放存储。
 * @param index 从零起的元素或成员下标。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_at(const ChaliJson *value, size_t index)
{
    yyjson_mut_val *child = NULL;
    if (value && yyjson_mut_is_arr(value->root))
        child = yyjson_mut_arr_get(value->root, index);
    else if (value)
    {
        yyjson_mut_obj_iter iterator;
        if (yyjson_mut_obj_iter_init(value->root, &iterator))
        {
            yyjson_mut_val *key;
            while ((key = yyjson_mut_obj_iter_next(&iterator)))
                if (!index--)
                {
                    child = yyjson_mut_obj_iter_get_val(key);
                    break;
                }
        }
    }
    return copy_node(child);
}
/**
 * @brief 按完整键字节创建子树独立快照。
 * @param value 拥有值、输入标量或待释放存储。
 * @param key 完整键字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_find(const ChaliJson *value, const char *key, size_t length)
{ return copy_node(value && key ? yyjson_mut_obj_getn(value->root, key, length) : NULL); }
/**
 * @brief 向 C 输出缓冲追加明确字节范围。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param text 输入字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
static int append(ChaliCBuffer *buffer, const char *text, size_t length)
{ return chali_c_buffer_append(buffer, text, length) == CHALI_C_OK; }
/**
 * @brief 输出字符串引号、控制字节转义与原始非 ASCII 字节。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param text 输入字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
static int quoted(ChaliCBuffer *buffer, const char *text, size_t length)
{
    size_t i;
    if (!append(buffer, "\"", 1)) return 0;
    for (i = 0; i < length; i++)
    {
        unsigned char ch = (unsigned char)text[i];
        const char *escape = NULL;
        switch (ch)
        {
        case '"': escape = "\\\""; break;
        case '\\': escape = "\\\\"; break;
        case '\b': escape = "\\b"; break;
        case '\f': escape = "\\f"; break;
        case '\n': escape = "\\n"; break;
        case '\r': escape = "\\r"; break;
        case '\t': escape = "\\t"; break;
        }
        if (escape)
        {
            if (!append(buffer, escape, 2)) return 0;
        }
        else if (ch < 32)
        {
            char hex[7];
            snprintf(hex, sizeof(hex), "\\u%04x", ch);
            if (!append(buffer, hex, 6)) return 0;
        }
        else if (!append(buffer, (const char *)&text[i], 1))
            return 0;
    }
    return append(buffer, "\"", 1);
}
/**
 * @brief 保持原有 JSON 浮点输出格式。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
static int real_text(ChaliCBuffer *buffer, double value)
{
    char number[64];
    int count;
    if (!isfinite(value)) return append(buffer, "null", 4);
    count = snprintf(number, sizeof(number), "%.17g", value);
    if (count < 0 || (size_t)count >= sizeof(number) || !append(buffer, number, (size_t)count)) return 0;
    return strpbrk(number, ".eE") || append(buffer, ".0", 2);
}
/**
 * @brief 递归输出已限定深度的 JSON 节点。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param node 只读树节点。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
static int write_node(ChaliCBuffer *buffer, yyjson_mut_val *node)
{
    size_t i, max;
    yyjson_mut_val *item, *key;
    if (yyjson_mut_is_arr(node))
    {
        if (!append(buffer, "[", 1)) return 0;
        yyjson_mut_arr_foreach(node, i, max, item)
        {
            if ((i && !append(buffer, ",", 1)) || !write_node(buffer, item)) return 0;
        }
        return append(buffer, "]", 1);
    }
    if (yyjson_mut_is_obj(node))
    {
        if (!append(buffer, "{", 1)) return 0;
        yyjson_mut_obj_foreach(node, i, max, key, item)
        {
            if ((i && !append(buffer, ",", 1)) || !quoted(buffer, yyjson_mut_get_str(key), yyjson_mut_get_len(key)) || !append(buffer, ":", 1) || !write_node(buffer, item)) return 0;
        }
        return append(buffer, "}", 1);
    }
    if (yyjson_mut_is_str(node)) return quoted(buffer, yyjson_mut_get_str(node), yyjson_mut_get_len(node));
    if (yyjson_mut_is_real(node)) return real_text(buffer, yyjson_mut_get_real(node));
    if (yyjson_mut_is_int(node))
    {
        char number[32];
        int n = snprintf(number, sizeof(number), "%lld", (long long)yyjson_mut_get_sint(node));
        return n > 0 && append(buffer, number, (size_t)n);
    }
    if (yyjson_mut_is_bool(node)) return yyjson_mut_get_bool(node) ? append(buffer, "true", 4) : append(buffer, "false", 5);
    return append(buffer, "null", 4);
}
/**
 * @brief 在成功后转移零结尾输出，失败时释放全部缓冲。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param success 前序操作是否成功。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
static char *finish(ChaliCBuffer *buffer, int success, size_t *length)
{
    char *out = NULL;
    size_t size = buffer->length;
    if (success && append(buffer, "", 1))
    {
        out = buffer->data;
        buffer->data = NULL;
        buffer->length = buffer->capacity = 0;
        if (length) *length = size;
    }
    chali_c_buffer_clear(buffer);
    return out;
}
/**
 * @brief 使用原有转义与浮点格式输出 JSON。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
char *chali_json_dump(const ChaliJson *value, size_t *length)
{
    ChaliCBuffer buffer;
    chali_c_buffer_init(&buffer, 1, NULL);
    return finish(&buffer, write_node(&buffer, value ? value->root : NULL), length);
}
/**
 * @brief 把任意字节按原有 JSON 字符串协议转义。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @param output_length 接收输出的字节数。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
char *chali_json_quote(const char *value, size_t length, size_t *output_length)
{
    ChaliCBuffer buffer;
    chali_c_buffer_init(&buffer, 1, NULL);
    return finish(&buffer, (!length || value) && quoted(&buffer, value, length), output_length);
}
/**
 * @brief 按原有十七位格式及非有限值规则输出浮点。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
char *chali_json_format_real(double value, size_t *length)
{
    ChaliCBuffer buffer;
    chali_c_buffer_init(&buffer, 1, NULL);
    return finish(&buffer, real_text(&buffer, value), length);
}
