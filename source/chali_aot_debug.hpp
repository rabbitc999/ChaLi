/**
 * @file chali_aot_debug.hpp
 * @brief 为原生函数生成独立的 DWARF 元数据。
 */
#pragma once
#include <map>
#include <string>
#include <vector>

namespace chali_aot
{
/**
 * @brief 源变量的实际帧槽及值类别。
 */
struct DebugVariable
{
    std::string name, pointer;
    bool floating = false;
    std::size_t argument = 0;
};
/**
 * @brief 一条四元式的原始源码位置。
 */
struct DebugLine
{
    std::string file;
    std::size_t line = 0;
};
/**
 * @brief 使用原生帧槽描述源码行和变量，不依赖解释器。
 */
class NativeDebugInfo
{
    std::size_t next_ = 1000000, unit_ = 0, empty_ = 0, integer_ = 0, real_ = 0;
    std::string metadata_;
    std::map<std::string, std::size_t> files_;
    /**
     * @brief 追加一个 LLVM 元数据节点。
     * @param value 节点正文。
     * @return 唯一节点编号。
     */
    std::size_t node(const std::string &value);
    /**
     * @brief 查询或建立源码文件元数据。
     * @param path UTF-8 源码路径。
     * @return 文件节点编号。
     */
    std::size_t file(const std::string &path);

public:
    /**
     * @brief 给一个函数附加源码位置和实际变量地址。
     * @param definition 完整 LLVM 函数定义。
     * @param name 源码函数名。
     * @param linkage LLVM 符号名。
     * @param lines 指令下标到源位置的映射。
     * @param variables 用户变量与参数槽。
     * @return 带调试描述的函数。
     */
    std::string annotate(const std::string &definition, const std::string &name, const std::string &linkage,
                         const std::map<std::size_t, DebugLine> &lines, const std::vector<DebugVariable> &variables);
    /**
     * @brief 将调试编译单元和版本标记加入 LLVM 模块。
     * @param module 完整模块文本。
     */
    void finish(std::string &module);
};
}  // namespace chali_aot
