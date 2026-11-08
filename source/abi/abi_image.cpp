/** @file abi_image.cpp @brief 图像私有数值内核及 C 帧入口，无模板胶水封送。 */
#include "chali_standard.h"
#include "allocator/c_heap_allocator.hpp"
#include "../allocator/chali_allocator.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <type_traits>
namespace
{
using Bytes = Memory::Vector<std::uint8_t>;
/** @brief 图片记录拥有的尺寸和 RGBA 像素。 */
struct ImageData
{
    int64_t width = 0, height = 0;
    Bytes pixels;
};
/** @brief 销毁图片及其最终缓冲。
 * @param pointer 拥有的图片。
 */
void CHALI_ABI_CALL destroyImage(void *pointer) { Memory::destroyCHeap(static_cast<ImageData *>(pointer)); }
/** @brief 深复制图片，不让异常跨越 C 回调边界。
 * @param pointer 源图片。
 * @return 独立副本，失败为空。
 */
void *CHALI_ABI_CALL copyImage(const void *pointer)
try
{
    return Memory::makeCHeapUnique<ImageData>(*static_cast<const ImageData *>(pointer)).release();
} catch (...) { return nullptr; }
/** @brief 发布完整图片，失败保持本地所有权。
 * @param frame 当前 C 帧。
 * @param image 待发布记录。
 * @return 新句柄，失败为零。
 */
int64_t publish(ChaliAbiFrame *frame, Memory::CHeapUniquePtr<ImageData> image)
{
    const auto handle = chali_abi_record_make_clonable(frame, "std::image::Image", image.get(), destroyImage, copyImage);
    if (handle) image.release();
    return handle;
}
/** @brief 检查并借用存活图片。
 * @param frame 当前 C 帧。
 * @param handle 图片句柄。
 * @return 图片，失败记录原诊断并停止当前内核。
 */
ImageData *imageData(ChaliAbiFrame *frame, int64_t handle)
{
    auto *image = static_cast<ImageData *>(chali_abi_record_probe(frame, handle, "std::image::Image"));
    if (!image)
    {
        chali_abi_fail(frame, "Image opcode 已释放或类型不匹配");
        throw std::runtime_error("Image opcode 已释放或类型不匹配");
    }
    return image;
}
/** @brief 记录不可恢复的接口错误并停止本次内核。
 * @param frame 当前 C 帧。
 * @param message 错误文本。
 */
[[noreturn]] void fatal(ChaliAbiFrame *frame, const char *message)
{
    chali_abi_fail(frame, message);
    throw std::runtime_error(message);
}
/** @brief 读取语言对象的小端八字节槽。
 * @param frame 当前 C 帧。
 * @param address 对象地址。
 * @param slot 槽序号。
 * @return 整数位模式。
 */
int64_t readSlot(ChaliAbiFrame *frame, int64_t address, size_t slot)
{
    if (address < 0 || slot > size_t(INT64_MAX / 8) || address > INT64_MAX - int64_t(slot * 8)) fatal(frame, "图片回执地址溢出");
    unsigned char bytes[8];
    if (!chali_abi_read(frame, address + int64_t(slot * 8), bytes, 8)) fatal(frame, "图片回执不可读");
    uint64_t value = 0;
    for (unsigned i = 0; i < 8; ++i) value |= uint64_t(bytes[i]) << (8 * i);
    return int64_t(value);
}
/** @brief 写入语言对象的小端八字节槽。
 * @param frame 当前 C 帧。
 * @param address 对象地址。
 * @param slot 槽序号。
 * @param value 整数位模式。
 */
void writeSlot(ChaliAbiFrame *frame, int64_t address, size_t slot, int64_t value)
{
    if (address < 0 || slot > size_t(INT64_MAX / 8) || address > INT64_MAX - int64_t(slot * 8)) fatal(frame, "图片回执地址溢出");
    unsigned char bytes[8];
    for (unsigned i = 0; i < 8; ++i) bytes[i] = unsigned(uint64_t(value) >> (8 * i));
    if (!chali_abi_write(frame, address + int64_t(slot * 8), bytes, 8)) fatal(frame, "图片回执不可写");
}
/** @brief 兼容直接图片句柄和图片引用槽。
 * @param frame 当前 C 帧。
 * @param reference 接收者。
 * @return 实际图片句柄。
 */
int64_t referenceHandle(ChaliAbiFrame *frame, int64_t reference)
{
    if (chali_abi_record_probe(frame, reference, "std::image::Image")) return reference;
    return readSlot(frame, reference, 0);
}
/** @brief 创建合法图片及独立像素缓冲。
 * @param frame 当前 C 帧。
 * @param width 宽度。
 * @param height 高度。
 * @param pixels RGBA 像素，空数组表示透明黑图。
 * @return 新图片句柄，失败为零。
 */
int64_t makeImage(ChaliAbiFrame *frame, int64_t width, int64_t height, Bytes pixels = {})
{
    if (width <= 0 || height <= 0 || width > 536870911 / height)
    {
        chali_abi_fail(frame, "图片尺寸无效或像素缓冲超过预算");
        return 0;
    }
    const auto size = size_t(width * height * 4);
    if (pixels.empty()) pixels.resize(size, 0);
    if (pixels.size() != size)
    {
        chali_abi_fail(frame, "图片像素缓冲长度与尺寸不符");
        return 0;
    }
    return publish(frame, Memory::makeCHeapUnique<ImageData>(ImageData{ width, height, std::move(pixels) }));
}
/** @brief 通过 C 数值服务读取并按原槽规则转换基础类型数组。
 * @tparam Scalar 目标基础类型。
 * @param frame 当前 C 帧。
 * @param handle 列表句柄。
 * @return 独立工作数组。
 */
template <class Scalar>
Memory::Vector<Scalar> readNumbers(ChaliAbiFrame *frame, int64_t handle)
{
    size_t count = 0;
    if (!chali_abi_list_numbers(frame, handle, nullptr, nullptr, 0, &count)) fatal(frame, "图片数值列表不可读");
    Memory::Vector<int64_t> bits(count);
    Memory::Vector<unsigned char> kinds(count);
    if (!chali_abi_list_numbers(frame, handle, bits.data(), kinds.data(), count, &count)) fatal(frame, "图片数值列表不可读");
    Memory::Vector<Scalar> result;
    result.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        double real = 0;
        if (kinds[i]) std::memcpy(&real, &bits[i], 8);
        if constexpr (std::is_floating_point_v<Scalar>)
            result.push_back(Scalar(kinds[i] ? real : double(bits[i])));
        else
            result.push_back(Scalar(kinds[i] ? int64_t(real) : bits[i]));
    }
    return result;
}
/** @brief 批量追加数值到回执已拥有的列表，保留原列表身份及追加语义。
 * @tparam Values 来源基础类型数组。
 * @param frame 当前 C 帧。
 * @param reply 回执地址。
 * @param slot 列表字段槽。
 * @param values 追加值。
 */
