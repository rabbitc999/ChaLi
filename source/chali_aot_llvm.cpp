/**
 * @file chali_aot_llvm.cpp
 * @brief 静态 LLVM 目标代码生成和 LLD 链接实现，不启动 clang 子进程。
 */
#include "chali_aot_llvm.hpp"
#include "chali_aot_runtime_assets.hpp"
#include "chali_temp.hpp"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>
#if CHALI_STATIC_LLVM
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/BitReader.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>
#include <lld/Common/Driver.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/Analysis/ModuleSummaryAnalysis.h>
#include <llvm/Analysis/ProfileSummaryInfo.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Transforms/Utils/AssignGUID.h>
#include "chali_stdlib.hpp"
#include <miniz_tinfl.h>
LLD_HAS_DRIVER(mingw)
LLD_HAS_DRIVER(elf)
#endif

namespace chali_aot::builtin
{
#if CHALI_STATIC_LLVM
namespace
{
/**
 * @brief 单次 AOT 链接的私有资产目录，退出作用域时删除本次创建的内容。
 */
struct LinkerAssets
{
    std::filesystem::path root;
    /**
     * @brief 原子创建独立临时目录，避免并发编译复用可写链接库。
     */
    LinkerAssets()
    {
        const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto parent = chali_temp::root();
        for (unsigned attempt = 0; attempt < 100; ++attempt)
        {
            auto candidate = parent / ("chali-llvm-link-" + std::to_string(tick) + "-" + std::to_string(attempt));
            std::error_code error;
            if (std::filesystem::create_directory(candidate, error))
            {
                root = candidate;
                break;
            }
        }
    }
    /**
     * @brief 删除当前对象成功创建的临时目录，不接触外部路径。
     */
    ~LinkerAssets()
    {
        if (root.empty()) return;
        std::error_code ignored;
        std::filesystem::remove_all(root, ignored);
    }
    /**
     * @brief 解码编入编译器的固定链接库资产并核对解压长度。
     * @return 全部文件成功恢复时为 true。
     */
    bool restore()
    {
        Memory::String asset;
        if (root.empty() || !chali_module::embedded::findGeneratorAssetByPath("aot/linker_assets.txt", &asset)) return false;
        std::istringstream input(std::string(asset.data(), asset.size()));
        std::string name, hex;
        std::size_t size = 0, files = 0;
        while (input >> name >> size >> hex)
        {
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-") != std::string::npos || name == ".." || hex.size() % 2) return false;
            std::vector<unsigned char> compressed(hex.size() / 2), output(size);
            for (std::size_t index = 0; index < compressed.size(); ++index)
            {
                unsigned byte = 0;
                std::istringstream digits(hex.substr(index * 2, 2));
                if (!(digits >> std::hex >> byte)) return false;
                compressed[index] = static_cast<unsigned char>(byte);
            }
            if (tinfl_decompress_mem_to_mem(output.data(), output.size(), compressed.data(), compressed.size(), 0) != output.size()) return false;
            std::ofstream file(root / name, std::ios::binary);
            file.write(reinterpret_cast<const char *>(output.data()), static_cast<std::streamsize>(output.size()));
            if (!file) return false;
            ++files;
        }
        return files == 14;
    }
};
/** @brief 按完整或 ThinLTO 写出模块；ThinLTO 携带实际模块摘要与内容散列。
 * @param program 已由 LLVM 解析及优化的模块。
 * @param path 私有产物路径。
 * @param thin 是否生成 ThinLTO 摘要。
 * @param diagnostics 接收文件错误。
 * @return 成功为真，文件创建或写入失败为假。 */
bool writeBitcode(LLVMModuleRef program, const Memory::String &path, bool thin, Memory::Vector<Memory::String> *diagnostics)
{
    if (!thin) return LLVMWriteBitcodeToFile(program, path.c_str()) == 0;
    std::error_code error;
    llvm::raw_fd_ostream stream(path.c_str(), error, llvm::sys::fs::OF_None);
    if (error)
    {
        if (diagnostics) diagnostics->push_back(Memory::String(error.message().c_str()));
        return false;
    }
    auto &module = *llvm::unwrap(program);
    llvm::AssignGUIDPass::runOnModule(module);
    llvm::ProfileSummaryInfo profile(module);
    const auto index = llvm::buildModuleSummaryIndex(module, [](const llvm::Function &) -> llvm::BlockFrequencyInfo * { return nullptr; }, &profile);
    llvm::WriteBitcodeToFile(module, stream, false, &index, true);
    stream.close();
    if (stream.has_error())
    {
        if (diagnostics) diagnostics->push_back("AOT：写入 ThinLTO 模块失败");
        stream.clear_error();
        return false;
    }
    return true;
}
}  // namespace
#endif
/**
 * @brief 返回当前隔离模块的静态后端编入状态。
 * @return 静态 LLVM 变体为 true，外置工具变体为 false。
 */
bool available()
{
#if CHALI_STATIC_LLVM
    return true;
#else
    return false;
#endif
}
/** @brief 返回实际静态链接的 LLVM 版本。
 * @return 静态版本字符串，外置变体为空。 */
Memory::String version()
{
#if CHALI_STATIC_LLVM
    return LLVM_VERSION_STRING;
#else
    return {};
#endif
}

/**
 * @brief 为本机 CRT 或独立跨目标入口组织参数并调用 LLD。
 * @param request 已检查目标和路径的链接请求。
 * @param product 工作区内的输出文件。
 * @param diagnostics 接收链接器的完整说明。
 * @return 成功为零，失败为非零。
 */
int link(const chali_linker::Request &request, const Memory::String &product, Memory::Vector<Memory::String> *diagnostics)
{
#if CHALI_STATIC_LLVM
    TargetInfo target;
    Memory::String error;
    if (!describeTarget(request.options.target, target, &error))
    {
        if (diagnostics) diagnostics->push_back(error);
        return 1;
    }
    LinkerAssets assets;
    RuntimeAssets runtime;
    std::string library, runtimeLibrary;
    if (!request.noRuntime)
    {
        if (!target.windows || !target.native || !assets.restore())
        {
            if (diagnostics) diagnostics->push_back("无法恢复本机 CRT 链接资产");
            return 1;
        }
        if (!runtime.restore(diagnostics)) return 1;
        library = assets.root.generic_u8string();
        runtimeLibrary = runtime.root.generic_u8string();
    }
    std::vector<std::string> storage{"ld.lld", "-m", target.windows ? "i386pep" : target.arm64 ? "aarch64linux" : "elf_x86_64", "-o", product.c_str(), "--gc-sections"};
    if (!request.options.subsystem.empty()) storage.insert(storage.end(), {"--subsystem", request.options.subsystem.c_str()});
    if (!request.options.debug) storage.push_back("--strip-all");
    if (request.options.shared) storage.push_back("--shared");
    if (target.windows && request.options.shared) storage.push_back("--exclude-all-symbols");
    if (!request.entry.empty()) { storage.push_back("--entry"); storage.emplace_back(request.entry.c_str()); }
    if (!request.noRuntime)
    {
        storage.push_back(library + (request.options.shared ? "/dllcrt2.o" : "/crt2.o"));
        storage.push_back(library + "/crtbegin.o");
        storage.push_back("-L" + library);
        storage.push_back("-L" + runtimeLibrary);
    }
    for (const auto &path : request.libraryDirectories) storage.push_back("-L" + std::string(path.c_str()));
    storage.push_back("--start-group");
    for (const auto &input : request.inputs) storage.emplace_back(input.c_str());
    if (request.options.lto) storage.push_back((request.options.debug && !request.options.debugOptimized) ? "--lto-O0" : "--lto-O3");
    for (const auto &name : request.libraryNames) storage.push_back("-l" + std::string(name.c_str()));
    if (!request.noRuntime)
    {
        storage.push_back(runtimeLibrary + "/libstdc++.a");
        for (const char *name : {"chali_aot_host", "gcc_eh", "gcc", "winpthread", "winhttp", "ws2_32", "crypt32", "bcrypt"}) storage.push_back(std::string("-l") + name);
        for (const char *name : {"gcc_libmingw32.a", "gcc_libmingwex.a", "gcc_libmoldname.a"}) storage.push_back(runtimeLibrary + "/" + name);
    }
    storage.push_back("--end-group");
    if (!request.noRuntime)
    {
        const auto builtins = library + "/libclang_rt.builtins-x86_64.a";
        for (int round = 0; round < 2; ++round)
            for (const auto &argument : std::vector<std::string>{"-lmingw32", builtins, "-lunwind", "-lmoldname", "-lmingwex", "-lmsvcrt", "-ladvapi32", "-lshell32", "-luser32", "-lkernel32"}) storage.push_back(argument);
        storage.push_back(library + "/crtend.o");
    }
    std::filesystem::path script = std::filesystem::u8path(product.c_str());
    script += target.windows ? ".def" : ".map";
    if (!request.options.exports.empty() || (!target.windows && request.options.shared))
    {
        std::ofstream text(script, std::ios::binary);
        text << (target.windows ? "EXPORTS\n" : request.options.exports.empty() ? "{\n" : "{ global:\n");
        for (const auto &name : request.options.exports) text << '"' << name.c_str() << '"' << (target.windows ? "\n" : ";\n");
        if (!target.windows) text << "local: *; };\n";
        text.close();
        if (!text) { if (diagnostics) diagnostics->push_back("无法写入导出符号说明"); return 1; }
        if (target.windows) storage.push_back(script.u8string());
        else { storage.push_back("--version-script"); storage.push_back(script.u8string()); }
    }
    std::vector<const char *> arguments;
    for (const auto &argument : storage) arguments.push_back(argument.c_str());
    std::string output;
    llvm::raw_string_ostream stream(output);
    const lld::DriverDef drivers[] = {{lld::MinGW, &lld::mingw::link}, {lld::Gnu, &lld::elf::link}};
    const auto result = lld::lldMain(arguments, stream, stream, drivers);
    if (!output.empty() && diagnostics) diagnostics->push_back(output.c_str());
    return result.retCode;
#else
    (void)request; (void)product;
    if (diagnostics) diagnostics->push_back("此构建未静态链接 LLD");
    return 1;
#endif
}

/**
 * @brief 在编入的 LLVM 与 LLD 内完成原生优化、代码生成和链接。
 * @param request 包含共享库开关与外部 C 库的请求。
 * @param module LLVM 模块的产物路径。
 * @param source AST 后端直接生成的 LLVM 代码。
 * @param product 原生产物路径。
 * @param diagnostics 接收完整的原生工具诊断。
 * @return 成功为零，未编入或编译失败为一。
 */
int compile(const Request &request, const Memory::String &module, const Memory::String &source, const Memory::String &product,
            Memory::Vector<Memory::String> *diagnostics)
{
#if CHALI_STATIC_LLVM
    TargetInfo targetInfo;
    Memory::String targetError;
    if (!describeTarget(request.options.target, targetInfo, &targetError))
    {
        if (diagnostics) diagnostics->push_back(targetError);
        return 1;
    }
    if (targetInfo.arm64)
    {
        LLVMInitializeAArch64TargetInfo();
        LLVMInitializeAArch64Target();
        LLVMInitializeAArch64TargetMC();
        LLVMInitializeAArch64AsmParser();
        LLVMInitializeAArch64AsmPrinter();
    }
    else
    {
        LLVMInitializeX86TargetInfo();
        LLVMInitializeX86Target();
        LLVMInitializeX86TargetMC();
        LLVMInitializeX86AsmParser();
        LLVMInitializeX86AsmPrinter();
    }
    LLVMContextRef context = LLVMContextCreate();
    LLVMMemoryBufferRef buffer = nullptr;
    LLVMModuleRef program = nullptr;
    LLVMTargetMachineRef machine = nullptr;
    char *message = nullptr;
    buffer = LLVMCreateMemoryBufferWithMemoryRangeCopy(source.data(), source.size(), module.c_str());
    bool failed = buffer == nullptr;
    if (!failed)
    {
        const bool bitcode = source.size() >= 4 && ((source[0] == 'B' && source[1] == 'C' && static_cast<unsigned char>(source[2]) == 0xc0 && static_cast<unsigned char>(source[3]) == 0xde) || (static_cast<unsigned char>(source[0]) == 0xde && static_cast<unsigned char>(source[1]) == 0xc0 && static_cast<unsigned char>(source[2]) == 0x17 && static_cast<unsigned char>(source[3]) == 0x0b));
        if (bitcode)
        {
            failed = LLVMParseBitcodeInContext2(context, buffer, &program) != 0;
            LLVMDisposeMemoryBuffer(buffer);
            if (failed && diagnostics) diagnostics->push_back("AOT：LLVM 无法解析已选择的 bitcode 模块");
        }
        else
            failed = LLVMParseIRInContext(context, buffer, &program, &message) != 0;
    }
    LLVMTargetRef target = nullptr;
    const char *triple = targetInfo.triple.c_str();
    if (!failed) failed = LLVMGetTargetFromTriple(triple, &target, &message) != 0;
    if (!failed)
    {
        const auto cpu = request.options.cpu.empty() ? targetInfo.cpu : request.options.cpu;
        machine = LLVMCreateTargetMachine(target, triple, cpu.c_str(), "", (request.options.debug && !request.options.debugOptimized) ? LLVMCodeGenLevelNone : LLVMCodeGenLevelAggressive, LLVMRelocPIC, LLVMCodeModelDefault);
        failed = machine == nullptr;
        if (!failed)
        {
            LLVMSetTarget(program, triple);
            LLVMTargetDataRef layout = LLVMCreateTargetDataLayout(machine);
            LLVMSetModuleDataLayout(program, layout);
            LLVMDisposeTargetData(layout);
        }
    }
    if (!failed)
    {
        if (request.options.lto)
        {
            auto &module = *llvm::unwrap(program);
            module.setModuleFlag(llvm::Module::Error, "EnableSplitLTOUnit", std::uint32_t(!request.options.thinLto));
            module.setModuleFlag(llvm::Module::Error, "ThinLTO", std::uint32_t(request.options.thinLto));
        }
        LLVMPassBuilderOptionsRef options = LLVMCreatePassBuilderOptions();
        LLVMErrorRef error = LLVMRunPasses(program, (request.options.debug && !request.options.debugOptimized) ? "default<O0>,globaldce" : "default<O3>,globaldce", machine, options);
        LLVMDisposePassBuilderOptions(options);
        if (error)
        {
            char *text = LLVMGetErrorMessage(error);
            if (diagnostics) diagnostics->push_back(text);
            LLVMDisposeErrorMessage(text);
            failed = true;
        }
    }
    const std::string object = std::string(module.c_str()) + (request.options.lto ? ".lto.bc" : ".native.o");
    // 保留 LLVM bitcode 到链接阶段，由 LLD 根据真实外部引用和导出确定内部化边界。
    if (!failed)
    {
        if (request.options.emitObjectOnly || request.options.emitAssemblyOnly || !request.options.lto)
        {
            std::string file = request.options.emitObjectOnly || request.options.emitAssemblyOnly ? std::string(product.c_str()) : object;
            failed = LLVMTargetMachineEmitToFile(machine, program, file.data(), request.options.emitAssemblyOnly ? LLVMAssemblyFile : LLVMObjectFile, &message) != 0;
        }
        else
        {
            failed = !writeBitcode(program, object.c_str(), request.options.thinLto, diagnostics);
            if (failed && diagnostics) diagnostics->push_back("AOT：无法写入链接时优化的 LLVM bitcode");
        }
    }
    if (message)
    {
        if (diagnostics) diagnostics->push_back(message);
        LLVMDisposeMessage(message);
    }
    if (machine) LLVMDisposeTargetMachine(machine);
    if (program) LLVMDisposeModule(program);
    LLVMContextDispose(context);
    if (failed) return 1;
    if (request.options.emitObjectOnly || request.options.emitAssemblyOnly) return 0;
    // 链接资产在打包时内嵌并恢复到专用临时目录，LLVM/LLD 本身均为静态代码。
    LinkerAssets assets;
    if (!assets.restore())
    {
        if (diagnostics) diagnostics->push_back("AOT：无法恢复内嵌 CRT 链接资产");
        return 1;
    }
    RuntimeAssets runtimeAssets;
    if (!runtimeAssets.restore(diagnostics)) return 1;
    const std::string runtimeLibrary = runtimeAssets.root.generic_u8string();
    const std::string library = assets.root.generic_u8string();
    const std::string builtins = (assets.root / "libclang_rt.builtins-x86_64.a").generic_u8string();
    std::vector<std::string> storage{ "ld.lld", "-m", "i386pep", "-Bdynamic", "-o", product.c_str(),
                                      library + (request.options.shared ? "/dllcrt2.o" : "/crt2.o"), library + "/crtbegin.o", "-L" + library, object };
    if (!request.options.subsystem.empty()) storage.insert(storage.end(), {"--subsystem", request.options.subsystem.c_str()});
    storage.push_back((request.options.debug && !request.options.debugOptimized) ? "--lto-O0" : "--lto-O3");
    storage.push_back((request.options.debug && !request.options.debugOptimized) ? "--lto-CGO0" : "--lto-CGO3");
    storage.push_back("--gc-sections");
    if (!request.options.debug) storage.push_back("--strip-all");
    if (request.options.shared) storage.push_back("--shared");
    storage.push_back("--start-group");
    for (const auto &object : request.options.linkObjects) storage.emplace_back(object.c_str());
    storage.push_back("-L" + runtimeLibrary);
    storage.push_back(runtimeLibrary + "/libstdc++.a");
    for (const char *name : { "chali_aot_host", "gcc_eh", "gcc", "winpthread", "winhttp", "ws2_32", "crypt32", "bcrypt" }) storage.push_back(std::string("-l") + name);
    for (const char *name : { "gcc_libmingw32.a", "gcc_libmingwex.a", "gcc_libmoldname.a" }) storage.push_back(runtimeLibrary + "/" + name);
    for (const auto &path : request.options.linkLibraries) storage.emplace_back(path.c_str());
    storage.push_back("--end-group");
    for (int round = 0; round < 2; ++round)
        for (const auto &argument : std::vector<std::string>{ "-lmingw32", builtins, "-lunwind", "-lmoldname", "-lmingwex", "-lmsvcrt", "-ladvapi32", "-lshell32", "-luser32", "-lkernel32" }) storage.push_back(argument);
    storage.push_back(library + "/crtend.o");
    std::vector<const char *> arguments;
    for (const auto &argument : storage) arguments.push_back(argument.c_str());
    std::string output;
    llvm::raw_string_ostream stream(output);
    const lld::DriverDef drivers[] = { { lld::MinGW, &lld::mingw::link } };
    const lld::Result result = lld::lldMain(arguments, stream, stream, drivers);
    if (!output.empty() && diagnostics) diagnostics->push_back(output.c_str());
    std::error_code ignored;
    std::filesystem::remove(std::filesystem::u8path(object), ignored);
    return result.retCode;
#else
    (void)request;
    (void)module;
    (void)source;
    (void)product;
    if (diagnostics) diagnostics->push_back("AOT：此构建未静态链接 LLVM");
    return 1;
#endif
}
}  // namespace chali_aot::builtin
