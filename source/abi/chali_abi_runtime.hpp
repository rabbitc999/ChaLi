/**
 * @file chali_abi_runtime.hpp
 * @brief 直通运行时句柄的完整定义，仅供 C++ 编译单元使用。
 *
 * `chali_abi.h` 里 `ChaliAbiRuntime` 只做前向声明，好让 C 胶水不依赖任何 C++ 类型；
 * 桥接层与原生分派在这里取得完整布局，从而构造帧并交给 C 入口。
 */
#pragma once
#include "chali_abi.h"

#include "ircode/runtime_context.hpp"
#include <string>

/**
 * @brief 直通调用上下文及不会越过 C 边界的首个错误。
 */
struct ChaliAbiRuntime
{
    stackvm::runtimeDetail::NativeContext *context = nullptr;  ///< 本次调用借用的运行时上下文。
    int32_t status = ChaliAbiOk;                               ///< 首个失败状态；每次外层调用独立持有。
    std::string diagnostic;                                    ///< 完整诊断，使用宿主堆而非语言临时 arena。
    const char *fallback = nullptr;                            ///< 诊断分配失败时仍可报告的静态文本。
    bool inheritedError = false;                               ///< 已有运行期或内存故障不能被模块诊断覆盖。
    bool explicitError = false;                                ///< 保留首条显式 C 模块诊断。
};

/**
 * @brief 保存首条错误，分配失败时使用静态后备诊断，永不抛异常。
 * @param frame 本次调用帧。
 * @param message 首条失败文本。
 */
void chali_abi_mark_failure(const ChaliAbiFrame *frame, const char *message) noexcept;
