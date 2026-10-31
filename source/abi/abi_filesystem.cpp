/** @file abi_filesystem.cpp @brief 文件系统与 UTC 时钟的 C 帧入口。 */
#include "chali_standard.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

namespace
{
/** @brief 复制完整路径实参，不保留运行时字节借用。
 * @param frame 当前帧。
 * @param index 实参下标。
 * @param path 接收独立 UTF-8 路径。
 * @return 读取成功为真。
 */
bool readPath(const ChaliAbiFrame *frame, int64_t index, std::string &path)
{
    int64_t handle = 0;
    const unsigned char *data = nullptr;
    size_t length = 0;
    if (!chali_abi_argument_integer(frame, index, &handle) || !chali_abi_bytes(frame, handle, &data, &length)) return false;
    path.assign(data ? reinterpret_cast<const char *>(data) : "", length);
    return true;
}
/** @brief 校验 IoReply 的槽偏移，不对逻辑地址直接解引用。
 * @param frame 当前帧。
 * @param reply 结果结构体的逻辑地址。
 * @param slot 固定槽编号。
 * @param address 接收无溢出的逻辑地址。
 * @return 地址可表示为真，实际范围由 C 读写服务检查。
 */
bool slotAddress(const ChaliAbiFrame *frame, int64_t reply, size_t slot, int64_t &address)
{
    const auto offset = int64_t(slot * 8);
    if (reply <= 0 || reply > INT64_MAX - offset)
    {
        chali_abi_fail(frame, "胶水层收到无效的 IoReply 引用");
        return false;
    }
    address = reply + offset;
    return true;
}
/** @brief 将固定宽度槽写入调用方的结果结构体。
 * @param frame 当前帧。
 * @param reply 结构体逻辑地址。
 * @param slot 槽编号。
 * @param value 原生槽值，与原宿主 ABI 相同。
 * @return 写入成功为真。
 */
bool writeSlot(const ChaliAbiFrame *frame, int64_t reply, size_t slot, int64_t value)
{
    int64_t address = 0;
    return slotAddress(frame, reply, slot, address) && chali_abi_write(frame, address, &value, sizeof(value));
}
/** @brief 写入可恢复的文件系统错误；落槽失败时回收新字符串。
 * @param frame 当前帧。
 * @param reply 调用方结果结构体。
 * @param message 操作系统或语义错误。
 */
void failReply(const ChaliAbiFrame *frame, int64_t reply, const std::string &message)
{
    const auto text = chali_abi_string(frame, message.data(), message.size());
    if (text && !writeSlot(frame, reply, 0, text)) chali_abi_string_release(frame, text);
}
/** @brief 编译期选择的文件系统操作。 */
enum class Operation
{
    List,
    Info,
    Create,
    Rename
};
/** @brief 执行宿主文件系统服务，仅通过 C 逻辑内存和集合服务访问语言数据。
 * @tparam op 编译期固定操作。
 * @param frame 路径、IoReply 引用及返回帧。
 */
template <Operation op>
void CHALI_ABI_CALL filesystemEntry(ChaliAbiFrame *frame)
try
{
    namespace fs = std::filesystem;
    std::string path, destination;
    int64_t reply = 0;
    if (!readPath(frame, 0, path)) return;
    if constexpr (op == Operation::Rename)
        if (!readPath(frame, 1, destination)) return;
    if (!chali_abi_argument_integer(frame, op == Operation::Rename ? 2 : 1, &reply)) return;
    std::error_code error;
    if constexpr (op == Operation::List)
    {
        fs::directory_iterator iterator(fs::u8path(path), error), end;
        if (error)
        {
            failReply(frame, reply, error.message());
            return;
        }
        std::vector<std::string> paths;
        for (; iterator != end; iterator.increment(error))
        {
            if (error)
            {
                failReply(frame, reply, error.message());
                return;
            }
            paths.push_back(iterator->path().generic_u8string());
        }
        if (error)
        {
            failReply(frame, reply, error.message());
            return;
        }
        std::sort(paths.begin(), paths.end());
        int64_t address = 0, list = 0;
        if (!slotAddress(frame, reply, 2, address) || !chali_abi_read(frame, address, &list, sizeof(list))) return;
        for (const auto &item : paths)
            if (!chali_abi_list_append_text(frame, list, item.data(), item.size())) return;
    }
    if constexpr (op == Operation::Info)
    {
        const auto nativePath = fs::u8path(path);
        const auto status = fs::status(nativePath, error);
        if (error || !fs::exists(status))
        {
            failReply(frame, reply, error ? error.message() : "path does not exist");
            return;
        }
        const bool regular = fs::is_regular_file(status);
        const auto size = regular ? fs::file_size(nativePath, error) : 0;
        if (error || size > uint64_t(INT64_MAX))
        {
            failReply(frame, reply, "cannot represent file size");
            return;
        }
        const auto modified = fs::last_write_time(nativePath, error);
        if (error)
        {
            failReply(frame, reply, error.message());
            return;
        }
        const auto system = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            modified - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        if (!writeSlot(frame, reply, 4, int64_t(size)) || !writeSlot(frame, reply, 5, regular) || !writeSlot(frame, reply, 6, fs::is_directory(status)) || !writeSlot(frame, reply, 7, std::chrono::duration_cast<std::chrono::seconds>(system.time_since_epoch()).count())) return;
    }
    if constexpr (op == Operation::Create) fs::create_directories(fs::u8path(path), error);
    if constexpr (op == Operation::Rename) fs::rename(fs::u8path(path), fs::u8path(destination), error);
    if (error)
    {
        failReply(frame, reply, error.message());
        return;
    }
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "文件系统宿主服务失败");
}
/** @brief 返回当前系统 UTC 时间的 Unix 秒数。
 * @param frame 无实参的返回帧。
 */
void CHALI_ABI_CALL utcNow(ChaliAbiFrame *frame)
{
    chali_abi_return_integer(frame, std::chrono::duration_cast<std::chrono::seconds>(
                                        std::chrono::system_clock::now().time_since_epoch())
                                        .count());
}
}  // namespace

/** @brief 取得文件系统与 UTC 时钟的只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 生命周期覆盖进程的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_filesystem_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::datetime::__utcNow", utcNow, 0 },
        { "std::io::__createDirectories", filesystemEntry<Operation::Create>, 2 },
        { "std::io::__fileInfo", filesystemEntry<Operation::Info>, 2 },
        { "std::io::__listDirectory", filesystemEntry<Operation::List>, 2 },
        { "std::io::__renamePath", filesystemEntry<Operation::Rename>, 3 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
