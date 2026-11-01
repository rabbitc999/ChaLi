#pragma once
#include "allocator/chali_allocator.hpp"
#include "chali_generic.hpp"
#include <cstddef>
#include <cstdint>
#include <iosfwd>

namespace chali_precompiled
{
/** @brief 标准模块在当前合并输入中的完整连续源码区间。 */
struct ModuleSlice
{
    Memory::String name;
    std::size_t begin = 0, end = 0;
};
/**
 * @brief 按实际内嵌来源识别导入段中的标准模块，不包含用户依赖或泛型实例尾部。
 * @param text 完整前端处理结果。
 * @param files 每一行的原始文件身份。
 * @param limit 导入段末尾的字节位置。
 * @return 按原顺序排列的独立模块区间。
 */
Memory::Vector<ModuleSlice> moduleSlices(const Memory::String &text, const Memory::Vector<Memory::String> &files, std::size_t limit);
/**
 * @brief 从编译器内嵌资产恢复与当前标准库核心完全对应的解析单元。
 * @param text 标准库核心经过文本前端处理后的内容。
 * @param error 资产匹配但损坏或格式不兼容时接收诊断；未匹配时保持为空。
 * @return 恢复成功返回 true，未提供对应资产或校验失败返回 false。
 */
bool load(const Memory::String &text, Memory::String &error);
/**
 * @brief 校验指定内存包并恢复核心单元，供内嵌装载及完整性回归共用。
 * @param bytes 不含进程指针的序列化字节。
 * @param size 包的完整字节数。
 * @param checksum 构建时记录的整包校验值。
 * @param text 当前核心的内容身份。
 * @param error 包损坏或格式不兼容时接收诊断。
 * @return 匹配且装载成功时返回 true。
 */
bool loadBytes(const unsigned char *bytes, std::size_t size, std::uint64_t checksum,
               const Memory::String &text, Memory::String &error);
/**
 * @brief 在编译器构建阶段导出标准库核心二进制，不包含用户节点。
 * @param out 接收带内容身份及完整性信息的二进制包。
 * @param text 已解析的标准库核心文本。
 * @param nodes 标准库核心节点数量。
 * @return 全部写入成功时返回 true。
 */
bool write(std::ostream &out, const Memory::String &text, std::size_t nodes);
/**
 * @brief 导出当前独立标准模块的解析包和重定位来源。
 * @param out 输出流。
 * @param text 已处理的模块内容。
 * @return 写入成功时返回 true。
 */
bool writeModule(std::ostream &out, const Memory::String &text);
/**
 * @brief 按需装载与当前内容一致的标准模块。
 * @param name 完整模块名称。
 * @param text 已处理的模块内容。
 * @param error 资产损坏或来源无法重定位时接收诊断。
 * @return 成功追加时返回 true，未匹配时返回 false 且不设置错误。
 */
bool loadModule(const Memory::String &name, const Memory::String &text, Memory::String &error);
/**
 * @brief 导出标准模块未实例化模板的版本化二进制包。
 * @param out 输出流。
 * @param module 资产目录模块名称。
 * @return 写入成功时返回 true。
 */
bool writeTemplates(std::ostream &out, const Memory::String &module);
/**
 * @brief 依据原始来源和完整声明恢复标准模板，并重绑定当前宏调用编号。
 * @param module 当前语言模块。
 * @param origin 模板在内嵌源码中的原始位置。
 * @param source 当前前端归一后的完整输入。
 * @param begin 模板标记的字节位置。
 * @param end 命中时接收声明结束位置。
 * @param item 命中时接收当前会话可用的模板副本。
 * @param error 资产损坏时接收诊断。
 * @return 1 表示命中，0 表示源码回退，-1 表示资产错误。
 */
int loadTemplate(const Memory::String &module, const preprocess::LineOrigin &origin, const Memory::String &source,
                 std::size_t begin, std::size_t &end, chali_generic::Generic &item, Memory::String &error);
}  // namespace chali_precompiled
