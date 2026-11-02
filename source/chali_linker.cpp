#include "chali_library.hpp"
/** @file chali_linker.cpp
 * @brief 原生链接命令、目标前置检查、外置工具参数及事务发布。
 */
#include "chali_linker.hpp"
#include "chali_aot_llvm.hpp"
#include "chali_aot_runtime_assets.hpp"
#include "chali_native_objects.hpp"
#include "chali_native_tools.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace chali_linker
{
namespace
{
/**
 * @brief 将目标登记转换为对象头检查所需格式。
 * @param target 已验证的目标。
 * @return 对应格式。
 */
chali_native_objects::Format format(const chali_aot::TargetInfo &target)
{
    return target.windows ? chali_native_objects::Format::CoffX64 : target.arm64 ? chali_native_objects::Format::ElfArm64 :
                                                                                   chali_native_objects::Format::ElfX64;
}
/**
 * @brief 校验原生导出／入口名称，不允许链接脚本和选项注入。
 * @param symbol 原始符号。
 * @param windows 是否允许 MS ABI 的问号修饰。
 * @return 当前支持的字面符号为真。
 */
bool symbolName(const Memory::String &symbol, bool windows)
{
    if (symbol.empty() || symbol.size() > 4096) return false;
    for (const unsigned char value : symbol)
        if (!std::isalnum(value) && value != '_' && value != '$' && value != '.' && value != '@' && !(windows && value == '?')) return false;
    return true;
}
/**
 * @brief 校验并规范化输入，禁止覆盖其中任一输入。
 * @param inputs 原始路径。
 * @param output 最终产物路径。
 * @param target 已验证目标。
 * @param libraries 是否接受归档及共享库。
 * @return 拥有绝对路径的输入数组。
 */
Memory::Vector<Memory::String> checkedInputs(const Memory::Vector<Memory::String> &inputs, const std::filesystem::path &output, const chali_aot::TargetInfo &target, bool libraries)
{
    Memory::Vector<Memory::String> result;
    for (const auto &input : inputs)
    {
        const auto path = std::filesystem::absolute(std::filesystem::u8path(input.c_str())).lexically_normal();
        if (std::filesystem::weakly_canonical(path) == std::filesystem::weakly_canonical(output)) throw std::runtime_error("原生产物不能覆盖链接输入");
        std::string error;
        if (!chali_native_objects::inspect(path, format(target), error, libraries)) throw std::runtime_error(error);
        result.push_back(Memory::String(path.u8string().c_str()));
    }
    return result;
}
/**
 * @brief 在指定工具旁或 PATH 查找外置归档器，不回退为其他格式的工具。
 * @param options 用户显式归档器或 LLVM clang 配置。
 * @return 归档器路径。
 */
std::string archiver(const chali_aot::Options &options)
{
    if (!options.compiler.empty()) return options.compiler.c_str();
    if (const char *value = std::getenv("CHALI_LLVM_AR"))
        if (*value) return value;
    const auto compiler = std::filesystem::u8path(chali_aot::llvmCompilerPath({}).c_str());
    const auto sibling = compiler.parent_path() /
#ifdef _WIN32
                         "llvm-ar.exe";
#else
                         "llvm-ar";
#endif
    if (std::filesystem::is_regular_file(sibling)) return sibling.u8string();
    return "llvm-ar";
}
}  // namespace
/**
 * @brief 用目标对应的 LLD 或 clang 链接，再发布最终产物。
 * @param original 链接请求。
 * @param output 完整输出路径。
 * @param diagnostics 接收原生诊断。
 * @return 成功为零，失败为一。
 */
int link(const Request &original, const Memory::String &output, Memory::Vector<Memory::String> *diagnostics)
{
    namespace fs = std::filesystem;
    try
    {
        Request request = original;
        chali_aot::TargetInfo target;
        Memory::String targetError;
        if (!chali_aot::describeTarget(request.options.target, target, &targetError)) throw std::runtime_error(targetError.c_str());
        const auto requestedOutput = fs::weakly_canonical(fs::absolute(fs::u8path(output.c_str())));
        for (const auto &package : request.packages)
            if (requestedOutput == fs::weakly_canonical(fs::u8path(package.c_str()))) throw std::runtime_error("链接输出不能覆盖输入库包");
        if (!request.libraryLock.empty() && requestedOutput == fs::weakly_canonical(fs::u8path(request.libraryLock.c_str())))
            throw std::runtime_error("链接输出不能覆盖库锁");
        chali_library::ImportSession libraries(request.options.target.c_str(), request.options.cpu.c_str(), request.options.libraryImplementation.c_str(), request.options.compiler.c_str());
        if (!request.libraryLock.empty()) libraries.loadLock(fs::u8path(request.libraryLock.c_str()));
        std::vector<fs::path> roots;
        for (const auto &directory : request.libraryDirectories) roots.push_back(fs::u8path(directory.c_str()));
        for (const auto &package : request.packages) libraries.openRecursive(fs::u8path(package.c_str()), roots);
        for (const auto &path : libraries.paths())
            if (requestedOutput == path) throw std::runtime_error("链接输出不能覆盖传递依赖库包");
        chali_aot::Options packageInputs = request.options;
        packageInputs.linkObjects.clear();
        packageInputs.linkBitcode.clear();
        packageInputs.linkLibraries.clear();
        packageInputs.unitData.clear();
        libraries.appendTo(packageInputs);
        request.inputs.insert(request.inputs.end(), packageInputs.linkObjects.begin(), packageInputs.linkObjects.end());
        request.options.linkBitcode.insert(request.options.linkBitcode.end(), packageInputs.linkBitcode.begin(), packageInputs.linkBitcode.end());
        for (const auto &library : packageInputs.linkLibraries) request.libraryNames.push_back(library.substr(2));
        if (request.inputs.empty() && request.libraryNames.empty() && request.options.linkBitcode.empty()) throw std::runtime_error("链接需要对象或库输入");
        if (!request.entry.empty() && !symbolName(request.entry, target.windows)) throw std::runtime_error("入口符号不受支持");
        for (const auto &name : request.options.exports)
            if (!symbolName(name, target.windows)) throw std::runtime_error("导出符号不受支持");
        const auto product = fs::absolute(fs::u8path(output.c_str())).lexically_normal();
        request.inputs = checkedInputs(request.inputs, product, target, true);
        for (auto &path : request.libraryDirectories)
        {
            const auto directory = fs::absolute(fs::u8path(path.c_str())).lexically_normal();
            if (!fs::is_directory(directory)) throw std::runtime_error("库搜索目录不存在：" + directory.u8string());
            path = directory.u8string().c_str();
        }
        for (const auto &name : request.libraryNames)
        {
            if (name.empty()) throw std::runtime_error("链接库名称为空");
            for (const unsigned char value : name)
                if (!std::isalnum(value) && value != '_' && value != '-' && value != '.' && value != '+') throw std::runtime_error("链接库名称不受支持；完整路径请作为输入传入");
        }
        if (!request.noRuntime && !target.native && request.options.runtimeDirectory.empty()) throw std::runtime_error("跨目标完整链接需要目标 --runtime-dir 和适用 sysroot；独立入口可用 --no-runtime");
        chali_native_tools::Workspace work;
        // 私有目录隔离写入，同时保留最终文件名，避免 DLL 导出表及宿主导入表记录错误的临时名字。
        const auto temporary = work.path / product.filename();
        int code = 1;
        if (chali_aot::builtin::available() && request.options.compiler.empty() && (request.noRuntime || (target.native && target.windows && request.options.sysroot.empty() && request.options.runtimeDirectory.empty())))
            code = chali_aot::builtin::link(request, Memory::String(temporary.u8string().c_str()), diagnostics);
        else
        {
            const auto compiler = chali_aot::llvmCompilerPath(request.options.compiler);
            std::vector<std::string> arguments{ compiler.c_str(), "--target=" + std::string(target.triple.c_str()), "-fuse-ld=lld", "-o", temporary.u8string(), "-Wl,--gc-sections" };
            if (!request.options.linkBitcode.empty())
            {
                arguments.push_back(request.options.thinLto ? "-flto=thin" : "-flto=full");
                if (request.options.thinLto) arguments.push_back("-fno-split-lto-unit");
                arguments.push_back(request.options.debug ? "-Wl,--lto-O0" : "-Wl,--lto-O3");
            }
            if (!request.options.sysroot.empty()) arguments.push_back("--sysroot=" + std::string(request.options.sysroot.c_str()));
            if (request.options.shared) arguments.push_back("-shared");
            if (!request.options.debug) arguments.push_back("-s");
            if (request.noRuntime) arguments.push_back("-nostdlib");
            if (!request.entry.empty()) arguments.push_back("-Wl,-e," + std::string(request.entry.c_str()));
            if (target.windows && request.options.shared) arguments.push_back("-Wl,--exclude-all-symbols");
            for (const auto &path : request.libraryDirectories) arguments.push_back("-L" + std::string(path.c_str()));
            arguments.push_back("-Wl,--start-group");
            for (const auto &input : request.inputs) arguments.push_back(input.c_str());
            for (const auto &bitcode : request.options.linkBitcode) arguments.push_back(bitcode.c_str());
            for (const auto &name : request.libraryNames) arguments.push_back("-l" + std::string(name.c_str()));
            chali_aot::RuntimeAssets runtime;
            fs::path directory;
            if (!request.noRuntime)
            {
                if (request.options.runtimeDirectory.empty())
                {
                    if (!runtime.restore(diagnostics)) return 1;
                    directory = runtime.root;
                }
                else
                {
                    directory = fs::u8path(request.options.runtimeDirectory.c_str());
                    if (!fs::is_regular_file(directory / "libchali_aot_host.a")) throw std::runtime_error("目标运行时缺少 libchali_aot_host.a");
                    if (fs::is_regular_file(directory / "target.txt"))
                    {
                        std::ifstream input(directory / "target.txt");
                        std::string identity;
                        std::getline(input, identity);
                        if (!identity.empty() && identity.back() == '\r') identity.pop_back();
                        if (!identity.empty() && identity != target.triple.c_str()) throw std::runtime_error("目标运行时身份不匹配");
                    }
                }
                arguments.push_back("-L" + directory.u8string());
                arguments.push_back("-lchali_aot_host");
                if (request.options.runtimeDirectory.empty())
                    arguments.push_back((directory / "libstdc++.a").u8string());
                else
                    arguments.push_back("-lstdc++");
                arguments.insert(arguments.end(), { "-lgcc_eh", "-lgcc" });
                if (target.windows)
                {
                    arguments.insert(arguments.end(), { "-static", "-lwinpthread", "-lwinhttp", "-lws2_32", "-lcrypt32", "-lbcrypt" });
                    if (request.options.runtimeDirectory.empty())
                        for (const auto name : { "gcc_libmingw32.a", "gcc_libmingwex.a", "gcc_libmoldname.a" }) arguments.push_back((directory / name).u8string());
                }
                else
                    arguments.insert(arguments.end(), { "-pthread", "-lm", "-ldl" });
            }
            arguments.push_back("-Wl,--end-group");
            if (!request.options.exports.empty() || (!target.windows && request.options.shared))
            {
                const auto script = work.path / (target.windows ? "exports.def" : "exports.map");
                std::ofstream text(script, std::ios::binary);
                text << (target.windows ? "EXPORTS\n" : request.options.exports.empty() ? "{\n" :
                                                                                          "{ global:\n");
                for (const auto &name : request.options.exports) text << '"' << name.c_str() << '"' << (target.windows ? "\n" : ";\n");
                if (!target.windows) text << "local: *; };\n";
                text.close();
                if (!text) throw std::runtime_error("无法生成导出符号说明");
                if (target.windows)
                    arguments.push_back(script.u8string());
                else
                    arguments.push_back("-Wl,--version-script=" + script.u8string());
            }
            std::string diagnostic;
            code = chali_native_tools::execute(arguments, work.path / "link.log", diagnostic);
            if (!diagnostic.empty() && diagnostics) diagnostics->push_back(Memory::String(diagnostic.c_str()));
        }
        if (code) return 1;
        std::string error;
        if (!fs::is_regular_file(temporary) || fs::file_size(temporary) == 0) throw std::runtime_error("链接器未生成产物");
        if (!chali_native_tools::publish(temporary, product, error)) throw std::runtime_error("无法发布链接产物：" + error);
        return 0;
    } catch (const std::exception &error)
    {
        if (diagnostics) diagnostics->push_back(Memory::String(error.what()));
        return 1;
    }
}
/**
 * @brief 校验对象后生成确定性归档，成功后替换最终文件。
 * @param objects 对象输入。
 * @param output 完整归档路径。
 * @param options 目标及外置归档器。
 * @param diagnostics 接收工具和格式诊断。
 * @return 成功为零，失败为一。
 */
int archive(const Memory::Vector<Memory::String> &objects, const Memory::String &output, const chali_aot::Options &options, Memory::Vector<Memory::String> *diagnostics)
{
    namespace fs = std::filesystem;
    try
    {
        chali_aot::TargetInfo target;
        Memory::String message;
        if (!chali_aot::describeTarget(options.target, target, &message)) throw std::runtime_error(message.c_str());
        const auto product = fs::absolute(fs::u8path(output.c_str())).lexically_normal();
        const auto inputs = checkedInputs(objects, product, target, false);
        const bool coff = target.windows && product.extension() == ".lib";
        chali_native_tools::Workspace work;
        const auto temporary = work.path / (coff ? "archive.lib" : "archive.a");
        std::string diagnostic;
        int code = 1;
        if (chali_aot::builtin::available() && options.compiler.empty())
            code = builtin::archive(inputs, temporary.u8string(), coff, diagnostic);
        else
        {
            std::vector<std::string> arguments{ archiver(options), coff ? "--format=coff" : "--format=gnu", "rcsD", temporary.u8string() };
            for (const auto &path : inputs) arguments.push_back(path.c_str());
            code = chali_native_tools::execute(arguments, work.path / "archive.log", diagnostic);
        }
        if (!diagnostic.empty() && diagnostics) diagnostics->push_back(Memory::String(diagnostic.c_str()));
        if (code) return 1;
        std::string error;
        if (!fs::is_regular_file(temporary) || fs::file_size(temporary) == 0) throw std::runtime_error("归档器未生成产物");
        if (!chali_native_tools::publish(temporary, product, error)) throw std::runtime_error("无法发布静态归档：" + error);
        return 0;
    } catch (const std::exception &error)
    {
        if (diagnostics) diagnostics->push_back(Memory::String(error.what()));
        return 1;
    }
}
/**
 * @brief 解析原生链接和归档选项，保留普通程序执行入口。
 * @param argc 归一化参数数目。
 * @param argv 命令数组。
 * @return 工具状态；参数错误返回二。
 */
int runCli(int argc, char **argv)
{
    const bool ar = argc > 1 && std::string(argv[1]) == "ar";
    Request request;
    Memory::String output;
    bool literal = false;
    for (int index = 2; index < argc; ++index)
    {
        const std::string argument = argv[index];
        if (literal)
        {
            request.inputs.push_back(argument.c_str());
            continue;
        }
        if (argument == "--")
        {
            literal = true;
            continue;
        }
        if (argument == "--help" || argument == "-h")
        {
            std::puts(ar ? "chali ar [OBJECT ...] --out LIB.a [--target TARGET] [--llvm LLVM-AR]" : "chali link INPUT ... [--out FILE] [--target TARGET] [--cpu CPU] [--shared] [--export SYMBOL] [--entry SYMBOL] [--no-runtime] [--package FILE.chlib] [--library-lock FILE] [--library-implementation native|bitcode] [--lto|--no-lto|--lto-mode off|full|thin] [-L DIR] [--library NAME] [--llvm CLANG] [--sysroot DIR] [--runtime-dir DIR] [--debug]");
            return 0;
        }
        if (!ar && argument == "--shared")
        {
            request.options.shared = true;
            continue;
        }
        if (!ar && argument == "--debug")
        {
            request.options.debug = true;
            continue;
        }
        if (!ar && (argument == "--lto" || argument == "--no-lto"))
        {
            request.options.lto = argument == "--lto";
            request.options.thinLto = false;
            continue;
        }
        if (!ar && argument == "--lto-mode")
        {
            if (++index >= argc || (std::string(argv[index]) != "off" && std::string(argv[index]) != "full" && std::string(argv[index]) != "thin"))
            {
                std::fputs("chali: --lto-mode 需要 off、full 或 thin\n", stderr);
                return 2;
            }
            request.options.lto = std::string(argv[index]) != "off";
            request.options.thinLto = std::string(argv[index]) == "thin";
            continue;
        }
        if (!ar && argument == "--no-runtime")
        {
            request.noRuntime = true;
            continue;
        }
        if (argument == "--out" || argument == "--target" || argument == "--llvm" || (!ar && argument == "--cpu") || (!ar && (argument == "--entry" || argument == "--export" || argument == "--sysroot" || argument == "--runtime-dir" || argument == "--library" || argument == "--library-path" || argument == "-L" || argument == "--link" || argument == "--package" || argument == "--library-lock" || argument == "--library-implementation")))
        {
            if (++index >= argc)
            {
                std::fprintf(stderr, "chali: 选项缺少值：%s\n", argument.c_str());
                return 2;
            }
            const auto value = argv[index];
            if (argument == "--out") output = value;
            if (argument == "--target") request.options.target = value;
            if (argument == "--cpu") request.options.cpu = value;
            if (argument == "--llvm") request.options.compiler = value;
            if (argument == "--entry") request.entry = value;
            if (argument == "--export") request.options.exports.push_back(value);
            if (argument == "--sysroot") request.options.sysroot = value;
            if (argument == "--runtime-dir") request.options.runtimeDirectory = value;
            if (argument == "--library") request.libraryNames.push_back(value);
            if (argument == "--library-path" || argument == "-L") request.libraryDirectories.push_back(value);
            if (argument == "--link") request.inputs.push_back(value);
            if (argument == "--package") request.packages.push_back(value);
            if (argument == "--library-lock") request.libraryLock = value;
            if (argument == "--library-implementation")
            {
                if (std::string(value) != "native" && std::string(value) != "bitcode")
                {
                    std::fputs("chali: --library-implementation 需要 native 或 bitcode\n", stderr);
                    return 2;
                }
                request.options.libraryImplementation = value;
            }
            continue;
        }
        if (!ar && argument.size() > 2 && argument.compare(0, 2, "-L") == 0)
        {
            request.libraryDirectories.push_back(argument.substr(2).c_str());
            continue;
        }
        if (!argument.empty() && argument[0] == '-')
        {
            std::fprintf(stderr, "chali: 未知原生工具选项：%s\n", argument.c_str());
            return 2;
        }
        request.inputs.push_back(argument.c_str());
    }
    if (ar && output.empty())
    {
        std::fputs("chali ar: 需要 --out 指定静态归档\n", stderr);
        return 2;
    }
    chali_aot::TargetInfo target;
    Memory::String error;
    if (!chali_aot::describeTarget(request.options.target, target, &error))
    {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 2;
    }
    if (output.empty()) output = target.windows ? (request.options.shared ? "a.dll" : "a.exe") : request.options.shared ? "a.so" :
                                                                                                                          "a.out";
    Memory::Vector<Memory::String> diagnostics;
    const int result = ar ? archive(request.inputs, output, request.options, &diagnostics) : link(request, output, &diagnostics);
    for (const auto &message : diagnostics) std::fprintf(stderr, "%s\n", message.c_str());
    return result;
}
}  // namespace chali_linker
