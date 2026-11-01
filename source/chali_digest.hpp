/** @file chali_digest.hpp
 * @brief 库包资产的流式 SHA-256 内容摘要，不承担签名或来源认证。
 */
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace chali_digest
{
/** @brief 按大端标准顺序保存的二百五十六位摘要。 */
using Digest = std::array<unsigned char, 32>;
/** @brief 不分配堆内存的单次 SHA-256 消息状态。 */
class Sha256
{
public:
    /** @brief 初始化 FIPS 180-4 的 SHA-256 状态。 */
    Sha256();
    /**
     * @brief 追加原始字节，零长度时允许空指针。
     * @param bytes 至少有 size 个字节的缓冲。
     * @param size 追加字节数。
     * @throws std::runtime_error 结束后继续写入、空缓冲或消息超长。
     */
    void update(const void *bytes, std::size_t size);
    /**
     * @brief 填充消息并取得摘要，同一状态重复调用返回相同结果。
     * @return 标准字节顺序的摘要。
     */
    Digest finish();

private:
    /**
     * @brief 压缩一个完整的六十四字节消息块。
     * @param block 至少六十四字节的原始数据。
     */
    void compress(const unsigned char *block);
    std::array<std::uint32_t, 8> state_;
    std::array<unsigned char, 64> buffer_{};
    std::uint64_t bytes_ = 0;
    std::size_t used_ = 0;
    bool finished_ = false;
    Digest digest_{};
};
/**
 * @brief 对完整字节视图计算 SHA-256。
 * @param bytes 不要求以零结尾的原始字节。
 * @return 原始摘要。
 */
Digest sha256(std::string_view bytes);
/**
 * @brief 输出固定六十四字符的小写十六进制摘要。
 * @param digest 原始摘要。
 * @return 小写十六进制文本。
 */
std::string hex(const Digest &digest);
}  // namespace chali_digest
