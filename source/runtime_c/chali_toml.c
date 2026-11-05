/**
 * @file chali_toml.c
 * @brief TOML 的独立 C 服务与旧接口兼容。
 */
#include "chali_toml.h"
#include "chali_json_internal.h"
#include "chali_buffer.h"
#include "tomlc17.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
struct ChaliToml
{
    ChaliJson *value;
    char *original;
    size_t length;
};
/**
 * @brief 创建未初始化的 C TOML 记录。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_new(void)
{
    ChaliToml *out = calloc(1, sizeof(*out));
    if (!out) return NULL;
    out->value = chali_json_new();
    if (!out->value)
    {
        free(out);
        return NULL;
    }
    return out;
}
/**
 * @brief 幂等关闭文档并释放原文，保留记录及诊断。
 * @param value 拥有值或只读输入节点。
 */
void chali_toml_close(ChaliToml *value)
{
    if (value)
    {
        chali_json_close(value->value);
        free(value->original);
        value->original = NULL;
        value->length = 0;
    }
}
/**
 * @brief 销毁记录及全部 C 存储。
 * @param value 拥有值或只读输入节点。
 */
void chali_toml_free(ChaliToml *value)
{
    if (value)
    {
        chali_toml_close(value);
        chali_json_free(value->value);
        free(value);
    }
}
/**
 * @brief 查询记录是否拥有有效节点。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int chali_toml_valid(const ChaliToml *value) { return value && chali_json_valid(value->value); }
/**
 * @brief 借用解析或转换诊断。
 * @param value 拥有值或只读输入节点。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_error(const ChaliToml *value) { return value ? chali_json_error(value->value) : ""; }
/**
 * @brief 在 C 解析器前保持旧接口的 TOML 1.0 语法与嵌套上限。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 操作或查询结果，失败使用零状态。
 */
static int valid_v1(const char *text, size_t length)
{
    size_t i, depth = 0;
    char stack[256];
    int state = 0;
    for (i = 0; i < length; i++)
    {
        char ch = text[i];
        if (i == 0 && length >= 3 && (unsigned char)text[0] == 0xef && (unsigned char)text[1] == 0xbb && (unsigned char)text[2] == 0xbf)
        {
            i = 2;
            continue;
        }
        if (state)
        {
            char end = (state == 1 || state == 3) ? '"' : '\'';
            if ((state == 1 || state == 3) && ch == '\\')
            {
                if (i + 1 < length && (text[i + 1] == 'x' || text[i + 1] == 'e')) return 0;
                if (i + 1 < length) i++;
                continue;
            }
            if (ch == end)
            {
                if (state <= 2)
                    state = 0;
                else if (i + 2 < length && text[i + 1] == end && text[i + 2] == end)
                {
                    i += 2;
                    while (i + 1 < length && text[i + 1] == end) i++;
                    state = 0;
                }
            }
            continue;
        }
        if (ch == '"' || ch == '\'')
        {
            state = ch == '"' ? 1 : 2;
            if (i + 2 < length && text[i + 1] == ch && text[i + 2] == ch)
            {
                state += 2;
                i += 2;
            }
        }
        else if (ch == '#')
        {
            if (depth && stack[depth - 1] == '{') return 0;
            while (i + 1 < length && text[i + 1] != '\n') i++;
        }
        else if (ch == '[' || ch == '{')
        {
            if (depth == 256) return 0;
            stack[depth++] = ch;
        }
        else if (ch == ']' || ch == '}')
        {
            size_t j = i;
            if (ch == '}')
            {
                while (j && (text[j - 1] == ' ' || text[j - 1] == '\t')) j--;
                if (j && text[j - 1] == ',') return 0;
            }
            if (depth) depth--;
        }
        else if ((ch == '\n' || ch == '\r') && depth && stack[depth - 1] == '{')
            return 0;
    }
    return 1;
}
/**
 * @brief 按原文中的行列定位 token，考虑起始 BOM。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @param line 从一开始的行号。
 * @param column 从一开始的字节列号。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
static const char *location(const char *text, size_t length, int line, int column)
{
    size_t i = length >= 3 && (unsigned char)text[0] == 0xef && (unsigned char)text[1] == 0xbb && (unsigned char)text[2] == 0xbf ? 3 : 0;
    while (line > 1 && i < length)
        if (text[i++] == '\n') line--;
    if (line != 1 || column < 1 || (size_t)(column - 1) > length - i) return NULL;
    return text + i + column - 1;
}
/**
 * @brief 从原始 token 恢复九位小数秒与闰秒，保存为 C 文档拥有的日期文本。
 * @param doc 接收结果的 C 文档。
 * @param value 拥有值或只读输入节点。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 由目标 C 文档拥有的节点，失败为空。
 */
