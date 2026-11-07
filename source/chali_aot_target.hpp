/**
 * @file chali_aot_target.hpp
 * @brief 受支持 LLVM 目标及目标运行时的解析，不把构建宿主当作产物平台。
 */
#pragma once
#include "allocator/chali_allocator.hpp"

namespace chali_build
{
/**
 * @brief 一个已经验证的六十四位小端 AOT 目标。
 */
struct TargetInfo
{
    Memory::String triple;   ///< LLVM 的规范目标三元组。
    Memory::String runtime;  ///< 内嵌的目标运行时 LLVM 模块。
    Memory::String cpu;      ///< 默认 CPU 名称。
    bool windows = false;    ///< 使用 Windows CRT、COFF 和 PE 产物。
    bool arm64 = false;      ///< 使用 AArch64 指令集。
    bool native = false;     ///< 三元组与当前编译器宿主一致。
    Memory::String dataLayout; ///< 与目标运行时一致的数据布局，用于独立实现兼容检查。
};

/**
 * @brief 取得编译器实际运行宿主对应的 LLVM 三元组。
 * @return 已登记的宿主三元组；未知宿主返回空串。
 */
Memory::String hostTarget();

/**
 * @brief 解析受支持的目标，空输入使用宿主目标。
 * @param requested 规范三元组或 linux-x64、linux-arm64、windows-x64 别名。
 * @param target 接收完整目标信息。
 * @param error 接收未知目标诊断，可为空。
 * @return 目标有对应运行时和布局时为 true。
 */
bool describeTarget(const Memory::String &requested, TargetInfo &target, Memory::String *error = nullptr);

/**
 * @brief 检查 Windows PE 子系统选项。
 * @param subsystem 为空或为 console、windows。
 * @param executable 是否链接 Windows 可执行文件。
 * @return 选项可用时返回空串，否则返回诊断。
 */
Memory::String windowsSubsystemError(const Memory::String &subsystem, bool executable);
}  // namespace chali_build
