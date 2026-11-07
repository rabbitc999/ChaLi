/**
 * @file aot_thread_services.hpp
 * @brief AOT 按需接入的线程、消息与事件服务契约。
 */
#pragma once
#include "runtime_internal.hpp"
#include "../abi/chali_thread_service.h"

namespace runtime::build
{
using runtimeDetail::NativeContext;

/**
 * @brief 同一原生线程的短期服务状态，共用一个 TLS 槽以减少重复寻址。
 * @note 各线程仍独立保存上下文、浮点返回标记、回调深度及清理状态。
 */
struct NativeThreadState
{
    NativeContext *activeContext;
    execution_detail::DataCapacityIndex *activeDataCapacity;
    std::size_t callbackDepth;
    std::uint64_t logicalSteps;
    unsigned char nativeResultFloat;
    bool nativeCleanup;
};
#if defined(_WIN32)
#define CHALI_AOT_SERVICE_LOCAL
#else
#define CHALI_AOT_SERVICE_LOCAL __attribute__((visibility("hidden")))
#endif
extern thread_local NativeThreadState nativeThreadState CHALI_AOT_SERVICE_LOCAL;


/**
 * @brief 子线程安全点使用的协作取消信号。
 */
struct NativeCancelled
{
};

/**
 * @brief 原生子线程主动正常退出时保留的状态码。
 */
struct NativeExit
{
    int code;
};


}