static yyjson_mut_val *timestamp(yyjson_mut_doc *doc, const toml_datum_t *value, const char *text, size_t length)
{
    char buffer[96];
    size_t n = 0;
    unsigned nano = 0, count = 0;
    int type = value->type;
    const char *start = location(text, length, value->lineno, value->colno), *at = NULL, *end = text + length;
    if (type != TOML_TIME) n = (size_t)snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", value->u.ts.year, value->u.ts.month, value->u.ts.day);
    if (type != TOML_DATE)
    {
        if (start) at = start + (type == TOML_TIME ? 0 : 11);
        if (!at || at > end || end - at < 8 || at[2] != ':' || at[5] != ':') return NULL;
        n += (size_t)snprintf(buffer + n, sizeof(buffer) - n, "%s%02d:%02d:%02d", type == TOML_TIME ? "" : "T", value->u.ts.hour, value->u.ts.minute, (at[6] - '0') * 10 + (at[7] - '0'));
        at += 8;
        if (at < end && *at == '.')
        {
            at++;
            while (at < end && *at >= '0' && *at <= '9')
            {
                if (count < 9) nano = nano * 10 + (unsigned)(*at - '0');
                count++;
                at++;
            }
            while (count < 9)
            {
                nano *= 10;
                count++;
            }
        }
        if (nano)
        {
            n += (size_t)snprintf(buffer + n, sizeof(buffer) - n, ".%09u", nano);
            while (n && buffer[n - 1] == '0') n--;
        }
        if (type == TOML_DATETIMETZ)
        {
            int tz = value->u.ts.tz;
            if (!tz)
                buffer[n++] = 'Z';
            else
            {
                int offset = tz < 0 ? -tz : tz;
                n += (size_t)snprintf(buffer + n, sizeof(buffer) - n, "%c%02d:%02d", tz < 0 ? '-' : '+', offset / 60, offset % 60);
            }
        }
    }
    buffer[n] = 0;
    return yyjson_mut_rawncpy(doc, buffer, n);
}
/**
 * @brief 按原有 C 浮点转换语义读取原始 token。
 * @param value 拥有值或只读输入节点。
 * @param source 原始输入字节或源记录。
 * @param length 输入字节数或接收字节数的指针。
 * @param result 接收旧语义下的浮点值。
 * @return 操作或查询结果，失败使用零状态。
 */
static int legacy_real(const toml_datum_t *value, const char *source, size_t length, double *result);
/**
 * @brief 将解析结果复制到独立 C 文档，原解析树可立即释放。
 * @param doc 接收结果的 C 文档。
 * @param value 拥有值或只读输入节点。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @param depth 当前结构嵌套层数。
 * @return 由目标 C 文档拥有的节点，失败为空。
 */
