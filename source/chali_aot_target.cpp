/**
 * @file chali_aot_target.cpp
 * @brief 显式登记目标布局、运行时和宿主关系。
 */
#include "chali_aot_target.hpp"

namespace chali_aot
{
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
        target.runtime = "aot/runtime_windows.ll";
    else if (triple == "x86_64-unknown-linux-gnu")
        target.runtime = "aot/runtime_linux.ll";
    else if (target.arm64)
        target.runtime = "aot/runtime_linux_arm64.ll";
    else
    {
        if (error) *error = "AOT：没有登记的目标运行时：" + triple + "；支持 windows-x64、linux-x64、linux-arm64";
        return false;
    }
    return true;
}
}  // namespace chali_aot
