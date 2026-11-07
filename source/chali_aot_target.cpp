/**
 * @file chali_aot_target.cpp
 * @brief 显式登记目标布局、运行时和宿主关系。
 */
#include "chali_aot_target.hpp"

namespace chali_build
{
/**
 * @brief 保存单次命令行编译的宏上下文。
 * @return 当前进程的上下文。
 */
MacroEnvironment &macroEnvironment()
{
    static MacroEnvironment environment;
    return environment;
}
/**
 * @brief 验证布尔标志名称，禁止通过名称携带赋值。
 * @param name 构建选项名称。
 * @return 非空且由字母、数字、下划线、点或连字符组成时为真。
 */
bool validBuildOption(const Memory::String &name)
{
    if (name.empty()) return false;
    for (const unsigned char c : name)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.')) return false;
    return true;
}
/**
 * @brief 根据选定目标三元组生成平台名称，不读取 CPU 调优选项。
 * @param cpu 为真时返回 CPU 架构，否则返回操作系统。
 * @return 简短的平台或架构名称。
 */
Memory::String targetMacroName(bool cpu)
{
    auto target = macroEnvironment().target;
    if (target.empty()) target = hostTarget();
    TargetInfo normalized;
    if (describeTarget(target, normalized)) target = normalized.triple;
    if (cpu)
    {
        const auto arch = target.substr(0, target.find('-'));
        if (arch == "x86_64" || arch == "amd64" || arch == "x64") return "amd64";
        if (arch == "aarch64" || arch == "arm64") return "arm64";
        if (arch.find("arm") == 0 || arch.find("thumb") == 0) return "arm32";
        if (arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "x86") return "x86";
        return arch.empty() ? Memory::String("unknown") : arch;
    }
    if (target.find("windows") != Memory::String::npos || target.find("mingw") != Memory::String::npos) return "win";
    if (target.find("android") != Memory::String::npos) return "android";
    if (target.find("ios") != Memory::String::npos) return "ios";
    if (target.find("darwin") != Memory::String::npos || target.find("macos") != Memory::String::npos) return "mac";
    if (target.find("linux") != Memory::String::npos) return "linux";
    if (target.find("freebsd") != Memory::String::npos) return "freebsd";
    if (target.find("wasi") != Memory::String::npos) return "wasi";
    return "unknown";
}

/**
 * @brief 查询当前编译器宿主，不从目标选项推断已验证的运行平台。
 * @return 当前受支持宿主的规范三元组。
 */
Memory::String hostTarget()
{
#if defined(_WIN32) && (defined(__x86_64__) || defined(_M_X64))
    return "x86_64-w64-windows-gnu";
#elif defined(__linux__) && defined(__aarch64__)
    return "aarch64-unknown-linux-gnu";
#elif defined(__linux__) && defined(__x86_64__)
    return "x86_64-unknown-linux-gnu";
#else
    return {};
#endif
}

/**
 * @brief 将受支持别名归一化，并选择对应目标运行时。
 * @param requested 用户或清单指定的目标。
 * @param target 接收目标信息。
 * @param error 接收失败诊断。
 * @return 已登记该目标时为 true。
 */
bool describeTarget(const Memory::String &requested, TargetInfo &target, Memory::String *error)
{
    target = TargetInfo{};
    auto triple = requested.empty() ? hostTarget() : requested;
    if (triple == "windows-x64" || triple == "x86_64-pc-windows-gnu") triple = "x86_64-w64-windows-gnu";
    if (triple == "linux-x64" || triple == "x86_64-linux-gnu" || triple == "x86_64-pc-linux-gnu") triple = "x86_64-unknown-linux-gnu";
    if (triple == "linux-arm64" || triple == "aarch64-linux-gnu") triple = "aarch64-unknown-linux-gnu";
    target.triple = triple;
    target.native = triple == hostTarget();
    target.windows = triple == "x86_64-w64-windows-gnu";
    target.arm64 = triple == "aarch64-unknown-linux-gnu";
    target.cpu = target.arm64 ? "generic" : "x86-64";
    if (target.windows)
    {
        target.runtime = "aot/runtime_windows.ll";
        target.dataLayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128";
    }
    else if (triple == "x86_64-unknown-linux-gnu")
    {
        target.runtime = "aot/runtime_linux.ll";
        target.dataLayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128";
    }
    else if (target.arm64)
    {
        target.runtime = "aot/runtime_linux_arm64.ll";
        target.dataLayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32";
    }
    else
    {
        if (error) *error = "AOT：没有登记的目标运行时：" + triple + "；支持 windows-x64、linux-x64、linux-arm64";
        return false;
    }
    return true;
}

/**
 * @brief 检查 Windows PE 子系统选项。
 * @param subsystem 为空或为 console、windows。
 * @param executable 是否链接 Windows 可执行文件。
 * @return 选项可用时返回空串，否则返回诊断。
 */
Memory::String windowsSubsystemError(const Memory::String &subsystem, bool executable)
{
    if (subsystem.empty()) return {};
    if (subsystem != "console" && subsystem != "windows") return "--subsystem 需要 console 或 windows";
    if (!executable) return "--subsystem 仅用于链接 Windows 可执行文件";
    return {};
}
}  // namespace chali_build
