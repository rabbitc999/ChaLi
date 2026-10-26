/**
 * @file backend_profile.cpp
 * @brief 可选 AOT 所有权操作计数，独立于正式性能计时。
 */
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <map>
#include <mutex>
#include <string>

namespace
{
/**
 * @brief 单个四元式位置的执行次数及请求字节数，不代表成功分配的存活字节。
 */
struct Counter
{
    std::uint64_t calls = 0;
    std::uint64_t bytes = 0;
};

/**
 * @brief 将 UTF-8 字节串写成合法 JSON 字符串。
 * @param out 目标流。
 * @param text 原始字符串。
 */
void writeString(std::ostream &out, const std::string &text)
{
    out << '"';
    for (const unsigned char byte : text)
    {
        if (byte == '"' || byte == '\\')
            out << '\\' << static_cast<char>(byte);
        else if (byte < 32)
            out << "\\u00" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(byte) << std::dec;
        else
            out << static_cast<char>(byte);
    }
    out << '"';
}

/**
 * @brief 收集插桩调用，正常进程退出时写出独立 JSON；不访问语言堆。
 */
struct Profile
{
    std::mutex mutex;
    std::map<std::string, Counter> counters;
    std::string output;

    /**
     * @brief 从运行环境读取输出位置；未设置时不收集数据。
     */
    Profile()
    {
        const auto *path = std::getenv("CHALI_BACKEND_PROFILE_OUTPUT");
        if (path) output = path;
    }

    /**
     * @brief 正常退出时保存全部线程累计数据；失败不改变被测程序结果。
     */
    ~Profile()
    {
        if (output.empty()) return;
        try
        {
            std::ofstream out(output, std::ios::binary);
            out << "{\"schema\":1,\"scope\":\"executed AOT quad ownership sites; allocation requests, not heap residency\",\"events\":[";
            bool first = true;
            for (const auto &entry : counters)
            {
                if (!first) out << ',';
                first = false;
                out << "\n{\"site\":";
                writeString(out, entry.first);
                out << ",\"count\":" << entry.second.calls << ",\"requested_bytes\":" << entry.second.bytes << '}';
            }
            out << "\n]}\n";
        } catch (...) {}
    }
};
}  // namespace

/**
 * @brief 记录一次插桩所有权操作；仅插桩 AOT 模块调用本入口。
 * @param site 包含操作种类、函数、源位置和四元式编号的常量字符串。
 * @param bytes 已知的非负请求字节数；零表示不适用或未知。
 */
extern "C" void chali_backend_profile_event(const char *site, std::int64_t bytes)
{
    static Profile profile;
    if (profile.output.empty()) return;
    std::lock_guard<std::mutex> lock(profile.mutex);
    auto &counter = profile.counters[site ? site : "unknown"];
    ++counter.calls;
    if (bytes > 0) counter.bytes += static_cast<std::uint64_t>(bytes);
}
