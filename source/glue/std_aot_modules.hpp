/**
 * @file std_aot_modules.hpp
 * @brief 按可达原生调用选择标准库宿主模块。
 */
#pragma once
#include "std_binding.hpp"
namespace std_registry
{
/**
 * @brief 用实际注册表名称匹配模块，覆盖压缩名称与别名。
 * @param names 所有可达原生调用名称。
 * @param diagnostics 绑定诊断接收器。
 * @return 必需模块名称；显式完整对照模式返回所有模块。
 */
Memory::Vector<Memory::String> aotModulesFor(const Memory::Vector<Memory::String> &names, Memory::Vector<Memory::String> *diagnostics);
}  // namespace std_registry