static yyjson_mut_val *convert(yyjson_mut_doc *doc, const toml_datum_t *value, const char *text, size_t length, unsigned depth)
{
    yyjson_mut_val *out;
    int i;
    if (depth > 1024) return NULL;
    switch (value->type)
    {
    case TOML_STRING: return yyjson_mut_strncpy(doc, value->u.str.ptr, (size_t)value->u.str.len);
    case TOML_INT64: return yyjson_mut_sint(doc, value->u.int64);
    case TOML_FP64:
    {
        double number;
        if (!legacy_real(value, text, length, &number)) return NULL;
        return yyjson_mut_real(doc, number);
    }
    case TOML_BOOLEAN: return yyjson_mut_bool(doc, value->u.boolean);
    case TOML_DATE:
    case TOML_TIME:
    case TOML_DATETIME:
    case TOML_DATETIMETZ: return timestamp(doc, value, text, length);
    case TOML_ARRAY:
        out = yyjson_mut_arr(doc);
        if (!out) return NULL;
        for (i = 0; i < value->u.arr.size; i++)
        {
            yyjson_mut_val *item = convert(doc, &value->u.arr.elem[i], text, length, depth + 1);
            if (!item || !yyjson_mut_arr_append(out, item)) return NULL;
        }
        return out;
    case TOML_TABLE:
        out = yyjson_mut_obj(doc);
        if (!out) return NULL;
        for (i = 0; i < value->u.tab.size; i++)
        {
            yyjson_mut_val *key = yyjson_mut_strncpy(doc, value->u.tab.key[i], (size_t)value->u.tab.len[i]);
            yyjson_mut_val *item = convert(doc, &value->u.tab.value[i], text, length, depth + 1);
            if (!key || !item || !yyjson_mut_obj_add(out, key, item)) return NULL;
        }
        return out;
    default: return NULL;
    }
}
/**
 * @brief 核对范围内当前位置是否为十进制数字。
 * @param s 只读字符范围。
 * @param n 字符范围长度。
 * @param at 起始字节下标。
 * @return 操作或查询结果，失败使用零状态。
 */
static int digit_at(const char *s, size_t n, size_t at) { return at < n && s[at] >= '0' && s[at] <= '9'; }
/**
 * @brief 检查旧浮点语法并移除数字间的下划线。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @param clean 接收移除下划线的浮点 token。
 * @return 操作或查询结果，失败使用零状态。
 */
static int float_syntax(const char *text, size_t length, char *clean)
{
    size_t i, j = 0, at = 0;
    int floating = 0;
    for (i = 0; i < length; i++)
    {
        if (text[i] == '_')
        {
            if (!i || i + 1 == length || !digit_at(text, length, i - 1) || !digit_at(text, length, i + 1)) return 0;
        }
        else
            clean[j++] = text[i];
    }
    clean[j] = 0;
    if (at < j && (clean[at] == '+' || clean[at] == '-')) at++;
    if (!digit_at(clean, j, at)) return 0;
    if (clean[at] == '0')
        at++;
    else
        while (digit_at(clean, j, at)) at++;
    if (at < j && clean[at] == '.')
    {
        floating = 1;
        at++;
        if (!digit_at(clean, j, at)) return 0;
        while (digit_at(clean, j, at)) at++;
    }
    if (at < j && (clean[at] == 'e' || clean[at] == 'E'))
    {
        floating = 1;
        at++;
        if (at < j && (clean[at] == '+' || clean[at] == '-')) at++;
        if (!digit_at(clean, j, at)) return 0;
        while (digit_at(clean, j, at)) at++;
    }
    return floating && at == j;
}
/**
 * @brief 为底层解析器替换超范围或过长浮点，占位后仍按原文恢复实际值。
 * @param out 接收结果的工作副本。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 操作或查询结果，失败使用零状态。
 */
