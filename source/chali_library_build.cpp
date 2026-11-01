/** @file chali_library_build.cpp
 * @brief 已检查标量函数的独立原生编译与库包发布。
 */
#include "chali_library.hpp"
#include "chali_aot.hpp"
#include "chali_aot_llvm.hpp"
#include "chali_native_tools.hpp"
#include "chali_package.hpp"
#include "json/json.hpp"
#include <set>
#include <algorithm>
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
        const bool borrowed = std::any_of(original.abiExports.begin(), original.abiExports.end(), [](const auto &entry) { return entry.abiConvention == "ChaLiBorrow"; });
        if (borrowed) package.manifest.requiredFeatures.push_back("borrowed-record-v1");
        package.manifest.dependencies = dependencies;
        package.manifest.license = original.options.libraryLicense.c_str();
        json::Object producer;
        producer.insert("generator", "ast-llvm");
        producer.insert("optimization", "O3");
        producer.insert("checked", original.options.checked);
        producer.insert("runtime", "consumer");
        producer.insert("llvmLinkage", chali_aot::builtin::available() && original.options.compiler.empty() ? "static" : "external");
        json::Object extensions;
        extensions.insert("producer", json::Value(std::move(producer)));
        package.manifest.extensions = json::Value(std::move(extensions)).dump();
        chali_aot::TargetInfo known;
        Memory::String error;
        if (!chali_aot::describeTarget(original.options.target, known, &error)) throw std::runtime_error(error.c_str());
        Target target;
        if (borrowed) target.nativeAbi = "chali-borrow-v1";
        target.triple = known.triple.c_str();
        target.objectFormat = known.windows ? "coff" : "elf";
        target.dataLayout = known.dataLayout.c_str();
        target.cpu = original.options.cpu.empty() ? std::string(known.cpu.c_str()) : std::string(original.options.cpu.c_str());
        target.implementation = "native/" + target.triple + "/library.o";
        for (const auto &name : original.options.librarySystemLibraries) target.systemLibraries.emplace_back(name.c_str());
        for (const auto &name : original.options.libraryRuntimeLibraries) target.runtimeLibraries.emplace_back(name.c_str());
        package.manifest.targets.push_back(target);
        std::uint64_t retained = interface.size() + 4096;
        std::set<std::string> names{ package.manifest.interface, target.implementation };
        for (const auto &asset : original.options.libraryLicenseAssets)
        {
            const std::string record(asset.c_str());
            const auto equal = record.find('=');
            if (equal == record.npos || equal == 0 || equal + 1 == record.size()) throw std::runtime_error("许可证资产格式应为 NAME=FILE");
            const auto name = record.substr(0, equal);
            if (name.size() > 240) throw std::runtime_error("许可证资产名字超过预算");
            if (!names.insert(name).second) throw std::runtime_error("许可证资产名字重复或与实现及接口冲突");
            const auto path = std::filesystem::u8path(record.substr(equal + 1));
            if (std::filesystem::weakly_canonical(output) == std::filesystem::weakly_canonical(path)) throw std::runtime_error("库输出不能覆盖许可证或署名输入");
            if (!std::filesystem::is_regular_file(path)) throw std::runtime_error("许可证输入不是普通文件");
            const auto size = std::filesystem::file_size(path);
            if (size > 32ull * 1024 * 1024 || size + 48 + name.size() > maximumPackageBytes - retained) throw std::runtime_error("许可证资产超过包预算");
            std::ifstream input(path, std::ios::binary);
            std::string bytes(static_cast<std::size_t>(size), '\0');
            input.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
            if (!input || input.peek() != std::char_traits<char>::eof()) throw std::runtime_error("读取许可证输入失败或读取期间长度变化");
            retained += size + 48 + name.size();
            package.manifest.licenseFiles.push_back(name);
            package.entries.push_back({ name, Kind::License, 0, std::move(bytes) });
        }
        // 在编译前检查全部名字、列表和声明，输入内容只读取一次。
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
        if (size > maximumPackageBytes - retained) throw std::runtime_error("编译后的库实现超过包预算");
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