template <class Values>
void appendNumbers(ChaliAbiFrame *frame, int64_t reply, size_t slot, const Values &values)
{
    Memory::Vector<int64_t> bits;
    bits.reserve(values.size());
    for (auto value : values) bits.push_back(int64_t(value));
    if (!chali_abi_list_append_numbers(frame, readSlot(frame, reply, slot), bits.data(), nullptr, bits.size())) fatal(frame, "图片结果列表追加失败");
}
/** @brief 返回可恢复错误并清空结果尺寸和阈值。
 * @param frame 当前 C 帧。
 * @param reply 回执地址。
 * @param message 错误文本。
 */
void replyError(ChaliAbiFrame *frame, int64_t reply, const Memory::String &message)
{
    const auto previous = readSlot(frame, reply, 0);
    const auto text = chali_abi_string(frame, message.data(), message.size());
    if (!text) fatal(frame, "图片错误文本创建失败");
    try
    {
        writeSlot(frame, reply, 0, text);
    } catch (...)
    {
        chali_abi_string_release(frame, text);
        throw;
    }
    if (previous) chali_abi_string_release(frame, previous);
    writeSlot(frame, reply, 1, 0);
    writeSlot(frame, reply, 2, 0);
    writeSlot(frame, reply, 8, 0);
}
/** @brief 批量写入回执 RGBA 列表。
 * @param frame 当前 C 帧。
 * @param reply 回执地址。
 * @param values RGBA 字节。
 */
void writePixels(ChaliAbiFrame *frame, int64_t reply, const Bytes &values) { appendNumbers(frame, reply, 3, values); }
/** @brief 批量写入回执编码字节列表。
 * @param frame 当前 C 帧。
 * @param reply 回执地址。
 * @param values 文件字节。
 */
void writeBytes(ChaliAbiFrame *frame, int64_t reply, const Bytes &values) { appendNumbers(frame, reply, 4, values); }
/** @brief 批量写入回执灰度列表。
 * @param frame 当前 C 帧。
 * @param reply 回执地址。
 * @param values 灰度字节。
 */
void writeGray(ChaliAbiFrame *frame, int64_t reply, const Bytes &values) { appendNumbers(frame, reply, 5, values); }
/** @brief 读取已验证的两个小端字节。
 * @param p 至少两个字节的地址。
 * @return 十六位整数。
 */
inline std::uint16_t readU16LE(const std::uint8_t *p) { return static_cast<std::uint16_t>(p[0] | (p[1] << 8)); }

/** @brief 读取已验证的四个小端字节。
 * @param p 至少四个字节的地址。
 * @return 三十二位整数。
 */
inline std::uint32_t readU32LE(const std::uint8_t *p)
{
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) | (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

/** @brief 按小端追加十六位整数。
 * @param out 目标字节数组。
 * @param v 整数值。
 */
inline void writeU16LE(Bytes &out, std::uint16_t v)
{
    out.push_back(static_cast<std::uint8_t>(v & 0xFF));
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
}

/** @brief 按小端追加三十二位整数。
 * @param out 目标字节数组。
 * @param v 整数值。
 */
inline void writeU32LE(Bytes &out, std::uint32_t v)
{
    out.push_back(static_cast<std::uint8_t>(v & 0xFF));
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFF));
}

/** @brief 按整数 BT.601 权重计算灰度。
 * @param r 红色分量。
 * @param g 绿色分量。
 * @param b 蓝色分量。
 * @return 灰度字节。
 */
inline std::uint8_t luma(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
    return static_cast<std::uint8_t>((299 * r + 587 * g + 114 * b) / 1000);
}

/** @brief 解码无压缩 BMP 并验证边界。
 * @param data 文件字节。
 * @param width 接收宽度。
 * @param height 接收高度。
 * @param pixels 接收 RGBA 像素。
 * @return 空字符串表示成功，否则为错误。
 */
