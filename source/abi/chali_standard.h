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
/**
 * @brief 取得图片拥有记录、编解码及像素操作的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_image_records(size_t *count);
/**
 * @brief 取得同步及异步 TCP 与连接资源的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_net_records(size_t *count);
/**
 * @brief 取得数据库驱动、拥有记录和结果访问的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_sql_records(size_t *count);
/**
 * @brief 取得 JSON 元数据 SQL 生成的 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_sql_ipage_records(size_t *count);
/**
 * @brief 取得大整数与高精度小数的 C 入口表。
 * @param count 接收表项数，可为空。
 * @return 进程存活期间有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_big_number_records(size_t *count);
/**
 * @brief 取得符号表达式的 C 入口表。
 * @param count 接收表项数，可为空。
 * @return 进程存活期间有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_cas_records(size_t *count);
/**
 * @brief 取得集合存储、迭代租约与程序参数的 C 入口表。
 * @param count 接收表项数，可为空。
 * @return 进程期有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_collection_records(size_t *count);
/**
 * @brief 取得宿主演示的 C 入口表。
 * @param count 接收表项数量，可为空。
 * @return 进程期有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_demo_records(size_t *count);
/**
 * @brief 取得演示类型及接口的普通 ChaLi 声明。
 * @param length 接收字节数，可为空。
 * @return 只读语言源码，不由 C++ 模板构造。
 */
const char *CHALI_ABI_CALL abi_demo_declarations(size_t *length);
/**
 * @brief 依既有完整标识符规则判断是否需要注入演示声明。
 * @param source 待编译源码，可为空。
 * @param length 源码字节数。
 * @return 至少使用一个演示声明名时为一。
 */
int CHALI_ABI_CALL abi_demo_declarations_used(const char *source,size_t length);
#ifdef __cplusplus
}
#endif
#endif
