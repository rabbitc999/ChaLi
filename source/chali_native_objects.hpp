/** @file chali_native_objects.hpp
 * @brief 原生链接输入的目标识别和归档边界检查，不执行输入内容。
 */
#pragma once
#include <filesystem>
#include <string>
namespace chali_native_objects
{
/** @brief 当前目标登记支持的六十四位小端对象格式。 */
enum class Format
{
    CoffX64,
    ElfX64,
    ElfArm64
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
}  // namespace chali_native_objects