inline Memory::String decodeBmp(const Bytes &data, std::int64_t &width, std::int64_t &height, Bytes &pixels)
{
    if (data.size() < 54 || data[0] != 'B' || data[1] != 'M') return "BMP：文件头不合法";
    const std::uint32_t offset = readU32LE(data.data() + 10);
    const std::uint32_t headerSize = readU32LE(data.data() + 14);
    if (headerSize < 40) return "BMP：仅支持 BITMAPINFOHEADER 及以上";
    if (data[30] != 0) return "BMP：仅支持无压缩 BI_RGB";
    const std::int32_t w = static_cast<std::int32_t>(readU32LE(data.data() + 18));
    const std::int32_t rawH = static_cast<std::int32_t>(readU32LE(data.data() + 22));
    if (w <= 0 || rawH == 0) return "BMP：宽高不合法";
    const bool topDown = rawH < 0;
    const std::int64_t h = rawH < 0 ? -static_cast<std::int64_t>(rawH) : rawH;
    const std::uint16_t planes = readU16LE(data.data() + 26);
    const std::uint16_t bpp = readU16LE(data.data() + 28);
    if (planes != 1 || (bpp != 24 && bpp != 32)) return "BMP：仅支持 24/32 位";
    const std::size_t stride = (static_cast<std::size_t>(w) * (bpp / 8) + 3) & ~std::size_t(3);
    if (static_cast<std::uint64_t>(offset) + static_cast<std::uint64_t>(stride) * static_cast<std::uint64_t>(h) > data.size())
        return "BMP：像素数据越界";
    pixels.assign(static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * 4, 0);
    for (std::int64_t row = 0; row < h; ++row)
    {
        const std::uint8_t *src = data.data() + offset + static_cast<std::size_t>(topDown ? row : (h - 1 - row)) * stride;
        std::uint8_t *dst = pixels.data() + static_cast<std::size_t>(row) * static_cast<std::size_t>(w) * 4;
        for (std::int64_t x = 0; x < w; ++x)
        {
            dst[x * 4 + 0] = src[x * (bpp / 8) + 2];
            dst[x * 4 + 1] = src[x * (bpp / 8) + 1];
            dst[x * 4 + 2] = src[x * (bpp / 8) + 0];
            dst[x * 4 + 3] = bpp == 32 ? src[x * 4 + 3] : 255;
        }
    }
    width = w;
    height = h;
    return "";
}

/** @brief 编码二十四位 BMP 文件。
 * @param width 图片宽度。
 * @param height 图片高度。
 * @param pixels 行优先 RGBA 像素。
 * @return 文件字节。
 */
inline Bytes encodeBmp(std::int64_t width, std::int64_t height, const Bytes &pixels)
{
    const std::size_t stride = (static_cast<std::size_t>(width) * 3 + 3) & ~std::size_t(3);
    const std::uint32_t dataSize = static_cast<std::uint32_t>(stride * static_cast<std::size_t>(height));
    Bytes out;
    out.reserve(54 + dataSize);
    out.push_back('B');
    out.push_back('M');
    writeU32LE(out, 54 + dataSize);
    writeU32LE(out, 0);
    writeU32LE(out, 54);
    writeU32LE(out, 40);
    writeU32LE(out, static_cast<std::uint32_t>(width));
    writeU32LE(out, static_cast<std::uint32_t>(height));
    writeU16LE(out, 1);
    writeU16LE(out, 24);
    writeU32LE(out, 0);
    writeU32LE(out, dataSize);
    writeU32LE(out, 2835);
    writeU32LE(out, 2835);
    writeU32LE(out, 0);
    writeU32LE(out, 0);
    for (std::int64_t row = height - 1; row >= 0; --row)
    {
        const std::uint8_t *src = pixels.data() + static_cast<std::size_t>(row) * static_cast<std::size_t>(width) * 4;
        for (std::int64_t x = 0; x < width; ++x)
        {
            out.push_back(src[x * 4 + 2]);
            out.push_back(src[x * 4 + 1]);
            out.push_back(src[x * 4 + 0]);
        }
        for (std::size_t fill = static_cast<std::size_t>(width) * 3; fill < stride; ++fill) out.push_back(0);
    }
    return out;
}

/** @brief 解码 P5 或 P6 二进制文件。
 * @param data 文件字节。
 * @param width 接收宽度。
 * @param height 接收高度。
 * @param pixels 接收 RGBA 像素。
 * @return 空字符串表示成功，否则为错误。
 */
inline Memory::String decodeNetpbm(const Bytes &data, std::int64_t &width, std::int64_t &height, Bytes &pixels)
{
    if (data.size() < 2 || data[0] != 'P') return "Netpbm：魔数不合法";
    const char kind = static_cast<char>(data[1]);
    if (kind != '5' && kind != '6') return "Netpbm：仅支持二进制 P5/P6";
    std::size_t pos = 2;
    std::int64_t fields[3] = { 0, 0, 0 };
    for (int i = 0; i < 3; ++i)
    {
        auto skip = [&] {
            while (pos < data.size())
            {
                const std::uint8_t c = data[pos];
                if (c == '#')
                {
                    while (pos < data.size() && data[pos] != '\n') ++pos;
                }
                else if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
                    ++pos;
                else
                    break;
            }
        };
        skip();
        if (pos >= data.size() || data[pos] < '0' || data[pos] > '9') return "Netpbm：头部不完整";
        std::int64_t value = 0;
        while (pos < data.size() && data[pos] >= '0' && data[pos] <= '9')
            value = value * 10 + (data[pos++] - '0');
        fields[i] = value;
    }
    ++pos;
    width = fields[0];
    height = fields[1];
    const std::int64_t maxval = fields[2];
    if (width <= 0 || height <= 0 || maxval <= 0 || maxval > 255) return "Netpbm：宽高或 maxval 不合法";
    const std::size_t channels = kind == '6' ? 3 : 1;
    if (data.size() - pos < static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * channels)
        return "Netpbm：像素数据不足";
    pixels.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4, 0);
    for (std::size_t p = 0; p < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); ++p)
    {
        std::uint8_t rgb[3];
        for (std::size_t c = 0; c < channels; ++c) rgb[c] = data[pos++];
        pixels[p * 4 + 0] = rgb[0];
        pixels[p * 4 + 1] = channels == 3 ? rgb[1] : rgb[0];
        pixels[p * 4 + 2] = channels == 3 ? rgb[2] : rgb[0];
        pixels[p * 4 + 3] = 255;
    }
    return "";
}

