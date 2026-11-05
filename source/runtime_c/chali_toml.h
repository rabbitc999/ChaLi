/**
 * @file chali_toml.h
 * @brief TOML 的独立 C 服务与旧接口兼容。
 */
#ifndef CHALI_TOML_C_H
#define CHALI_TOML_C_H
#include "chali_json.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct ChaliToml ChaliToml;
/**
 * @brief 创建未初始化的 C TOML 记录。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_new(void);
/**
 * @brief 解析 TOML 1.0，保留完整原文并恢复旧接口的数值和日期协议。
 * @param text 输入字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_parse(const char *text, size_t length);
/**
 * @brief 深复制节点、原文与错误，副本可独立关闭。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_clone(const ChaliToml *value);
/**
 * @brief 幂等关闭文档并释放原文，保留记录及诊断。
 * @param value 拥有值或只读输入节点。
 */
void chali_toml_close(ChaliToml *value);
/**
 * @brief 销毁记录及全部 C 存储。
 * @param value 拥有值或只读输入节点。
 */
void chali_toml_free(ChaliToml *value);
/**
 * @brief 查询记录是否拥有有效节点。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int chali_toml_valid(const ChaliToml *value);
/**
 * @brief 借用解析或转换诊断。
 * @param value 拥有值或只读输入节点。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_error(const ChaliToml *value);
/**
 * @brief 返回原有节点编号，其中七表示日期或时间。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int chali_toml_kind(const ChaliToml *value);
/**
 * @brief 读取数组长度或表成员数。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
size_t chali_toml_size(const ChaliToml *value);
/**
 * @brief 按原有整型或可精确转换的浮点规则取整数。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int64_t chali_toml_integer(const ChaliToml *value);
/**
 * @brief 读取数值近似。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
double chali_toml_real(const ChaliToml *value);
/**
 * @brief 读取布尔值。
 * @param value 拥有值或只读输入节点。
 * @return 操作或查询结果，失败使用零状态。
 */
int chali_toml_boolean(const ChaliToml *value);
/**
 * @brief 借用完整字符串字节。
 * @param value 拥有值或只读输入节点。
 * @param length 输入字节数或接收字节数的指针。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_text(const ChaliToml *value, size_t *length);
/**
 * @brief 借用恢复到九位小数秒的日期或时间文本。
 * @param value 拥有值或只读输入节点。
 * @param length 输入字节数或接收字节数的指针。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_date_time(const ChaliToml *value, size_t *length);
/**
 * @brief 按插入顺序借用表键。
 * @param value 拥有值或只读输入节点。
 * @param index 从零开始的元素或成员下标。
 * @param length 输入字节数或接收字节数的指针。
 * @return 借用的只读字节，记录关闭后失效；不存在时为空。
 */
const char *chali_toml_key(const ChaliToml *value, size_t index, size_t *length);
/**
 * @brief 创建指定子节点的独立快照。
 * @param value 拥有值或只读输入节点。
 * @param index 从零开始的元素或成员下标。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_at(const ChaliToml *value, size_t index);
/**
 * @brief 按完整键字节创建独立子树快照。
 * @param value 拥有值或只读输入节点。
 * @param key 完整键字节。
 * @param length 输入字节数或接收字节数的指针。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_find(const ChaliToml *value, const char *key, size_t length);
/**
 * @brief 原始根节点保留原文，子树按旧接口的 TOML 规则输出。
 * @param value 拥有值或只读输入节点。
 * @param length 输入字节数或接收字节数的指针。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
char *chali_toml_dump(const ChaliToml *value, size_t *length);
/**
 * @brief 复制 JSON 对象为 TOML，拒绝任意层的空值。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliToml *chali_toml_from_json(const ChaliJson *value);
/**
 * @brief 复制 TOML 为 JSON，保持反射模型对日期节点的拒绝规则。
 * @param value 拥有值或只读输入节点。
 * @return 独立拥有的 C 结果，失败为空；调用方负责归还。
 */
ChaliJson *chali_toml_to_json(const ChaliToml *value);
#ifdef __cplusplus
}
#endif
#endif
