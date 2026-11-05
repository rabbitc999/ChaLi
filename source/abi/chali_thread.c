/**
 * @file chali_thread.c
 * @brief 使用 Windows CRT 线程或 POSIX pthread 提供独立 C 线程服务。
 */
#include "chali_thread.h"
#include <errno.h>
#include <stdlib.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

struct ChaliThread
{
    ChaliThreadEntry entry;
    void *argument;
#ifdef _WIN32
    HANDLE native;
    unsigned identifier;
#else
    pthread_t native;
#endif
};

/**
 * @brief 在 Windows CRT 线程内调用公共 C 入口。
 * @param argument 已初始化的独占线程状态。
 * @return 线程退出码。
 */
#ifdef _WIN32
static unsigned __stdcall thread_entry(void *argument)
{
    ChaliThread *thread = (ChaliThread *)argument;
    thread->entry(thread->argument);
    return 0;
}
#else
/**
 * @brief 在 POSIX 线程内调用公共 C 入口。
 * @param argument 已初始化的独占线程状态。
 * @return 空的线程返回值。
 */
static void *thread_entry(void *argument)
{
    ChaliThread *thread = (ChaliThread *)argument;
    thread->entry(thread->argument);
    return NULL;
}
#endif

/**
 * @brief 分配独占状态并启动平台线程，失败时释放所有本层资源。
 * @param entry C 线程入口。
 * @param argument 入口参数。
 * @param output 接收句柄。
 * @return 平台错误码，成功为零。
 */
int chali_thread_create(ChaliThreadEntry entry, void *argument, ChaliThread **output)
{
    if (!output) return EINVAL;
    *output = NULL;
    if (!entry) return EINVAL;
    ChaliThread *thread = (ChaliThread *)malloc(sizeof(*thread));
    if (!thread) return ENOMEM;
    thread->entry = entry;
    thread->argument = argument;
#ifdef _WIN32
    thread->native = (HANDLE)_beginthreadex(NULL, 0, thread_entry, thread, 0, &thread->identifier);
    const int error = thread->native ? 0 : (errno ? errno : EAGAIN);
#else
    const int error = pthread_create(&thread->native, NULL, thread_entry, thread);
#endif
    if (error)
    {
        free(thread);
        return error;
    }
    *output = thread;
    return 0;
}

/**
 * @brief 等待平台线程退出并释放相应的系统资源。
 * @param thread 独占线程状态。
 * @return 平台错误码，成功为零。
 */
int chali_thread_join(ChaliThread *thread)
{
    if (!thread) return EINVAL;
#ifdef _WIN32
    if (GetCurrentThreadId() == thread->identifier) return EDEADLK;
    if (WaitForSingleObject(thread->native, INFINITE) != WAIT_OBJECT_0) return EINVAL;
    if (!CloseHandle(thread->native)) return EINVAL;
#else
    const int error = pthread_join(thread->native, NULL);
    if (error) return error;
#endif
    free(thread);
    return 0;
}
