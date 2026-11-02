/** @file timing.c
 * @brief 复用冻结的单调时钟计时壳，通过原生标量导出调用 LTO 负载。
 */
#include "kernel.h"
/** @brief 本负载不需要额外构造数据。
 * @return 无。 */
void library_lto_setup(void) {}
#define feature_setup     library_lto_setup
#define feature_work      f_work
#define CHALI_FEATURE_AOT 1
#include "../features/timing.c"
