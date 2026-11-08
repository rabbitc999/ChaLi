/** @file abi_file.cpp @brief 文件资源的 C 帧入口与私有文件流内核。 */
#include "chali_standard.h"
#include "allocator/c_heap_allocator.hpp"
#include "allocator/chali_allocator.hpp"
#include "chali_file.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace
{
/** @brief C 文件服务的所有权适配，不持有 C++ 流。 */
class FileDevice
{
    ChaliFile file_{};
    bool unreadableEof_ = false;
public:
    /** @brief 打开 C 文件。 @param path 路径。 @param mode 访问模式。 */
    FileDevice(const Memory::String &path, const Memory::String &mode)
    { chali_file_open(&file_, path.c_str(), mode.c_str()); }
    /** @brief 释放仍然打开的 C 文件句柄。 */
    ~FileDevice() { if (file_.stream) chali_file_close(&file_); }
    FileDevice(const FileDevice &) = delete;
    FileDevice &operator=(const FileDevice &) = delete;
    /** @brief 查询文件是否打开。 @return 打开为真。 */
    bool isOpen() const { return file_.stream != nullptr; }
    /** @brief 读取一行，去掉 LF 并保留 CR。 @param line 输出。 @param error 错误。 @return 成功为真。 */
    bool readLine(Memory::String &line, Memory::String &error)
    {
        line.clear();
        if (!file_.readable) { unreadableEof_ = true; return true; }
        unsigned char byte;
        int status;
        while ((status = chali_file_get(&file_, &byte)) > 0)
        {
            if (byte == '\n') return true;
            line.push_back(static_cast<char>(byte));
        }
        if (status >= 0) return true;
        error = "读取文件失败";
        return false;
    }
    /** @brief 分块读取字节。 @param count 上限。 @param bytes 输出。 @param error 错误。 @return 成功为真。 */
    bool readBytes(std::size_t count, Memory::String &bytes, Memory::String &error)
    {
        bytes.clear();
        if (!file_.readable) { error = "文件未以读取模式打开"; return false; }
        char block[4096];
        while (bytes.size() < count)
        {
            std::size_t read = 0;
            const bool ok = chali_file_read(&file_, block, std::min(count - bytes.size(), sizeof(block)), &read);
            if (read) bytes.append(block, read);
            if (!ok) { error = "读取文件失败"; return false; }
            if (chali_file_eof(&file_)) break;
        }
        return true;
    }
    /** @brief 写入并刷新。 @param text 输入。 @param error 错误。 @return 成功为真。 */
    bool write(const Memory::String &text, Memory::String &error)
    {
        if (!unreadableEof_ && chali_file_write(&file_, text.data(), text.size())) return true;
        error = "写入文件失败";
        return false;
    }
    /** @brief 刷新输出。 @param error 错误。 @return 成功为真。 */
    bool flush(Memory::String &error)
    {
        if (!unreadableEof_ && chali_file_flush(&file_)) return true;
        error = "刷新文件失败";
        return false;
    }
    /** @brief 调整位置。 @param offset 偏移。 @param origin 基准。 @param position 结果。 @param error 错误。 @return 成功为真。 */
    bool seek(std::int64_t offset, int origin, std::int64_t &position, Memory::String &error)
    {
        if (origin < 0 || origin > 2) { error = "未知的 seek 起点"; return false; }
        unreadableEof_ = false;
        if (chali_file_seek(&file_, offset, origin, &position)) return true;
        error = "文件定位失败";
        return false;
    }
    /** @brief 查询末尾标记。 @return 到达末尾为真。 */
    bool eof() const { return unreadableEof_ || chali_file_eof(&file_); }
    /** @brief 关闭并清空句柄。 */
    void close() { chali_file_close(&file_); }
    /** @brief 检查关闭结果。 @param error 错误。 @return 成功为真。 */
    bool closeChecked(Memory::String &error)
    {
        if (chali_file_close(&file_)) return true;
        error = "文件关闭失败";
        return false;
    }
};

/** @brief 独立拥有的文件流、路径及最近操作状态；文件不可复制。 */
struct FileRecord
{
    Memory::CHeapUniquePtr<FileDevice> file;
    Memory::String path, error;
    bool succeeded = true;
};
constexpr const char *fileType = "file";
/** @brief 销毁文件记录；私有流析构自动关闭仍打开的文件。
 * @param record 拥有的文件记录。
 */
void CHALI_ABI_CALL destroyFile(void *record) { Memory::destroyCHeap(static_cast<FileRecord *>(record)); }
/** @brief 复制文本实参，保留显式长度和零字节。
 * @param frame 当前帧。
 * @param index 实参下标。
 * @param output 接收独立内容。
 * @return 读取成功为真。
 */
bool textArgument(const ChaliAbiFrame *frame, int64_t index, Memory::String &output)
{
    int64_t handle = 0;
    const unsigned char *data = nullptr;
    size_t length = 0;
    if (!chali_abi_argument_integer(frame, index, &handle) || !chali_abi_bytes(frame, handle, &data, &length)) return false;
    output.assign(data ? reinterpret_cast<const char *>(data) : "", length);
    return true;
}
/** @brief 固定文件操作。 */
enum class Operation
{
    Open,
    TryOpen,
    Valid,
    Read,
    Write,
    Eof,
    Close,
    Drop,
    TryRead,
    TryReadLine,
    TryReadBytes,
    TryWrite,
    TryWriteBytes,
    TryFlush,
    TrySeek,
    Succeeded,
    LastError,
    TryClose
};
/** @brief 在 C 帧内执行文件操作，私有对象不跨越公开 ABI。
 * @tparam op 编译期操作。
 * @tparam reference 首实参是否为保存文件句柄的语言引用槽。
 * @param frame 实参和返回帧。
 */
template <Operation op, bool reference = false>
void CHALI_ABI_CALL fileEntry(ChaliAbiFrame *frame)
try
{
    if constexpr (op == Operation::Open || op == Operation::TryOpen)
    {
        Memory::String path, mode;
        if (!textArgument(frame, 0, path) || !textArgument(frame, 1, mode)) return;
        auto record = Memory::makeCHeapUnique<FileRecord>();
        record->file = Memory::makeCHeapUnique<FileDevice>(path, mode);
        if (!record->file->isOpen())
        {
            if constexpr (op == Operation::Open)
                chali_abi_fail(frame, ("打不开文件：" + path + "（mode=" + mode + "）").c_str());
            chali_abi_return_integer(frame, 0);
            return;
        }
        record->path = std::move(path);
        const auto handle = chali_abi_record_make(frame, fileType, record.get(), destroyFile);
        if (handle) record.release();
        chali_abi_return_integer(frame, handle);
        return;
    }
    int64_t handle = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    if constexpr (reference)
    {
        unsigned char bytes[8];
        if (!chali_abi_read(frame, handle, bytes, sizeof(bytes)))
        {
            chali_abi_fail(frame, "胶水对象引用指向的句柄槽无效");
            return;
        }
        uint64_t bits = 0;
        for (unsigned index = 0; index < 8; ++index) bits |= uint64_t(bytes[index]) << (8 * index);
        handle = int64_t(bits);
    }
    if constexpr (op == Operation::Drop)
    {
        if (!chali_abi_record_release(frame, handle)) chali_abi_fail(frame, "[CHL1205] drop 的对象已经释放或句柄无效");
        chali_abi_return_integer(frame, 0);
        return;
    }
    auto *record = static_cast<FileRecord *>(chali_abi_record_probe(frame, handle, fileType));
    if constexpr (op == Operation::Valid)
    {
        chali_abi_return_integer(frame, record && record->file && record->file->isOpen());
        return;
    }
    if constexpr (op == Operation::Succeeded)
    {
        chali_abi_return_integer(frame, record && record->succeeded);
        return;
    }
    if constexpr (op == Operation::LastError)
    {
        const Memory::String error = record ? record->error : "无效的 file 句柄";
        chali_abi_return_integer(frame, chali_abi_string_bytes(frame, error.data(), error.size()));
        return;
    }
    if constexpr (op == Operation::TryClose)
    {
        if (record && record->file && record->file->isOpen())
        {
            record->error.clear();
            record->succeeded = record->file->closeChecked(record->error);
            if (!record->succeeded)
            {
                chali_abi_return_integer(frame, -1);
                return;
            }
        }
        chali_abi_return_integer(frame, 0);
        return;
    }
    if constexpr (op == Operation::Read || op == Operation::Write || op == Operation::Close || op == Operation::Eof)
    {
        const char *who = op == Operation::Read ? "fread" : op == Operation::Write ? "fwrite" :
                                                        op == Operation::Close     ? "fclose" :
                                                                                     "feof";
        if (!record)
        {
            chali_abi_fail(frame, (Memory::String(who) + " 用的 file 已经释放过了（句柄 " + Memory::toString(handle) + "）").c_str());
            return;
        }
        if (!record->file || !record->file->isOpen())
        {
            chali_abi_fail(frame, (Memory::String(who) + " 用到的文件已经关闭").c_str());
            return;
        }
        if constexpr (op == Operation::Eof) chali_abi_return_integer(frame, record->file->eof());
        if constexpr (op == Operation::Close)
        {
            record->file->close();
            chali_abi_return_integer(frame, 0);
        }
        if constexpr (op == Operation::Read)
        {
            Memory::String line, error;
            if (!record->file->readLine(line, error))
            {
                chali_abi_fail(frame, (error + "：" + record->path).c_str());
                return;
            }
            chali_abi_return_integer(frame, chali_abi_string_bytes(frame, line.data(), line.size()));
        }
        if constexpr (op == Operation::Write)
        {
            Memory::String text, error;
            if (!textArgument(frame, 1, text)) return;
            if (!record->file->write(text, error))
            {
                chali_abi_fail(frame, (error + "：" + record->path).c_str());
                return;
            }
            chali_abi_return_integer(frame, int64_t(text.size()));
        }
        return;
    }
    constexpr int64_t failed = op == Operation::TryWrite || op == Operation::TryWriteBytes || op == Operation::TryFlush || op == Operation::TrySeek ? -1 : 0;
    if (!record)
    {
        chali_abi_return_integer(frame, failed);
        return;
    }
    record->error.clear();
    record->succeeded = false;
    if (!record->file || !record->file->isOpen())
    {
        record->error = "文件已经关闭";
        chali_abi_return_integer(frame, failed);
        return;
    }
    if constexpr (op == Operation::TryRead || op == Operation::TryReadLine)
    {
        Memory::String line, error;
        if (!record->file->readLine(line, error))
        {
            record->error = error.empty() ? "读取文件失败" : error;
            chali_abi_return_integer(frame, 0);
            return;
        }
        if constexpr (op == Operation::TryReadLine)
        {
            if (line.find('\0') != Memory::String::npos)
            {
                record->error = "text contains a zero byte";
                chali_abi_return_integer(frame, 0);
                return;
            }
            if (!line.empty() && line.back() == '\r') line.pop_back();
        }
        record->succeeded = true;
        chali_abi_return_integer(frame, chali_abi_string_bytes(frame, line.data(), line.size()));
    }
    if constexpr (op == Operation::TryReadBytes)
    {
        int64_t count = 0;
        if (!chali_abi_argument_integer(frame, 1, &count)) return;
        if (count < 0 || uint64_t(count) > SIZE_MAX)
        {
            record->error = "非法字节读取数量";
            chali_abi_return_integer(frame, 0);
            return;
        }
        Memory::String bytes, error;
        if (!record->file->readBytes(size_t(count), bytes, error))
        {
            record->error = error.empty() ? "字节读取失败" : error;
            chali_abi_return_integer(frame, 0);
            return;
        }
        const auto list = chali_abi_list_from_bytes(frame, reinterpret_cast<const unsigned char *>(bytes.data()), bytes.size());
        if (list) record->succeeded = true;
        chali_abi_return_integer(frame, list);
    }
    if constexpr (op == Operation::TryWrite || op == Operation::TryWriteBytes)
    {
        Memory::String text, error;
        if constexpr (op == Operation::TryWrite)
        {
            if (!textArgument(frame, 1, text)) return;
        }
        else
        {
            int64_t list = 0;
            size_t length = 0;
            if (!chali_abi_argument_integer(frame, 1, &list) || !chali_abi_list_bytes(frame, list, nullptr, 0, &length)) return;
            Memory::Vector<unsigned char> bytes(length);
            if (!chali_abi_list_bytes(frame, list, bytes.data(), bytes.size(), &length)) return;
            text.assign(bytes.empty() ? "" : reinterpret_cast<const char *>(bytes.data()), bytes.size());
        }
        if (!record->file->write(text, error))
        {
            record->error = error.empty() ? (op == Operation::TryWrite ? "写入文件失败" : "字节写入失败") : error;
            chali_abi_return_integer(frame, -1);
            return;
        }
        record->succeeded = true;
        chali_abi_return_integer(frame, int64_t(text.size()));
    }
    if constexpr (op == Operation::TryFlush)
    {
        if (!record->file->flush(record->error))
        {
            if (record->error.empty()) record->error = "刷新文件失败";
            chali_abi_return_integer(frame, -1);
            return;
        }
        record->succeeded = true;
        chali_abi_return_integer(frame, 0);
    }
    if constexpr (op == Operation::TrySeek)
    {
        int64_t offset = 0, origin = 0, position = -1;
        if (!chali_abi_argument_integer(frame, 1, &offset) || !chali_abi_argument_integer(frame, 2, &origin)) return;
        if (!record->file->seek(offset, int(origin), position, record->error))
        {
            if (record->error.empty()) record->error = "文件定位失败";
            chali_abi_return_integer(frame, -1);
            return;
        }
        record->succeeded = true;
        chali_abi_return_integer(frame, position);
    }
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "文件宿主服务失败");
}
}  // namespace

