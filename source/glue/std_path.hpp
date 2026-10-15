#ifndef PARSE_ENGINE_STD_PATH_HPP
#define PARSE_ENGINE_STD_PATH_HPP

#include "../magel_glue.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>

namespace std_path_glue
{

/* `std::path::PathString` 的纯词法路径处理。从 VM 内部原生表（`kSlotNativeEntries`）
 * 搬到 Registry 胶水，语义逐条照搬原实现：反斜杠一律归一成 `/`，`..` 只回退上一段、
 * 顶到绝对路径开头就保留，不访问文件系统。 */
inline Memory::String normalizePathText(Memory::String path)
{
    std::replace(path.begin(), path.end(), '\\', '/');
    Memory::String prefix;
    std::size_t begin = 0u;
    bool absolute = false;
    if (path.size() >= 2u && std::isalpha(static_cast<unsigned char>(path[0])) && path[1] == ':')
    {
        prefix = path.substr(0u, 2u);
        begin = 2u;
        if (begin < path.size() && path[begin] == '/')
        {
            absolute = true;
            ++begin;
        }
    }
    else if (!path.empty() && path[0] == '/')
    {
        absolute = true;
        begin = 1u;
    }
    Memory::Vector<Memory::String> parts;
    while (begin <= path.size())
    {
        const std::size_t end = path.find('/', begin);
        Memory::String part = path.substr(
            begin, end == Memory::String::npos ? Memory::String::npos : end - begin);
        if (!part.empty() && part != ".")
        {
            if (part == ".." && !parts.empty() && parts.back() != "..")
                parts.pop_back();
            else if (part != ".." || !absolute)
                parts.push_back(part);
        }
        if (end == Memory::String::npos) break;
        begin = end + 1u;
    }
    Memory::String result = prefix;
    if (absolute) result += '/';
    for (const Memory::String &part : parts)
    {
        if (!result.empty() && result.back() != '/') result += '/';
        result += part;
    }
    if (result.empty()) return ".";
    return result;
}

inline Memory::String fileNameText(const Memory::String &path)
{
    const Memory::String normalized = normalizePathText(path);
    const std::size_t slash = normalized.find_last_of('/');
    return slash == Memory::String::npos ? normalized : normalized.substr(slash + 1u);
}

inline bool bind(magel_glue::Registry &registry, Memory::String *error = nullptr)
{
    using Memory::String;
    return registry.bindTyped<String, String>(
               "std::path::__pathNormalize",
               [](String path) { return normalizePathText(path); }, error)
           && registry.bindTyped<String, String, String>(
               "std::path::__pathJoin",
               [](String left, String right) {
                   const bool rightAbsolute =
                       !right.empty() && (right[0] == '/' || right[0] == '\\' || (right.size() >= 2u && std::isalpha(static_cast<unsigned char>(right[0])) && right[1] == ':'));
                   const String joined =
                       rightAbsolute ? right : left + (left.empty() ? "" : "/") + right;
                   return normalizePathText(joined);
               },
               error)
           && registry.bindTyped<String, String>(
               "std::path::__pathFileName",
               [](String path) { return fileNameText(path); }, error)
           && registry.bindTyped<String, String>(
               "std::path::__pathExtension",
               [](String path) {
                   const String name = fileNameText(path);
                   const std::size_t dot = name.find_last_of('.');
                   return (dot == String::npos || dot == 0u) ? String() : name.substr(dot);
               },
               error);
}

}  // namespace std_path_glue

#endif
