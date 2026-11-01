/** @file chali_archive_llvm.cpp
 * @brief 使用 LLVM ArchiveWriter 写确定性完整归档，不生成薄归档。
 */
#include "chali_linker.hpp"
#if CHALI_STATIC_LLVM
#include <llvm/Object/ArchiveWriter.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <vector>
#endif

namespace chali_linker::builtin
{
/**
 * @brief 写入标准符号索引并固定归档时间、UID 和 GID。
 * @param objects 已检查目标的对象路径。
 * @param output 工作区完整输出路径。
 * @param coff 选择 MS COFF 或 GNU 容器格式。
 * @param diagnostic 接收 LLVM 错误。
 * @return 成功为零，失败或当前变体未编入时为一。
 */
int archive(const Memory::Vector<Memory::String> &objects, const std::string &output, bool coff, std::string &diagnostic)
{
#if CHALI_STATIC_LLVM
    std::vector<llvm::NewArchiveMember> members;
    members.reserve(objects.size());
    for (const auto &path : objects)
    {
        auto member = llvm::NewArchiveMember::getFile(path.c_str(), true);
        if (!member)
        {
            diagnostic = llvm::toString(member.takeError());
            return 1;
        }
        member->MemberName = llvm::sys::path::filename(member->Buf->getBufferIdentifier());
        members.push_back(std::move(*member));
    }
    auto error = llvm::writeArchive(output, members, llvm::SymtabWritingMode::NormalSymtab,
                                    coff ? llvm::object::Archive::K_COFF : llvm::object::Archive::K_GNU, true, false);
    if (error)
    {
        diagnostic = llvm::toString(std::move(error));
        return 1;
    }
    return 0;
#else
    (void)objects;
    (void)output;
    (void)coff;
    diagnostic = "当前编译器没有内置 LLVM 归档器";
    return 1;
#endif
}
}  // namespace chali_linker::builtin
