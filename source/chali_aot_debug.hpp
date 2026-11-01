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
    std::string typeName = {};  ///< 用户类型名；空时兼容旧帧槽描述。
    std::size_t bits = 64;      ///< 实际帧槽中该值的表示位宽。
    std::string encoding = {};  ///< DWARF 基础值编码。
    std::size_t scope = 0, line = 0, column = 0;
    std::size_t block = 0;  ///< 声明生效的原生基本块，零表示函数序言。
    std::string file = {};
    bool readOnly = false;
    std::string storage = {};  ///< 非空表示原始槽只提供源码停点镜像，写入必须定位真实绑定。
};
/**
 * @brief AST 或兼容四元式节点的原始源码位置及词法范围。
 */
struct DebugLine
{
    std::string file;
    std::size_t line = 0;
    std::size_t column = 1, scope = 0;
};
/**
 * @brief 用户块及声明后区间的词法作用域；零号表示函数作用域。
 */
struct DebugScope
{
    std::size_t parent = 0;
    DebugLine origin;
};
/**
 * @brief 使用原生帧槽描述源码行和变量，不依赖解释器。
 */
class NativeDebugInfo
{
    /** @brief 单个函数的源码名、参数和局部声明，仅在调试产物中保存。 */
    struct FunctionSource
    {
        std::string name, linkage;
        DebugLine origin;
        std::vector<DebugVariable> variables;
        std::map<std::size_t, DebugScope> scopes;
    };
    /** @brief 原生基本块地址与源码位置之间的只读对应。 */
    struct PositionSource
    {
        std::size_t function = 0, block = 0;
        DebugLine position;
    };
    std::vector<FunctionSource> functions_;
    std::vector<PositionSource> positions_;
    std::size_t next_ = 1000000, unit_ = 0, empty_ = 0, integer_ = 0, real_ = 0;
    std::string metadata_;
    std::map<std::string, std::size_t> files_;
    std::map<std::string, std::size_t> types_;
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
     * @brief 使用用户词法层级和声明位置标注真实机器帧。
     * @param definition 完整 LLVM 函数定义。
     * @param name 源函数名。
     * @param linkage LLVM 符号。
     * @param lines 指令对应源码位置及作用域。
     * @param variables 变量位置和真实存储编码。
     * @param scopes 用户词法作用域树。
     * @return 带 DWARF 的函数定义。
     */
    std::string annotate(const std::string &definition, const std::string &name, const std::string &linkage,
                         const std::map<std::size_t, DebugLine> &lines, const std::vector<DebugVariable> &variables,
                         const std::map<std::size_t, DebugScope> &scopes);
    /**
     * @brief 将调试编译单元和版本标记加入 LLVM 模块。
     * @param module 完整模块文本。
     */
    void finish(std::string &module);
};
}  // namespace chali_aot
