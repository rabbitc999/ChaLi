/** @file chali_bitcode.cpp
 * @brief 实际 LLVM 版本核对与 bitcode 到独立原生对象的物化。
 */
#include "chali_aot.hpp"
#include "chali_aot_llvm.hpp"
#include "chali_native_tools.hpp"
#include <fstream>
#include <regex>
#include <set>
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
namespace
{
/** @brief 仅规范化 LLVM 自行导出的模块模式标志，保留其他元数据及源码。
 * @param input LLVM 文本输入，不接受未经过 LLVM 读取的原始用户文本。
 * @param output 私有工作区中的独立规范化文件。
 * @param thin 选择 ThinLTO 或完整 LTO。
 * @return 无，超预算、格式异常或读写失败时抛出异常。 */
void normalizeLtoFlags(const std::filesystem::path &input, const std::filesystem::path &output, bool thin)
{
    if (std::filesystem::file_size(input) > 256ull * 1024 * 1024) throw std::runtime_error("LLVM 模式转换文本超过预算");
    std::ifstream source(input, std::ios::binary);
    if (!source) throw std::runtime_error("无法读取 LLVM 模式转换文本");
    std::string line;
    std::set<std::string> flags;
    const std::regex reference("![0-9]+");
    bool found = false;
    while (std::getline(source, line))
        if (line.rfind("!llvm.module.flags = !{", 0) == 0)
        {
            if (found || line.size() > 65536) throw std::runtime_error("LLVM 模块标志重复或超过预算");
            found = true;
            for (std::sregex_iterator at(line.begin(), line.end(), reference), end; at != end; ++at)
                if (!flags.insert(at->str()).second || flags.size() > 1024) throw std::runtime_error("LLVM 模块标志引用重复或超过预算");
        }
    if (!source.eof()) throw std::runtime_error("读取 LLVM 模块标志失败");
    source.clear();
    source.seekg(0);
    std::ofstream target(output, std::ios::binary);
    if (!target) throw std::runtime_error("无法写入 LLVM 模式转换文本");
    const std::regex value(R"flag(^(![0-9]+ = !\{i32 [0-9]+, !")(ThinLTO|EnableSplitLTOUnit)(", i32 )([0-9]+)(\})$)flag");
    while (std::getline(source, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const auto equal = line.find(" = ");
        if (equal != std::string::npos && flags.count(line.substr(0, equal)))
        {
            std::smatch match;
            if (std::regex_match(line, match, value))
                line = match[1].str() + match[2].str() + match[3].str() + ((match[2] == "ThinLTO" ? thin : !thin) ? "1" : "0") + match[5].str();
        }
        target << line << '\n';
    }
    target.close();
    if (!source.eof() || !target) throw std::runtime_error("LLVM 模式转换文本读写失败");
}
/** @brief 将调用方已核对的 bitcode 编译为对象或所选模式的模块，不执行最终链接。
 * @param bytes 已验证的不可变模块。
 * @param options 目标与工具选项。
 * @param output 私有工作区对象路径。
 * @param diagnostics 接收失败说明。
 * @param module 是否保留 LLVM 模块而非生成机器对象。
 * @return 成功零，失败一。 */
int compileBitcode(const Memory::String &bytes, const Options &options, const Memory::String &output,
                   Memory::Vector<Memory::String> *diagnostics, bool module)
{
    try
    {
        if (builtin::available() && options.compiler.empty())
        {
            Request request;
            request.options = options;
            request.options.emitObjectOnly = !module;
            request.options.emitBitcodeOnly = module;
            request.options.emitLLVMOnly = request.options.emitAssemblyOnly = false;
            if (!module) request.options.lto = false;
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
        auto current = input;
        if (module)
        {
            const auto text = work.path / "module.ll";
            std::string report;
            if (chali_native_tools::execute({ compiler.c_str(), "--target=" + std::string(target.triple.c_str()), "-O0", "-S", "-emit-llvm", "-x", "ir", input.u8string(), "-o", text.u8string() }, work.path / "decode.log", report))
                throw std::runtime_error("外置 LLVM 无法读取待转换模块：" + report);
            current = work.path / "normalized.ll";
            normalizeLtoFlags(text, current, options.lto && options.thinLto);
        }
        std::vector<std::string> command{ compiler.c_str(), "--target=" + std::string(target.triple.c_str()),
                                          options.debug ? "-O0" : "-O3", "-c", "-x", "ir", current.u8string(), "-o", output.c_str() };
        if (module)
        {
            command.push_back("-emit-llvm");
            command.push_back(options.lto && options.thinLto ? "-flto=thin" : "-flto=full");
            if (options.lto && options.thinLto) command.push_back("-fno-split-lto-unit");
        }
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
}  // namespace
/** @brief 将已验证的模块编译为独立原生对象。
 * @param bytes 已验证的 bitcode。
 * @param options 目标和 LLVM 工具配置。
 * @param output 对象产物路径。
 * @param diagnostics 接收失败说明。
 * @return 成功零，失败非零。 */
int bitcodeObject(const Memory::String &bytes, const Options &options, const Memory::String &output,
                  Memory::Vector<Memory::String> *diagnostics)
{
    return compileBitcode(bytes, options, output, diagnostics, false);
}
/** @brief 为当前完整／ThinLTO 模式重新编码已验证的模块。
 * @param bytes 已验证的 bitcode。
 * @param options 目标、LLVM 工具和 LTO 模式。
 * @param output 模块产物路径。
 * @param diagnostics 接收失败说明。
 * @return 成功零，失败非零。 */
int bitcodeModule(const Memory::String &bytes, const Options &options, const Memory::String &output,
                  Memory::Vector<Memory::String> *diagnostics)
{
    return compileBitcode(bytes, options, output, diagnostics, true);
}
}  // namespace chali_aot
