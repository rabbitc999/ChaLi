/* ChaLi LLVM AOT C ABI */
#ifndef CHALI_AOT_H
#define CHALI_AOT_H
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 分配本模块的运行时内存、装入数据并执行全局初始化。
 * @note 必须先于导出函数调用，使用结束后调用 chali_finish。
 */
void chali_init(void);
/**
 * @brief 执行全局资源清理、刷新输出并释放本模块的运行时内存。
 * @note 收尾之后不能继续使用原有句柄。
 */
void chali_finish(void);
#ifdef __cplusplus
}
#endif
#endif
