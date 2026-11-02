/** @file chali_library_tools.cpp
 * @brief 库包格式工具：生成规范清单、封装现有资产并检查包内容。
 */
#include "chali_library.hpp"
#include "chali_digest.hpp"
#include "chali_aot_target.hpp"
#include "chali_package.hpp"
#include "chali_native_tools.hpp"
#include "json/json.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <stdexcept>

/** @brief 编译器原生驱动入口。
 * @param argc 参数数量。
 * @param argv 完整参数数组。
 * @return 编译状态。 */
int chali_main_entry(int argc, char **argv);
namespace chali_library
{
namespace
{
/**
 * @brief 有界读取普通文件，拒绝读取过程中的截断和增长。
 * @param path UTF-8 文件路径。
 * @param limit 本次读取的预算。
 * @return 拥有型原始字节。
 */
std::string file(const std::filesystem::path &path, std::uint64_t limit)
{
    if (!std::filesystem::is_regular_file(path)) throw std::runtime_error("库输入不是普通文件：" + path.u8string());
    const auto size = std::filesystem::file_size(path);
    if (size > limit) throw std::runtime_error("库输入超过长度预算：" + path.u8string());
    std::ifstream stream(path, std::ios::binary);
    if (!stream) throw std::runtime_error("无法打开库输入：" + path.u8string());
    std::string result(static_cast<std::size_t>(size), '\0');
    stream.read(result.data(), result.size());
    if (static_cast<std::size_t>(stream.gcount()) != result.size() || stream.peek() != std::char_traits<char>::eof()) throw std::runtime_error("库输入在读取期间变化：" + path.u8string());
    return result;
}
/**
 * @brief 将模板清单写入标准输出或事务发布至指定文件。
 * @param bytes 完整规范清单及结尾换行。
 * @param path 空值为标准输出。
 */
void manifestOutput(const std::string &bytes, const std::string &path)
{
    if (path.empty())
    {
        if (std::fwrite(bytes.data(), 1, bytes.size(), stdout) != bytes.size() || std::fflush(stdout)) throw std::runtime_error("清单输出失败");
        return;
    }
    chali_native_tools::Workspace work;
    const auto temporary = work.path / "manifest.json";
    std::ofstream stream(temporary, std::ios::binary);
    stream.write(bytes.data(), bytes.size());
    stream.close();
    if (!stream) throw std::runtime_error("无法写入私有清单");
    std::string error;
    if (!chali_native_tools::publish(temporary, std::filesystem::u8path(path), error)) throw std::runtime_error("无法发布清单：" + error);
}
/**
 * @brief 将命令行种类名转换为固定编号。
 * @param name 已支持的载荷种类。
 * @return 对应种类。
 */
Kind kind(const std::string &name)
{
    if (name == "interface") return Kind::Interface;
    if (name == "templates") return Kind::Templates;
    if (name == "object") return Kind::Object;
    if (name == "archive") return Kind::Archive;
    if (name == "bitcode") return Kind::Bitcode;
    if (name == "bytecode") return Kind::Bytecode;
    if (name == "source") return Kind::Source;
    if (name == "license") return Kind::License;
    if (name == "unit-data") return Kind::UnitData;
    throw std::invalid_argument("未知库资产种类：" + name);
}
/**
 * @brief 输出只包含清单和摘要的检查报告，不输出负载源码。
 * @param package 已验证的库包。
 * @return JSON 检查报告。
 */
std::string inspection(const Package &package)
{
    json::Object report;
    report.insert("format", int(packageFormat));
    report.insert("sha256", package.contentSha256);
    report.insert("manifest", *json::parse(manifestJson(package.manifest)).value);
    json::Array entries;
    for (const auto &entry : package.entries)
    {
        json::Object record;
        record.insert("name", entry.name);
        record.insert("kind", int(static_cast<std::uint16_t>(entry.kind)));
        record.insert("optional", bool(entry.flags & 1u));
        record.insert("bytes", std::int64_t(entry.bytes.size()));
        record.insert("sha256", chali_digest::hex(chali_digest::sha256(entry.bytes)));
        entries.emplace_back(std::move(record));
    }
    report.insert("entries", std::move(entries));
    return json::Value(std::move(report)).dump(2) + "\n";
}
}  // namespace
/**
 * @brief 派发受支持的语言库编译或格式工具，不执行初始化。
 * @param argc 归一化参数数量。
 * @param argv lib 子命令参数。
 * @return 成功零、输入或工具失败一、用法错误二。
 */
int runCli(int argc, char **argv)
{
    try
    {
        if (argc < 3) throw std::invalid_argument("使用 chali lib template／pack／inspect；--help 查看格式工具用法");
        const std::string mode = argv[2];
        if (mode == "--help" || mode == "help" || mode == "-h")
        {
            std::puts("chali lib build FILE.chl --module NAME --package-version VERSION --out FILE.chlib [--target TARGET] [--cpu CPU] [--license SPDX] [--license-asset NAME=FILE] [--system-library NAME] [--runtime-library NAME] [--with-bitcode]\nchali lib template --module NAME --package-version VERSION [--target TARGET] [--cpu CPU] [--license SPDX] [--license-asset NAME] [--dependency MODULE=VERSION#SHA256] [--system-library NAME] [--runtime-library NAME] [--out MANIFEST.json]\nchali lib pack --manifest MANIFEST.json --entry KIND:NAME=FILE ... --out FILE.chlib\nchali lib lock FILE.chlib ... --out chali.libraries.lock\nchali lib inspect FILE.chlib [--module EXPECTED] [--package-version EXPECTED] [--target TARGET] [--cpu CPU]\nKIND: interface, templates, object, archive, bitcode, bytecode, source, license, unit-data\nbuild 编译受支持的语言函数；其余格式工具封装或检查资产。导出与导入不执行库初始化。");
            return 0;
        }
        if (mode == "build")
        {
            std::vector<std::string> arguments{ argv[0], "aot" };
            for (int index = 3; index < argc; ++index)
            {
                const std::string option = argv[index];
                arguments.push_back(option == "--module" ? "--library-module" : option == "--package-version" ? "--library-version" :
                                                                            option == "--license"             ? "--library-license" :
                                                                            option == "--license-asset"       ? "--library-license-asset" :
                                                                            option == "--system-library"      ? "--library-system-library" :
                                                                            option == "--runtime-library"     ? "--library-runtime-library" :
                                                                                                                option);
            }
            std::vector<char *> values;
            for (auto &argument : arguments) values.push_back(argument.data());
            return chali_main_entry(static_cast<int>(values.size()), values.data());
        }
        if (mode == "lock")
        {
            std::string output;
            std::vector<std::filesystem::path> inputs;
            bool literal = false;
            for (int index = 3; index < argc; ++index)
            {
                const std::string argument = argv[index];
                if (!literal && argument == "--")
                {
                    literal = true;
                    continue;
                }
                if (!literal && argument == "--out")
                {
                    if (++index >= argc || !output.empty()) throw std::invalid_argument("lock --out 需要唯一路径");
                    output = argv[index];
                }
                else if (!literal && !argument.empty() && argument.front() == '-')
                    throw std::invalid_argument("未知 lock 选项：" + argument);
                else
                    inputs.push_back(std::filesystem::u8path(argument));
            }
            if (output.empty() || inputs.empty() || inputs.size() > 128) throw std::invalid_argument("lock 需要一至一百二十八个包及 --out 文件");
            const auto product = std::filesystem::weakly_canonical(std::filesystem::u8path(output));
            std::vector<Dependency> records;
            std::vector<Dependency> required;
            for (const auto &path : inputs)
            {
                if (std::filesystem::weakly_canonical(path) == product) throw std::runtime_error("库锁不能覆盖输入包");
                Package package;
                std::string error;
                if (!read(path, package, error)) throw std::runtime_error(path.u8string() + ": " + error);
                records.push_back({ package.manifest.module, package.manifest.version, package.contentSha256 });
                required.insert(required.end(), package.manifest.dependencies.begin(), package.manifest.dependencies.end());
            }
            for (const auto &dependency : required)
            {
                const auto found = std::find_if(records.begin(), records.end(), [&](const auto &record) { return record.module == dependency.module; });
                if (found == records.end() || found->version != dependency.version || found->sha256 != dependency.sha256)
                    throw std::runtime_error("库锁输入未包含匹配的传递依赖：" + dependency.module);
            }
            manifestOutput(lockJson(std::move(records)) + "\n", output);
            return 0;
        }
        if (mode != "template" && mode != "pack" && mode != "inspect") throw std::invalid_argument("未知 lib 子命令：" + mode);
        Manifest manifest;
        std::string target, cpu, output, input, manifestPath;
        std::vector<std::string> assets, dependencies, systemLibraries, runtimeLibraries;
        bool literal = false;
        for (int index = 3; index < argc; ++index)
        {
            const std::string option = argv[index];
            if (option == "--" && !literal)
            {
                literal = true;
                continue;
            }
            if (!literal && (option == "--help" || option == "-h"))
            {
                char *arguments[] = { argv[0], argv[1], const_cast<char *>("--help") };
                return runCli(3, arguments);
            }
            if (!literal && (option == "--module" || option == "--package-version" || option == "--target" || option == "--cpu" || option == "--out" || option == "--manifest" || option == "--entry" || option == "--license" || option == "--license-asset" || option == "--dependency" || option == "--system-library" || option == "--runtime-library"))
            {
                if (++index >= argc) throw std::invalid_argument("库选项缺少值：" + option);
                const std::string value = argv[index];
                if (option == "--module")
                    manifest.module = value;
                else if (option == "--package-version")
                    manifest.version = value;
                else if (option == "--target")
                    target = value;
                else if (option == "--cpu")
                    cpu = value;
                else if (option == "--out")
                    output = value;
                else if (option == "--manifest")
                    manifestPath = value;
                else if (option == "--entry")
                    assets.push_back(value);
                else if (option == "--license")
                    manifest.license = value;
                else if (option == "--license-asset")
                    manifest.licenseFiles.push_back(value);
                else if (option == "--dependency")
                    dependencies.push_back(value);
                else if (option == "--system-library")
                    systemLibraries.push_back(value);
                else if (option == "--runtime-library")
                    runtimeLibraries.push_back(value);
                continue;
            }
            if (mode == "inspect" && input.empty() && (literal || option.empty() || option.front() != '-'))
            {
                input = option;
                continue;
            }
            throw std::invalid_argument("未知库选项或多余参数：" + option);
        }
        if (mode == "template")
        {
            if (manifest.module.empty() || manifest.version.empty()) throw std::invalid_argument("template 需要 --module 和 --package-version");
            if (!input.empty() || !manifestPath.empty() || !assets.empty()) throw std::invalid_argument("template 不接受资产或输入清单");
            manifest.compilerVersion = chali_package::compilerVersion;
            manifest.requiredFeatures = { "native-c-v1" };
            for (const auto &record : dependencies)
            {
                const auto equal = record.find('='), hash = record.rfind('#');
                if (equal == record.npos || hash == record.npos || equal >= hash) throw std::invalid_argument("依赖格式应为 MODULE=VERSION#SHA256");
                manifest.dependencies.push_back({ record.substr(0, equal), record.substr(equal + 1, hash - equal - 1), record.substr(hash + 1) });
            }
            chali_aot::TargetInfo known;
            Memory::String error;
            if (!chali_aot::describeTarget(Memory::String(target.c_str()), known, &error)) throw std::invalid_argument(error.c_str());
            Target implementation;
            implementation.triple = known.triple.c_str();
            implementation.objectFormat = known.windows ? "coff" : "elf";
            implementation.dataLayout = known.dataLayout.c_str();
            implementation.cpu = cpu.empty() ? std::string(known.cpu.c_str()) : cpu;
            implementation.implementation = "native/" + implementation.triple + "/library.a";
            implementation.systemLibraries = std::move(systemLibraries);
            implementation.runtimeLibraries = std::move(runtimeLibraries);
            manifest.targets.push_back(std::move(implementation));
            manifestOutput(manifestJson(manifest) + "\n", output);
            return 0;
        }
        if (!dependencies.empty() || !systemLibraries.empty() || !runtimeLibraries.empty() || !manifest.license.empty() || !manifest.licenseFiles.empty()) throw std::invalid_argument("依赖与许可证选项只适用于 template");
        if (mode == "inspect")
        {
            if (input.empty() || !output.empty() || !manifestPath.empty() || !assets.empty()) throw std::invalid_argument("inspect 需要一个库包路径，不接受输出或资产");
            Package package;
            std::string error;
            if (!read(std::filesystem::u8path(input), package, error)) throw std::runtime_error(error);
            if ((!manifest.module.empty() && manifest.module != package.manifest.module) || (!manifest.version.empty() && manifest.version != package.manifest.version)) throw std::runtime_error("库模块或精确包版本不匹配");
            if (!target.empty() || !cpu.empty()) select(package, target, cpu);
            manifestOutput(inspection(package), {});
            return 0;
        }
        if (output.empty() || manifestPath.empty() || assets.empty() || !manifest.module.empty() || !manifest.version.empty() || !target.empty() || !cpu.empty()) throw std::invalid_argument("pack 需要 --manifest、--entry 和 --out，其余元数据来自清单");
        if (assets.size() > 4096) throw std::invalid_argument("库资产数量超过 4096 项预算");
        auto bytes = file(std::filesystem::u8path(manifestPath), 1024 * 1024 + 2);
        while (!bytes.empty() && (bytes.back() == '\r' || bytes.back() == '\n')) bytes.pop_back();
        Package package;
        package.manifest = parseManifest(bytes);
        const auto product = std::filesystem::weakly_canonical(std::filesystem::u8path(output));
        if (product == std::filesystem::weakly_canonical(std::filesystem::u8path(manifestPath))) throw std::runtime_error("库包不能覆盖输入清单");
        std::uint64_t total = 56 + bytes.size();
        for (const auto &asset : assets)
        {
            const auto colon = asset.find(':'), equal = asset.find('=');
            if (colon == asset.npos || equal == asset.npos || colon >= equal) throw std::invalid_argument("资产格式应为 KIND:NAME=FILE");
            Entry entry;
            entry.kind = kind(asset.substr(0, colon));
            entry.name = asset.substr(colon + 1, equal - colon - 1);
            const auto path = std::filesystem::u8path(asset.substr(equal + 1));
            if (product == std::filesystem::weakly_canonical(path)) throw std::runtime_error("库包不能覆盖输入资产");
            const auto metadata = 48 + entry.name.size();
            if (metadata > maximumPackageBytes - total) throw std::runtime_error("库包超出总预算");
            total += metadata;
            entry.bytes = file(path, maximumPackageBytes - total);
            total += entry.bytes.size();
            package.entries.push_back(std::move(entry));
        }
        std::string error;
        if (!write(std::filesystem::u8path(output), package, error)) throw std::runtime_error(error);
        return 0;
    } catch (const std::invalid_argument &error)
    {
        std::fprintf(stderr, "chali lib: %s\n", error.what());
        return 2;
    } catch (const std::exception &error)
    {
        std::fprintf(stderr, "chali lib: %s\n", error.what());
        return 1;
    }
}
}  // namespace chali_library