/**
 * @brief 取得文件资源只读 C 入口表；文件流只存在于实现内部。
 * @param count 接收入口数量，可为空。
 * @return 生命周期覆盖进程的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_file_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "__fileTryReadLine", fileEntry<Operation::TryReadLine, true>, 1 },
        { "__std_file_close", fileEntry<Operation::Close, false>, 1 },
        { "__std_file_drop", fileEntry<Operation::Drop, false>, 1 },
        { "__std_file_eof", fileEntry<Operation::Eof, false>, 1 },
        { "__std_file_open", fileEntry<Operation::Open, false>, 2 },
        { "__std_file_read", fileEntry<Operation::Read, false>, 1 },
        { "__std_file_try_open", fileEntry<Operation::TryOpen, false>, 2 },
        { "__std_file_valid", fileEntry<Operation::Valid, false>, 1 },
        { "__std_file_write", fileEntry<Operation::Write, false>, 2 },
        { "std::io::__fileLastError", fileEntry<Operation::LastError, true>, 1 },
        { "std::io::__fileSucceeded", fileEntry<Operation::Succeeded, true>, 1 },
        { "std::io::__fileTryClose", fileEntry<Operation::TryClose, true>, 1 },
        { "std::io::__fileTryFlush", fileEntry<Operation::TryFlush, true>, 1 },
        { "std::io::__fileTryRead", fileEntry<Operation::TryRead, true>, 1 },
        { "std::io::__fileTryReadBytes", fileEntry<Operation::TryReadBytes, true>, 2 },
        { "std::io::__fileTryReadLine", fileEntry<Operation::TryReadLine, true>, 1 },
        { "std::io::__fileTrySeek", fileEntry<Operation::TrySeek, true>, 3 },
        { "std::io::__fileTryWrite", fileEntry<Operation::TryWrite, true>, 2 },
        { "std::io::__fileTryWriteBytes", fileEntry<Operation::TryWriteBytes, true>, 2 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}

#define CHALI_BUILD_BIND_MODULE file
#include "aot_bind_module.h"
