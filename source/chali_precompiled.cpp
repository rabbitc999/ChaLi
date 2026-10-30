#include "chali_precompiled.hpp"
#include "chali_ast.hpp"
#include "chali_incremental.hpp"
#include "chali_package.hpp"
#include "chali_i18n.hpp"
#include <sstream>

namespace chali_precompiled
{
namespace
{
constexpr std::uint64_t kPackageMagic = 0x31424c5355484348ull;
#if __has_include("chali_precompiled_data.inc")
#include "chali_precompiled_data.inc"
#else
constexpr unsigned char kStandardUnit[] = { 0 };
constexpr std::size_t kStandardUnitSize = 0;
constexpr std::uint64_t kStandardUnitChecksum = 0;
#endif
}  // namespace

/**
 * @brief 导出带核心文本身份与负载校验的预编译二进制。
 * @param out 构建步骤指定的输出流。
 * @param text 已解析的标准库核心文本。
 * @param nodes 要写入的核心节点数量。
 * @return 序列化和输出均成功时返回 true。
 */
bool write(std::ostream &out, const Memory::String &text, std::size_t nodes)
{
    std::ostringstream body(std::ios::binary);
    if (!M_WriteStandardUnit(body, nodes) || !body) return false;
    const auto bytes = body.str();
    chali_incremental::writeNumber(out, kPackageMagic);
    chali_incremental::writeNumber(out, chali_incremental::hashBytes(chali_package::compilerVersion, chali_incremental::hashBytes(text)));
    chali_incremental::writeNumber(out, text.size());
    chali_incremental::writeNumber(out, bytes.size());
    chali_incremental::writeNumber(out, chali_incremental::hashBytes(Memory::String(bytes.data(), bytes.size())));
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
 * @return 包匹配并恢复成功时返回 true。
 */
bool loadBytes(const unsigned char *bytes, std::size_t packageSize, std::uint64_t packageChecksum,
               const Memory::String &text, Memory::String &error)
{
    error.clear();
    if (packageSize == 0) return false;
    if (!bytes || packageSize < 40
        || packageChecksum != chali_incremental::hashBytes(Memory::String(reinterpret_cast<const char *>(bytes), packageSize)))
    {
        error = CHALI_TEXT("内嵌标准库二进制整包校验失败");
        return false;
    }
    std::istringstream input(std::string(reinterpret_cast<const char *>(bytes), packageSize), std::ios::binary);
    std::uint64_t magic = 0, identity = 0, length = 0, size = 0, checksum = 0;
    if (!chali_incremental::readNumber(input, magic) || magic != kPackageMagic
        || !chali_incremental::readNumber(input, identity) || !chali_incremental::readNumber(input, length)
        || !chali_incremental::readNumber(input, size) || !chali_incremental::readNumber(input, checksum)
        || size != packageSize - 40)
    {
        error = CHALI_TEXT("内嵌标准库二进制包头损坏");
        return false;
    }
    if (length != text.size()
        || identity != chali_incremental::hashBytes(chali_package::compilerVersion, chali_incremental::hashBytes(text))) return false;
    const Memory::String body(reinterpret_cast<const char *>(bytes + 40), static_cast<std::size_t>(size));
    if (checksum != chali_incremental::hashBytes(body) || !M_ReadStandardUnit(input))
    {
        error = CHALI_TEXT("内嵌标准库二进制内容损坏或与编译器不兼容");
        return false;
    }
    return true;
}
}  // namespace chali_precompiled
