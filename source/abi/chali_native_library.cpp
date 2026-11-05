/** @file chali_native_library.cpp @brief 私有动态库加载及符号查询。 */
#include "chali_native_library.hpp"
#include "../allocator/c_heap_allocator.hpp"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <dlfcn.h>
#endif

namespace chali_host_detail
{
/** @brief 释放本对象持有的库引用，不销毁库内的语言值。 */
NativeLibrary::~NativeLibrary()
{
#ifdef _WIN32
    if (handle) FreeLibrary(static_cast<HMODULE>(handle));
#elif defined(__unix__) || defined(__APPLE__)
    if (handle) dlclose(handle);
#endif
}

/**
 * @brief 打开原生库，保持原有路径及系统诊断规则。
 * @param inputPath UTF-8 路径。
 * @param error 可选的诊断输出。
 * @return 打开成功时为 true。
 */
bool NativeLibrary::open(const Memory::String &inputPath, Memory::String *error)
{
    Memory::String path = inputPath;
    for (char &value : path)
        if (value == '\\') value = '/';
#ifdef _WIN32
    if (handle) FreeLibrary(static_cast<HMODULE>(handle));
    handle = nullptr;
    const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, nullptr, 0);
    if (length > 0)
    {
        Memory::CHeapVector<wchar_t> wide(static_cast<std::size_t>(length));
        if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, wide.data(), length) > 0)
            handle = LoadLibraryW(wide.data());
    }
#elif defined(__unix__) || defined(__APPLE__)
    if (handle) dlclose(handle);
    handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
    if (handle) return true;
    if (error)
    {
        *error = "无法加载动态库 " + path;
#if defined(__unix__) || defined(__APPLE__)
        if (const char *reason = dlerror()) *error += ": " + Memory::String(reason);
#endif
    }
    return false;
}

/**
 * @brief 从已打开的库中取得精确符号地址。
 * @param name 原始 C 符号名。
 * @param error 可选的诊断输出。
 * @return 原生地址，失败时为空。
 */
void *NativeLibrary::resolve(const Memory::String &name, Memory::String *error) const
{
    if (!handle)
    {
        if (error) *error = "动态库尚未加载";
        return nullptr;
    }
    void *address = nullptr;
#ifdef _WIN32
    address = reinterpret_cast<void *>(GetProcAddress(static_cast<HMODULE>(handle), name.c_str()));
#elif defined(__unix__) || defined(__APPLE__)
    dlerror();
    address = dlsym(handle, name.c_str());
#endif
    if (!address && error) *error = "动态库中找不到 C 符号：" + name;
    return address;
}
}  // namespace chali_host_detail
