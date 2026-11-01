/** @file chali_assembler.hpp
 * @brief 目标机器汇编的统一命令和内置 LLVM MC 接口。
 */
#pragma once
#include "chali_aot.hpp"
#include <string>
namespace chali_assembler
{
/**
 * @brief 将 GAS 风格的 .s 文件汇编为对象，失败时保留原产物。
 * @param input 原始汇编路径，不执行 .S 预处理。
 * @param output 完整对象文件路径。
 * @param options 目标、CPU、LLVM 驱动、包含路径和调试选项。
 * @param diagnostics 接收原始源码位置及工具诊断。
 * @return 成功为零，失败非零。
 */
int assemble(const Memory::String &input, const Memory::String &output, const chali_aot::Options &options, Memory::Vector<Memory::String> *diagnostics);
/**
 * @brief 执行独立汇编命令，不经过语言解析器。
 * @param argc 归一化参数数量。
 * @param argv 归一化参数数组。
 * @return 成功为零、汇编失败为一、参数错误为二。
 */
int runCli(int argc, char **argv);
namespace builtin
{
/**
 * @brief 使用进程内 LLVM MC 汇编，输出至调用方独占的工作区。
 * @param input 汇编源文件绝对路径。
 * @param output 工作区内对象路径。
 * @param options 目标、CPU、包含路径及调试设置。
 * @param diagnostic 接收汇编诊断。
 * @return 成功为零，失败为一。
 */
int assemble(const std::string &input, const std::string &output, const chali_aot::Options &options, std::string &diagnostic);
}  // namespace builtin
}  // namespace chali_assembler
