#include "chali_precompiled.hpp"
#include "chali_ast.hpp"
#include "chali_incremental.hpp"
#include "chali_package.hpp"
#include "chali_i18n.hpp"
#include "lztext/lztext.hpp"
#include <algorithm>
#include <sstream>

namespace chali_precompiled
{
namespace
{
constexpr std::uint64_t kPackageMagic = 0x31424c5355484348ull;
constexpr std::uint64_t kPackageVersion = 2;
/** @brief 单个只读标准模块的内嵌包目录项。 */
struct StandardModuleEntry
{
    const char *name;
    const unsigned char *bytes;
    std::size_t size;
    std::uint64_t checksum;
};
constexpr std::size_t kPackageHeaderSize = 64;
#if defined(CHALI_WITH_REFLECTION) && CHALI_WITH_REFLECTION
constexpr std::uint64_t kPackageFeatures = 1;
#else
constexpr std::uint64_t kPackageFeatures = 0;
#endif
#if __has_include("chali_precompiled_data.inc")
#include "chali_precompiled_data.inc"
#else
constexpr unsigned char kStandardUnit[] = { 0 };
constexpr std::size_t kStandardUnitSize = 0;
constexpr std::uint64_t kStandardUnitChecksum = 0;
#endif
#if !defined(CHALI_EMBED_MODULE_UNITS)
constexpr StandardModuleEntry kStandardModules[] = { { nullptr, nullptr, 0, 0 } };
#endif
}  // namespace

/**
 * @brief 导出带核心文本身份与负载校验的预编译二进制。
 * @param out 构建步骤指定的输出流。
 * @param text 已解析的标准库核心文本。
 * @param nodes 要写入的核心节点数量。
 * @param module 为 true 时写出压缩的独立模块及重定位信息。
 * @return 序列化和输出均成功时返回 true。
 */
static bool writePackage(std::ostream &out, const Memory::String &text, std::size_t nodes, bool module)
{
    std::ostringstream body(std::ios::binary);
    if (!(module ? M_WriteModuleUnit(body) : M_WriteStandardUnit(body, nodes)) || !body) return false;
    auto bytes = body.str();
    if (module)
    {
        const auto compressed = lztext::compress(bytes);
        bytes.assign(reinterpret_cast<const char *>(compressed.data()), compressed.size());
    }
    chali_incremental::writeNumber(out, kPackageMagic);
    chali_incremental::writeNumber(out, kPackageVersion);
    chali_incremental::writeNumber(out, chali_incremental::hashBytes(chali_package::compilerVersion));
    chali_incremental::writeNumber(out, chali_incremental::hashBytes(text));
    chali_incremental::writeNumber(out, text.size());
    chali_incremental::writeNumber(out, bytes.size());
    chali_incremental::writeNumber(out, chali_incremental::hashBytes(Memory::String(bytes.data(), bytes.size())));
    chali_incremental::writeNumber(out, kPackageFeatures | (module ? 2u : 0u));
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(out);
}

/**
 * @brief 校验并装载与当前核心文本对应的进程内只读二进制资产。
 * @param text 当前核心的内容身份。
 * @param error 匹配资产损坏或不兼容时接收原因。
 * @return 恢复成功返回 true，没有对应资产或恢复失败返回 false。
 */
bool load(const Memory::String &text, Memory::String &error)
{
    return loadBytes(kStandardUnit, kStandardUnitSize, kStandardUnitChecksum, text, error);
}

/**
 * @brief 在恢复竞技场之前验证整包身份、长度和完整性，失败时保留原竞技场。
 * @param bytes 内存二进制包。
 * @param packageSize 完整包长度。
 * @param packageChecksum 构建时记录的整包校验值。
 * @param text 当前标准库核心文本。
 * @param error 损坏或不兼容时接收说明。
 * @param module 为 true 时验证独立模块并追加到当前解析状态。
 * @return 包匹配并恢复成功时返回 true。
 */
static bool loadPackage(const unsigned char *bytes, std::size_t packageSize, std::uint64_t packageChecksum,
                        const Memory::String &text, Memory::String &error, bool module)
{
    error.clear();
    if (packageSize == 0) return false;
    if (!bytes || packageSize < kPackageHeaderSize
        || packageChecksum != chali_incremental::hashBytes(Memory::String(reinterpret_cast<const char *>(bytes), packageSize)))
    {
        error = CHALI_TEXT("内嵌标准库二进制整包校验失败");
        return false;
    }
    std::istringstream input(std::string(reinterpret_cast<const char *>(bytes), packageSize), std::ios::binary);
    std::uint64_t magic = 0, version = 0, compiler = 0, identity = 0, length = 0, size = 0, checksum = 0, features = 0;
    if (!chali_incremental::readNumber(input, magic) || magic != kPackageMagic
        || !chali_incremental::readNumber(input, version) || version != kPackageVersion
        || !chali_incremental::readNumber(input, compiler) || compiler != chali_incremental::hashBytes(chali_package::compilerVersion)
        || !chali_incremental::readNumber(input, identity) || !chali_incremental::readNumber(input, length)
        || !chali_incremental::readNumber(input, size) || !chali_incremental::readNumber(input, checksum)
        || !chali_incremental::readNumber(input, features) || features != (kPackageFeatures | (module ? 2u : 0u))
        || size != packageSize - kPackageHeaderSize)
    {
        error = CHALI_TEXT("内嵌标准库二进制包头损坏");
        return false;
    }
    if (length != text.size()
        || identity != chali_incremental::hashBytes(text)) return false;
    const Memory::String body(reinterpret_cast<const char *>(bytes + kPackageHeaderSize), static_cast<std::size_t>(size));
    if (checksum != chali_incremental::hashBytes(body))
    {
        error = CHALI_TEXT("内嵌标准库二进制内容损坏或与编译器不兼容");
        return false;
    }
    bool restored = false;
    if (module)
    {
        /* 解压前限制容器中的原始长度，不允许损坏长度触发超额分配。 */
        std::uint64_t rawSize = 0;
        unsigned shift = 0;
        std::size_t at = 5;
        bool bounded = body.size() > at && body.compare(0, 4, "MLZ1") == 0;
        while (bounded && at < body.size())
        {
            const auto byte = static_cast<unsigned char>(body[at++]);
            if (shift >= 28) { bounded = false; break; }
            rawSize |= std::uint64_t(byte & 127) << shift;
            if (!(byte & 128)) break;
            shift += 7;
            if (at == body.size()) bounded = false;
        }
        std::string decoded;
        if (bounded && rawSize <= 64u * 1024u * 1024u
            && lztext::decompress(reinterpret_cast<const unsigned char *>(body.data()), body.size(), decoded))
        {
            std::istringstream restoredInput(decoded, std::ios::binary);
            restored = M_AppendModuleUnit(restoredInput);
        }
    }
    else restored = M_ReadStandardUnit(input);
    if (!restored)
    {
        error = CHALI_TEXT("内嵌标准库二进制内容损坏或与编译器不兼容");
        return false;
    }
    return true;
}
/**
 * @brief 导出核心解析包。
 * @param out 输出流。
 * @param text 核心内容身份。
 * @param nodes 核心节点数量。
 * @return 写入成功时返回 true。
 */
bool write(std::ostream &out, const Memory::String &text, std::size_t nodes)
{
    return writePackage(out, text, nodes, false);
}
/**
 * @brief 验证并恢复核心内存包。
 * @param bytes 包字节。
 * @param size 完整长度。
 * @param checksum 构建时摘要。
 * @param text 当前核心文本。
 * @param error 失败原因。
 * @return 恢复成功时返回 true。
 */
bool loadBytes(const unsigned char *bytes, std::size_t size, std::uint64_t checksum,
               const Memory::String &text, Memory::String &error)
{
    return loadPackage(bytes, size, checksum, text, error, false);
}
/**
 * @brief 移除当前会话的内部宏编号，保留参数序号、普通字符串及源码内容。
 * @param text 已完成文本前端处理的独立标准模块。
 * @return 与导入顺序无关的模块内容身份。
 */
static Memory::String moduleIdentity(const Memory::String &text)
{
    Memory::String result;
    for (std::size_t i = 0; i < text.size();)
    {
        if (text[i] == '"' || text[i] == char(39))
        {
            const auto quote = text[i];
            result += text[i++];
            while (i < text.size())
            {
                const auto ch = text[i++];
                result += ch;
                if (ch == '\\' && i < text.size()) result += text[i++];
                else if (ch == quote) break;
            }
            continue;
        }
        if (text.compare(i, 2, "//") == 0 || text.compare(i, 2, "/*") == 0)
        {
            auto end = text.compare(i, 2, "//") == 0 ? text.find('\n', i) : text.find("*/", i + 2);
            if (end == Memory::String::npos) end = text.size();
            else if (text.compare(i, 2, "/*") == 0) end += 2;
            result.append(text, i, end - i);
            i = end;
            continue;
        }
        const auto prefix = text.compare(i, 15, "@!macrofragment") == 0 ? std::size_t(15) :
                            text.compare(i, 2, "@!") == 0 ? std::size_t(2) : std::size_t(0);
        if (prefix && i + prefix < text.size() && text[i + prefix] >= '0' && text[i + prefix] <= '9')
        {
            result.append(text, i, prefix);
            result += '0';
            i += prefix;
            while (i < text.size() && text[i] >= '0' && text[i] <= '9') ++i;
        }
        else result += text[i++];
    }
    return result;
}
/**
 * @brief 导出带身份与重定位信息的独立标准模块。
 * @param out 输出流。
 * @param text 当前模块经过前端处理的源码。
 * @return 写入成功时返回 true。
 */
bool writeModule(std::ostream &out, const Memory::String &text)
{
    return writePackage(out, moduleIdentity(text), M_NodeCount(), true);
}
/**
 * @brief 按需验证并追加一个内嵌标准模块，不读取磁盘单元。
 * @param name 完整模块名。
 * @param text 当前前端处理后的模块内容。
 * @param error 匹配资产损坏或重定位失败时接收原因。
 * @return 资产匹配且追加成功时返回 true。
 */
bool loadModule(const Memory::String &name, const Memory::String &text, Memory::String &error)
{
    for (const auto &entry : kStandardModules)
        if (entry.name && name == entry.name)
            return loadPackage(entry.bytes, entry.size, entry.checksum, moduleIdentity(text), error, true);
    error.clear();
    return false;
}
/**
 * @brief 从合并来源映射识别各个标准模块的连续区间。
 * @param text 完整处理后的源码。
 * @param files 行来源文件表。
 * @param limit 只扫描导入段的字节边界。
 * @return 保留导入顺序的标准模块区间，不包括核心。
 */
Memory::Vector<ModuleSlice> moduleSlices(const Memory::String &text, const Memory::Vector<Memory::String> &files, std::size_t limit)
{
    Memory::Vector<ModuleSlice> result;
    const Memory::String prefix = "<内建 ";
    limit = std::min(limit, text.size());
    std::size_t at = 0, line = 0;
    while (at < limit && line < files.size())
    {
        const auto newline = text.find('\n', at);
        const auto end = newline == Memory::String::npos ? limit : std::min(limit, newline + 1);
        const auto &file = files[line++];
        if (file.compare(0, prefix.size(), prefix) == 0 && file.size() > prefix.size() && file.back() == '>')
        {
            const auto name = file.substr(prefix.size(), file.size() - prefix.size() - 1);
            if (name.compare(0, 5, "std::") == 0 && name != "std::lang")
            {
                if (!result.empty() && result.back().name == name && result.back().end == at) result.back().end = end;
                else result.push_back({ name, at, end });
            }
        }
        at = end;
    }
    return result;
}
}  // namespace chali_precompiled