/** @brief 编码 P5 或 P6 二进制文件。
 * @param magic P5 或 P6 魔数。
 * @param width 图片宽度。
 * @param height 图片高度。
 * @param pixels RGBA 像素。
 * @return 文件字节。
 */
inline Bytes encodeNetpbm(const char *magic, std::int64_t width, std::int64_t height, const Bytes &pixels)
{
    Bytes out;
    Memory::String header = Memory::String(magic) + "\n" + Memory::toString(width) + " " + Memory::toString(height) + "\n255\n";
    out.insert(out.end(), header.begin(), header.end());
    for (std::size_t p = 0; p < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); ++p)
    {
        if (magic[1] == '6')
        {
            out.push_back(pixels[p * 4 + 0]);
            out.push_back(pixels[p * 4 + 1]);
            out.push_back(pixels[p * 4 + 2]);
        }
        else
        {
            out.push_back(luma(pixels[p * 4 + 0], pixels[p * 4 + 1], pixels[p * 4 + 2]));
        }
    }
    return out;
}

/** @brief 执行 std::image::__std_imageNew 的固定图像操作。
 * @param frame 当前 C 帧。
 * @return 图像、数值或独立像素结果。
 */
std::int64_t imageNew(ChaliAbiFrame *frame) { return publish(frame, Memory::makeCHeapUnique<ImageData>()); }
/** @brief 解码 std::image::__std_imageNew 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryNew(ChaliAbiFrame *frame)
try
{
    chali_abi_return_integer(frame, int64_t(imageNew(frame)));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageDrop 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 */
void imageDrop(ChaliAbiFrame *frame, std::int64_t handle)
{
    if (chali_abi_record_probe(frame, handle, "std::image::Image")) chali_abi_record_release(frame, handle);
}
/** @brief 解码 std::image::__std_imageDrop 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryDrop(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    imageDrop(frame, static_cast<std::int64_t>(a0));
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageCopy 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 * @return 图像、数值或独立像素结果。
 */
std::int64_t imageCopy(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *image = imageData(frame, handle);
    return image ? publish(frame, Memory::makeCHeapUnique<ImageData>(*image)) : 0;
}
/** @brief 解码 std::image::__std_imageCopy 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryCopy(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(imageCopy(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageMake 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param width 按语言声明传入的数值或缓冲。
 * @param height 按语言声明传入的数值或缓冲。
 * @return 图像、数值或独立像素结果。
 */
std::int64_t imageMake(ChaliAbiFrame *frame, std::int64_t width, std::int64_t height) { return makeImage(frame, width, height); }
/** @brief 解码 std::image::__std_imageMake 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryMake(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    chali_abi_return_integer(frame, int64_t(imageMake(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageFromPixels 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param width 按语言声明传入的数值或缓冲。
 * @param height 按语言声明传入的数值或缓冲。
 * @param pixels 按语言声明传入的数值或缓冲。
 * @return 图像、数值或独立像素结果。
 */
std::int64_t imageFromPixels(ChaliAbiFrame *frame, std::int64_t width, std::int64_t height, Bytes pixels) { return makeImage(frame, width, height, std::move(pixels)); }
/** @brief 解码 std::image::__std_imageFromPixels 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryFromPixels(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    auto pixels = readNumbers<uint8_t>(frame, a2);
    chali_abi_return_integer(frame, int64_t(imageFromPixels(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), std::move(pixels))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageWidth 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 * @return 图像、数值或独立像素结果。
 */
std::int64_t imageWidth(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *image = imageData(frame, handle);
    return image ? image->width : 0;
}
/** @brief 解码 std::image::__std_imageWidth 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryWidth(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(imageWidth(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageHeight 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 * @return 图像、数值或独立像素结果。
 */
std::int64_t imageHeight(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *image = imageData(frame, handle);
    return image ? image->height : 0;
}
/** @brief 解码 std::image::__std_imageHeight 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryHeight(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    chali_abi_return_integer(frame, int64_t(imageHeight(frame, static_cast<std::int64_t>(a0))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imagePixels 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 * @return 图像、数值或独立像素结果。
 */
Bytes imagePixels(ChaliAbiFrame *frame, std::int64_t handle)
{
    auto *image = imageData(frame, handle);
    return image ? image->pixels : Bytes{};
}
/** @brief 解码 std::image::__std_imagePixels 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryPixels(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    const auto values = imagePixels(frame, static_cast<std::int64_t>(a0));
    chali_abi_return_integer(frame, chali_abi_list_from_bytes(frame, values.data(), values.size()));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageByte 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 * @param index 按语言声明传入的数值或缓冲。
 * @return 图像、数值或独立像素结果。
 */
std::uint8_t imageByte(ChaliAbiFrame *frame, std::int64_t handle, std::int64_t index)
{
    auto *image = imageData(frame, handle);
    if (!image) return std::uint8_t{};
    if (index < 0 || static_cast<std::uint64_t>(index) >= image->pixels.size())
    {
        chali_abi_fail(frame, "图片像素下标越界");
        return std::uint8_t{};
    }
    return image->pixels[static_cast<std::size_t>(index)];
}
/** @brief 解码 std::image::__std_imageByte 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryByte(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    chali_abi_return_integer(frame, int64_t(imageByte(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1))));
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__std_imageSetByte 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param reference 按语言声明传入的数值或缓冲。
 * @param index 按语言声明传入的数值或缓冲。
 * @param value 按语言声明传入的数值或缓冲。
 */
