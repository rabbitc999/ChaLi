/**
 * @file abi_serialization.c
 * @brief JSON/TOML 互转的纯 C 帧接口，按需登记独立模块。
 */
#include "chali_standard.h"
#include "../runtime_c/chali_json.h"
#include "../runtime_c/chali_toml.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief 复制带成功标记的独占结果，释放临时包后再返回语言句柄。
 * @param frame 当前 C 帧。
 * @param success 成功时为真。
 * @param text 结果或错误字节。
 * @param length 明确字节数。
 */
static void return_packet(ChaliAbiFrame *frame, int success, const char *text, size_t length)
{
    char *packet;
    if (length > SIZE_MAX - 2 || !(packet = malloc(length + 2)))
    {
        chali_abi_fail(frame, "格式转换内核分配失败");
        return;
    }
    packet[0] = success ? '1' : '0';
    if (length) memcpy(packet + 1, text, length);
    packet[length + 1] = 0;
    chali_abi_return_integer(frame, chali_abi_string(frame, packet, length + 1));
    free(packet);
}
/**
 * @brief 解析独立 C 树，保持旧转换的空值、日期和对象根限制。
 * @param frame 文本句柄、输出格式布尔值和返回槽。
 */
static void CHALI_ABI_CALL transcode_entry(ChaliAbiFrame *frame)
{
    int64_t handle = 0, toml_output = 0;
    const unsigned char *input = NULL;
    size_t length = 0, output_length = 0;
    ChaliJson *json = NULL;
    ChaliToml *toml = NULL;
    char *output = NULL;
    const char *error = NULL;
    if (!chali_abi_argument_integer(frame, 0, &handle) || !chali_abi_argument_integer(frame, 1, &toml_output) || !chali_abi_bytes(frame, handle, &input, &length)) return;
    if (toml_output)
    {
        json = chali_json_parse((const char *)input, length);
        if (!json)
            error = "格式转换内核分配失败";
        else if (!chali_json_valid(json) || chali_json_kind(json) != 6)
            error = "TOML 序列化要求对象根节点";
        else
        {
            toml = chali_toml_from_json(json);
            if (!toml)
                error = "格式转换内核分配失败";
            else if (!chali_toml_valid(toml))
                error = chali_toml_error(toml);
            else
                output = chali_toml_dump(toml, &output_length);
        }
    }
    else
    {
        toml = chali_toml_parse((const char *)input, length);
        if (!toml)
            error = "格式转换内核分配失败";
        else if (!chali_toml_valid(toml))
            error = "TOML 文本格式无效";
        else
        {
            json = chali_toml_to_json(toml);
            if (!json)
                error = "格式转换内核分配失败";
            else if (!chali_json_valid(json))
                error = chali_json_error(json);
            else
                output = chali_json_dump(json, &output_length);
        }
    }
    if (!error && !output) error = "格式转换内核分配失败";
    if (error)
        return_packet(frame, 0, error, strlen(error));
    else
        return_packet(frame, 1, output, output_length);
    free(output);
    chali_json_free(json);
    chali_toml_free(toml);
}
/**
 * @brief 返回格式转换的只读入口表。
 * @param count 接收入口数量，可为空。
 * @return 进程存活期间有效的表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_serialization_records(size_t *count)
{
    static const ChaliAbiRecord records[] = { { "std::serialization::__transcode", transcode_entry, 2 } };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
#ifndef GLUE_NO_STD_JSON
#define CHALI_BUILD_BIND_MODULE serialization
#include "aot_bind_module.h"
#endif
