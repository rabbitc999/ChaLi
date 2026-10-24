/**
 * @file chali_aot_runtime_assets.hpp
 * @brief 两种 LLVM 后端共用的原生服务静态链接资产。
 */
#pragma once
#include "chali_aot.hpp"
#include <filesystem>

namespace chali_aot
{
/**
 * @brief 单次编译私有的运行时链接目录，离开作用域时回收。
 */
struct RuntimeAssets
{
    std::filesystem::path root;
    /**
     * @brief 创建独立临时目录，避免并行构建互相覆盖。
     */
    RuntimeAssets();
    /**
     * @brief 仅删除本对象创建的目录及其内容。
     */
    ~RuntimeAssets();
    /**
     * @brief 还原构建时内嵌的原生服务与静态链接依赖。
     * @param diagnostics 接收缺失或损坏资产的诊断。
     * @return 所有文件完整恢复时返回 true。
     */
    bool restore(Memory::Vector<Memory::String> *diagnostics);
    /**
     * @brief 用内嵌胶水头生成复杂 C ABI 的宿主封送目标文件。
     * @param source 前端生成的类型定义与绑定函数。
     * @param object 成功时接收本目录内的目标文件路径。
     * @param diagnostics 接收编译器的完整失败信息。
     * @return 文件恢复和宿主编译均成功时返回 true。
     */
    bool prepareAbi(const Memory::String &source, Memory::String *object, Memory::Vector<Memory::String> *diagnostics);
};
}  // namespace chali_aot
