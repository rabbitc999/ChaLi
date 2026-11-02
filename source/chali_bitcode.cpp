/** @file chali_bitcode.cpp
 * @brief 实际 LLVM 版本核对与 bitcode 到独立原生对象的物化。
 */
#include "chali_aot.hpp"
#include "chali_aot_llvm.hpp"
#include "chali_native_tools.hpp"
#include <fstream>
#include <stdexcept>

namespace chali_aot
{
/** @brief 使用实际静态 LLVM 或外置 clang 报告核对版本，不采用包声明代替工具版本。
 * @param options 实际 LLVM 工具选择。
 * @param diagnostics 可选完整失败说明。
 * @return 精确版本，失败为空串。 */
Memory::String llvmVersion(const Options &options, Memory::Vector<Memory::String> *diagnostics)
{
    if (builtin::available() && options.compiler.empty()) return builtin::version();
    try
    {
        chali_native_tools::Workspace work;
        std::string report;
        const auto compiler = llvmCompilerPath(options.compiler);
        if (chali_native_tools::execute({ std::string(compiler.c_str()), "--version" }, work.path / "version.log", report))
            throw std::runtime_error("无法查询外置 LLVM 版本：" + report);
        const auto marker = report.find("clang version ");
        if (marker == std::string::npos) throw std::runtime_error("外置工具未报告 clang 版本：" + report);
        const auto start = marker + 14, end = report.find_first_of(" \t\r\n", start);
        const auto version = report.substr(start, end == std::string::npos ? end : end - start);
        if (version.empty() || version.front() < '0' || version.front() > '9') throw std::runtime_error("外置 LLVM 版本格式非法");
        return Memory::String(version.c_str());
    } catch (const std::exception &error)
    {
        if (diagnostics) diagnostics->push_back(Memory::String(error.what()));
        return {};
    }
}
/** @brief 将调用方已核对的 bitcode 物化为单独对象，不执行最终链接。
 * @param bytes 已验证的不可变模块。
 * @param options 目标与工具选项。
 * @param output 私有工作区对象路径。
 * @param diagnostics 接收失败说明。
 * @return 成功零，失败一。 */
int bitcodeObject(const Memory::String &bytes, const Options &options, const Memory::String &output,
                  Memory::Vector<Memory::String> *diagnostics)
{
    try
    {
        if (builtin::available() && options.compiler.empty())
        {
            Request request;
            request.options = options;
            request.options.emitObjectOnly = true;
            request.options.emitBitcodeOnly = request.options.emitLLVMOnly = request.options.emitAssemblyOnly = false;
            request.options.lto = false;
            return builtin::compile(request, output, bytes, output, diagnostics);
        }
        TargetInfo target;
        Memory::String error;
        if (!describeTarget(options.target, target, &error)) throw std::runtime_error(error.c_str());
        chali_native_tools::Workspace work;
        const auto input = work.path / "input.bc";
        std::ofstream stream(input, std::ios::binary);
        stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        stream.close();
        if (!stream) throw std::runtime_error("无法暂存已验证的 bitcode");
        const auto compiler = llvmCompilerPath(options.compiler);
        std::vector<std::string> command{ compiler.c_str(), "--target=" + std::string(target.triple.c_str()),
                                          options.debug ? "-O0" : "-O3", "-c", "-x", "ir", input.u8string(), "-o", output.c_str() };
        if (!options.cpu.empty()) command.push_back(std::string(target.arm64 ? "-mcpu=" : "-march=") + options.cpu.c_str());
        if (!options.sysroot.empty()) command.push_back("--sysroot=" + std::string(options.sysroot.c_str()));
        if (!target.windows) command.push_back("-fPIC");
        std::string report;
        const auto result = chali_native_tools::execute(command, work.path / "object.log", report);
        if (!report.empty() && diagnostics) diagnostics->push_back(report.c_str());
        return result;
    } catch (const std::exception &error)
    {
        if (diagnostics) diagnostics->push_back(Memory::String(error.what()));
        return 1;
    }
}
}  // namespace chali_aot
