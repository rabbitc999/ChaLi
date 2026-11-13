/**
 * @file chali_aot_target.hpp
 * @brief 受支持 LLVM 目标及目标运行时的解析，不把构建宿主当作产物平台。
 */
#pragma once
#include "allocator/chali_allocator.hpp"

namespace chali_build
{
/**
 * @brief 编译期目标宏与布尔构建选项的进程内上下文。
 */
struct MacroEnvironment
{
    Memory::String target;
    Memory::Set<Memory::String> flags;
};
/**
 * @brief 获取当前编译任务的目标宏上下文。
 * @return 可配置的上下文。
 */
MacroEnvironment &macroEnvironment();
/**
 * @brief 检查仅表示存在性的构建选项名称。
 * @param name 选项名称，不接受等号与取值。
 * @return 名称有效时为真。
 */
bool validBuildOption(const Memory::String &name);
/**
 * @brief 查询目标平台宏，处理目标三元组而非宿主或 CPU 调优名称。
 * @param cpu 为真时查询架构，否则查询操作系统。
 * @return 归一化名称；不能识别时返回 unknown。
 */
Memory::String targetMacroName(bool cpu);

/**
 * @brief 一个已登记的小端 BUILD 目标。
 */
struct TargetInfo
{
    Memory::String triple;      ///< LLVM 的规范目标三元组。
    Memory::String runtime;     ///< 内嵌的目标运行时 LLVM 模块。
    Memory::String cpu;         ///< 默认 CPU 名称。
    bool x86 = false;           ///< 使用 32 位 x86 指令集和 ABI。
    bool riscv64 = false;       ///< 使用 RV64GC 与 LP64D ABI。
    bool mipsel = false;        ///< 使用小端 MIPS32r2 与 O32 ABI。
    bool windows = false;       ///< 使用 Windows CRT、COFF 和 PE 产物。
    bool arm64 = false;         ///< 使用 AArch64 指令集。
    bool native = false;        ///< 三元组与当前编译器宿主一致。
    Memory::String dataLayout;  ///< 与目标运行时一致的数据布局，用于独立实现兼容检查。

    /** @brief 是否为 RV32GC / ILP32D Linux 目标。 */
    bool riscv32() const { return triple == "riscv32-unknown-linux-gnu"; }
    /** @brief 是否使用 RISC-V Linux 硬浮点 ABI。 */
    bool riscv() const { return riscv64 || riscv32(); }
    /** @brief 是否为 ARMv7 Linux 硬浮点目标。 */
    bool arm32() const { return triple == "armv7-unknown-linux-gnueabihf"; }
    /** @brief 是否为 MIPS64 小端 Linux N64 目标。 */
    bool mips64el() const { return triple == "mips64el-unknown-linux-gnuabi64"; }
    /** @brief 目标 C ABI 的指针和 size_t 是否使用 32 位。 */
    bool pointer32() const { return x86 || mipsel || arm32() || riscv32() || mcu(); }
    /** @brief 是否为 Cortex-M4 裸机目标。 */
    bool cortexM4() const { return triple == "thumbv7em-none-eabi"; }
    /** @brief 是否为 RV32 裸机目标。 */
    bool mcuRv32() const { return triple == "riscv32-unknown-elf"; }
    /** @brief 是否使用板级运行时。 */
    bool mcu() const { return cortexM4() || mcuRv32(); }
};

/**
 * @brief 取得编译器实际运行宿主对应的 LLVM 三元组。
 * @return 已登记的宿主三元组；未知宿主返回空串。
 */
Memory::String hostTarget();

/**
 * @brief 解析受支持的目标，空输入使用宿主目标。
 * @param requested 规范三元组或已登记的平台别名。
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
