/**
 * @file chali_aot_runtime_assets.cpp
 * @brief 将内嵌原生服务解压至隔离的链接目录。
 */
#include "chali_aot_runtime_assets.hpp"
#include "chali_aot_target.hpp"
#include "chali_stdlib.hpp"
#include "chali_temp.hpp"
#include "chali_native_tools.hpp"
#include <miniz_tinfl.h>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

namespace chali_build
{
/**
 * @brief 创建独立临时目录，避免并行构建互相覆盖。
 */
RuntimeAssets::RuntimeAssets()
{
    const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
    for (unsigned attempt = 0; attempt < 100; ++attempt)
    {
        const auto path = chali_temp::root() / ("chali-native-link-" + std::to_string(tick) + "-" + std::to_string(attempt));
        std::error_code error;
        if (std::filesystem::create_directory(path, error))
        {
            root = path;
            break;
        }
    }
}

/**
 * @brief 仅删除本对象创建的目录及其内容。
 */
RuntimeAssets::~RuntimeAssets()
{
    if (root.empty()) return;
    std::error_code error;
    std::filesystem::remove_all(root, error);
}

/**
 * @brief 还原一组内嵌的静态链接文件。
 * @param asset 内嵌资产表中的路径。
 * @param root 本次链接的隔离目录。
 * @param link 可选的归档路径输出。
 * @return 完整恢复的文件数；遇到错误返回零。
 */
static std::size_t restoreArchives(const char *asset, const std::filesystem::path &root, Memory::Vector<Memory::String> *link)
{
    Memory::String text;
    if (root.empty() || !chali_module::embedded::findGeneratorAssetByPath(asset, &text)) return 0;
    std::istringstream input(std::string(text.data(), text.size()));
    std::string name, hex;
    std::size_t size = 0, count = 0;
    while (input >> name >> size >> hex)
    {
        if (name.empty() || name == ".." || name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.+-") != std::string::npos || hex.size() % 2) return false;
        std::vector<unsigned char> packed(hex.size() / 2), bytes(size);
        for (std::size_t index = 0; index < packed.size(); ++index)
        {
            const auto digit = [](unsigned char value) -> int {
                if (value >= '0' && value <= '9') return value - '0';
                if (value >= 'a' && value <= 'f') return value - 'a' + 10;
                return -1;
            };
            const auto high = digit(hex[index * 2]), low = digit(hex[index * 2 + 1]);
            if (high < 0 || low < 0) return false;
            packed[index] = static_cast<unsigned char>(high * 16 + low);
        }
        if (tinfl_decompress_mem_to_mem(bytes.data(), bytes.size(), packed.data(), packed.size(), 0) != bytes.size()) return false;
        std::ofstream file(root / name, std::ios::binary);
        file.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        if (!file) return false;
        if (link) link->push_back((root / name).generic_u8string());
        ++count;
    }
    return input.eof() ? count : 0;
}

/**
 * @brief 还原构建时内嵌的原生服务与静态链接依赖。
 * @param diagnostics 接收缺失或损坏资产的诊断。
 * @return 所有文件完整恢复时返回 true。
 */
bool RuntimeAssets::restore(Memory::Vector<Memory::String> *diagnostics)
{
    if (restoreArchives("aot/host_assets.txt", root, nullptr) >= 4) return true;
    if (diagnostics) diagnostics->push_back("BUILD：缺少或无法恢复共享原生服务资产");
    return false;
}

/**
 * @brief 恢复实际使用的扩展归档，交由链接器按符号提取代码。
 * @param libraries 已发现的内置服务标识。
 * @param link 接收链接输入。
 * @param diagnostics 接收缺失或损坏资产的诊断。
 * @return 所需归档完整时为真。
 */
bool RuntimeAssets::prepareNative(const Memory::Set<Memory::String> &libraries, Memory::Vector<Memory::String> &link,
                                  Memory::Vector<Memory::String> *diagnostics)
{
    const auto append = [&](const char *name) {
        const auto asset = Memory::String("aot/native_") + hostTarget() + "_" + name + ".txt";
        if (restoreArchives(asset.c_str(), root, &link)) return true;
        if (diagnostics) diagnostics->push_back(Memory::String("BUILD：此编译器缺少内置原生归档 ") + name +
            "；请先构建 native-extensions 并重编译 ChaLi");
        return false;
    };
    const bool gui = libraries.count("@chali/desktop") != 0;
    const bool audio = libraries.count("@chali/audio") != 0;
    if (gui && !append("desktop")) return false;
    if (audio && !append("audio")) return false;
    if (libraries.count("@chali/xml") && !append("xml")) return false;
    if (libraries.count("@chali/crypto") && !append("crypto")) return false;
    if ((gui || audio) && !append("raylib")) return false;
    return true;
}

/**
 * @brief 用内嵌胶水头生成复杂 C ABI 的宿主封送目标文件。
 * @param source 前端生成的类型定义与绑定函数。
 * @param object 成功时接收本目录内的目标文件路径。
 * @param diagnostics 接收编译器的完整失败信息。
 * @return 文件恢复和宿主编译均成功时返回 true。
 */
bool RuntimeAssets::prepareAbi(const Memory::String &source, Memory::String *object, Memory::Vector<Memory::String> *diagnostics)
{
    if (root.empty()) return false;
    for (const auto *asset = chali_module::embedded::generatorAssetTable(); asset && asset->path; ++asset)
    {
        const auto relative = std::filesystem::u8path(asset->path);
        const auto extension = relative.extension().string();
        if (extension != ".h" && extension != ".hpp" && relative.generic_string() != "allocator/chali_pool.c") continue;
        if (relative.is_absolute()) return false;
        for (const auto &part : relative)
            if (part == "..") return false;
        const auto path = root / relative;
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);
        if (error) return false;
        std::ofstream output(path, std::ios::binary);
        output.write(asset->text, static_cast<std::streamsize>(std::strlen(asset->text)));
        if (!output) return false;
    }
    const auto cpp = root / "abi_binding.cpp", target = root / "abi_binding.o", log = root / "abi_compiler.log";
    {
        std::ofstream output(cpp, std::ios::binary);
        output << source;
        if (!output) return false;
    }
    const char *configured = std::getenv("CHALI_ABI_CXX");
    const std::string compiler = configured && *configured ? configured : "g++";
    const std::vector<std::string> arguments{compiler, "-std=c++17", "-O2", "-fPIC", "-ffunction-sections", "-fdata-sections", "-DCHALI_CUSTOM_ALLOCATOR=1",
        "-DCHALI_WITH_REFLECTION=" + std::to_string(CHALI_WITH_REFLECTION),
        "-DRUNTIME_WITH_REFLECTION=" + std::to_string(RUNTIME_WITH_REFLECTION),
        "-I" + root.u8string(), "-I" + (root / "allocator").u8string(), "-I" + (root / "ircode").u8string(),
        "-I" + (root / "stackvm").u8string(), "-c", cpp.u8string(), "-o", target.u8string()};
    std::string toolDiagnostic;
    const int status = chali_native_tools::execute(arguments, log, toolDiagnostic);
    if (status != 0)
    {
        if (diagnostics)
        {
            diagnostics->push_back("BUILD：复杂 ABI 宿主封送编译失败；可用 CHALI_ABI_CXX 指定同位数 C++ 编译器");
            diagnostics->push_back(Memory::String(toolDiagnostic.c_str()));
        }
        return false;
    }
    *object = target.u8string();
    return true;
}
}  // namespace chali_build
