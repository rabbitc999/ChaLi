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
#include <llvm-c/IRReader.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>
#include <lld/Common/Driver.h>
#include <llvm/Support/raw_ostream.h>
#include "chali_stdlib.hpp"
#include <miniz_tinfl.h>
LLD_HAS_DRIVER(mingw)
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

/**
 * @brief 在编入的 LLVM 与 LLD 内完成原生优化、代码生成和链接。
 * @param request 包含共享库开关与外部 C 库的请求。
 * @param module LLVM 模块文件路径。
 * @param product 原生产物路径。
 * @param diagnostics 接收完整的原生工具诊断。
 * @return 成功为零，未编入或编译失败为一。
 */
int compile(const Request &request, const Memory::String &module, const Memory::String &product,
            Memory::Vector<Memory::String> *diagnostics)
{
#if CHALI_STATIC_LLVM
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmParser();
    LLVMInitializeX86AsmPrinter();
    LLVMContextRef context = LLVMContextCreate();
    LLVMMemoryBufferRef buffer = nullptr;
    LLVMModuleRef program = nullptr;
    LLVMTargetMachineRef machine = nullptr;
    char *message = nullptr;
    bool failed = LLVMCreateMemoryBufferWithContentsOfFile(module.c_str(), &buffer, &message) != 0;
    if (!failed) failed = LLVMParseIRInContext(context, buffer, &program, &message) != 0;
    LLVMTargetRef target = nullptr;
    const char *triple = "x86_64-w64-windows-gnu";
    if (!failed) failed = LLVMGetTargetFromTriple(triple, &target, &message) != 0;
    if (!failed)
    {
        machine = LLVMCreateTargetMachine(target, triple, "x86-64", "", request.options.debug ? LLVMCodeGenLevelNone : LLVMCodeGenLevelAggressive, LLVMRelocPIC, LLVMCodeModelDefault);
        failed = machine == nullptr;
    }
    if (!failed)
    {
        LLVMPassBuilderOptionsRef options = LLVMCreatePassBuilderOptions();
        LLVMErrorRef error = LLVMRunPasses(program, request.options.debug ? "default<O0>,globaldce" : "default<O3>,globaldce", machine, options);
        LLVMDisposePassBuilderOptions(options);
        if (error)
        {
            char *text = LLVMGetErrorMessage(error);
            if (diagnostics) diagnostics->push_back(text);
            LLVMDisposeErrorMessage(text);
            failed = true;
        }
    }
    const std::string object = std::string(module.c_str()) + ".lto.bc";
    // 保留 LLVM bitcode 到链接阶段，由 LLD 根据真实外部引用和导出确定内部化边界。
    if (!failed) {
        LLVMSetTarget(program, triple);
        failed = LLVMWriteBitcodeToFile(program, object.c_str()) != 0;
        if (failed && diagnostics) diagnostics->push_back("AOT：无法写入链接时优化的 LLVM bitcode");
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
    storage.push_back(request.options.debug ? "--lto-O0" : "--lto-O3");
    storage.push_back(request.options.debug ? "--lto-CGO0" : "--lto-CGO3");
    storage.push_back("--gc-sections");
    if (!request.options.debug) storage.push_back("--strip-all");
    if (request.options.shared) storage.push_back("--shared");
    for (const auto &object : request.options.linkObjects) storage.emplace_back(object.c_str());
    storage.push_back("-L" + runtimeLibrary);
    storage.push_back("--start-group");
    storage.push_back(runtimeLibrary + "/libstdc++.a");
    for (const char *name : { "chali_aot_host", "gcc_eh", "gcc", "winpthread", "winhttp", "ws2_32", "crypt32", "bcrypt" }) storage.push_back(std::string("-l") + name);
    for (const char *name : { "gcc_libmingw32.a", "gcc_libmingwex.a", "gcc_libmoldname.a" }) storage.push_back(runtimeLibrary + "/" + name);
    storage.push_back("--end-group");
    for (const auto &path : request.options.linkLibraries) storage.emplace_back(path.c_str());
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
    (void)product;
    if (diagnostics) diagnostics->push_back("AOT：此构建未静态链接 LLVM");
    return 1;
#endif
}
}  // namespace chali_aot::builtin
