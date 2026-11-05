/**
 * @file chali_json.h
 * @brief 运行时 JSON 的独立 C 服务；编译器 JSON 保持原实现。
 */
#ifndef CHALI_JSON_C_H
#define CHALI_JSON_C_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct ChaliJson ChaliJson;
/**
 * @brief 创建未初始化的 C 拥有值。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_new(void);
/**
 * @brief 解析独立 JSON 文档并保持原有数值及重复键协议。
 * @param text 输入字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_parse(const char *text, size_t length);
/**
 * @brief 深复制完整值和错误，副本独立于原记录的关闭。
 * @param source 只读源节点或记录。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_clone(const ChaliJson *source);
/**
 * @brief 释放记录及其文档。
 * @param value 拥有值、输入标量或待释放存储。
 */
void chali_json_free(ChaliJson *value);
/**
 * @brief 释放文档并保留可重复关闭的记录。
 * @param value 拥有值、输入标量或待释放存储。
 */
void chali_json_close(ChaliJson *value);
/**
 * @brief 查询记录是否持有有效 JSON 根节点。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int chali_json_valid(const ChaliJson *value);
/**
 * @brief 借用记录中的解析错误文本。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
const char *chali_json_error(const ChaliJson *value);
/**
 * @brief 读取零空值、一布尔、二整数、三浮点、四字符串、五数组、六对象类型。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int chali_json_kind(const ChaliJson *value);
/**
 * @brief 读取数组元素或对象成员个数。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
size_t chali_json_size(const ChaliJson *value);
/**
 * @brief 按原有整数与可精确整型浮点的转换规则读取值。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int64_t chali_json_integer(const ChaliJson *value);
/**
 * @brief 读取整数或浮点近似值。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
double chali_json_real(const ChaliJson *value);
/**
 * @brief 读取布尔值。
 * @param value 拥有值、输入标量或待释放存储。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
int chali_json_boolean(const ChaliJson *value);
/**
 * @brief 借用字符串内容及明确字节长度。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
const char *chali_json_text(const ChaliJson *value, size_t *length);
/**
 * @brief 按插入顺序借用对象键，保留嵌入零字节。
 * @param value 拥有值、输入标量或待释放存储。
 * @param index 从零起的元素或成员下标。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return 当前查询或操作的结果，失败使用空或零状态。
 */
const char *chali_json_key(const ChaliJson *value, size_t index, size_t *length);
/**
 * @brief 为下标对应子节点创建独立快照，越界返回无值记录。
 * @param value 拥有值、输入标量或待释放存储。
 * @param index 从零起的元素或成员下标。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_at(const ChaliJson *value, size_t index);
/**
 * @brief 按完整键字节创建子树独立快照。
 * @param value 拥有值、输入标量或待释放存储。
 * @param key 完整键字节。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
ChaliJson *chali_json_find(const ChaliJson *value, const char *key, size_t length);
/**
 * @brief 使用原有转义与浮点格式输出 JSON。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
char *chali_json_dump(const ChaliJson *value, size_t *length);
/**
 * @brief 把任意字节按原有 JSON 字符串协议转义。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @param output_length 接收输出的字节数。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
char *chali_json_quote(const char *value, size_t length, size_t *output_length);
/**
 * @brief 按原有十七位格式及非有限值规则输出浮点。
 * @param value 拥有值、输入标量或待释放存储。
 * @param length 明确字节数或接收结果字节数的指针。
 * @return C 堆拥有结果，失败为空；调用者负责释放。
 */
char *chali_json_format_real(double value, size_t *length);
#ifdef __cplusplus
}
#endif
#endif
