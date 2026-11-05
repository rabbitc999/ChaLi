/**
 * @file chali_thread_service.h
 * @brief 原生线程与对象载荷适配的 C 服务边界。
 */
#ifndef CHALI_THREAD_SERVICE_H
#define CHALI_THREAD_SERVICE_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 同步接收服务结果，kind 为零整数、一浮点位模式、二 UTF-8 文本。
 * @param user 调用方状态。
 * @param kind 结果类型。
 * @param bits 数值原始位模式。
 * @param text 借用文本地址，仅在回调期间有效。
 * @param size 文本字节数。
 */
typedef void (*ChaliServiceResult)(void *user, int kind, int64_t bits, const char *text, size_t size);
/**
 * @brief 可按需链接的并发服务表，程序、上下文与分配器均通过不透明指针传递。
 */
typedef struct ChaliThreadServices
{
    void *(*create)(const void *context, const void *program, void *allocator, int enabled);
    void (*destroy)(void *machine);
    void (*finish)(void *context);
    void (*poll)(void *context);
    int (*invoke)(const void *program, void *context, const char *name, size_t name_size,
                  size_t count, const int64_t *bits, const unsigned char *kinds, size_t callback_depth,
                  ChaliServiceResult result, void *user);
} ChaliThreadServices;
/**
 * @brief 取得完整服务表。
 * @return 模块内只读服务表。
 */
const ChaliThreadServices *chali_aot_thread_services(void);
/**
 * @brief 由生成模块选择服务，未使用并发的普通程序返回空指针。
 * @return 所选服务表，旧对象默认采用完整服务。
 */
const ChaliThreadServices *chali_program_thread_services(void);
#ifdef __cplusplus
}
#endif
#endif
