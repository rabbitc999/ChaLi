/** @file chali_standard.h @brief C 宿主的标准库宿主入口的集中注册。 */
#ifndef CHALI_STANDARD_H
#define CHALI_STANDARD_H
#include "chali_abi.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 登记当前构建启用的标准库模块，复用同一函数表。
 * @param registry 未封存的 C 注册句柄。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；失败后应销毁该初始化中的表，不继续调用部分标准库。
 */
int32_t CHALI_ABI_CALL chali_abi_bind_standard(ChaliAbiRegistry *registry, ChaliAbiError *error);
/**
 * @brief 取得进程管道与系统调用的只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_process_records(size_t *count);
/**
 * @brief 取得 lztext 文本压缩的只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_compress_records(size_t *count);
/**
 * @brief 取得 JSON 拥有值及序列化的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_json_records(size_t *count);
/**
 * @brief 取得 TOML 拥有值的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_toml_records(size_t *count);
/**
 * @brief 取得正则模式的只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_regex_records(size_t *count);
/**
 * @brief 取得文件系统与 UTC 时钟的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_filesystem_records(size_t *count);
/**
 * @brief 取得文件打开、读写、定位及释放的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_file_records(size_t *count);
/**
 * @brief 取得线性代数求解与分解的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_linalg_records(size_t *count);
/**
 * @brief 取得数值缓冲及全部基础类型访问的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_scalar_records(size_t *count);
#ifdef __cplusplus
}
#endif
#endif
