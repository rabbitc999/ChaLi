/**
 * @file chali_native_tools.hpp
 * @brief 原生工具的私有工作区、参数数组执行和成功产物发布。
 */
#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace chali_native_tools
{
/** @brief 拥有根 tmp 下单次原生工具的临时目录，不允许复制所有权。 */
struct Workspace
{
    std::filesystem::path path;  ///< 本对象独占的工作目录。
    /** @brief 创建独占目录；失败抛出带原因的异常。 */
    Workspace();
    /** @brief 回收本对象创建的目录，不跟随目录符号链接。 */
    ~Workspace();
    /** @brief 禁止复制工作区所有权。 */
    Workspace(const Workspace &) = delete;
    /** @brief 禁止赋值工作区所有权。 */
    Workspace &operator=(const Workspace &) = delete;
};
/**
 * @brief 用参数数组运行工具，合并诊断并等待或有界终止子进程。
 * @param arguments 包含工具路径的完整参数。
 * @param log 本次工作区中的诊断日志路径。
 * @param diagnostic 接收最多 128 KiB 的诊断。
 * @return 子进程退出码；启动失败或超时返回一。
 */
int execute(const std::vector<std::string> &arguments, const std::filesystem::path &log, std::string &diagnostic);
/**
 * @brief 在目标同目录暂存后原子替换，Windows 占用有界等待五秒。
 * @param source 已经生成成功的工作区文件。
 * @param destination 用户请求的产物路径。
 * @param error 接收复制或替换失败原因。
 * @return 成功替换为真；失败保留原产物并删除本次暂存文件。
 */
bool publish(const std::filesystem::path &source, const std::filesystem::path &destination, std::string &error);
}  // namespace chali_native_tools
