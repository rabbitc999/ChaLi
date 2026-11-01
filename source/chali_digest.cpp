/** @file chali_digest.cpp
 * @brief SHA-256 消息调度、压缩与填充，依照 NIST FIPS 180-4 第 6.2 节。
 * @see https://csrc.nist.gov/pubs/fips/180-4/final
 */
#include "chali_digest.hpp"
#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace chali_digest
{
namespace
{
constexpr std::uint32_t constants[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
/**
 * @brief 在三十二位无符号字上循环右移。
 * @param value 原字。
 * @param bits 一到三十一之间的固定移位量。
 * @return 右旋结果。
 */
std::uint32_t rotate(std::uint32_t value, unsigned bits)
{
    return (value >> bits) | (value << (32 - bits));
}
}  // namespace
/** @brief 设置标准初始向量，不依赖宿主字节序。 */
Sha256::Sha256() : state_{ { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 } } {}
/**
 * @brief 大端读取一个消息块，执行六十四轮并更新链状态。
 * @param block 至少六十四个字节。
 */
void Sha256::compress(const unsigned char *block)
{
    std::array<std::uint32_t, 64> words{};
    for (unsigned index = 0; index < 16; ++index)
        words[index] = (std::uint32_t(block[index * 4]) << 24) | (std::uint32_t(block[index * 4 + 1]) << 16)
                       | (std::uint32_t(block[index * 4 + 2]) << 8) | block[index * 4 + 3];
    for (unsigned index = 16; index < 64; ++index)
    {
        const auto left = words[index - 15], right = words[index - 2];
        words[index] = words[index - 16] + (rotate(left, 7) ^ rotate(left, 18) ^ (left >> 3)) + words[index - 7]
                       + (rotate(right, 17) ^ rotate(right, 19) ^ (right >> 10));
    }
    auto a = state_[0], b = state_[1], c = state_[2], d = state_[3], e = state_[4], f = state_[5], g = state_[6], h = state_[7];
    for (unsigned index = 0; index < 64; ++index)
    {
        const auto first = h + (rotate(e, 6) ^ rotate(e, 11) ^ rotate(e, 25)) + ((e & f) ^ ((~e) & g)) + constants[index] + words[index];
        const auto second = (rotate(a, 2) ^ rotate(a, 13) ^ rotate(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + first;
        d = c;
        c = b;
        b = a;
        a = first + second;
    }
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
    state_[5] += f;
    state_[6] += g;
    state_[7] += h;
}
/**
 * @brief 校验消息长度后按完整块流式处理输入。
 * @param bytes 可读字节缓冲。
 * @param size 缓冲长度。
 */
void Sha256::update(const void *bytes, std::size_t size)
{
    if (finished_) throw std::runtime_error("SHA-256 消息已经结束");
    if ((!bytes && size) || size > std::numeric_limits<std::uint64_t>::max() / 8 - bytes_)
        throw std::runtime_error("SHA-256 输入为空或超过长度上限");
    bytes_ += size;
    auto input = static_cast<const unsigned char *>(bytes);
    while (size)
    {
        const auto count = std::min(size, buffer_.size() - used_);
        std::memcpy(buffer_.data() + used_, input, count);
        used_ += count;
        input += count;
        size -= count;
        if (used_ == buffer_.size())
        {
            compress(buffer_.data());
            used_ = 0;
        }
    }
}
/**
 * @brief 追加一位、一段零和原消息的六十四位比特长度。
 * @return 标准大端编码的摘要；重复结束不会改变它。
 */
Digest Sha256::finish()
{
    if (finished_) return digest_;
    const auto bits = bytes_ * 8;
    buffer_[used_++] = 0x80;
    if (used_ > 56)
    {
        std::fill(buffer_.begin() + used_, buffer_.end(), 0);
        compress(buffer_.data());
        used_ = 0;
    }
    std::fill(buffer_.begin() + used_, buffer_.begin() + 56, 0);
    for (unsigned index = 0; index < 8; ++index) buffer_[63 - index] = static_cast<unsigned char>(bits >> (index * 8));
    compress(buffer_.data());
    for (unsigned index = 0; index < 32; ++index) digest_[index] = static_cast<unsigned char>(state_[index / 4] >> (24 - (index % 4) * 8));
    finished_ = true;
    return digest_;
}
/**
 * @brief 在局部状态中摘要一段完整消息。
 * @param bytes 任意字节视图。
 * @return 消息摘要。
 */
Digest sha256(std::string_view bytes)
{
    Sha256 state;
    state.update(bytes.data(), bytes.size());
    return state.finish();
}
/**
 * @brief 将摘要的每个半字节转换为固定小写十六进制。
 * @param digest 原始摘要。
 * @return 六十四字符文本。
 */
std::string hex(const Digest &digest)
{
    constexpr char digits[] = "0123456789abcdef";
    std::string result;
    result.reserve(64);
    for (const auto value : digest)
    {
        result += digits[value >> 4];
        result += digits[value & 15];
    }
    return result;
}
}  // namespace chali_digest
