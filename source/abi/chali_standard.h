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
#ifdef __cplusplus
}
#endif
#endif
