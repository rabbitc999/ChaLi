/** @file abi_process.c @brief 进程管道与系统调用的薄 C 接口。 */
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "chali_standard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief 独占一个可读命令管道，关闭后保留空记录直到 drop。 */
typedef struct ProcessRecord
{
    FILE *pipe;
} ProcessRecord;

/**
 * @brief 关闭平台进程管道并收集退出状态。
 * @param pipe 有效的可读管道。
 * @return 平台退出状态，保持 pclose 的原始约定。
 */
static int closePipe(FILE *pipe)
{
#ifdef _WIN32
    return _pclose(pipe);
#else
    return pclose(pipe);
#endif
}

/**
 * @brief 关闭剩余管道并释放独占记录。
 * @param value 由本模块分配的记录。
 */
static void CHALI_ABI_CALL destroyProcess(void *value)
{
    ProcessRecord *record = (ProcessRecord *)value;
    if (record->pipe) closePipe(record->pipe);
    free(record);
}

/**
 * @brief 复制命令字节并补终止零，不跨宿主调用借用语言缓冲区。
 * @param frame 包含一个 String 参数的调用帧。
 * @return 调用方以 free 释放的 C 字符串；失败为空。
 */
static char *copyCommand(ChaliAbiFrame *frame)
{
    int64_t handle = 0;
    const unsigned char *bytes = NULL;
    size_t length = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle) || !chali_abi_bytes(frame, handle, &bytes, &length)) return NULL;
    if (length == SIZE_MAX)
    {
        chali_abi_fail(frame, "命令文本过长");
        return NULL;
    }
    char *text = (char *)malloc(length + 1);
    if (!text)
    {
        chali_abi_fail(frame, "命令文本分配失败");
        return NULL;
    }
    if (length) memcpy(text, bytes, length);
    text[length] = 0;
    return text;
}

/**
 * @brief 启动可读命令并将管道所有权交给宿主记录服务。
 * @param frame 包含命令文本，返回 Process 的逻辑句柄。
 */
static void CHALI_ABI_CALL processRun(ChaliAbiFrame *frame)
{
    char *command = copyCommand(frame);
    if (!command) return;
#ifdef _WIN32
    FILE *pipe = _popen(command, "r");
#else
    FILE *pipe = popen(command, "r");
#endif
    if (!pipe)
    {
        const char *prefix = "启动命令失败：";
        size_t prefixSize = strlen(prefix), length = strlen(command);
        char *message = length <= SIZE_MAX - prefixSize - 1 ? (char *)malloc(prefixSize + length + 1) : NULL;
        if (message)
        {
            memcpy(message, prefix, prefixSize);
            memcpy(message + prefixSize, command, length + 1);
        }
        chali_abi_fail(frame, message ? message : prefix);
        free(message);
        free(command);
        return;
    }
    free(command);
    ProcessRecord *record = (ProcessRecord *)malloc(sizeof(*record));
    if (!record)
    {
        closePipe(pipe);
        chali_abi_fail(frame, "进程记录分配失败");
        return;
    }
    record->pipe = pipe;
    int64_t handle = chali_abi_record_make(frame, "std::process::Process", record, destroyProcess);
    if (!handle) destroyProcess(record);
    chali_abi_return_integer(frame, handle);
}

/**
 * @brief 读取最多 511 个字节，去除尾部 CR／LF，EOF 返回空文本。
 * @param frame 包含 Process 逻辑句柄，返回独立拥有的字符串。
 */
static void CHALI_ABI_CALL processRead(ChaliAbiFrame *frame)
{
    int64_t handle = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    ProcessRecord *record = (ProcessRecord *)chali_abi_record_borrow(frame, handle, "std::process::Process");
    if (!record) return;
    if (!record->pipe)
    {
        chali_abi_fail(frame, "pread 用到的进程句柄无效或已经关闭");
        return;
    }
    char buffer[512] = { 0 };
    size_t length = 0;
    if (fgets(buffer, sizeof(buffer), record->pipe))
    {
        length = strlen(buffer);
        while (length && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r')) --length;
    }
    chali_abi_return_integer(frame, chali_abi_string(frame, buffer, length));
}

/**
 * @brief 显式关闭管道，保留空记录以供稍后 drop。
 * @param frame 包含 Process 逻辑句柄，返回平台退出状态。
 */
static void CHALI_ABI_CALL processClose(ChaliAbiFrame *frame)
{
    int64_t handle = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    ProcessRecord *record = (ProcessRecord *)chali_abi_record_borrow(frame, handle, "std::process::Process");
    if (!record) return;
    if (!record->pipe)
    {
        chali_abi_fail(frame, "pclose 用到的进程句柄无效或已经关闭");
        return;
    }
    int result = closePipe(record->pipe);
    record->pipe = NULL;
    chali_abi_return_integer(frame, result);
}

/**
 * @brief 严格释放 Process 记录，未关闭的管道由销毁回调关闭。
 * @param frame 包含记录的逻辑句柄。
 */
static void CHALI_ABI_CALL processDrop(ChaliAbiFrame *frame)
{
    int64_t handle = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    if (chali_abi_record_release(frame, handle)) chali_abi_return_integer(frame, 0);
}

/**
 * @brief 调用系统命令并返回平台退出状态。
 * @param frame 包含命令文本，返回 system 的原始结果。
 */
static void CHALI_ABI_CALL processSystem(ChaliAbiFrame *frame)
{
    char *command = copyCommand(frame);
    if (!command) return;
    int result = system(command);
    free(command);
    chali_abi_return_integer(frame, result);
}

/**
 * @brief 取得进程服务的只读 C 函数表。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的入口表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_process_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::process::__processRun", processRun, 1 },
        { "std::process::__processRead", processRead, 1 },
        { "std::process::__processClose", processClose, 1 },
        { "std::process::__processDrop", processDrop, 1 },
        { "std::process::__system", processSystem, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}

#ifndef GLUE_NO_STD_PROCESS
#define CHALI_AOT_BIND_MODULE process
#include "aot_bind_module.h"
#endif