static int normalize_range(char *out, const char *text, size_t length)
{
    size_t i;
    int state = 0, line_start = 1, header = 0;
    unsigned depth = 0;
    for (i = 0; i < length; i++)
    {
        char ch = text[i];
        if (i == 0 && length >= 3 && (unsigned char)text[0] == 0xef && (unsigned char)text[1] == 0xbb && (unsigned char)text[2] == 0xbf)
        {
            i = 2;
            continue;
        }
        if (state)
        {
            char end = (state == 1 || state == 3) ? '"' : '\'';
            if ((state == 1 || state == 3) && ch == '\\')
            {
                if (i + 1 < length) i++;
                continue;
            }
            if (ch == end)
            {
                if (state <= 2)
                    state = 0;
                else if (i + 2 < length && text[i + 1] == end && text[i + 2] == end)
                {
                    i += 2;
                    while (i + 1 < length && text[i + 1] == end) i++;
                    state = 0;
                }
            }
            continue;
        }
        if (ch == '\n' || ch == '\r')
        {
            line_start = 1;
            header = 0;
            continue;
        }
        if (line_start && (ch == ' ' || ch == '\t')) continue;
        if (line_start && !depth && ch == '[') header = 1;
        line_start = 0;
        if (ch == '"' || ch == '\'')
        {
            state = ch == '"' ? 1 : 2;
            if (i + 2 < length && text[i + 1] == ch && text[i + 2] == ch)
            {
                state += 2;
                i += 2;
            }
            continue;
        }
        if (ch == '#')
        {
            while (i + 1 < length && text[i + 1] != '\n') i++;
            continue;
        }
        if (header) continue;
        if (ch == '[' || ch == '{')
            depth++;
        else if (ch == ']' || ch == '}')
        {
            if (depth) depth--;
        }
        else if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-')
        {
            size_t end = i, next;
            char *clean;
            char *parsed_end;
            while (end < length && strchr("0123456789.eE+-_", text[end])) end++;
            next = end;
            while (next < length && (text[next] == ' ' || text[next] == '\t')) next++;
            if (next < length && text[next] == '=')
            {
                i = end ? end - 1 : i;
                continue;
            }
            if (end < length && !strchr(" \t\r\n,]}#", text[end])) continue;
            clean = malloc(end - i + 1);
            if (!clean) return 0;
            if (float_syntax(text + i, end - i, clean))
            {
                errno = 0;
                (void)strtod(clean, &parsed_end);
                if ((errno == ERANGE || end - i >= 118) && !*parsed_end && end - i >= 3)
                {
                    memcpy(out + i, "0.0", 3);
                    memset(out + i + 3, ' ', end - i - 3);
                }
            }
            free(clean);
            if (end > i) i = end - 1;
        }
    }
    return 1;
}
/**
 * @brief 按原有 C 浮点转换语义读取原始 token。
 * @param value 拥有值或只读输入节点。
 * @param source 原始输入字节或源记录。
 * @param length 输入字节数或接收字节数的指针。
 * @param result 接收旧语义下的浮点值。
 * @return 操作或查询结果，失败使用零状态。
 */
