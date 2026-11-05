/**
 * @file source_location.cpp
 * @brief 运行时源码位置与调试索引，不读取四元式文件。
 */
#include "runtime_internal.hpp"

namespace stackvm
{
using namespace runtimeDetail;
/**
 * @brief 将源码位置编码为可读文本。
 * @retval 生成或读取的结果文本。
 */
Memory::String SourceLocation::toString() const
{
    if (line > 0)
    {
        return file.empty() ? ("第 " + Memory::toString(line) + " 行") : (file + ":" + Memory::toString(line));
    }
    if (quadLine > 0u)
    {
        return (file.empty() ? Memory::String("四元式") : file) + ":" + Memory::toString(quadLine);
    }
    return function.empty() ? Memory::String("?") : function;
}

/**
 * @brief 追加一条记录；与上一条位置完全相同则跳过（省地方）。
 * @param instruction 指令下标；必须不小于已有的最后一条。
 * @param location 位置。
 */
void DebugInfo::add(std::size_t instruction, SourceLocation location)
{
    if (!location.known()) return;
    if (!entries.empty())
    {
        const auto &last = entries.back();
        // 同一指令下标重复登记（函数标签本身不产生指令，"标签"与"紧跟它的第一条语句"
        // 会落在同一条指令上）：后来者更精确，覆盖掉。这样**每个下标最多一条**。
        if (last.first == instruction)
        {
            entries.back().second = std::move(location);
            return;
        }
        // 位置没变（一条语句的几十条指令）就不用再记，表因此保持很小。
        if (last.second.function == location.function && last.second.file == location.file && last.second.line == location.line && last.second.quadLine == location.quadLine)
            return;
    }
    entries.emplace_back(instruction, std::move(location));
}

/**
 * @brief 取不超过 instruction 的最近一条记录。
 * @param instruction 指令下标。
 * @return 命中返回指针；表里没有更早的记录返回 nullptr。
 */
const SourceLocation *DebugInfo::lookup(std::size_t instruction) const
{
    if (entries.empty() || instruction < entries.front().first) return nullptr;
    std::size_t low = 0u;
    std::size_t high = entries.size();
    while (low + 1u < high)
    {
        const std::size_t middle = (low + high) / 2u;
        if (entries[middle].first <= instruction)
            low = middle;
        else
            high = middle;
    }
    return &entries[low].second;
}
}  // namespace stackvm
