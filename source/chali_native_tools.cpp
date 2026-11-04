/** @file chali_native_tools.cpp
 * @brief 原生工具子进程及产物事务，不通过 shell 拼接命令。
 */
#include "chali_native_tools.hpp"
#include "chali_temp.hpp"
#include "console/utf8.hpp"
#include <atomic>
#include <chrono>
#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <csignal>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace chali_native_tools
{
namespace
{
/**
 * @brief 为目录及同目录暂存文件生成进程内不重复的名称后缀。
 * @return 时钟和原子序号构成的后缀；文件创建仍使用排他语义。
 */
std::string suffix()
{
    static std::atomic<unsigned long long> counter{ 0 };
    return std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" + std::to_string(++counter);
}
}  // namespace
/** @brief 创建根 tmp 下单次工具的独占目录，失败抛出异常。 */
Workspace::Workspace()
{
    const auto root = chali_temp::root();
    for (unsigned attempt = 0; attempt < 100; ++attempt)
    {
        const auto candidate = root / ("chali-native-tool-" + suffix());
        std::error_code error;
        if (std::filesystem::create_directory(candidate, error))
        {
            path = candidate;
            return;
        }
        if (error && error != std::errc::file_exists) throw std::runtime_error("无法创建原生工具工作区：" + error.message());
    }
    throw std::runtime_error("无法创建独占原生工具工作区");
}
/** @brief 删除本次拥有的工作目录，析构不抛出文件系统异常。 */
Workspace::~Workspace()
{
    std::error_code error;
    if (!path.empty()) std::filesystem::remove_all(path, error);
}
/**
 * @brief 无 shell 执行参数数组并等待子进程，最多等待 180 秒。
 * @param arguments 工具路径及其参数。
 * @param log 工作区诊断文件。
 * @param diagnostic 接收启动、超时及最多 128 KiB 工具输出。
 * @return 实际退出码或一。
 */
int execute(const std::vector<std::string> &arguments, const std::filesystem::path &log, std::string &diagnostic)
{
    if (arguments.empty())
    {
        diagnostic = "缺少原生工具路径";
        return 1;
    }
    int code = 1;
#ifdef _WIN32
    std::wstring command;
    for (const auto &argument : arguments)
    {
        if (!command.empty()) command += L' ';
        command += tool_console::quoteArgument(std::filesystem::u8path(argument).wstring());
    }
    SECURITY_ATTRIBUTES attributes{ sizeof(attributes), nullptr, TRUE };
    const HANDLE file = CreateFileW(log.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &attributes, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
    {
        diagnostic = "无法创建原生工具诊断日志";
        return 1;
    }
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = startup.hStdError = file;
    PROCESS_INFORMATION child{};
    const BOOL started = CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startup, &child);
    const DWORD startError = started ? 0 : GetLastError();
    CloseHandle(file);
    if (!started)
    {
        diagnostic = "无法启动原生工具，系统错误 " + std::to_string(startError);
        return 1;
    }
    CloseHandle(child.hThread);
    const DWORD waited = WaitForSingleObject(child.hProcess, 180000);
    if (waited == WAIT_OBJECT_0)
    {
        DWORD result = 1;
        if (GetExitCodeProcess(child.hProcess, &result)) code = result <= 255 ? int(result) : 1;
    }
    else
    {
        TerminateProcess(child.hProcess, 1);
        WaitForSingleObject(child.hProcess, INFINITE);
        diagnostic = "原生工具执行超时或等待失败\n";
    }
    CloseHandle(child.hProcess);
#else
    std::vector<char *> values;
    for (const auto &argument : arguments) values.push_back(const_cast<char *>(argument.c_str()));
    values.push_back(nullptr);
    const int file = open(log.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (file < 0)
    {
        diagnostic = "无法创建原生工具诊断日志";
        return 1;
    }
    const auto child = fork();
    if (child == 0)
    {
        if (dup2(file, STDOUT_FILENO) < 0 || dup2(file, STDERR_FILENO) < 0) _exit(126);
        close(file);
        execvp(values[0], values.data());
        _exit(127);
    }
    close(file);
    if (child < 0)
    {
        diagnostic = "无法启动原生工具";
        return 1;
    }
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(180);
    for (;;)
    {
        int status = 0;
        const auto result = waitpid(child, &status, WNOHANG);
        if (result == child)
        {
            code = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
            break;
        }
        if (result < 0 && errno != EINTR) break;
        if (std::chrono::steady_clock::now() >= deadline)
        {
            kill(child, SIGKILL);
            while (waitpid(child, &status, 0) < 0 && errno == EINTR) {}
            diagnostic = "原生工具执行超时\n";
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
#endif
    std::ifstream input(log, std::ios::binary);
    std::string text(128u * 1024u, '\0');
    input.read(text.data(), static_cast<std::streamsize>(text.size()));
    text.resize(static_cast<size_t>(input.gcount()));
    diagnostic += text;
    return code;
}
/**
 * @brief 暂存到目标的同目录文件，Windows 短暂占用最多等待五秒，失败保留原产物。
 * @param source 已成功的工作区文件。
 * @param destination 最终路径。
 * @param error 接收失败说明。
 * @return 替换成功为真。
 */
bool publish(const std::filesystem::path &source, const std::filesystem::path &destination, std::string &error)
{
    std::filesystem::path staged;
    std::error_code ec;
    for (unsigned attempt = 0; attempt < 100; ++attempt)
    {
        staged = destination.parent_path() / std::filesystem::u8path(destination.filename().u8string() + ".chali-" + suffix() + ".tmp");
#ifdef _WIN32
        const HANDLE file = CreateFileW(staged.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE)
        {
            CloseHandle(file);
            break;
        }
        if (GetLastError() != ERROR_FILE_EXISTS && GetLastError() != ERROR_ALREADY_EXISTS)
        {
            error = "无法创建产物暂存文件";
            return false;
        }
#else
        const int file = open(staged.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (file >= 0)
        {
            close(file);
            break;
        }
        if (errno != EEXIST)
        {
            error = "无法创建产物暂存文件";
            return false;
        }
#endif
        staged.clear();
    }
    if (staged.empty())
    {
        error = "无法创建产物暂存文件";
        return false;
    }
    bool copied = false;
    {
        std::ifstream input(source, std::ios::binary);
        std::ofstream output(staged, std::ios::binary | std::ios::trunc);
        const bool opened = input.is_open() && output.is_open();
        char buffer[64u * 1024u];
        while (opened && input && output)
        {
            input.read(buffer, sizeof(buffer));
            output.write(buffer, input.gcount());
        }
        output.close();
        copied = opened && !input.bad() && !output.fail();
    }
    if (!copied)
    {
        error = "无法复制原生工具产物";
        std::filesystem::remove(staged, ec);
        return false;
    }
#ifndef _WIN32
    const auto mode = std::filesystem::status(source, ec).permissions();
    if (!ec) std::filesystem::permissions(staged, mode, std::filesystem::perm_options::replace, ec);
    if (ec)
    {
        error = ec.message();
        std::filesystem::remove(staged, ec);
        return false;
    }
#endif
    // 同目录只暂存一个文件，不创建 tmp 外的临时目录；替换失败不触碰原文件。
#ifdef _WIN32
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    bool ok = false;
    while (!(ok = MoveFileExW(staged.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0))
    {
        const auto failure = GetLastError();
        if ((failure != ERROR_ACCESS_DENIED && failure != ERROR_SHARING_VIOLATION && failure != ERROR_LOCK_VIOLATION)
            || std::chrono::steady_clock::now() >= deadline)
        {
            error = "无法替换产物，系统错误 " + std::to_string(failure);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
#else
    std::filesystem::rename(staged, destination, ec);
    const bool ok = !ec;
    if (!ok) error = ec.message();
#endif
    if (!ok) std::filesystem::remove(staged, ec);
    return ok;
}
}  // namespace chali_native_tools
