/** @file chali_library_build.cpp
 * @brief 已检查语言函数的独立原生编译与库包发布。
 */
#include "chali_library.hpp"
#include "chali_aot.hpp"
#include "ast/ast.hpp"
#include "chali_aot_llvm.hpp"
#include "chali_native_tools.hpp"
#include "chali_package.hpp"
#include "chali_digest.hpp"
#include "json/json.hpp"
#include <set>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <stdexcept>

namespace chali_library
{
/**
 * @brief 判断公开签名是否直接传递枚举值或其借用。
 * @param type 已检查的函数结果或参数类型。
 * @return 枚举及枚举引用返回 true，其他类型返回 false。
 */
static bool enumValueType(const ast::TypePtr &type)
{
    if (!type) return false;
    const auto value = type->kind == ast::Type::Kind::Ref ? type->element : type;
    return value && value->kind == ast::Type::Kind::Struct && !value->variants.empty();
}

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
        const auto templates = exportTemplates(module);
        if (!templates.empty())
        {
            package.manifest.templates = "templates/definitions.json";
            package.manifest.requiredFeatures.push_back("generic-definitions-v1");
        }
        const bool ownedRecords = original.syntax && original.syntax->nativeOwnedAggregates;
        const bool owned = ownedRecords || std::any_of(original.abiExports.begin(), original.abiExports.end(), [](const auto &entry) { return entry.abiConvention == "ChaLiOwned"; });
        if (owned) package.manifest.requiredFeatures.push_back("owned-string-value-v1");
        if (ownedRecords) package.manifest.requiredFeatures.push_back("owned-record-value-v1");
        const bool borrowed = owned || std::any_of(original.abiExports.begin(), original.abiExports.end(), [](const auto &entry) { return entry.abiConvention == "ChaLiBorrow" || entry.abiConvention == "ChaLiOwned"; });
        if (borrowed) package.manifest.requiredFeatures.push_back("borrowed-record-v1");
        const bool values = owned || std::any_of(original.abiExports.begin(), original.abiExports.end(), [](const auto &entry) { return entry.abiConvention == "ChaLiValue" || entry.abiConvention == "ChaLiOwned"; });
        if (values) package.manifest.requiredFeatures.push_back("plain-record-value-v1");
        if (std::any_of(original.abiExports.begin(), original.abiExports.end(), [](const auto &entry) { return entry.hasLifetimeContract; }))
            package.manifest.requiredFeatures.push_back("lifetime-contract-v1");
        if (std::any_of(original.abiExports.begin(), original.abiExports.end(), [](const auto &entry) { return entry.borrowedResult; }))
            package.manifest.requiredFeatures.push_back("borrowed-result-v1");
        if (original.syntax)
        {
            std::set<Memory::String> labels;
            for (const auto &entry : original.abiExports) labels.insert(entry.label);
            const bool enums = std::any_of(original.syntax->functions.begin(), original.syntax->functions.end(), [&](const auto &function) {
                return labels.count(function.name) && (enumValueType(function.result) || std::any_of(function.parameters.begin(), function.parameters.end(), [](const auto &parameter) { return enumValueType(parameter.second); }));
            });
            if (enums) package.manifest.requiredFeatures.push_back("plain-enum-value-v1");
        }
        if (original.syntax && !original.syntax->nativeTypeSymbols.empty()) package.manifest.requiredFeatures.push_back("shared-type-id-v1");
        package.manifest.dependencies = dependencies;
        if (!templates.empty()) templateSource(package.manifest, templates);
        package.manifest.license = original.options.libraryLicense.c_str();
        json::Object producer;
        producer.insert("generator", "ast-llvm");
        producer.insert("optimization", (original.options.debug && !original.options.debugOptimized) ? "O0" : "O3");
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
        if (values) target.nativeAbi = "chali-value-v1";
        if (owned) target.nativeAbi = "chali-owned-v1";
        target.triple = known.triple.c_str();
        target.objectFormat = known.windows ? "coff" : "elf";
        target.dataLayout = known.dataLayout.c_str();
        target.cpu = original.options.cpu.empty() ? std::string(known.cpu.c_str()) : std::string(original.options.cpu.c_str());
        target.implementation = "native/" + target.triple + "/library.o";
        if (original.options.libraryBitcode)
        {
            target.bitcode = "native/" + target.triple + "/library.bc";
            Memory::Vector<Memory::String> diagnostics;
            target.llvmVersion = chali_aot::llvmVersion(original.options, &diagnostics).c_str();
            if (target.llvmVersion.empty()) throw std::runtime_error(diagnostics.empty() ? "无法核对 bitcode 生产 LLVM 版本" : diagnostics.front().c_str());
        }
        for (const auto &name : original.options.librarySystemLibraries) target.systemLibraries.emplace_back(name.c_str());
        for (const auto &name : original.options.libraryRuntimeLibraries) target.runtimeLibraries.emplace_back(name.c_str());
        package.manifest.targets.push_back(target);
        std::uint64_t retained = interface.size() + templates.size() + 4096;
        std::set<std::string> names{ package.manifest.interface, target.implementation };
        if (!templates.empty()) names.insert(package.manifest.templates);
        if (!target.bitcode.empty()) names.insert(target.bitcode);
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
        request.options.libraryHasTemplates = !templates.empty();
        request.options.shared = true;
        request.options.emitObjectOnly = true;
        request.options.emitLLVMOnly = request.options.emitAssemblyOnly = request.options.emitBitcodeOnly = false;
        request.options.outputPrefix = (work.path / "library").u8string().c_str();
        request.options.exports.clear();
        request.options.linkLibraries.clear();
        request.options.linkObjects.clear();
        request.options.linkBitcode.clear();
        request.abiGlue.clear();
        Memory::Vector<Memory::String> diagnostics;
        chali_aot::UnitData unit;
        if (chali_aot::run(request, &diagnostics, &unit))
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
        if (!templates.empty()) package.entries.push_back({ package.manifest.templates, Kind::Templates, 0, templates });
        package.entries.push_back({ target.implementation, Kind::Object, 0, std::move(bytes) });
        retained += size;
        if (original.options.libraryBitcode)
        {
            request.options.emitObjectOnly = false;
            request.options.emitBitcodeOnly = true;
            request.options.outputPrefix = (work.path / "bitcode").u8string().c_str();
            chali_aot::UnitData bitcodeUnit;
            diagnostics.clear();
            if (chali_aot::run(request, &diagnostics, &bitcodeUnit))
            {
                for (const auto &diagnostic : diagnostics) std::fprintf(stderr, "chali lib: %s\n", diagnostic.c_str());
                return 1;
            }
            if (bitcodeUnit.symbol != unit.symbol || bitcodeUnit.bytes != unit.bytes || bitcodeUnit.layout != unit.layout)
                throw std::runtime_error("原生与 bitcode 实现的单元数据身份不一致");
            const auto path = work.path / "bitcode.bc";
            const auto length = std::filesystem::file_size(path);
            if (length > maximumPackageBytes - retained) throw std::runtime_error("bitcode 实现超过库包预算");
            std::ifstream input(path, std::ios::binary);
            std::string code(static_cast<std::size_t>(length), '\0');
            input.read(code.data(), static_cast<std::streamsize>(code.size()));
            if (!input || input.peek() != std::char_traits<char>::eof()) throw std::runtime_error("读取本次 bitcode 实现失败");
            const auto info = inspectBitcode(code);
            if (info.llvmVersion != target.llvmVersion || info.triple != target.triple || info.dataLayout != target.dataLayout)
                throw std::runtime_error("生成 bitcode 的实际目标、布局或 LLVM 版本与清单不一致");
            retained += length;
            package.entries.push_back({ target.bitcode, Kind::Bitcode, 0, std::move(code) });
        }
        if (!unit.symbol.empty())
        {
            target.unitData = "native/" + target.triple + "/data.bin";
            target.unitLayout = "native/" + target.triple + "/data.layout";
            target.unitSymbol = unit.symbol.c_str();
            if (!names.insert(target.unitData).second || !names.insert(target.unitLayout).second) throw std::runtime_error("单元数据资产与许可证名字冲突");
            package.manifest.requiredFeatures.push_back("unit-data-v1");
            package.entries.push_back({ target.unitData, Kind::UnitData, 0, std::string(unit.bytes.data(), unit.bytes.size()) });
            package.entries.push_back({ target.unitLayout, Kind::UnitData, 0, std::string(unit.layout.data(), unit.layout.size()) });
            package.manifest.targets[0] = target;
        }
        if (original.options.debug)
        {
            json::Object descriptor;
            descriptor.insert("format", 1);
            descriptor.insert("prefix", "chali_debug_" + chali_digest::hex(chali_digest::sha256(module)));
            descriptor.insert("optimized", original.options.debugOptimized);
            descriptor.insert("implementationSha256", chali_digest::hex(chali_digest::sha256(package.find(target.implementation)->bytes)));
            descriptor.insert("bitcodeSha256", target.bitcode.empty() ? "" : chali_digest::hex(chali_digest::sha256(package.find(target.bitcode)->bytes)));
            json::Object debug;
            debug.insert(target.triple, json::Value(std::move(descriptor)));
            auto metadata = json::parse(package.manifest.extensions);
            metadata.value->asObject()->insert("nativeDebug", json::Value(std::move(debug)));
            package.manifest.extensions = metadata.value->dump();
        }
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
