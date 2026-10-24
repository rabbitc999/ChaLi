/**
 * @file chali_aot_llvm.hpp
 * @brief 可选静态 LLVM 与 LLD 后端的隔离接口。
 */
#pragma once
#include "chali_aot.hpp"

namespace chali_aot::builtin
{
/**
 * @brief 查询当前编译器是否实际链接了 LLVM 与 LLD 静态库。
 * @return 编入后端时为 true，外置工具变体为 false。
 */
bool available();

/**
 * @brief 使用进程内 LLVM 优化并生成对象，再使用进程内 LLD 链接。
 * @param request AOT 编译选项及额外链接库。
 * @param module 已写入磁盘的 LLVM 模块路径。
 * @param product 目标可执行文件或共享库路径。
 * @param diagnostics 接收 LLVM 与链接诊断。
 * @return 成功为零，失败为非零。
 */
int compile(const Request &request, const Memory::String &module, const Memory::String &product,
            Memory::Vector<Memory::String> *diagnostics);
}  // namespace chali_aot::builtin
