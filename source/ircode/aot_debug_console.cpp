/**
 * @file aot_debug_console.cpp
 * @brief 仅在调试模块中链接的终端与参数文件服务。
 */
#include "../chali_i18n.hpp"
#include "../allocator/chali_allocator.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

extern "C" {
void chali_host_enable_debug(bool (*)());
void chali_host_set_debug_arguments(const Memory::Vector<Memory::String> *);
void chali_panic(const char *);
}
namespace
{
/**
 * @brief 读取可选的调试参数文件，未配置时由普通进程参数路径处理。
 * @return 已读取并安装调试参数时返回 true。
 */
bool processDebugArguments()
{
    const char *path = std::getenv("CHALI_DEBUG_ARGUMENTS");
    if (path && *path)
    {
        std::ifstream input(std::filesystem::u8path(path), std::ios::binary | std::ios::ate);
        const auto length = input.tellg();
        if (!input || length < 4 || length > 16 * 1024 * 1024) chali_panic("调试参数文件长度无效");
        Memory::String bytes(static_cast<std::size_t>(length), '\0');
        input.seekg(0);
        if (!input.read(bytes.data(), length)) chali_panic("无法读取调试参数文件");
        std::size_t offset = 0;
        const auto readSize = [&]() {
            if (bytes.size() - offset < 4) chali_panic("调试参数文件被截断");
            std::uint32_t value = 0;
            for (unsigned shift = 0; shift != 32; shift += 8) value |= static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[offset++])) << shift;
            return value;
        };
        const auto count = readSize();
        if (count > 65536) chali_panic("调试参数数量超过限制");
        Memory::Vector<Memory::String> arguments;
        for (std::uint32_t at = 0; at < count; ++at)
        {
            const auto size = readSize();
            if (size > bytes.size() - offset) chali_panic("调试参数文件被截断");
            arguments.emplace_back(bytes.data() + offset, size);
            offset += size;
        }
        if (offset != bytes.size()) chali_panic("调试参数文件包含尾随数据");
        chali_host_set_debug_arguments(&arguments);
        return true;
    }

    return false;
}
}
extern "C" {
/**
 * @brief 启用调试参数传输并连接会话专属终端，Linux 的 @ 前缀表示抽象套接字。
 * @return 无返回值；未设置终端时保持宿主标准流，连接失败则报告并退出。
 */
void chali_debug_console()
{
    chali_host_enable_debug(processDebugArguments);
    const char *endpoint = std::getenv("CHALI_DEBUG_CONSOLE");
    if (!endpoint || !*endpoint) return;
    const auto openDescriptor = [&](const char *suffix) {
        const auto name = std::string(endpoint) + suffix;
#ifdef _WIN32
        const HANDLE pipe = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (pipe == INVALID_HANDLE_VALUE) return -1;
        const int descriptor = _open_osfhandle(reinterpret_cast<intptr_t>(pipe), _O_RDWR | _O_BINARY);
        if (descriptor < 0) CloseHandle(pipe);
        return descriptor;
#elif defined(__linux__)
        sockaddr_un address{};
        address.sun_family = AF_UNIX;
        if (name.size() >= sizeof(address.sun_path)) return -1;
        const bool abstract = name.front() == '@';
        if (abstract) std::memcpy(address.sun_path + 1, name.data() + 1, name.size() - 1);
        else std::strcpy(address.sun_path, name.c_str());
        const auto addressSize = abstract ? offsetof(sockaddr_un, sun_path) + name.size() : sizeof(address);
        const int descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
        if (descriptor >= 0 && connect(descriptor, reinterpret_cast<sockaddr *>(&address), static_cast<socklen_t>(addressSize)) == 0) return descriptor;
        if (descriptor >= 0) close(descriptor);
        return -1;
#else
        return -1;
#endif
    };
    const int input = openDescriptor("-stdin"), output = openDescriptor("-stdout");
#ifdef _WIN32
    const bool valid = input >= 0 && output >= 0 && _dup2(input, 0) == 0 && _dup2(output, 1) == 0 && _dup2(output, 2) == 0;
    if (input > 2) _close(input);
    if (output > 2) _close(output);
#elif defined(__linux__)
    const bool valid = input >= 0 && output >= 0 && dup2(input, 0) >= 0 && dup2(output, 1) >= 0 && dup2(output, 2) >= 0;
    if (input > 2) close(input);
    if (output > 2) close(output);
#else
    const bool valid = false;
#endif
    if (!valid)
    {
        std::fputs("chali: cannot connect debug terminal\n", stderr);
        std::exit(2);
    }
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);
}

}