static int legacy_real(const toml_datum_t *value, const char *source, size_t length, double *result)
{
    const char *at = location(source, length, value->lineno, value->colno), *end = source + length;
    size_t n = 0, j = 0, i;
    char *clean;
    if (!at || at > end) return 0;
    while (at + n < end && !strchr(" \t\r\n,]}#", at[n])) n++;
    clean = malloc(n + 1);
    if (!clean) return 0;
    for (i = 0; i < n; i++)
        if (at[i] != '_') clean[j++] = at[i];
    clean[j] = 0;
    *result = strtod(clean, NULL);
    free(clean);
    return 1;
}
/**
 * @brief 创建解析工作副本，处理闰秒与过长小数秒而不改变后续 token 位置。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
static char *parse_source(const char *text, size_t length)
{
    char *out = malloc(length + 1);
    size_t i;
    int state = 0;
    if (!out) return NULL;
    if (length) memcpy(out, text, length);
    out[length] = 0;
    for (i = 0; i < length; i++)
    {
        char ch = text[i];
        if (i == 0 && length >= 3 && (unsigned char)text[0] == 0xef && (unsigned char)text[1] == 0xbb && (unsigned char)text[2] == 0xbf)
        {
            i = 2;
            continue;
        }
        if (state)
        {
            char end = (state == 1 || state == 3) ? '"' : '\'';
            if ((state == 1 || state == 3) && ch == '\\')
            {
                if (i + 1 < length) i++;
                continue;
            }
            if (ch == end)
            {
                if (state <= 2)
                    state = 0;
                else if (i + 2 < length && text[i + 1] == end && text[i + 2] == end)
                {
                    i += 2;
                    while (i + 1 < length && text[i + 1] == end) i++;
                    state = 0;
                }
            }
            continue;
        }
        if (ch == '"' || ch == '\'')
        {
            state = ch == '"' ? 1 : 2;
            if (i + 2 < length && text[i + 1] == ch && text[i + 2] == ch)
            {
                state += 2;
                i += 2;
            }
        }
        else if (ch == '#')
        {
            while (i + 1 < length && text[i + 1] != '\n') i++;
        }
        else if (length - i >= 8 && text[i + 2] == ':' && text[i + 5] == ':' && ch >= '0' && ch <= '2' && text[i + 1] >= '0' && text[i + 1] <= '9' && text[i + 3] >= '0' && text[i + 3] <= '5' && text[i + 4] >= '0' && text[i + 4] <= '9')
        {
            size_t first = i + 9, last = first, suffix = 0;
            if (text[i + 6] == '6' && text[i + 7] == '0')
            {
                out[i + 6] = '5';
                out[i + 7] = '9';
            }
            if (i + 8 < length && text[i + 8] == '.')
            {
                while (last < length && text[last] >= '0' && text[last] <= '9') last++;
                if (last - first > 9)
                {
                    if (last < length && (text[last] == 'Z' || text[last] == 'z'))
                        suffix = 1;
                    else if (length - last >= 6 && (text[last] == '+' || text[last] == '-') && text[last + 3] == ':')
                        suffix = 6;
                    if (suffix) memcpy(out + first + 9, text + last, suffix);
                    memset(out + first + 9 + suffix, ' ', last - first - 9);
                }
            }
        }
    }
    if (!normalize_range(out, text, length))
    {
        free(out);
        return NULL;
    }
    return out;
}
/**
 * @brief 解析 TOML 1.0，保留完整原文并恢复旧接口的数值和日期协议。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_parse(const char *text, size_t length)
{
    ChaliToml *out = chali_toml_new();
    toml_result_t parsed;
    char *normalized;
    if (!out) return NULL;
    if ((!text && length) || length > INT_MAX || !valid_v1(text, length))
    {
        snprintf(out->value->error, sizeof(out->value->error), "TOML v1.0 syntax or nesting limit violation");
        return out;
    }
    normalized = parse_source(text ? text : "", length);
    if (!normalized)
    {
        chali_toml_free(out);
        return NULL;
    }
    parsed = toml_parse(normalized, (int)length);
    free(normalized);
    if (!parsed.ok)
    {
        snprintf(out->value->error, sizeof(out->value->error), "%s", parsed.errmsg);
        toml_free(parsed);
        return out;
    }
    out->value->document = yyjson_mut_doc_new(NULL);
    if (out->value->document) out->value->root = convert(out->value->document, &parsed.toptab, text ? text : "", length, 0);
    toml_free(parsed);
    if (!out->value->root)
    {
        chali_toml_close(out);
        snprintf(out->value->error, sizeof(out->value->error), "TOML node conversion failed");
        return out;
    }
    out->original = malloc(length + 1);
    if (!out->original)
    {
        chali_toml_free(out);
        return NULL;
    }
    if (length) memcpy(out->original, text, length);
    out->original[length] = 0;
    out->length = length;
    yyjson_mut_doc_set_root(out->value->document, out->value->root);
    return out;
}
/**
 * @brief 深复制节点、原文与错误，副本可独立关闭。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_clone(const ChaliToml *value)
{
    ChaliToml *out = chali_toml_new();
    if (!out) return NULL;
    chali_json_free(out->value);
    out->value = chali_json_clone(value ? value->value : NULL);
    if (!out->value)
    {
        chali_toml_free(out);
        return NULL;
    }
    if (value && value->original)
    {
        out->original = malloc(value->length + 1);
        if (!out->original)
        {
            chali_toml_free(out);
            return NULL;
        }
        memcpy(out->original, value->original, value->length + 1);
        out->length = value->length;
    }
    return out;
}
/**
 * @brief 返回原有节点编号，其中七表示日期或时间。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int chali_toml_kind(const ChaliToml *value)
{
    if (value && yyjson_mut_is_raw(value->value->root)) return 7;
    return chali_json_kind(value ? value->value : NULL);
}
/**
 * @brief 读取数组长度或表成员数。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
size_t chali_toml_size(const ChaliToml *value) { return chali_json_size(value ? value->value : NULL); }
/**
 * @brief 按原有整型或可精确转换的浮点规则取整数。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int64_t chali_toml_integer(const ChaliToml *value) { return chali_json_integer(value ? value->value : NULL); }
/**
 * @brief 读取数值近似。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
double chali_toml_real(const ChaliToml *value) { return chali_json_real(value ? value->value : NULL); }
/**
 * @brief 读取布尔值。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int chali_toml_boolean(const ChaliToml *value) { return chali_json_boolean(value ? value->value : NULL); }
/**
 * @brief 借用完整字符串字节。
 * @param value 拥有值或只读输入节点。
 * @param length 输入字节数或接收字节数的指针。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_text(const ChaliToml *value, size_t *length) { return chali_json_text(value ? value->value : NULL, length); }
/**
 * @brief 按插入顺序借用表键。
 * @param value 拥有值或只读输入节点。
 * @param index 从零开始的元素或成员下标。
 * @param length 输入字节数或接收字节数的指针。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_key(const ChaliToml *value, size_t index, size_t *length) { return chali_json_key(value ? value->value : NULL, index, length); }
/**
 * @brief 借用恢复到九位小数秒的日期或时间文本。
 * @param value 拥有值或只读输入节点。
 * @param length 输入字节数或接收字节数的指针。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_date_time(const ChaliToml *value, size_t *length)
{
    yyjson_mut_val *root = value ? value->value->root : NULL;
    if (length) *length = yyjson_mut_is_raw(root) ? yyjson_mut_get_len(root) : 0;
    return yyjson_mut_get_raw(root);
}
/**
 * @brief 接管独立 C JSON 文档并包装为 TOML 记录。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
static ChaliToml *own_child(ChaliJson *value)
{
    ChaliToml *out;
    if (!value) return NULL;
    out = calloc(1, sizeof(*out));
    if (!out)
    {
        chali_json_free(value);
        return NULL;
    }
    out->value = value;
    return out;
}
/**
 * @brief 创建指定子节点的独立快照。
 * @param value 拥有值或只读输入节点。
 * @param index 从零开始的元素或成员下标。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_at(const ChaliToml *value, size_t index)
{ return own_child(chali_json_at(value ? value->value : NULL, index)); }
/**
 * @brief 按完整键字节创建独立子树快照。
 * @param value 拥有值或只读输入节点。
 * @param key 完整键字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_find(const ChaliToml *value, const char *key, size_t length)
{ return own_child(chali_json_find(value ? value->value : NULL, key, length)); }
/**
 * @brief 向 C 输出缓冲追加完整字节。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 操作或查询结果，失败使用零状态。
 */
