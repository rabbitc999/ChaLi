/**
 * @file chali_json_c_reference.cpp
 * @brief 使用未修改的编译器 JSON 实现作为运行时迁移的差分参考。
 */
#include "../source/json/json.hpp"
#include <cstdlib>
#include <cstring>
extern "C" {
/**
 * @brief 通过原解析器创建独立参考值。
 * @param text JSON 字节。
 * @param length 输入字节数。
 * @return 参考值，解析失败为空。
 */
void *oracle_parse(const char *text, size_t length)
{
    auto result = json::parse(std::string_view(text, length));
    return result.value ? new json::Value(std::move(*result.value)) : nullptr;
}
/**
 * @brief 释放参考值。
 * @param value 参考值或空。
 */
void oracle_free(void *value) { delete static_cast<json::Value *>(value); }
/**
 * @brief 使用原有格式输出参考值。
 * @param value 有效参考值。
 * @param length 接收字节数。
 * @return 由 oracle_text_free 释放的文本。
 */
char *oracle_dump(void *value, size_t *length)
{
    auto text = static_cast<json::Value *>(value)->dump();
    auto *out = static_cast<char *>(std::malloc(text.size() + 1));
    if (!out) return nullptr;
    *length = text.size();
    std::memcpy(out, text.c_str(), text.size() + 1);
    return out;
}
/**
 * @brief 释放参考序列化文本。
 * @param value 参考文本。
 */
void oracle_text_free(char *value) { std::free(value); }
/**
 * @brief 读取原有类型编号。
 * @param value 有效参考值。
 * @return JSON 类型编号。
 */
int oracle_kind(void *value)
{
    const auto &v = *static_cast<json::Value *>(value);
    return v.isBool() ? 1 : v.isInt() ? 2 :
                        v.isDouble()  ? 3 :
                        v.isString()  ? 4 :
                        v.isArray()   ? 5 :
                        v.isObject()  ? 6 :
                                        0;
}
}