void imageSetByte(ChaliAbiFrame *frame, std::int64_t reference, std::int64_t index, std::uint8_t value)
{
    auto *image = imageData(frame, referenceHandle(frame, reference));
    if (!image) return;
    if (index < 0 || static_cast<std::uint64_t>(index) >= image->pixels.size())
    {
        chali_abi_fail(frame, "图片像素下标越界");
        return;
    }
    image->pixels[static_cast<std::size_t>(index)] = value;
}
/** @brief 解码 std::image::__std_imageSetByte 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entrySetByte(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    imageSetByte(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), static_cast<std::uint8_t>(a2));
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__decode 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param data 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageDecode(ChaliAbiFrame *frame, Bytes data, int64_t reply)
{
    std::int64_t width = 0, height = 0;
    Bytes pixels;
    Memory::String message;
    if (data.size() >= 2 && data[0] == 'B' && data[1] == 'M')
        message = decodeBmp(data, width, height, pixels);
    else if (data.size() >= 2 && data[0] == 'P')
        message = decodeNetpbm(data, width, height, pixels);
    else
        message = "图片解码：无法识别的格式（支持 BMP/P5/P6）";
    if (!message.empty())
    {
        replyError(frame, reply, message);
        return;
    }
    writeSlot(frame, reply, 1, width);
    writeSlot(frame, reply, 2, height);
    writePixels(frame, reply, pixels);
}
/** @brief 解码 std::image::__decode 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryDecode(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    auto data = readNumbers<uint8_t>(frame, a0);
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    imageDecode(frame, std::move(data), a1);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__encodeBmp 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param imageHandle 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageEncodeBmp(ChaliAbiFrame *frame, std::int64_t imageHandle, int64_t reply)
{
    auto *image = imageData(frame, imageHandle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "BMP 编码：宽高必须为正");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "BMP 编码：像素缓冲长度不符");
        return;
    }
    writeBytes(frame, reply, encodeBmp(width, height, pixels));
}
/** @brief 解码 std::image::__encodeBmp 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryEncodeBmp(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    imageEncodeBmp(frame, static_cast<std::int64_t>(a0), a1);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__encodePpm 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param imageHandle 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageEncodePpm(ChaliAbiFrame *frame, std::int64_t imageHandle, int64_t reply)
{
    auto *image = imageData(frame, imageHandle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "PPM 编码：宽高必须为正");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "PPM 编码：像素缓冲长度不符");
        return;
    }
    writeBytes(frame, reply, encodeNetpbm("P6", width, height, pixels));
}
/** @brief 解码 std::image::__encodePpm 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryEncodePpm(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    imageEncodePpm(frame, static_cast<std::int64_t>(a0), a1);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__encodePgm 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param imageHandle 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageEncodePgm(ChaliAbiFrame *frame, std::int64_t imageHandle, int64_t reply)
{
    auto *image = imageData(frame, imageHandle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "PGM 编码：宽高必须为正");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "PGM 编码：像素缓冲长度不符");
        return;
    }
    writeBytes(frame, reply, encodeNetpbm("P5", width, height, pixels));
}
/** @brief 解码 std::image::__encodePgm 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryEncodePgm(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    imageEncodePgm(frame, static_cast<std::int64_t>(a0), a1);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__convolve 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 * @param kernel 按语言声明传入的数值或缓冲。
 * @param kw 按语言声明传入的数值或缓冲。
 * @param kh 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageConvolve(ChaliAbiFrame *frame, std::int64_t handle, Memory::Vector<double> kernel, std::int64_t kw, std::int64_t kh, int64_t reply)
{
    auto *image = imageData(frame, handle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "卷积：宽高必须为正");
        return;
    }
    if (kw <= 0 || kh <= 0 || kw % 2 == 0 || kh % 2 == 0 || kw > 15 || kh > 15)
    {
        replyError(frame, reply, "卷积：核边长必须是不超过 15 的奇数");
        return;
    }
    if (kernel.size() != static_cast<std::size_t>(kw) * static_cast<std::size_t>(kh))
    {
        replyError(frame, reply, "卷积：核元素数与 kw*kh 不符");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "卷积：像素缓冲长度不符");
        return;
    }
    const std::int64_t cx = kw / 2, cy = kh / 2;
    Bytes out(pixels.size(), 0);
    for (std::int64_t y = 0; y < height; ++y)
    {
        for (std::int64_t x = 0; x < width; ++x)
        {
            double acc[3] = { 0.0, 0.0, 0.0 };
            for (std::int64_t ky = 0; ky < kh; ++ky)
            {
                const std::int64_t sy = std::min(std::max(y + ky - cy, static_cast<std::int64_t>(0)), height - 1);
                for (std::int64_t kx = 0; kx < kw; ++kx)
                {
                    const std::int64_t sx = std::min(std::max(x + kx - cx, static_cast<std::int64_t>(0)), width - 1);
                    const double weight = kernel[static_cast<std::size_t>(ky) * static_cast<std::size_t>(kw) + static_cast<std::size_t>(kx)];
                    const std::uint8_t *p = pixels.data() + (static_cast<std::size_t>(sy) * static_cast<std::size_t>(width) + static_cast<std::size_t>(sx)) * 4;
                    acc[0] += weight * p[0];
                    acc[1] += weight * p[1];
                    acc[2] += weight * p[2];
                }
            }
            std::uint8_t *dst = out.data() + (static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * 4;
            for (int c = 0; c < 3; ++c)
                dst[c] = static_cast<std::uint8_t>(std::min(std::max(acc[c] + 0.5, 0.0), 255.0));
            dst[3] = pixels[(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * 4 + 3];
        }
    }
    writeSlot(frame, reply, 1, width);
    writeSlot(frame, reply, 2, height);
    writePixels(frame, reply, out);
}
/** @brief 解码 std::image::__convolve 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryConvolve(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    auto kernel = readNumbers<double>(frame, a1);
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    int64_t a3 = 0;
    if (!chali_abi_argument_integer(frame, 3, &a3)) return;
    int64_t a4 = 0;
    if (!chali_abi_argument_integer(frame, 4, &a4)) return;
    imageConvolve(frame, static_cast<std::int64_t>(a0), std::move(kernel), static_cast<std::int64_t>(a2), static_cast<std::int64_t>(a3), a4);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__pool 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param handle 按语言声明传入的数值或缓冲。
 * @param mode 按语言声明传入的数值或缓冲。
 * @param kw 按语言声明传入的数值或缓冲。
 * @param kh 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imagePool(ChaliAbiFrame *frame, std::int64_t handle, std::int64_t mode, std::int64_t kw, std::int64_t kh, int64_t reply)
{
    auto *image = imageData(frame, handle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "池化：宽高必须为正");
        return;
    }
    if (kw <= 0 || kh <= 0)
    {
        replyError(frame, reply, "池化：窗口边长必须为正");
        return;
    }
    if (mode != 0 && mode != 1)
    {
        replyError(frame, reply, "池化：mode 只能是 0(max) 或 1(avg)");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "池化：像素缓冲长度不符");
        return;
    }
    const std::int64_t ow = width / kw, oh = height / kh;
    if (ow <= 0 || oh <= 0)
    {
        replyError(frame, reply, "池化：窗口大于图片");
        return;
    }
    Bytes out(static_cast<std::size_t>(ow) * static_cast<std::size_t>(oh) * 4, 0);
    for (std::int64_t oy = 0; oy < oh; ++oy)
    {
        for (std::int64_t ox = 0; ox < ow; ++ox)
        {
            std::int64_t best[4] = { 0, 0, 0, 0 };
            std::int64_t sum[4] = { 0, 0, 0, 0 };
            std::int64_t count = 0;
            for (std::int64_t ky = 0; ky < kh; ++ky)
            {
                for (std::int64_t kx = 0; kx < kw; ++kx)
                {
                    const std::uint8_t *p = pixels.data() + (static_cast<std::size_t>(oy * kh + ky) * static_cast<std::size_t>(width) + static_cast<std::size_t>(ox * kw + kx)) * 4;
                    for (int c = 0; c < 4; ++c)
                    {
                        if ((ky == 0 && kx == 0) || p[c] > best[c]) best[c] = p[c];
                        sum[c] += p[c];
                    }
                    ++count;
                }
            }
            std::uint8_t *dst = out.data() + (static_cast<std::size_t>(oy) * static_cast<std::size_t>(ow) + static_cast<std::size_t>(ox)) * 4;
            for (int c = 0; c < 4; ++c) dst[c] = static_cast<std::uint8_t>(mode == 0 ? best[c] : (sum[c] + count / 2) / count);
        }
    }
    writeSlot(frame, reply, 1, ow);
    writeSlot(frame, reply, 2, oh);
    writePixels(frame, reply, out);
}
/** @brief 解码 std::image::__pool 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryPool(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    int64_t a3 = 0;
    if (!chali_abi_argument_integer(frame, 3, &a3)) return;
    int64_t a4 = 0;
    if (!chali_abi_argument_integer(frame, 4, &a4)) return;
    imagePool(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), static_cast<std::int64_t>(a2), static_cast<std::int64_t>(a3), a4);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__toGray 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param imageHandle 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageToGray(ChaliAbiFrame *frame, std::int64_t imageHandle, int64_t reply)
{
    auto *image = imageData(frame, imageHandle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "灰度转换：宽高必须为正");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "灰度转换：像素缓冲长度不符");
        return;
    }
    Bytes gray(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), 0);
    for (std::size_t p = 0; p < gray.size(); ++p)
        gray[p] = luma(pixels[p * 4 + 0], pixels[p * 4 + 1], pixels[p * 4 + 2]);
    writeGray(frame, reply, gray);
}
/** @brief 解码 std::image::__toGray 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryToGray(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    imageToGray(frame, static_cast<std::int64_t>(a0), a1);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__fromGray 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param width 按语言声明传入的数值或缓冲。
 * @param height 按语言声明传入的数值或缓冲。
 * @param gray 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageFromGray(ChaliAbiFrame *frame, std::int64_t width, std::int64_t height, Bytes gray, int64_t reply)
{
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "灰度还原：宽高必须为正");
        return;
    }
    if (gray.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height))
    {
        replyError(frame, reply, "灰度还原：灰度缓冲长度不符");
        return;
    }
    Bytes out(gray.size() * 4, 0);
    for (std::size_t p = 0; p < gray.size(); ++p)
    {
        out[p * 4 + 0] = gray[p];
        out[p * 4 + 1] = gray[p];
        out[p * 4 + 2] = gray[p];
        out[p * 4 + 3] = 255;
    }
    writeSlot(frame, reply, 1, width);
    writeSlot(frame, reply, 2, height);
    writePixels(frame, reply, out);
}
/** @brief 解码 std::image::__fromGray 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryFromGray(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    auto gray = readNumbers<uint8_t>(frame, a2);
    int64_t a3 = 0;
    if (!chali_abi_argument_integer(frame, 3, &a3)) return;
    imageFromGray(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), std::move(gray), a3);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__toARGB 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param imageHandle 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageToARGB(ChaliAbiFrame *frame, std::int64_t imageHandle, int64_t reply)
{
    auto *image = imageData(frame, imageHandle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "ARGB 转换：宽高必须为正");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "ARGB 转换：像素缓冲长度不符");
        return;
    }
    Memory::Vector<int64_t> values;
    values.reserve(size_t(width * height));
    for (std::size_t p = 0; p < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); ++p)
    {
        const std::uint32_t value = (static_cast<std::uint32_t>(pixels[p * 4 + 3]) << 24) | (static_cast<std::uint32_t>(pixels[p * 4 + 0]) << 16) | (static_cast<std::uint32_t>(pixels[p * 4 + 1]) << 8) | static_cast<std::uint32_t>(pixels[p * 4 + 2]);
        values.push_back(static_cast<std::int64_t>(value));
    }
    appendNumbers(frame, reply, 6, values);
}
/** @brief 解码 std::image::__toARGB 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryToARGB(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    imageToARGB(frame, static_cast<std::int64_t>(a0), a1);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__fromARGB 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param width 按语言声明传入的数值或缓冲。
 * @param height 按语言声明传入的数值或缓冲。
 * @param argb 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageFromARGB(ChaliAbiFrame *frame, std::int64_t width, std::int64_t height, Memory::Vector<std::uint32_t> argb, int64_t reply)
{
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "ARGB 还原：宽高必须为正");
        return;
    }
    if (argb.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height))
    {
        replyError(frame, reply, "ARGB 还原：缓冲长度不符");
        return;
    }
    Bytes out(argb.size() * 4, 0);
    for (std::size_t p = 0; p < argb.size(); ++p)
    {
        const std::uint32_t value = argb[p];
        out[p * 4 + 0] = static_cast<std::uint8_t>((value >> 16) & 0xFF);
        out[p * 4 + 1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
        out[p * 4 + 2] = static_cast<std::uint8_t>(value & 0xFF);
        out[p * 4 + 3] = static_cast<std::uint8_t>((value >> 24) & 0xFF);
    }
    writeSlot(frame, reply, 1, width);
    writeSlot(frame, reply, 2, height);
    writePixels(frame, reply, out);
}
/** @brief 解码 std::image::__fromARGB 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryFromARGB(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    auto argb = readNumbers<std::uint32_t>(frame, a2);
    int64_t a3 = 0;
    if (!chali_abi_argument_integer(frame, 3, &a3)) return;
    imageFromARGB(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), std::move(argb), a3);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__toRGB565 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param imageHandle 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageToRGB565(ChaliAbiFrame *frame, std::int64_t imageHandle, int64_t reply)
{
    auto *image = imageData(frame, imageHandle);
    if (!image) return;
    const auto width = image->width, height = image->height;
    const auto &pixels = image->pixels;
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "RGB565 转换：宽高必须为正");
        return;
    }
    if (pixels.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4)
    {
        replyError(frame, reply, "RGB565 转换：像素缓冲长度不符");
        return;
    }
    Memory::Vector<int64_t> values;
    values.reserve(size_t(width * height));
    for (std::size_t p = 0; p < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); ++p)
    {
        const std::uint16_t value = static_cast<std::uint16_t>(((pixels[p * 4 + 0] & 0xF8) << 8) | ((pixels[p * 4 + 1] & 0xFC) << 3) | (pixels[p * 4 + 2] >> 3));
        values.push_back(value);
    }
    appendNumbers(frame, reply, 7, values);
}
/** @brief 解码 std::image::__toRGB565 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryToRGB565(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    imageToRGB565(frame, static_cast<std::int64_t>(a0), a1);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__fromRGB565 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param width 按语言声明传入的数值或缓冲。
 * @param height 按语言声明传入的数值或缓冲。
 * @param rgb565 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageFromRGB565(ChaliAbiFrame *frame, std::int64_t width, std::int64_t height, Memory::Vector<std::uint16_t> rgb565, int64_t reply)
{
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "RGB565 还原：宽高必须为正");
        return;
    }
    if (rgb565.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height))
    {
        replyError(frame, reply, "RGB565 还原：缓冲长度不符");
        return;
    }
    Bytes out(rgb565.size() * 4, 0);
    for (std::size_t p = 0; p < rgb565.size(); ++p)
    {
        const std::uint16_t value = rgb565[p];
        const int r5 = (value >> 11) & 0x1F, g6 = (value >> 5) & 0x3F, b5 = value & 0x1F;
        out[p * 4 + 0] = static_cast<std::uint8_t>((r5 << 3) | (r5 >> 2));
        out[p * 4 + 1] = static_cast<std::uint8_t>((g6 << 2) | (g6 >> 4));
        out[p * 4 + 2] = static_cast<std::uint8_t>((b5 << 3) | (b5 >> 2));
        out[p * 4 + 3] = 255;
    }
    writeSlot(frame, reply, 1, width);
    writeSlot(frame, reply, 2, height);
    writePixels(frame, reply, out);
}
/** @brief 解码 std::image::__fromRGB565 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryFromRGB565(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    auto rgb565 = readNumbers<std::uint16_t>(frame, a2);
    int64_t a3 = 0;
    if (!chali_abi_argument_integer(frame, 3, &a3)) return;
    imageFromRGB565(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), std::move(rgb565), a3);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__otsu 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param width 按语言声明传入的数值或缓冲。
 * @param height 按语言声明传入的数值或缓冲。
 * @param gray 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageOtsu(ChaliAbiFrame *frame, std::int64_t width, std::int64_t height, Bytes gray, int64_t reply)
{
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "大津法：宽高必须为正");
        return;
    }
    if (gray.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height))
    {
        replyError(frame, reply, "大津法：灰度缓冲长度不符");
        return;
    }
    std::int64_t histogram[256] = { 0 };
    for (std::uint8_t value : gray) ++histogram[value];
    const std::int64_t total = static_cast<std::int64_t>(gray.size());
    double sumAll = 0.0;
    for (int v = 0; v < 256; ++v) sumAll += static_cast<double>(v) * histogram[v];
    double sumBackground = 0.0;
    std::int64_t countBackground = 0;
    double bestVariance = -1.0;
    int bestThreshold = 0;
    for (int v = 0; v < 256; ++v)
    {
        countBackground += histogram[v];
        if (countBackground == 0) continue;
        const std::int64_t countForeground = total - countBackground;
        if (countForeground == 0) break;
        sumBackground += static_cast<double>(v) * histogram[v];
        const double wB = static_cast<double>(countBackground) / total;
        const double wF = static_cast<double>(countForeground) / total;
        const double meanB = sumBackground / countBackground;
        const double meanF = (sumAll - sumBackground) / countForeground;
        const double variance = wB * wF * (meanB - meanF) * (meanB - meanF);
        if (variance > bestVariance)
        {
            bestVariance = variance;
            bestThreshold = v;
        }
    }
    writeSlot(frame, reply, 8, bestThreshold);
}
/** @brief 解码 std::image::__otsu 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryOtsu(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    auto gray = readNumbers<uint8_t>(frame, a2);
    int64_t a3 = 0;
    if (!chali_abi_argument_integer(frame, 3, &a3)) return;
    imageOtsu(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), std::move(gray), a3);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
/** @brief 执行 std::image::__binarize 的固定图像操作。
 * @param frame 当前 C 帧。
 * @param width 按语言声明传入的数值或缓冲。
 * @param height 按语言声明传入的数值或缓冲。
 * @param gray 按语言声明传入的数值或缓冲。
 * @param threshold 按语言声明传入的数值或缓冲。
 * @param reply 按语言声明传入的回执地址。
 */
