/** @file chali_assembler.cpp
 * @brief 汇编命令、外置集成汇编器及事务产物发布。
 */
#include "chali_assembler.hpp"
#include "chali_aot_llvm.hpp"
#include "chali_native_tools.hpp"
#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace chali_assembler
{
/**
 * @brief 校验输入并选择内置 MC 或外置 clang，成功后发布对象文件。
 * @param input 原始 .s 路径。
 * @param output 完整对象路径。
 * @param options 目标、CPU、包含目录、驱动及调试设置。
 * @param diagnostics 接收原始汇编诊断。
 * @return 成功为零，失败为一。
 */
int assemble(const Memory::String &input, const Memory::String &output, const chali_build::Options &options, Memory::Vector<Memory::String> *diagnostics)
{
    namespace fs = std::filesystem;
    try
    {
        chali_build::TargetInfo target;
        Memory::String targetError;
        if (!chali_build::describeTarget(options.target, target, &targetError)) throw std::runtime_error(targetError.c_str());
        const auto source = fs::absolute(fs::u8path(input.c_str())).lexically_normal();
        const auto product = fs::absolute(fs::u8path(output.c_str())).lexically_normal();
        if (source.extension() != ".s") throw std::runtime_error("汇编入口只接受 .s；.S 预处理及 NASM／MASM 不在当前方言范围内");
        if (!fs::is_regular_file(source) || fs::file_size(source) > 64u * 1024u * 1024u) throw std::runtime_error("汇编输入不是普通文件或超过 64 MiB");
        if (fs::weakly_canonical(source) == fs::weakly_canonical(product)) throw std::runtime_error("汇编输出不能覆盖输入文件");
        chali_native_tools::Workspace work;
        const auto object = work.path / "assembly.o";
        std::string diagnostic;
        int code = 1;
        if (chali_build::builtin::available() && options.compiler.empty())
            code = builtin::assemble(source.u8string(), object.u8string(), options, diagnostic);
        else
        {
            const auto compiler = chali_build::llvmCompilerPath(options.compiler);
            if (!options.cpu.empty())
            {
                std::string listing;
                if (chali_native_tools::execute({ compiler.c_str(), "--target=" + std::string(target.triple.c_str()), "--print-supported-cpus" }, work.path / "cpus.log", listing) != 0)
                    throw std::runtime_error("无法查询外置 LLVM 的目标 CPU：" + listing);
                bool supported = false;
                std::istringstream lines(listing);
                std::string line;
                while (std::getline(lines, line))
                {
                    std::istringstream words(line);
                    std::string name, extra;
                    if ((words >> name) && !(words >> extra) && name == options.cpu.c_str()) supported = true;
                }
                if (!supported) throw std::runtime_error("汇编目标不支持指定 CPU：" + std::string(options.cpu.c_str()));
            }
            std::vector<std::string> arguments{ compiler.c_str(), "--target=" + std::string(target.triple.c_str()), "-x", "assembler", "-integrated-as", "-c", source.u8string(), "-o", object.u8string(), "-I", source.parent_path().u8string() };
            if (!target.windows) arguments.push_back("-Wa,--noexecstack");
            if (!options.cpu.empty()) arguments.push_back(std::string(target.arm64 ? "-mcpu=" : "-march=") + options.cpu.c_str());
            if (options.debug) arguments.insert(arguments.end(), { "-g", "-gdwarf-4" });
            for (const auto &include : options.assemblyIncludes) arguments.insert(arguments.end(), { "-I", fs::absolute(fs::u8path(include.c_str())).u8string() });
            code = chali_native_tools::execute(arguments, work.path / "assembler.log", diagnostic);
        }
        if (!diagnostic.empty() && diagnostics) diagnostics->push_back(Memory::String(diagnostic.c_str()));
        if (code != 0) return 1;
        std::string error;
        if (!fs::is_regular_file(object) || fs::file_size(object) == 0) throw std::runtime_error("汇编器没有生成对象文件");
        if (!chali_native_tools::publish(object, product, error)) throw std::runtime_error("无法发布对象文件：" + error);
        return 0;
    } catch (const std::exception &error)
    {
        if (diagnostics) diagnostics->push_back(Memory::String(error.what()));
        return 1;
    }
}
/**
 * @brief 解析独立汇编参数并输出统一诊断，不运行生成的对象。
 * @param argc 参数数量。
 * @param argv 归一化参数数组。
 * @return 成功为零，汇编失败为一，命令错误为二。
 */
int runCli(int argc, char **argv)
{
    chali_build::Options options;
    Memory::String input, output;
    for (int i = 2; i < argc; ++i)
    {
        const std::string argument = argv[i];
        if (chali_build::builtin::available() && (argument == "--llvm" || argument.compare(0, 7, "--llvm=") == 0))
        {
            std::fputs("chali: 静态 LLVM 编译器不支持 --llvm；请使用 llvm-external 变体指定外部工具链。\n", stderr);
            return 2;
        }
        if (argument == "--help" || argument == "-h")
        {
            std::printf("chali asm FILE.s [--out FILE.o] [--target TARGET] [--cpu CPU] [-I DIR] [--include-path DIR] [--debug]%s\n",
                        chali_build::builtin::available() ? "" : " [--llvm CLANG]");
            return 0;
        }
        if (argument == "--debug")
        {
            options.debug = true;
            continue;
        }
        if (argument == "--out" || argument == "--target" || argument == "--cpu" || argument == "-I" || argument == "--include-path" || argument == "--llvm")
        {
            if (++i >= argc)
            {
                std::fprintf(stderr, "chali asm: 选项缺少值：%s\n", argument.c_str());
                return 2;
            }
            if (argument == "--out") output = argv[i];
            if (argument == "--target") options.target = argv[i];
            if (argument == "--cpu") options.cpu = argv[i];
            if (argument == "--llvm") options.compiler = argv[i];
            if (argument == "-I" || argument == "--include-path") options.assemblyIncludes.push_back(argv[i]);
            continue;
        }
        if (argument.size() > 2 && argument.compare(0, 2, "-I") == 0)
        {
            options.assemblyIncludes.push_back(argument.substr(2).c_str());
            continue;
        }
        if (!argument.empty() && argument[0] == '-')
        {
            std::fprintf(stderr, "chali asm: 未知选项：%s\n", argument.c_str());
            return 2;
        }
        if (!input.empty())
        {
            std::fputs("chali asm: 每次只接受一个 .s 文件\n", stderr);
            return 2;
        }
        input = argument.c_str();
    }
    if (input.empty())
    {
        std::fputs("chali asm: 缺少 .s 输入；使用 --help 查看用法\n", stderr);
        return 2;
    }
    chali_build::TargetInfo target;
    Memory::String error;
    if (!chali_build::describeTarget(options.target, target, &error))
    {
        std::fprintf(stderr, "chali asm: %s\n", error.c_str());
        return 2;
    }
    if (output.empty())
    {
        auto path = std::filesystem::u8path(input.c_str());
        path.replace_extension(target.windows ? ".obj" : ".o");
        output = path.u8string().c_str();
    }
    Memory::Vector<Memory::String> diagnostics;
    const auto code = assemble(input, output, options, &diagnostics);
    for (const auto &message : diagnostics) std::fprintf(stderr, "%s\n", message.c_str());
    return code;
}
}  // namespace chali_assembler
