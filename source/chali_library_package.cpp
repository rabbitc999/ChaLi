/** @file chali_library_package.cpp
 * @brief 有界读取库包，显式小端编码元数据并逐条验证 SHA-256。
 */
#include "chali_library.hpp"
#include "chali_digest.hpp"
#include "chali_aot_target.hpp"
#include "chali_native_objects.hpp"
#include "chali_native_tools.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <stdexcept>

namespace chali_library
{
namespace
{
constexpr char magic[] = "CHLIB1\r\n";
/**
 * @brief 在固定缓冲内显式编码小端整数。
 * @param output 至少 width 字节的缓冲。
 * @param value 无符号整数。
 * @param width 编码宽度，至多八字节。
 */
void encode(unsigned char *output, std::uint64_t value, unsigned width)
{
    for (unsigned index = 0; index < width; ++index) output[index] = static_cast<unsigned char>(value >> (index * 8));
}
/**
 * @brief 从固定缓冲显式读取小端整数。
 * @param input 至少 width 字节的数据。
 * @param width 编码宽度，至多八字节。
 * @return 无符号值。
 */
std::uint64_t decode(const unsigned char *input, unsigned width)
{
    std::uint64_t result = 0;
    for (unsigned index = 0; index < width; ++index) result |= std::uint64_t(input[index]) << (index * 8);
    return result;
}
/** @brief 保留实际文件长度的顺序读取器，不按未验证长度分配。 */
struct Reader
{
    std::ifstream input;          ///< 已打开的普通文件。
    std::uint64_t size = 0;       ///< 文件真实长度。
    std::uint64_t position = 0;   ///< 下一个读取位置。
    chali_digest::Sha256 digest;  ///< 从同一流累计整包摘要。
    /**
     * @brief 打开文件并在任何分配前核对总预算。
     * @param path 完整库包路径。
     */
    explicit Reader(const std::filesystem::path &path)
    {
        if (!std::filesystem::is_regular_file(path)) throw std::runtime_error("库包不是普通文件");
        size = std::filesystem::file_size(path);
        if (size < 56 || size > maximumPackageBytes) throw std::runtime_error("库包超出长度预算");
        input.open(path, std::ios::binary);
        if (!input) throw std::runtime_error("无法打开库包");
    }
    /**
     * @brief 检查剩余真实文件长度后读取固定字节。
     * @param output 调用方缓冲。
     * @param count 读取长度。
     */
    void bytes(void *output, std::size_t count)
    {
        if (count > size - position) throw std::runtime_error("库包数据截断或声明长度越界");
        input.read(static_cast<char *>(output), static_cast<std::streamsize>(count));
        if (static_cast<std::size_t>(input.gcount()) != count) throw std::runtime_error("库包读取失败");
        digest.update(output, count);
        position += count;
    }
    /**
     * @brief 只在长度落在真实剩余区间内时创建负载。
     * @param count 无符号声明长度。
     * @return 拥有型原始字节。
     */
    std::string string(std::uint64_t count)
    {
        if (count > size - position) throw std::runtime_error("库包负载长度超出文件");
        std::string result(static_cast<std::size_t>(count), '\0');
        bytes(result.data(), result.size());
        return result;
    }
};
/**
 * @brief 对已打开的流写出完整数据，错误立即报告。
 * @param stream 当前私有工作文件。
 * @param bytes 缓冲起点。
 * @param count 有效长度。
 */
void output(std::ofstream &stream, const void *bytes, std::size_t count)
{
    stream.write(static_cast<const char *>(bytes), static_cast<std::streamsize>(count));
    if (!stream) throw std::runtime_error("库包写入失败");
}
/**
 * @brief 摘要条目的完整可变元数据、名字与负载。
 * @param header 不含摘要的十六字节小端头。
 * @param entry 已验证条目。
 * @return 对完整记录身份的摘要。
 */
chali_digest::Digest recordDigest(const std::array<unsigned char, 16> &header, const Entry &entry)
{
    chali_digest::Sha256 state;
    state.update(header.data(), header.size());
    state.update(entry.name.data(), entry.name.size());
    state.update(entry.bytes.data(), entry.bytes.size());
    return state.finish();
}
/**
 * @brief 在已经摘要核对的拥有型负载上验证对象，不重新打开包文件。
 * @param package 已核对引用的包。
 */
void nativePayloads(const Package &package)
{
    for (const auto &target : package.manifest.targets)
    {
        if (target.implementation.empty()) continue;
        const auto *entry = package.find(target.implementation);
        chali_build::TargetInfo known;
        if (!entry || !chali_build::describeTarget(Memory::String(target.triple.c_str()), known)) throw std::runtime_error("库目标实现未登记");
        const auto format = known.windows ? chali_native_objects::Format::CoffX64 : known.arm64 ? chali_native_objects::Format::ElfArm64 :
                                                                                                  chali_native_objects::Format::ElfX64;
        std::string error;
        if (!chali_native_objects::inspectBytes(entry->bytes, format, error, entry->kind == Kind::Archive)) throw std::runtime_error("库原生实现不兼容：" + error);
    }
}
}  // namespace
/**
 * @brief 完整恢复并校验一个库包，失败不改变调用方已有结果。
 * @param path 普通库包文件。
 * @param package 成功后替换的包。
 * @param error 接收拒绝原因。
 * @return 全部字段、内容和对象检查通过为真。
 */
bool read(const std::filesystem::path &path, Package &package, std::string &error)
{
    try
    {
        Reader input(path);
        std::array<unsigned char, 56> header{};
        input.bytes(header.data(), header.size());
        if (std::memcmp(header.data(), magic, 8) != 0 || decode(header.data() + 8, 4) != packageFormat) throw std::runtime_error("不是兼容的 CHLIB 库包格式");
        const auto count = decode(header.data() + 12, 4), manifestBytes = decode(header.data() + 16, 8);
        if (!count || count > 4096 || !manifestBytes || manifestBytes > 1024 * 1024) throw std::runtime_error("库清单或条目数量超出预算");
        const auto manifest = input.string(manifestBytes);
        const auto digest = chali_digest::sha256(manifest);
        if (std::memcmp(header.data() + 24, digest.data(), digest.size()) != 0) throw std::runtime_error("库清单 SHA-256 不匹配");
        Package result;
        result.manifest = parseManifest(manifest);
        std::string previous;
        for (std::uint64_t index = 0; index < count; ++index)
        {
            std::array<unsigned char, 16> record{};
            chali_digest::Digest expected{};
            input.bytes(record.data(), record.size());
            input.bytes(expected.data(), expected.size());
            const auto nameBytes = decode(record.data(), 2), payloadBytes = decode(record.data() + 8, 8);
            if (!nameBytes || nameBytes > 240 || payloadBytes > maximumPackageBytes || nameBytes > input.size - input.position || payloadBytes > input.size - input.position - nameBytes)
                throw std::runtime_error("库条目名字或负载长度越界");
            Entry entry;
            entry.kind = static_cast<Kind>(decode(record.data() + 2, 2));
            entry.flags = static_cast<std::uint32_t>(decode(record.data() + 4, 4));
            entry.name = input.string(nameBytes);
            if (!previous.empty() && entry.name <= previous) throw std::runtime_error("库条目未按名字排序或包含重复项");
            previous = entry.name;
            entry.bytes = input.string(payloadBytes);
            if (recordDigest(record, entry) != expected) throw std::runtime_error("库条目 SHA-256 不匹配：" + entry.name);
            result.entries.push_back(std::move(entry));
        }
        if (input.position != input.size || input.input.peek() != std::char_traits<char>::eof()) throw std::runtime_error("库包有未登记的尾部数据");
        validate(result);
        nativePayloads(result);
        result.contentSha256 = chali_digest::hex(input.digest.finish());
        package = std::move(result);
        error.clear();
        return true;
    } catch (const std::exception &exception)
    {
        error = path.u8string() + "：" + exception.what();
        return false;
    }
}
/**
 * @brief 在专用工作区写出规范包，对原生区间检查通过后事务发布。
 * @param path 最终文件名。
 * @param package 清单与原始负载。
 * @param error 接收失败信息。
 * @return 成功发布为真，失败保留原文件。
 */
bool write(const std::filesystem::path &path, const Package &package, std::string &error)
{
    try
    {
        validate(package);
        nativePayloads(package);
        const auto manifest = manifestJson(package.manifest);
        std::vector<const Entry *> order;
        for (const auto &entry : package.entries) order.push_back(&entry);
        std::sort(order.begin(), order.end(), [](const Entry *a, const Entry *b) { return a->name < b->name; });
        chali_native_tools::Workspace work;
        const auto temporary = work.path / "library.chlib";
        std::ofstream stream(temporary, std::ios::binary);
        if (!stream) throw std::runtime_error("无法创建私有库包文件");
        std::array<unsigned char, 56> header{};
        std::memcpy(header.data(), magic, 8);
        encode(header.data() + 8, packageFormat, 4);
        encode(header.data() + 12, order.size(), 4);
        encode(header.data() + 16, manifest.size(), 8);
        const auto digest = chali_digest::sha256(manifest);
        std::memcpy(header.data() + 24, digest.data(), digest.size());
        output(stream, header.data(), header.size());
        output(stream, manifest.data(), manifest.size());
        for (const auto *entry : order)
        {
            std::array<unsigned char, 16> record{};
            encode(record.data(), entry->name.size(), 2);
            encode(record.data() + 2, static_cast<std::uint16_t>(entry->kind), 2);
            encode(record.data() + 4, entry->flags, 4);
            encode(record.data() + 8, entry->bytes.size(), 8);
            const auto checksum = recordDigest(record, *entry);
            output(stream, record.data(), record.size());
            output(stream, checksum.data(), checksum.size());
            output(stream, entry->name.data(), entry->name.size());
            output(stream, entry->bytes.data(), entry->bytes.size());
        }
        stream.close();
        if (!stream) throw std::runtime_error("库包关闭失败");
        if (!chali_native_tools::publish(temporary, path, error)) throw std::runtime_error("无法发布库包：" + error);
        error.clear();
        return true;
    } catch (const std::exception &exception)
    {
        error = path.u8string() + "：" + exception.what();
        return false;
    }
}
}  // namespace chali_library
