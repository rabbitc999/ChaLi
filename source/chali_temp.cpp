/**
 * @file chali_temp.cpp
 * @brief 定位仓库临时目录，不向项目同级或系统目录写入临时文件夹。
 */
#include "chali_temp.hpp"
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace chali_temp
{
/**
 * @brief 在候选路径的祖先中查找 ChaLi 源码根。
 * @param directory 当前目录或可执行文件所在目录。
 * @return 找到的源码根，找不到时返回空路径。
 */
static std::filesystem::path repository(std::filesystem::path directory)
{
    std::error_code error;
    while (!directory.empty())
    {
        if (std::filesystem::is_regular_file(directory / "VERSION", error) && std::filesystem::is_regular_file(directory / "source/chali.gram", error)) return directory;
        const auto parent = directory.parent_path();
        if (parent == directory) break;
        directory = parent;
    }
    return {};
}

/**
 * @brief 查找项目根并创建唯一允许的内部临时目录。
 * @param error 接收实际文件系统错误。
 * @return 项目 tmp 的规范路径，失败返回空路径。
 */
std::filesystem::path root(std::error_code &error)
{
    error.clear();
    const auto working = std::filesystem::current_path(error);
    if (error) return {};
    auto base = repository(working);
    if (base.empty())
    {
#ifdef _WIN32
        wchar_t name[32768];
        const auto size = GetModuleFileNameW(nullptr, name, 32768);
        if (size && size < 32768)
        {
            const auto directory = std::filesystem::path(name).parent_path();
            base = repository(directory);
            if (base.empty()) base = directory;
        }
#elif defined(__linux__)
        const auto executable = std::filesystem::read_symlink("/proc/self/exe", error);
        if (!error)
        {
            base = repository(executable.parent_path());
            if (base.empty()) base = executable.parent_path();
        }
#endif
    }
    if (base.empty()) base = working;
    error.clear();
    base = std::filesystem::canonical(base, error);
    if (error) return {};
    const auto temporary = base / "tmp";
    const auto resolved = std::filesystem::weakly_canonical(temporary, error);
    if (error) return {};
    if (resolved.parent_path() != base)
    {
        error = std::make_error_code(std::errc::permission_denied);
        return {};
    }
    std::filesystem::create_directories(resolved, error);
    return error ? std::filesystem::path() : resolved;
}

/**
 * @brief 取得可用的项目临时目录并保留原始失败原因。
 * @return 项目 tmp 绝对路径。
 */
std::filesystem::path root()
{
    std::error_code error;
    auto directory = root(error);
    if (error) throw std::filesystem::filesystem_error("无法创建项目 tmp 目录", directory, error);
    return directory;
}
}  // namespace chali_temp
