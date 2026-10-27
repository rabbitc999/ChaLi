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

/**
 * @brief 直通调用可见的运行时句柄，只持有上下文指针。
 */
struct ChaliAbiRuntime
{
    stackvm::runtimeDetail::NativeContext *context = nullptr;  ///< 本次调用借用的运行时上下文。
};
