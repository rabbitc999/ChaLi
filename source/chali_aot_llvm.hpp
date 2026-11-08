/**
 * @file chali_aot_llvm.hpp
 * @brief 可选静态 LLVM 与 LLD 后端的隔离接口。
 */
#pragma once
#include "chali_aot.hpp"
#include "chali_linker.hpp"

namespace chali_build::builtin
{
/**
 * @brief 查询当前编译器是否实际链接了 LLVM 与 LLD 静态库。
 * @return 编入后端时为 true，外置工具变体为 false。
 */
bool available();
/** @brief 返回实际静态 LLVM 的精确版本，外置变体返回空串。
 * @return 静态 LLVM 版本或空串。 */
Memory::String version();

/**
 * @brief 使用进程内 LLD 链接已检查的原生输入。
 * @param request 对象、目标及运行时配置。
 * @param product 工作区产物路径。
 * @param diagnostics 接收链接错误。
 * @return 成功为零，失败为一。
 */
int link(const chali_linker::Request &request, const Memory::String &product, Memory::Vector<Memory::String> *diagnostics);

/**
 * @brief 使用进程内 LLVM 优化并生成对象，再使用进程内 LLD 链接。
 * @param request BUILD 编译选项及额外链接库。
 * @param module LLVM 模块的产物路径，用于链接期产物命名。
 * @param source 直接生成的 LLVM 代码，静态后端直接从内存解析。
 * @param product 目标可执行文件或共享库路径。
 * @param diagnostics 接收 LLVM 与链接诊断。
 * @return 成功为零，失败为非零。
 */
int compile(const Request &request, const Memory::String &module, const Memory::String &source, const Memory::String &product,
            Memory::Vector<Memory::String> *diagnostics);
}  // namespace chali_build::builtin
