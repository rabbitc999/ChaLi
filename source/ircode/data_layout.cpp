/**
 * @file data_layout.cpp
 * @brief 独立的数据布局编码与读取，供应用宿主和编译器共用。
 */
#include "artifact_binary_detail.hpp"

namespace stackvm
{
using namespace runtimeDetail;
/**
 * @brief 解析文本或二进制布局，失败不修改输出。
 * @param bytes 布局数据。
 * @param rows 输出记录。
 * @return 是否成功。
 */
bool decodeDataLayout(std::string_view bytes, Memory::Vector<LayoutRow> &rows)
{
    Memory::Vector<LayoutRow> decoded;
    if (bytes.size() >= 4 && bytes.substr(0, 4) == "CHLY")
    {
        if (bytes.size() < 5 || bytes[4] != 1) return false;
        artifact_binary::Reader r{ bytes, 5 };
        std::uint64_t count = 0;
        if (!r.number(count) || count > (bytes.size() - r.at) / 3) return false;
        for (std::uint64_t i = 0; i < count; ++i)
        {
            LayoutRow row;
            std::uint64_t offset = 0, flags = 0, size = 0, stride = 0;
            if (!r.string(row.name) || row.name.empty() || !r.number(offset) || offset > SIZE_MAX || !r.number(flags) || flags > 3 || ((flags & 1) && (!r.number(size) || size > SIZE_MAX)) || ((flags & 2) && (!r.number(stride) || stride > SIZE_MAX))) return false;
            row.offset = offset;
            row.size = size;
            row.stride = stride;
            row.hasSize = flags & 1;
            row.hasStride = flags & 2;
            decoded.push_back(std::move(row));
        }
        if (r.at != bytes.size()) return false;
    }
    else
    {
        std::size_t lineAt = 0;
        while (lineAt < bytes.size())
        {
            const auto end = bytes.find('\n', lineAt);
            auto line = bytes.substr(lineAt, end == std::string_view::npos ? bytes.size() - lineAt : end - lineAt);
            lineAt = end == std::string_view::npos ? bytes.size() : end + 1;
            const auto next = [&line]() {
                const auto first = line.find_first_not_of(" \t\r\n\v\f");
                if (first == std::string_view::npos)
                {
                    line = {};
                    return std::string_view{};
                }
                line.remove_prefix(first);
                const auto last = line.find_first_of(" \t\r\n\v\f");
                const auto token = line.substr(0, last);
                line = last == std::string_view::npos ? std::string_view{} : line.substr(last);
                return token;
            };
            if (line.find_first_not_of(" \t\r\n") == std::string_view::npos) continue;
            const auto name = next();
            if (name.empty()) return false;
            LayoutRow row;
            row.name.assign(name.data(), name.size());
            bool hasOffset = false;
            for (auto field = next(); !field.empty(); field = next())
            {
                const auto eq = field.find('=');
                if (eq == std::string_view::npos) continue;
                const auto key = field.substr(0, eq);
                if (key != "offset" && key != "size" && key != "stride") continue;
                std::uint64_t value = 0;
                const auto part = field.substr(eq + 1);
                const auto parsed = std::from_chars(part.data(), part.data() + part.size(), value);
                if (parsed.ec != std::errc() || parsed.ptr != part.data() + part.size() || value > SIZE_MAX) return false;
                if (key == "offset")
                {
                    row.offset = value;
                    hasOffset = true;
                }
                if (key == "size")
                {
                    row.size = value;
                    row.hasSize = true;
                }
                if (key == "stride")
                {
                    row.stride = value;
                    row.hasStride = true;
                }
            }
            if (!hasOffset) return false;
            decoded.push_back(std::move(row));
        }
    }
    rows = std::move(decoded);
    return true;
}
}  // namespace stackvm
