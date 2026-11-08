/** @file abi_regex.cpp @brief 正则表达式的 C 帧入口及不可变模式内核。 */
#include "chali_standard.h"
#include "allocator/c_heap_allocator.hpp"
#include "allocator/chali_allocator.hpp"
#include <memory>
#include <regex>
#include <string>

namespace
{
/** @brief 独立拥有的模式引用；共享不可变编译结果，不共享匹配状态。 */
struct Pattern
{ std::shared_ptr<const std::regex> compiled; };
constexpr const char *patternName = "std::regex::Regex";
/** @brief 销毁一份模式引用。
 * @param value 拥有的记录。
 */
void CHALI_ABI_CALL destroyPattern(void *value) { Memory::destroyCHeap(static_cast<Pattern *>(value)); }
/** @brief 创建独立模式引用，失败不抛出异常。
 * @param value 只读记录。
 * @return 新引用或空指针。
 */
void *CHALI_ABI_CALL clonePattern(const void *value)
try
{
    return Memory::makeCHeapUnique<Pattern>(*static_cast<const Pattern *>(value)).release();
} catch (...) { return nullptr; }
/** @brief 完整创建引用后交给语言运行时。
 * @param frame 当前帧。
 * @param compiled 不可变编译模式，可为空。
 * @return 拥有型句柄，失败为零。
 */
int64_t makePattern(const ChaliAbiFrame *frame, std::shared_ptr<const std::regex> compiled = {})
{
    auto value = Memory::makeCHeapUnique<Pattern>();
    value->compiled = std::move(compiled);
    auto handle = chali_abi_record_make_clonable(frame, patternName, value.get(), destroyPattern, clonePattern);
    if (handle) value.release();
    return handle;
}
/** @brief 复制文本实参，避免保留会被后续分配失效的借用视图。
 * @param frame 当前帧。
 * @param index 实参下标。
 * @param text 接收独立输入。
 * @return 读取成功为真。
 */
bool textArgument(const ChaliAbiFrame *frame, int64_t index, Memory::String &text)
{
    int64_t handle = 0;
    const unsigned char *data = nullptr;
    size_t length = 0;
    if (!chali_abi_argument_integer(frame, index, &handle) || !chali_abi_bytes(frame, handle, &data, &length)) return false;
    text.assign(data ? reinterpret_cast<const char *>(data) : "", length);
    return true;
}
/** @brief 编译期选择的正则操作。 */
enum class Operation
{
    New,
    Copy,
    Drop,
    Valid,
    Compile,
    FullMatch,
    Contains,
    Find,
    Capture,
    ReplaceFirst,
    ReplaceAll
};
/** @brief 执行固定正则操作，通过 C 错误通道返回异常。
 * @tparam op 编译期操作。
 * @param frame 实参和返回帧。
 */
template <Operation op>
void CHALI_ABI_CALL entry(ChaliAbiFrame *frame)
try
{
    if constexpr (op == Operation::New)
    {
        chali_abi_return_integer(frame, makePattern(frame));
        return;
    }
    if constexpr (op == Operation::Compile)
    {
        Memory::String text;
        if (!textArgument(frame, 0, text)) return;
        std::shared_ptr<const std::regex> compiled;
        try
        {
            compiled = std::allocate_shared<const std::regex>(Memory::CHeapAllocator<const std::regex>(), text, std::regex::ECMAScript);
        } catch (const std::regex_error &) {}
        chali_abi_return_integer(frame, makePattern(frame, std::move(compiled)));
        return;
    }
    int64_t handle = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    if constexpr (op == Operation::Copy)
    {
        chali_abi_return_integer(frame, chali_abi_record_clone(frame, handle, patternName, clonePattern));
        return;
    }
    if constexpr (op == Operation::Drop)
    {
        if (!chali_abi_record_release(frame, handle)) chali_abi_fail(frame, "[CHL1205] drop 的对象已经释放或句柄无效");
        chali_abi_return_integer(frame, 0);
        return;
    }
    Memory::String text, replacement;
    int64_t groupSlot = 0;
    if constexpr (op >= Operation::FullMatch)
        if (!textArgument(frame, 1, text)) return;
    if constexpr (op == Operation::Capture)
        if (!chali_abi_argument_integer(frame, 2, &groupSlot)) return;
    if constexpr (op == Operation::ReplaceFirst || op == Operation::ReplaceAll)
        if (!textArgument(frame, 2, replacement)) return;
    auto *pattern = static_cast<Pattern *>(chali_abi_record_probe(frame, handle, patternName));
    if constexpr (op == Operation::Valid)
    {
        chali_abi_return_integer(frame, pattern && bool(pattern->compiled));
        return;
    }
    if (!pattern || !pattern->compiled)
    {
        chali_abi_fail(frame, "正则表达式无效；请先调用 std::regex::Regex::compile");
        return;
    }
    const auto compiled = pattern->compiled;
    if constexpr (op == Operation::FullMatch) chali_abi_return_integer(frame, std::regex_match(text, *compiled));
    if constexpr (op == Operation::Contains) chali_abi_return_integer(frame, std::regex_search(text, *compiled));
    if constexpr (op == Operation::Find || op == Operation::Capture)
    {
        using Match = std::sub_match<Memory::String::const_iterator>;
        std::match_results<Memory::String::const_iterator, Memory::Allocator<Match>> match;
        const bool found = std::regex_search(text, match, *compiled);
        if constexpr (op == Operation::Find)
            chali_abi_return_integer(frame, found ? int64_t(match.position()) : -1);
        else
        {
            // 旧入口的宿主参数为 int，保留槽缩窄后的组编号行为。
            const int group = static_cast<int>(groupSlot);
            const Memory::String captured = found && group >= 0 && size_t(group) < match.size() ? Memory::String(match[size_t(group)].first, match[size_t(group)].second) : Memory::String();
            chali_abi_return_integer(frame, chali_abi_string_bytes(frame, captured.data(), captured.size()));
        }
    }
    if constexpr (op == Operation::ReplaceFirst || op == Operation::ReplaceAll)
    {
        const auto flags = op == Operation::ReplaceFirst ? std::regex_constants::format_first_only : std::regex_constants::match_default;
        const auto output = std::regex_replace(text, *compiled, replacement, flags);
        chali_abi_return_integer(frame, chali_abi_string_bytes(frame, output.data(), output.size()));
    }
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "正则表达式内核失败");
}
}  // namespace

/** @brief 取得正则表达式只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 存活至进程结束的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_regex_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::regex::__regexCapture", entry<Operation::Capture>, 3 },
        { "std::regex::__regexCompileHandle", entry<Operation::Compile>, 1 },
        { "std::regex::__regexContains", entry<Operation::Contains>, 2 },
        { "std::regex::__regexCopy", entry<Operation::Copy>, 1 },
        { "std::regex::__regexDrop", entry<Operation::Drop>, 1 },
        { "std::regex::__regexFindByteIndex", entry<Operation::Find>, 2 },
        { "std::regex::__regexFullMatch", entry<Operation::FullMatch>, 2 },
        { "std::regex::__regexNew", entry<Operation::New>, 0 },
        { "std::regex::__regexReplaceAll", entry<Operation::ReplaceAll>, 3 },
        { "std::regex::__regexReplaceFirst", entry<Operation::ReplaceFirst>, 3 },
        { "std::regex::__regexValid", entry<Operation::Valid>, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}

#ifndef GLUE_NO_STD_REGEX
#define CHALI_BUILD_BIND_MODULE regex
#include "aot_bind_module.h"
#endif
