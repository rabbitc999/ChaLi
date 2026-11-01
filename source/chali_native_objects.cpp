/** @file chali_native_objects.cpp
 * @brief 使用有界读取识别 ELF／COFF 和 ar 成员，不按不可信计数分配数组。
 */
#include "chali_native_objects.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace chali_native_objects
{
namespace
{
/** @brief 一个文件内的已验证字节范围，共享流只用于同步读取。 */
struct View
{
    std::ifstream *file;           ///< 当前打开的文件，内存视图时为空。
    std::uint64_t base;            ///< 范围的物理起点。
    std::uint64_t size;            ///< 范围内字节数。
    const char *memory = nullptr;  ///< 可选的调用方原始字节，不持有所有权。
    /**
     * @brief 在范围内读取固定长度数据，拒绝截断和偏移越界。
     * @param offset 相对偏移。
     * @param output 接收字节的调用方缓冲。
     * @param count 需要读取的长度。
     */
    void read(std::uint64_t offset, void *output, std::size_t count)
    {
        if (offset > size || count > size - offset) throw std::runtime_error("对象或归档数据被截断");
        if (memory)
        {
            std::memcpy(output, memory + base + offset, count);
            return;
        }
        file->clear();
        file->seekg(static_cast<std::streamoff>(base + offset));
        file->read(static_cast<char *>(output), static_cast<std::streamsize>(count));
        if (static_cast<std::size_t>(file->gcount()) != count) throw std::runtime_error("无法读取对象或归档数据");
    }
};
/**
 * @brief 从固定小端头读取十六位字段。
 * @param data 至少两个字节的头数据。
 * @return 无符号字段。
 */
std::uint16_t word(const unsigned char *data)
{
    return std::uint16_t(data[0]) | (std::uint16_t(data[1]) << 8);
}
/**
 * @brief 解析 ar 的十进制长度字段，不接受符号、溢出或空字段。
 * @param text 固定宽度的长度字段。
 * @return 已校验的成员长度。
 */
std::uint64_t decimal(std::string text)
{
    const auto first = text.find_first_not_of(' '), last = text.find_last_not_of(' ');
    if (first == text.npos) throw std::runtime_error("归档缺少成员长度");
    std::uint64_t value = 0;
    for (auto index = first; index <= last; ++index)
    {
        const auto digit = text[index];
        if (digit < '0' || digit > '9' || value > (std::numeric_limits<std::uint64_t>::max() - unsigned(digit - '0')) / 10)
            throw std::runtime_error("归档成员长度非法或溢出");
        value = value * 10 + unsigned(digit - '0');
    }
    return value;
}
/**
 * @brief 校验一段对象或归档；成员不允许再次嵌套归档。
 * @param view 文件内的有界范围。
 * @param target 期望对象格式。
 * @param archive 是否允许根范围为归档。
 * @param bitcode 是否允许内部 bitcode 输入。
 */
void check(View view, Format target, bool archive, bool bitcode)
{
    std::array<unsigned char, 64> header{};
    const auto count = static_cast<std::size_t>(std::min<std::uint64_t>(view.size, header.size()));
    view.read(0, header.data(), count);
    const std::string magic(reinterpret_cast<const char *>(header.data()), std::min<std::size_t>(count, 8));
    if (magic == "!<thin>\n") throw std::runtime_error("不支持依赖外部成员路径的薄归档，请使用自包含归档");
    if (magic == "!<arch>\n")
    {
        if (!archive) throw std::runtime_error("归档成员必须是对象文件，不能嵌套归档");
        std::uint64_t at = 8;
        unsigned members = 0;
        while (at < view.size)
        {
            if (++members > 1000000) throw std::runtime_error("归档成员数量超过预算");
            std::array<char, 60> entry{};
            view.read(at, entry.data(), entry.size());
            if (entry[58] != '`' || entry[59] != '\n') throw std::runtime_error("归档成员头无效");
            std::string name(entry.data(), 16);
            name.erase(name.find_last_not_of(' ') + 1);
            const auto bytes = decimal(std::string(entry.data() + 48, 10));
            at += 60;
            if (bytes > view.size - at) throw std::runtime_error("归档成员超出文件边界");
            auto data = at, length = bytes;
            if (name.compare(0, 3, "#1/") == 0)
            {
                const auto nameSize = decimal(name.substr(3));
                if (nameSize > length || nameSize > 4096) throw std::runtime_error("归档成员名称越界");
                name.resize(static_cast<std::size_t>(nameSize));
                view.read(data, name.data(), name.size());
                while (!name.empty() && name.back() == '\0') name.pop_back();
                data += nameSize;
                length -= nameSize;
            }
            const bool index = name == "/" || name == "//" || name == "/SYM64/" || name == "__.SYMDEF" || name == "__.SYMDEF SORTED" || name == "__.SYMDEF_64" || name == "__.SYMDEF_64 SORTED";
            if (!index) check(View{ view.file, view.base + data, length, view.memory }, target, false, bitcode);
            at += bytes;
            if (at & 1u)
            {
                if (at == view.size) throw std::runtime_error("归档缺少对齐字节");
                ++at;
            }
        }
        return;
    }
    if (count >= 4 && ((header[0] == 'B' && header[1] == 'C' && header[2] == 0xc0 && header[3] == 0xde) || (header[0] == 0xde && header[1] == 0xc0 && header[2] == 0x17 && header[3] == 0x0b)))
    {
        if (!bitcode) throw std::runtime_error("当前入口不接受 bitcode，请使用明确的 LTO 输入路径");
        return;
    }
    if (count >= 4 && header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F')
    {
        if (target == Format::CoffX64 || count < 64 || header[4] != 2 || header[5] != 1 || header[6] != 1)
            throw std::runtime_error("ELF 格式、位宽或字节序与目标不匹配");
        const auto kind = word(header.data() + 16), machine = word(header.data() + 18);
        if ((kind != 1 && (kind != 3 || !archive)) || machine != (target == Format::ElfArm64 ? 183 : 62))
            throw std::runtime_error("ELF 类型或架构与目标不匹配");
        return;
    }
    if (target == Format::CoffX64 && count >= 20)
    {
        const bool extended = word(header.data()) == 0 && word(header.data() + 2) == 0xffff;
        const auto machine = word(header.data() + (extended ? 6 : 0));
        if (machine == 0x8664 && (!extended || word(header.data() + 4) == 0 || count >= 56)) return;
    }
    throw std::runtime_error("输入不是匹配目标的 ELF／COFF 对象或自包含静态归档");
}
}  // namespace
/**
 * @brief 以有界视图检查链接输入，保留 LLVM 对符号和重定位的最终验证。
 * @param path 原生输入文件。
 * @param target 期望格式。
 * @param error 接收带文件路径的说明。
 * @param allowArchive 是否允许静态归档。
 * @param allowBitcode 是否允许内部 LTO 内容。
 * @return 基础格式与目标检查通过为真。
 */
bool inspect(const std::filesystem::path &path, Format target, std::string &error, bool allowArchive, bool allowBitcode)
{
    std::error_code code;
    const auto size = std::filesystem::file_size(path, code);
    if (code)
    {
        error = path.u8string() + "：无法读取文件长度";
        return false;
    }
    return inspectRegion(path, 0, size, target, error, allowArchive, allowBitcode);
}
/**
 * @brief 打开容器并在已验证文件范围内复用对象检查器。
 * @param path 普通容器文件。
 * @param offset 对象开始偏移。
 * @param size 对象范围长度。
 * @param target 期望对象格式。
 * @param error 接收失败原因。
 * @param allowArchive 是否允许归档。
 * @param allowBitcode 是否允许内部 bitcode。
 * @return 检查通过为真。
 */
bool inspectRegion(const std::filesystem::path &path, std::uint64_t offset, std::uint64_t size, Format target, std::string &error, bool allowArchive, bool allowBitcode)
{
    try
    {
        if (!std::filesystem::is_regular_file(path)) throw std::runtime_error("链接输入不是普通文件");
        const auto fileSize = std::filesystem::file_size(path);
        if (fileSize > std::uint64_t(std::numeric_limits<std::streamoff>::max()) || offset > fileSize || size > fileSize - offset) throw std::runtime_error("链接输入区间超过文件或流偏移范围");
        std::ifstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("无法打开链接输入");
        check(View{ &file, offset, size }, target, allowArchive, allowBitcode);
        return true;
    } catch (const std::exception &exception)
    {
        error = path.u8string() + "：" + exception.what();
        return false;
    }
}
/**
 * @brief 在拥有型容器负载上复用完全相同的头与归档检查。
 * @param bytes 当前调用期间保持有效的字节。
 * @param target 期望格式。
 * @param error 接收拒绝原因。
 * @param allowArchive 是否接受归档。
 * @param allowBitcode 是否接受内部 bitcode。
 * @return 基础检查通过为真。
 */
bool inspectBytes(std::string_view bytes, Format target, std::string &error, bool allowArchive, bool allowBitcode)
{
    try
    {
        if (bytes.empty()) throw std::runtime_error("原生对象负载为空");
        check(View{ nullptr, 0, bytes.size(), bytes.data() }, target, allowArchive, allowBitcode);
        error.clear();
        return true;
    } catch (const std::exception &exception)
    {
        error = exception.what();
        return false;
    }
}
}  // namespace chali_native_objects