static int append(ChaliCBuffer *buffer, const char *text, size_t length)
{ return chali_c_buffer_append(buffer, text, length) == CHALI_C_OK; }
/**
 * @brief 复用原有 JSON/TOML 字符串转义并归还临时 C 文本。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 操作或查询结果，失败使用零状态。
 */
static int quote(ChaliCBuffer *buffer, const char *text, size_t length)
{
    size_t size = 0;
    char *encoded = chali_json_quote(text, length, &size);
    int ok = encoded && append(buffer, encoded, size);
    free(encoded);
    return ok;
}
/**
 * @brief 输出裸键或有引号的 TOML 键。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param key 完整键字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 操作或查询结果，失败使用零状态。
 */
static int write_key(ChaliCBuffer *buffer, const char *key, size_t length)
{
    size_t i;
    int bare = length != 0;
    for (i = 0; i < length; i++)
    {
        unsigned char ch = (unsigned char)key[i];
        if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '-')) bare = 0;
    }
    return bare ? append(buffer, key, length) : quote(buffer, key, length);
}
/**
 * @brief 按原有规则递归输出内联节点。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
static int write_inline(ChaliCBuffer *buffer, yyjson_mut_val *value)
{
    size_t i, max;
    yyjson_mut_val *key, *item;
    if (yyjson_mut_is_obj(value))
    {
        if (!yyjson_mut_obj_size(value)) return append(buffer, "{}", 2);
        if (!append(buffer, "{ ", 2)) return 0;
        yyjson_mut_obj_foreach(value, i, max, key, item)
        {
            if ((i && !append(buffer, ", ", 2)) || !write_key(buffer, yyjson_mut_get_str(key), yyjson_mut_get_len(key)) || !append(buffer, " = ", 3) || !write_inline(buffer, item)) return 0;
        }
        return append(buffer, " }", 2);
    }
    if (yyjson_mut_is_arr(value))
    {
        if (!append(buffer, "[", 1)) return 0;
        yyjson_mut_arr_foreach(value, i, max, item) if ((i && !append(buffer, ", ", 2)) || !write_inline(buffer, item)) return 0;
        return append(buffer, "]", 1);
    }
    if (yyjson_mut_is_str(value)) return quote(buffer, yyjson_mut_get_str(value), yyjson_mut_get_len(value));
    if (yyjson_mut_is_raw(value)) return append(buffer, yyjson_mut_get_raw(value), yyjson_mut_get_len(value));
    if (yyjson_mut_is_bool(value)) return yyjson_mut_get_bool(value) ? append(buffer, "true", 4) : append(buffer, "false", 5);
    if (yyjson_mut_is_int(value))
    {
        char text[32];
        int n = snprintf(text, sizeof(text), "%lld", (long long)yyjson_mut_get_sint(value));
        return n > 0 && append(buffer, text, (size_t)n);
    }
    if (yyjson_mut_is_real(value))
    {
        double number = yyjson_mut_get_real(value);
        size_t length;
        char *text;
        int result;
        if (isnan(number)) return append(buffer, "nan", 3);
        if (isinf(number)) return number < 0 ? append(buffer, "-inf", 4) : append(buffer, "inf", 3);
        text = chali_json_format_real(number, &length);
        result = text && append(buffer, text, length);
        free(text);
        return result;
    }
    return 0;
}
/**
 * @brief 核对非空数组的全部元素是否为表。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
static int array_of_tables(yyjson_mut_val *value)
{
    size_t i, max;
    yyjson_mut_val *item;
    if (!yyjson_mut_arr_size(value)) return 0;
    yyjson_mut_arr_foreach(value, i, max, item) if (!yyjson_mut_is_obj(item)) return 0;
    return 1;
}
/**
 * @brief 先输出普通成员，再输出子表及表数组。
 * @param buffer 已初始化的 C 输出缓冲。
 * @param table 只读表节点。
 * @param path 可复用的表路径工作缓冲。
 * @return 操作或查询结果，失败使用零状态。
 */
