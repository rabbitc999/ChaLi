/**
 * @file abi_serialization.cpp
 * @brief JSON 与 TOML 内核的窄 C 帧接口；字节封装由 ChaLi 实现。
 */
#include "chali_abi.h"
#include "../json/json.hpp"
#include "../toml/toml.hpp"
#include <string>
#include <stdexcept>

namespace
{
/**
 * @brief 将精确 JSON 节点递归转换为 TOML，拒绝无对应值的 null。
 * @param value JSON 节点。
 * @return 拥有独立存储的 TOML 节点。
 */
toml::Value toToml(const json::Value &value)
{
    if (value.isBool()) return toml::Value(*value.asBool());
    if (value.isInt()) return toml::Value(*value.asInt());
    if (value.isDouble()) return toml::Value(*value.asDouble());
    if (value.isString()) return toml::Value(std::string(*value.asString()));
    if (const auto *array = value.asArray())
    {
        toml::Array result;
        for (const auto &child : *array) result.push_back(toToml(child));
        return toml::Value(std::move(result));
    }
    if (const auto *object = value.asObject())
    {
        toml::Object result;
        for (const auto &member : *object) result.insert(member.key, toToml(member.value));
        return toml::Value(std::move(result));
    }
    throw std::invalid_argument("TOML 不支持 null");
}
/**
 * @brief 将 TOML 标量和复合节点递归转成 JSON；日期保留文本表示。
 * @param value TOML 节点。
 * @return 拥有独立存储的 JSON 节点。
 */
json::Value toJson(const toml::Value &value)
{
    if (value.isBool()) return json::Value(*value.asBool());
    if (value.isInt()) return json::Value(*value.asInt());
    if (value.isDouble()) return json::Value(*value.asDouble());
    if (value.isString()) return json::Value(std::string(*value.asString()));
    if (const auto *array = value.asArray())
    {
        json::Array result;
        for (const auto &child : *array) result.push_back(toJson(child));
        return json::Value(std::move(result));
    }
    if (const auto *object = value.asTable())
    {
        json::Object result;
        for (const auto &member : *object) result.insert(member.key, toJson(member.value));
        return json::Value(std::move(result));
    }
    throw std::invalid_argument("反射模型不接受 TOML 日期节点，请使用字符串字段");
}
/**
 * @brief 在 JSON 和 TOML 文本之间转换并保留失败状态。
 * @param input 输入文本。
 * @param tomlOutput 为 true 时输出 TOML，否则输出 JSON。
 * @return 首字节为成功标记 1 或失败标记 0，后续为内容或诊断。
 */
std::string transcode(std::string input, bool tomlOutput)
{
    try
    {
        std::string output;
        if (tomlOutput)
        {
            auto parsed = json::parse(input.c_str());
            if (!parsed.ok() || !parsed.value->isObject()) throw std::invalid_argument("TOML 序列化要求对象根节点");
            output = toToml(*parsed.value).dump();
        }
        else
        {
            auto parsed = toml::parse(input.c_str());
            if (!parsed.ok()) throw std::invalid_argument("TOML 文本格式无效");
            output = toJson(*parsed.value).dump();
        }
        return std::string(("1" + output).c_str());
    } catch (const std::exception &failure) { return std::string((std::string("0") + failure.what()).c_str()); }
}
}  // namespace

/**
 * @brief 读取独立文本后调用格式内核，返回拥有型语言字符串。
 * @param frame 包含 String 句柄与输出格式布尔槽的当前调用帧。
 */
static void CHALI_ABI_CALL transcodeEntry(ChaliAbiFrame *frame) try
{
    int64_t handle = 0;
    int64_t tomlOutput = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle) ||
        !chali_abi_argument_integer(frame, 1, &tomlOutput)) return;
    const unsigned char *bytes = nullptr;
    size_t length = 0;
    if (!chali_abi_bytes(frame, handle, &bytes, &length)) return;
    const auto output = transcode(std::string(reinterpret_cast<const char *>(bytes), length), tomlOutput != 0);
    chali_abi_return_integer(frame, chali_abi_string(frame, output.data(), output.size()));
}
catch (const std::exception &error)
{
    chali_abi_fail(frame, error.what());
}
catch (...)
{
    chali_abi_fail(frame, "格式转换内核失败");
}

/**
 * @brief 取得格式转换的只读 C 函数表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_serialization_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        {"std::serialization::__transcode", transcodeEntry, 2},
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
