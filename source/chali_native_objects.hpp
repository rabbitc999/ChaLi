/** @file chali_native_objects.hpp
 * @brief 原生链接输入的目标识别和归档边界检查，不执行输入内容。
 */
#pragma once
#include <filesystem>
#include <cstdint>
#include <string>
#include <string_view>
namespace chali_native_objects
{
/** @brief 当前目标登记支持的六十四位小端对象格式。 */
enum class Format
{
    CoffX64,
    ElfX64,
    ElfArm64,
    ElfX86,
    ElfRiscv64,
    ElfRiscv32,
    ElfMipsel,
    ElfArm32,
    ElfMips64el
};
/**
 * @brief 检查原生输入的格式、架构及自包含归档成员的文件边界。
 * @param path 普通对象、共享库或静态归档的路径。
 * @param target 已由目标登记器选择的对象格式。
 * @param error 接收带文件身份的拒绝原因。
 * @param allowArchive 是否接受自包含静态归档。
 * @param allowBitcode 是否允许可信内部 LTO 输入；不取代 LLVM bitcode 验证。
 * @return 基础检查通过为真；实际符号及重定位仍由 LLVM／LLD 验证。
 */
bool inspect(const std::filesystem::path &path, Format target, std::string &error, bool allowArchive = true, bool allowBitcode = false);
/**
 * @brief 在容器文件的有界区间内检查原生对象，不提取成员。
 * @param path 容器文件路径。
 * @param offset 区间的绝对文件偏移。
 * @param size 区间长度。
 * @param target 期望的目标格式。
 * @param error 接收文件及拒绝原因。
 * @param allowArchive 是否允许自包含归档。
 * @param allowBitcode 是否接受内部 bitcode。
 * @return 全部边界及基础格式检查通过为真。
 */
bool inspectRegion(const std::filesystem::path &path, std::uint64_t offset, std::uint64_t size, Format target, std::string &error, bool allowArchive = true, bool allowBitcode = false);
/**
 * @brief 检查调用方拥有的对象字节，避免重新打开容器产生身份竞态。
 * @param bytes 在调用期间有效的原始对象或归档。
 * @param target 期望格式。
 * @param error 接收拒绝原因。
 * @param allowArchive 是否接受自包含归档。
 * @param allowBitcode 是否接受内部 bitcode。
 * @return 基础格式和边界检查通过为真。
 */
bool inspectBytes(std::string_view bytes, Format target, std::string &error, bool allowArchive = true, bool allowBitcode = false);
}  // namespace chali_native_objects
