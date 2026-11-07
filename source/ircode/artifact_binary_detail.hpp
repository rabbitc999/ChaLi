/**
 * @file artifact_binary_detail.hpp
 * @brief 二进制资产共用的有界整数与字符串编码。
 */
#pragma once
#include "runtime_internal.hpp"

namespace runtime
{
namespace artifact_binary
{
/**
 * @brief 写入无符号 LEB128。
 * @param out 输出。
 * @param value 数值。
 */
inline void number(Memory::String &out, std::uint64_t value)
{
    do
    {
        out += static_cast<char>((value & 127u) | (value >= 128u ? 128u : 0u));
        value >>= 7u;
    } while (value);
}
/**
 * @brief 写入长度前缀字节串。
 * @param out 输出。
 * @param value 字节串。
 */
inline void string(Memory::String &out, std::string_view value)
{
    number(out, value.size());
    out.append(value.data(), value.size());
}
/**
 * @brief 有界读取器；非法整数、截断或非规范编码使读取失败。
 */
struct Reader
{
    std::string_view bytes;
    std::size_t at = 0;
    /**
     * @brief 读取变长整数。
     * @param value 输出。
     * @return 是否成功。
     */
    bool number(std::uint64_t &value)
    {
        value = 0;
        for (unsigned shift = 0; shift < 64; shift += 7)
        {
            if (at == bytes.size()) return false;
            const auto ch = static_cast<unsigned char>(bytes[at++]);
            if (shift == 63 && ch > 1) return false;
            value |= std::uint64_t(ch & 127u) << shift;
            if (!(ch & 128u)) return shift == 0 || ch != 0;
        }
        return false;
    }
    /**
     * @brief 读取长度前缀字节串。
     * @param value 输出。
     * @return 是否成功。
     */
    bool string(Memory::String &value)
    {
        std::uint64_t size = 0;
        if (!number(size) || size > bytes.size() - at) return false;
        value.assign(bytes.data() + at, static_cast<std::size_t>(size));
        at += size;
        return true;
    }
};
}  // namespace artifact_binary
}