static int write_table(ChaliCBuffer *buffer, yyjson_mut_val *table, ChaliCBuffer *path)
{
    size_t i, max;
    yyjson_mut_val *key, *value;
    yyjson_mut_obj_foreach(table, i, max, key, value)
    {
        if (yyjson_mut_is_obj(value) || array_of_tables(value)) continue;
        if (!write_key(buffer, yyjson_mut_get_str(key), yyjson_mut_get_len(key)) || !append(buffer, " = ", 3) || !write_inline(buffer, value) || !append(buffer, "\n", 1)) return 0;
    }
    yyjson_mut_obj_foreach(table, i, max, key, value)
    {
        size_t previous = path->length;
        if (previous && !append(path, ".", 1)) return 0;
        if (!write_key(path, yyjson_mut_get_str(key), yyjson_mut_get_len(key))) return 0;
        if (yyjson_mut_is_obj(value))
        {
            if (buffer->length && ((char *)buffer->data)[buffer->length - 1] == '\n' && !append(buffer, "\n", 1)) return 0;
            if (!append(buffer, "[", 1) || !append(buffer, path->data, path->length) || !append(buffer, "]\n", 2) || !write_table(buffer, value, path)) return 0;
        }
        else if (array_of_tables(value))
        {
            size_t a, n;
            yyjson_mut_val *item;
            yyjson_mut_arr_foreach(value, a, n, item)
            {
                if (buffer->length && ((char *)buffer->data)[buffer->length - 1] == '\n' && !append(buffer, "\n", 1)) return 0;
                if (!append(buffer, "[[", 2) || !append(buffer, path->data, path->length) || !append(buffer, "]]\n", 3) || !write_table(buffer, item, path)) return 0;
            }
        }
        path->length = previous;
    }
    return 1;
}
/**
 * @brief 原始根节点保留原文，子树按旧接口的 TOML 规则输出。
 * @param value 拥有值或只读输入节点。
 * @param length 输入字节数或接收字节数的指针。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
char *chali_toml_dump(const ChaliToml *value, size_t *length)
{
    ChaliCBuffer buffer, path;
    int ok;
    char *out = NULL;
    if (value && value->original)
    {
        out = malloc(value->length + 1);
        if (out)
        {
            memcpy(out, value->original, value->length + 1);
            if (length) *length = value->length;
        }
        return out;
    }
    if (!chali_toml_valid(value)) return NULL;
    chali_c_buffer_init(&buffer, 1, NULL);
    chali_c_buffer_init(&path, 1, NULL);
    ok = yyjson_mut_is_obj(value->value->root) ? write_table(&buffer, value->value->root, &path) : write_inline(&buffer, value->value->root);
    if (ok && append(&buffer, "", 1))
    {
        out = buffer.data;
        if (length) *length = buffer.length - 1;
        buffer.data = NULL;
        buffer.length = buffer.capacity = 0;
    }
    chali_c_buffer_clear(&buffer);
    chali_c_buffer_clear(&path);
    return out;
}
/**
 * @brief 递归检查空值与日期的格式互转限制。
 * @param value 拥有值或只读输入节点。
 * @param toml_output 目标为 TOML 时为真。
 * @return 操作或查询结果，失败使用零状态。
 */
