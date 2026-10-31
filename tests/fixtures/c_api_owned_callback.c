/** @file c_api_owned_callback.c @brief 真实 C 库调用语言闭包并返回需释放的文本。 */
#include "chali_abi.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define API __declspec(dllexport)
#else
#define API __attribute__((visibility("default")))
#endif
static int64_t released = 0;
/**
 * @brief 执行同步 C 跳板并把结果格式化为拥有文本。
 * @param callback 编译器提供的真实 C 跳板。
 * @param value 实参。
 * @return 调用方必须经 release_owned 释放的文本。
 */
API char *CHALI_ABI_CALL owned_after(int64_t (CHALI_ABI_CALL *callback)(int64_t), int64_t value)
{
    const int64_t result = callback(value);
    char *text = (char *)malloc(32);
    if (text) snprintf(text, 32, "%lld", (long long)result);
    return text;
}
/**
 * @brief 释放返回文本并记录次数。
 * @param pointer 拥有的文本指针。
 */
API void CHALI_ABI_CALL release_owned(void *pointer) { ++released; free(pointer); }
/**
 * @brief 查询本进程的释放次数。
 * @return 成功释放的数量。
 */
API int64_t CHALI_ABI_CALL release_count(void) { return released; }
