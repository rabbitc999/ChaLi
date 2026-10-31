/** @file abi_file.cpp @brief 文件资源的 C 帧入口与私有文件流内核。 */
#include "chali_standard.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace
{
/** @brief 私有文件流内核；不作为宿主 ABI 暴露。 */
class FileDevice
{
public:
    /**
     * @brief 保存桌面宿主文件流及其打开模式。
     * @param path 目标文件或目录的路径。
     * @param mode 文件访问或工具运行模式。
     */
    FileDevice(const std::string &path, const std::string &mode)
    {
        std::ios::openmode flags = std::ios::binary;
        readable_ = mode.find('r') != std::string::npos || mode.find('+') != std::string::npos;
        writable_ = mode.find('w') != std::string::npos || mode.find('a') != std::string::npos || mode.find('+') != std::string::npos;
        if (readable_) flags |= std::ios::in;
        if (mode.find('w') != std::string::npos) flags |= std::ios::out | std::ios::trunc;
        if (mode.find('a') != std::string::npos) flags |= std::ios::out | std::ios::app;
        if (mode.find('+') != std::string::npos) flags |= std::ios::in | std::ios::out;
        try
        {
            stream_.open(std::filesystem::u8path(path), flags);
        } catch (const std::filesystem::filesystem_error &)
        {
        } catch (const std::range_error &) {}
    }

    /**
     * @brief 判断文件句柄是否处于打开状态。
     * @retval 文件句柄处于打开状态时为 true，否则为 false。
     */
    [[nodiscard]] bool isOpen() const { return stream_.is_open(); }

    /**
     * @brief 从文件读取一行并报告到达末尾或读取错误。
     * @param line 接收去掉 LF 的行文本；旧读取接口保留行尾 CR。
     * @param error 接收失败原因的错误信息。
     * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
     */
    bool readLine(std::string &line, std::string &error)
    {
        if (std::getline(stream_, line)) return true;
        if (stream_.bad())
        {
            error = "读取文件失败";
            return false;
        }
        stream_.clear(stream_.rdstate() & ~std::ios::failbit);
        line.clear();
        return true;
    }

    /**
     * @brief 读取原始字节，EOF 是成功的短读取。
     * @param count 上限。
     * @param bytes 结果。
     * @param error 错误。
     * @return 是否成功。
     */
    bool readBytes(std::size_t count, std::string &bytes, std::string &error)
    {
        bytes.clear();
        if (!readable_)
        {
            error = "文件未以读取模式打开";
            return false;
        }
        char block[4096];
        while (bytes.size() < count)
        {
            const auto size = std::min(count - bytes.size(), sizeof(block));
            stream_.read(block, static_cast<std::streamsize>(size));
            const auto read = stream_.gcount();
            if (read > 0) bytes.append(block, static_cast<std::size_t>(read));
            if (stream_.bad() || (stream_.fail() && !stream_.eof()))
            {
                error = "读取文件失败";
                return false;
            }
            if (stream_.eof())
            {
                stream_.clear(stream_.rdstate() & ~std::ios::failbit);
                break;
            }
        }
        return true;
    }

    /**
     * @brief 将给定文本写入文件。
     * @param text 待处理的文本。
     * @param error 接收失败原因的错误信息。
     * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
     */
    bool write(const std::string &text, std::string &error)
    {
        stream_ << text;
        if (!stream_)
        {
            error = "写入文件失败";
            return false;
        }
        stream_.flush();
        return true;
    }

    /**
     * @brief 刷新尚未提交的输出数据。
     * @param error 接收失败原因的错误信息。
     * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
     */
    bool flush(std::string &error)
    {
        stream_.flush();
        if (stream_) return true;
        error = "刷新文件失败";
        return false;
    }

    /**
     * @brief 按指定基准调整文件读写位置。
     * @param offset 相对于起点的字节偏移。
     * @param origin 定位基准：零为开头，一为当前位置，二为结尾。
     * @param position 接收定位后的文件字节位置。
     * @param error 接收失败原因的错误信息。
     * @retval 满足检查条件或操作完成时为 true，检查失败或无法完成时为 false。
     */
    bool seek(std::int64_t offset, int origin, std::int64_t &position,
              std::string &error)
    {
        std::ios::seekdir direction = std::ios::beg;
        if (origin == 1)
            direction = std::ios::cur;
        else if (origin == 2)
            direction = std::ios::end;
        else if (origin != 0)
        {
            error = "未知的 seek 起点";
            return false;
        }
        stream_.clear();
        if (readable_) stream_.seekg(static_cast<std::streamoff>(offset), direction);
        if (!stream_)
        {
            error = "文件定位失败";
            return false;
        }
        if (writable_) stream_.seekp(static_cast<std::streamoff>(offset), direction);
        if (!stream_)
        {
            error = "文件定位失败";
            return false;
        }
        const std::streampos readPosition = readable_ ? stream_.tellg() : std::streampos(-1);
        const std::streampos writePosition = writable_ ? stream_.tellp() : std::streampos(-1);
        const std::streampos selected = readPosition != std::streampos(-1) ? readPosition : writePosition;
        if (selected == std::streampos(-1))
        {
            error = "读取文件位置失败";
            return false;
        }
        position = static_cast<std::int64_t>(selected);
        return true;
    }

    /**
     * @brief 判断文件读取是否到达末尾。
     * @retval 文件读取到达末尾时为 true，否则为 false。
     */
    [[nodiscard]] bool eof() const { return stream_.eof(); }
    /**
     * @brief 关闭文件并释放宿主句柄。
     */
    void close() { stream_.close(); }
    /**
     * @brief 关闭桌面文件并检查缓冲写入或关闭错误。
     * @param error 接收失败原因。
     * @return 关闭成功时为 true。
     */
    bool closeChecked(std::string &error)
    {
        stream_.clear();
        stream_.close();
        if (stream_.fail())
        {
            error = "文件关闭失败";
            return false;
        }
        return true;
    }

private:
    std::fstream stream_;
    bool readable_ = false;
    bool writable_ = false;
};

/** @brief 独立拥有的文件流、路径及最近操作状态；文件不可复制。 */
struct FileRecord
{
    std::unique_ptr<FileDevice> file;
    std::string path, error;
    bool succeeded = true;
};
constexpr const char *fileType = "file";
/** @brief 销毁文件记录；私有流析构自动关闭仍打开的文件。
 * @param record 拥有的文件记录。
 */
void CHALI_ABI_CALL destroyFile(void *record) { delete static_cast<FileRecord *>(record); }
/** @brief 复制文本实参，保留显式长度和零字节。
 * @param frame 当前帧。
 * @param index 实参下标。
 * @param output 接收独立内容。
 * @return 读取成功为真。
 */
bool textArgument(const ChaliAbiFrame *frame, int64_t index, std::string &output)
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
        std::string path, mode;
        if (!textArgument(frame, 0, path) || !textArgument(frame, 1, mode)) return;
        auto record = std::make_unique<FileRecord>();
        record->file = std::make_unique<FileDevice>(path, mode);
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
        const std::string error = record ? record->error : "无效的 file 句柄";
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
            chali_abi_fail(frame, (std::string(who) + " 用的 file 已经释放过了（句柄 " + std::to_string(handle) + "）").c_str());
            return;
        }
        if (!record->file || !record->file->isOpen())
        {
            chali_abi_fail(frame, (std::string(who) + " 用到的文件已经关闭").c_str());
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
            std::string line, error;
            if (!record->file->readLine(line, error))
            {
                chali_abi_fail(frame, (error + "：" + record->path).c_str());
                return;
            }
            chali_abi_return_integer(frame, chali_abi_string_bytes(frame, line.data(), line.size()));
        }
        if constexpr (op == Operation::Write)
        {
            std::string text, error;
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
        std::string line, error;
        if (!record->file->readLine(line, error))
        {
            record->error = error.empty() ? "读取文件失败" : error;
            chali_abi_return_integer(frame, 0);
            return;
        }
        if constexpr (op == Operation::TryReadLine)
        {
            if (line.find('\0') != std::string::npos)
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
        std::string bytes, error;
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
        std::string text, error;
        if constexpr (op == Operation::TryWrite)
        {
            if (!textArgument(frame, 1, text)) return;
        }
        else
        {
            int64_t list = 0;
            size_t length = 0;
            if (!chali_abi_argument_integer(frame, 1, &list) || !chali_abi_list_bytes(frame, list, nullptr, 0, &length)) return;
            std::vector<unsigned char> bytes(length);
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
