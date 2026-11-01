/** @file chali_library_build.cpp
 * @brief 已检查标量函数的独立原生编译与库包发布。
 */
#include "chali_library.hpp"
#include "chali_aot.hpp"
#include "chali_native_tools.hpp"
#include "chali_package.hpp"
#include <cstdio>
#include <fstream>
#include <stdexcept>

namespace chali_library
{
/**
 * @brief 直接沿 AST→LLVM 编译库单元，包内不携带实现源码或运行时副本。
 * @param original 已完成语言检查的函数及公开签名。
 * @param module 完整模块身份。
 * @param version 精确包版本。
 * @param interface 不含实现体的语言声明。
 * @param dependencies 实际已验证的依赖身份。
 * @param output 最终包路径。
 * @return 成功为零；失败保留旧包并返回一。
 */
int buildNative(const chali_aot::Request &original, const std::string &module, const std::string &version,
                const std::string &interface, const std::vector<Dependency> &dependencies, const std::filesystem::path &output)
{
    try
    {
        if (interface.size() > 32ull * 1024 * 1024) throw std::runtime_error("库接口超出文本预算");
        Package package;
        package.manifest.module = module;
        package.manifest.version = version;
        package.manifest.compilerVersion = chali_package::compilerVersion;
        package.manifest.requiredFeatures = { "native-c-v1", "scalar-unit-v1" };
        package.manifest.dependencies = dependencies;
        chali_aot::TargetInfo known;
        Memory::String error;
        if (!chali_aot::describeTarget(original.options.target, known, &error)) throw std::runtime_error(error.c_str());
        Target target;
        target.triple = known.triple.c_str();
        target.objectFormat = known.windows ? "coff" : "elf";
        target.dataLayout = known.dataLayout.c_str();
        target.cpu = original.options.cpu.empty() ? std::string(known.cpu.c_str()) : std::string(original.options.cpu.c_str());
        target.implementation = "native/" + target.triple + "/library.o";
        package.manifest.targets.push_back(target);
        manifestJson(package.manifest);
        chali_native_tools::Workspace work;
        auto request = original;
        request.options.libraryModule = module.c_str();
        request.options.shared = true;
        request.options.emitObjectOnly = true;
        request.options.emitLLVMOnly = request.options.emitAssemblyOnly = false;
        request.options.outputPrefix = (work.path / "library").u8string().c_str();
        request.options.exports.clear();
        request.options.linkLibraries.clear();
        request.options.linkObjects.clear();
        request.abiGlue.clear();
        Memory::Vector<Memory::String> diagnostics;
        if (chali_aot::run(request, &diagnostics))
        {
            for (const auto &diagnostic : diagnostics) std::fprintf(stderr, "chali lib: %s\n", diagnostic.c_str());
            return 1;
        }
        const auto file = work.path / "library.o";
        const auto size = std::filesystem::file_size(file);
        if (size > maximumPackageBytes - interface.size() - 4096) throw std::runtime_error("编译后的库实现超过包预算");
        std::ifstream stream(file, std::ios::binary);
        std::string bytes(static_cast<std::size_t>(size), '\0');
        stream.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        if (!stream || stream.peek() != std::char_traits<char>::eof()) throw std::runtime_error("读取本次原生实现失败");
        package.entries.push_back({ package.manifest.interface, Kind::Interface, 0, interface });
        package.entries.push_back({ target.implementation, Kind::Object, 0, std::move(bytes) });
        std::string diagnostic;
        if (!write(output, package, diagnostic)) throw std::runtime_error(diagnostic);
        return 0;
    } catch (const std::exception &error)
    {
        std::fprintf(stderr, "chali lib: %s\n", error.what());
        return 1;
    }
}
}  // namespace chali_library