void imageBinarize(ChaliAbiFrame *frame, std::int64_t width, std::int64_t height, Bytes gray, std::int64_t threshold, int64_t reply)
{
    if (width <= 0 || height <= 0)
    {
        replyError(frame, reply, "二值化：宽高必须为正");
        return;
    }
    if (threshold < 0 || threshold > 255)
    {
        replyError(frame, reply, "二值化：阈值必须在 0..255");
        return;
    }
    if (gray.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height))
    {
        replyError(frame, reply, "二值化：灰度缓冲长度不符");
        return;
    }
    Bytes out(gray.size(), 0);
    for (std::size_t p = 0; p < gray.size(); ++p) out[p] = gray[p] > threshold ? 255 : 0;
    writeGray(frame, reply, out);
}
/** @brief 解码 std::image::__binarize 的 C 实参并隔离异常。
 * @param frame 实参及返回帧。
 */
void CHALI_ABI_CALL entryBinarize(ChaliAbiFrame *frame)
try
{
    int64_t a0 = 0;
    if (!chali_abi_argument_integer(frame, 0, &a0)) return;
    int64_t a1 = 0;
    if (!chali_abi_argument_integer(frame, 1, &a1)) return;
    int64_t a2 = 0;
    if (!chali_abi_argument_integer(frame, 2, &a2)) return;
    auto gray = readNumbers<uint8_t>(frame, a2);
    int64_t a3 = 0;
    if (!chali_abi_argument_integer(frame, 3, &a3)) return;
    int64_t a4 = 0;
    if (!chali_abi_argument_integer(frame, 4, &a4)) return;
    imageBinarize(frame, static_cast<std::int64_t>(a0), static_cast<std::int64_t>(a1), std::move(gray), static_cast<std::int64_t>(a3), a4);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "图片处理失败");
}
}  // namespace
/** @brief 取得图像模块的 C 入口表。
 * @param count 接收入口数，可为空。
 * @return 生命周期覆盖进程的入口表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_image_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::image::__binarize", entryBinarize, 5 },
        { "std::image::__convolve", entryConvolve, 5 },
        { "std::image::__decode", entryDecode, 2 },
        { "std::image::__encodeBmp", entryEncodeBmp, 2 },
        { "std::image::__encodePgm", entryEncodePgm, 2 },
        { "std::image::__encodePpm", entryEncodePpm, 2 },
        { "std::image::__fromARGB", entryFromARGB, 4 },
        { "std::image::__fromGray", entryFromGray, 4 },
        { "std::image::__fromRGB565", entryFromRGB565, 4 },
        { "std::image::__otsu", entryOtsu, 4 },
        { "std::image::__pool", entryPool, 5 },
        { "std::image::__std_imageByte", entryByte, 2 },
        { "std::image::__std_imageCopy", entryCopy, 1 },
        { "std::image::__std_imageDrop", entryDrop, 1 },
        { "std::image::__std_imageFromPixels", entryFromPixels, 3 },
        { "std::image::__std_imageHeight", entryHeight, 1 },
        { "std::image::__std_imageMake", entryMake, 2 },
        { "std::image::__std_imageNew", entryNew, 0 },
        { "std::image::__std_imagePixels", entryPixels, 1 },
        { "std::image::__std_imageSetByte", entrySetByte, 3 },
        { "std::image::__std_imageWidth", entryWidth, 1 },
        { "std::image::__toARGB", entryToARGB, 2 },
        { "std::image::__toGray", entryToGray, 2 },
        { "std::image::__toRGB565", entryToRGB565, 2 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}

#ifndef GLUE_NO_STD_IMAGE
#define CHALI_BUILD_BIND_MODULE image
#include "aot_bind_module.h"
#endif
