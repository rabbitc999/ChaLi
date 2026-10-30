#pragma once
#include "allocator/chali_allocator.hpp"
#include <cstddef>
#include <cstdint>
#include <iosfwd>

namespace chali_precompiled
{
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
}  // namespace chali_precompiled