static int convertible(yyjson_mut_val *value, int toml_output)
{
    size_t i, max;
    yyjson_mut_val *item, *key;
    if (toml_output ? yyjson_mut_is_null(value) : yyjson_mut_is_raw(value)) return 0;
    if (yyjson_mut_is_arr(value)) { yyjson_mut_arr_foreach(value, i, max, item) if (!convertible(item, toml_output)) return 0; }
    if (yyjson_mut_is_obj(value))
    {
        yyjson_mut_obj_foreach(value, i, max, key, item)
        {
            (void)key;
            if (!convertible(item, toml_output)) return 0;
        }
    }
    return 1;
}
/**
 * @brief 复制 JSON 对象为 TOML，拒绝任意层的空值。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_from_json(const ChaliJson *value)
{
    ChaliToml *out = chali_toml_new();
    if (!out) return NULL;
    if (!value || !yyjson_mut_is_obj(value->root))
    {
        snprintf(out->value->error, sizeof(out->value->error), "TOML 序列化要求对象根节点");
        return out;
    }
    if (!convertible(value->root, 1))
    {
        snprintf(out->value->error, sizeof(out->value->error), "TOML 不支持 null");
        return out;
    }
    chali_json_free(out->value);
    out->value = chali_json_clone(value);
    if (!out->value)
    {
        chali_toml_free(out);
        return NULL;
    }
    return out;
}
/**
 * @brief 复制 TOML 为 JSON，保持反射模型对日期节点的拒绝规则。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliJson *chali_toml_to_json(const ChaliToml *value)
{
    ChaliJson *out;
    if (value && chali_toml_valid(value) && convertible(value->value->root, 0)) return chali_json_clone(value->value);
    out = chali_json_new();
    if (out) snprintf(out->error, sizeof(out->error), "反射模型不接受 TOML 日期节点，请使用字符串字段");
    return out;
}
