/** @file chali_assembler_llvm.cpp
 * @brief 使用当前 LLVM SDK 的 MC 接口汇编，不恢复旧执行器或启动外部工具。
 */
#include "chali_assembler.hpp"
#if CHALI_STATIC_LLVM
#include <llvm-c/Target.h>
#include <llvm/MC/MCAsmBackend.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCCodeEmitter.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCObjectFileInfo.h>
#include <llvm/MC/MCObjectWriter.h>
#include <llvm/MC/MCParser/MCAsmParser.h>
#include <llvm/MC/MCParser/MCTargetAsmParser.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCStreamer.h>
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <filesystem>
#include <memory>
#endif

namespace chali_assembler::builtin
{
#if CHALI_STATIC_LLVM
namespace
{
/**
 * @brief 保存 LLVM MC 给出的原始文件、行列及消息。
 * @param message 当前源码诊断。
 * @param opaque 调用方字符串缓冲。
 */
void captureDiagnostic(const llvm::SMDiagnostic &message, void *opaque)
{
    auto &text = *static_cast<std::string *>(opaque);
    if (text.size() >= 128u * 1024u) return;
    llvm::raw_string_ostream stream(text);
    message.print("chali asm", stream, false, false);
    stream.flush();
    if (text.size() > 128u * 1024u) text.resize(128u * 1024u);
}
}  // namespace
#endif
/**
 * @brief 创建单次 LLVM MC 上下文，将原始汇编解析为目标对象。
 * @param input 原始输入绝对路径。
 * @param output 调用方工作区内的输出路径。
 * @param options 目标、CPU、包含路径与调试开关。
 * @param diagnostic 接收原始来源诊断。
 * @return 成功为零；当前变体无内置 MC 或汇编失败时为一。
 */
int assemble(const std::string &input, const std::string &output, const chali_build::Options &options, std::string &diagnostic)
{
#if CHALI_STATIC_LLVM
    chali_build::TargetInfo selected;
    Memory::String error;
    if (!chali_build::describeTarget(options.target, selected, &error))
    {
        diagnostic = error.c_str();
        return 1;
    }
    if (selected.arm64)
    {
        LLVMInitializeAArch64TargetInfo();
        LLVMInitializeAArch64Target();
        LLVMInitializeAArch64TargetMC();
        LLVMInitializeAArch64AsmParser();
    }
    else
    {
        LLVMInitializeX86TargetInfo();
        LLVMInitializeX86Target();
        LLVMInitializeX86TargetMC();
        LLVMInitializeX86AsmParser();
    }
    llvm::Triple triple(selected.triple.c_str());
    const auto *target = llvm::TargetRegistry::lookupTarget("", triple, diagnostic);
    if (!target) return 1;
    auto buffer = llvm::MemoryBuffer::getFile(input, true);
    if (!buffer)
    {
        diagnostic = buffer.getError().message();
        return 1;
    }
    llvm::SourceMgr sources;
    sources.setVirtualFileSystem(llvm::vfs::getRealFileSystem());
    sources.setDiagHandler(captureDiagnostic, &diagnostic);
    sources.AddNewSourceBuffer(std::move(*buffer), llvm::SMLoc());
    std::vector<std::string> includes{ std::filesystem::u8path(input).parent_path().u8string() };
    for (const auto &path : options.assemblyIncludes) includes.push_back(std::filesystem::absolute(std::filesystem::u8path(path.c_str())).u8string());
    sources.setIncludeDirs(includes);
    llvm::MCTargetOptions settings;
    settings.MCNoExecStack = !selected.windows;
    settings.DwarfVersion = 4;
    const auto cpu = options.cpu.empty() ? selected.cpu : options.cpu;
    std::unique_ptr<llvm::MCRegisterInfo> registers(target->createMCRegInfo(triple));
    std::unique_ptr<llvm::MCInstrInfo> instructions(target->createMCInstrInfo());
    std::unique_ptr<llvm::MCSubtargetInfo> subtarget(target->createMCSubtargetInfo(triple, cpu.c_str(), ""));
    if (!registers || !instructions || !subtarget || !subtarget->isCPUStringValid(cpu.c_str()))
    {
        diagnostic = "汇编目标或 CPU 不可用";
        return 1;
    }
    std::unique_ptr<llvm::MCAsmInfo> info(target->createMCAsmInfo(*registers, triple, settings));
    if (!info)
    {
        diagnostic = "汇编目标缺少语法信息";
        return 1;
    }
    llvm::MCContext context(triple, *info, *registers, *subtarget, &sources);
    std::unique_ptr<llvm::MCObjectFileInfo> objectInfo(target->createMCObjectFileInfo(context, true));
    context.setObjectFileInfo(objectInfo.get());
    context.setGenDwarfForAssembly(options.debug);
    context.setDwarfVersion(4);
    if (options.debug) context.setGenDwarfRootFile(input, sources.getMemoryBuffer(sources.getMainFileID())->getBuffer());
    std::error_code ec;
    llvm::raw_fd_ostream destination(output, ec, llvm::sys::fs::OF_None);
    if (ec)
    {
        diagnostic = ec.message();
        return 1;
    }
    std::unique_ptr<llvm::MCAsmBackend> backend(target->createMCAsmBackend(*subtarget, *registers, settings));
    std::unique_ptr<llvm::MCCodeEmitter> emitter(target->createMCCodeEmitter(*instructions, context));
    if (!backend || !emitter)
    {
        diagnostic = "汇编目标缺少对象编码器";
        return 1;
    }
    auto writer = backend->createObjectWriter(destination);
    std::unique_ptr<llvm::MCStreamer> stream(target->createMCObjectStreamer(triple, context, std::move(backend), std::move(writer), std::move(emitter), *subtarget));
    if (!stream)
    {
        diagnostic = "汇编目标缺少对象流";
        return 1;
    }
    std::unique_ptr<llvm::MCAsmParser> parser(llvm::createMCAsmParser(sources, context, *stream, *info));
    std::unique_ptr<llvm::MCTargetAsmParser> targetParser(target->createMCAsmParser(*subtarget, *parser, *instructions));
    if (!targetParser)
    {
        diagnostic = "汇编目标缺少解析器";
        return 1;
    }
    parser->setTargetParser(*targetParser);
    const bool failed = parser->Run(false) || context.hadError();
    destination.flush();
    if (destination.has_error())
    {
        diagnostic += "无法写入汇编对象\n";
        return 1;
    }
    return failed ? 1 : 0;
#else
    (void)input;
    (void)output;
    (void)options;
    diagnostic = "当前编译器没有内置 LLVM MC";
    return 1;
#endif
}
}  // namespace chali_assembler::builtin
