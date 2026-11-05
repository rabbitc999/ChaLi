/**
 * @file chali_toml_c_reference.cpp
 * @brief 保留的编译器 TOML 实现的独立差分入口。
 */
#include "../source/toml/toml.hpp"
#include <cstdlib>
#include <cstring>
extern "C" {
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_parse 的差分参考。
 * @param text 原始 TOML 字节。
 * @param length 字节长度或接收长度的指针。
 * @return 原接口查询结果；拥有结果由对应参考释放函数归还。
 */
void *oracle_parse(const char *text, size_t length)
{
    auto parsed = toml::parse(std::string_view(text, length));
    return parsed.value ? new toml::Value(std::move(*parsed.value)) : nullptr;
}
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_free 的差分参考。
 * @param value 参考拥有值。
 */
void oracle_free(void *value) { delete static_cast<toml::Value *>(value); }
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_kind 的差分参考。
 * @param value 参考拥有值。
 * @return 原接口查询结果；拥有结果由对应参考释放函数归还。
 */
int oracle_kind(void *value)
{
    auto &v = *static_cast<toml::Value *>(value);
    return v.isBool() ? 1 : v.isInt() ? 2 :
                        v.isDouble()  ? 3 :
                        v.isString()  ? 4 :
                        v.isArray()   ? 5 :
                        v.isTable()   ? 6 :
                                        7;
}
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_size 的差分参考。
 * @param value 参考拥有值。
 * @return 原接口查询结果；拥有结果由对应参考释放函数归还。
 */
size_t oracle_size(void *value)
{
    auto &v = *static_cast<toml::Value *>(value);
    return v.isArray() ? v.asArray()->size() : v.isTable() ? v.asTable()->size() :
                                                             0;
}
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_at 的差分参考。
 * @param value 参考拥有值。
 * @param index 从零开始的下标。
 * @return 原接口查询结果；拥有结果由对应参考释放函数归还。
 */
void *oracle_at(void *value, size_t index)
{
    auto &v = *static_cast<toml::Value *>(value);
    if (auto *a = v.asArray(); a && index < a->size()) return new toml::Value((*a)[index]);
    if (auto *a = v.asTable(); a && index < a->size()) return new toml::Value((a->begin() + index)->value);
    return nullptr;
}
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_key 的差分参考。
 * @param value 参考拥有值。
 * @param index 从零开始的下标。
 * @param length 字节长度或接收长度的指针。
 * @return 原接口查询结果；拥有结果由对应参考释放函数归还。
 */
const char *oracle_key(void *value, size_t index, size_t *length)
{
    auto *a = static_cast<toml::Value *>(value)->asTable();
    *length = 0;
    if (!a || index >= a->size()) return nullptr;
    auto &text = (a->begin() + index)->key;
    *length = text.size();
    return text.data();
}
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_dump 的差分参考。
 * @param value 参考拥有值。
 * @param length 字节长度或接收长度的指针。
 * @return 原接口查询结果；拥有结果由对应参考释放函数归还。
 */
char *oracle_dump(void *value, size_t *length)
{
    auto &v = *static_cast<toml::Value *>(value);
    auto text = v.isTable() ? v.dump() : v.inlineDump();
    char *out = (char *)malloc(text.size() + 1);
    *length = text.size();
    memcpy(out, text.c_str(), text.size() + 1);
    return out;
}
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_date 的差分参考。
 * @param value 参考拥有值。
 * @param length 字节长度或接收长度的指针。
 * @return 原接口查询结果；拥有结果由对应参考释放函数归还。
 */
char *oracle_date(void *value, size_t *length)
{
    auto date = static_cast<toml::Value *>(value)->asDateTime();
    auto text = date ? date->toString() : std::string();
    char *out = (char *)malloc(text.size() + 1);
    *length = text.size();
    memcpy(out, text.c_str(), text.size() + 1);
    return out;
}
/**
 * @brief 使用未修改的编译器 TOML 实现作为 oracle_text_free 的差分参考。
 * @param value 参考拥有值。
 */
void oracle_text_free(void *value) { free(value); }
}
