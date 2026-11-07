/** @file chali_linker.hpp
 * @brief 独立原生链接和确定性静态归档的编译器内部接口。
 */
#pragma once
#include "chali_aot.hpp"
#include <string>
namespace chali_linker
{
/** @brief 一次独立原生链接，符号名使用对象文件中的原始身份。 */
struct Request
{
    chali_build::Options options;                         ///< 目标、驱动、调试、共享库、导出及目标运行时。
    Memory::Vector<Memory::String> inputs;              ///< 对象、归档及适用的动态库文件。
    Memory::Vector<Memory::String> libraryDirectories;  ///< 库搜索目录。
    Memory::Vector<Memory::String> libraryNames;        ///< 交给链接器的 -l 名称。
    Memory::String entry;                               ///< 可选原始入口符号。
    bool noRuntime = false;                             ///< 不添加 CRT 和 ChaLi 运行时的显式独立链接。
};
/**
 * @brief 校验目标并链接现有原生输入，失败时保留最终路径。
 * @param request 链接配置和输入。
 * @param output 完整最终文件名。
 * @param diagnostics 接收格式、链接及发布诊断。
 * @return 成功为零，失败为一。
 */
int link(const Request &request, const Memory::String &output, Memory::Vector<Memory::String> *diagnostics);
/**
 * @brief 将目标一致的对象封装为带符号索引的确定性自包含归档。
 * @param objects 对象文件路径，可以为空。
 * @param output 完整归档文件名。
 * @param options 目标和外置 LLVM 工具路径。
 * @param diagnostics 接收拒绝、归档和发布诊断。
 * @return 成功为零，失败为一。
 */
int archive(const Memory::Vector<Memory::String> &objects, const Memory::String &output, const chali_build::Options &options, Memory::Vector<Memory::String> *diagnostics);
/**
 * @brief 派发 link／ar 命令，不经过语言解析和执行。
 * @param argc 归一化参数数量。
 * @param argv 包含命令名的参数数组。
 * @return 成功为零、工具失败为一、选项错误为二。
 */
int runCli(int argc, char **argv);
namespace builtin
{
/**
 * @brief 用静态 LLVM 归档器生成调用方工作区内的归档。
 * @param objects 已校验的对象绝对路径。
 * @param output 工作区输出路径。
 * @param coff 是否写 MS COFF 归档，否则写 GNU 归档。
 * @param diagnostic 接收 LLVM 错误。
 * @return 成功为零，失败为一。
 */
int archive(const Memory::Vector<Memory::String> &objects, const std::string &output, bool coff, std::string &diagnostic);
}  // namespace builtin
}  // namespace chali_linker
