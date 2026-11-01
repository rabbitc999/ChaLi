/* 随 chali_precompiled.cpp 编译，复用版本化包读取器和只读目录。 */
namespace
{
/** @brief 模板匹配词素；内部宏编号独立保存，普通字面量保持原字节。 */
struct TemplateToken
{
    std::string_view text;
    std::size_t reference = 0;
    bool marker = false, valid = true;
};

/**
 * @brief 读取模板匹配词素，字符串和注释中的伪内部编号不参与重绑定。
 * @param source 需要扫描的源码。
 * @param at 读取位置，成功后移到词素之后。
 * @return 原字节视图或内部宏引用词素。
 */
TemplateToken NextTemplateToken(std::string_view source, std::size_t &at)
{
    const auto begin = at;
    if (at >= source.size()) return { {}, 0, false, false };
    if (source[at] == '"' || source[at] == char(39))
    {
        const auto quote = source[at++];
        while (at < source.size())
        {
            const auto ch = source[at++];
            if (ch == char(92) && at < source.size()) ++at;
            else if (ch == quote) break;
        }
        return { source.substr(begin, at - begin) };
    }
    if (source.substr(at, 2) == "//" || source.substr(at, 2) == "/*")
    {
        const auto line = source.substr(at, 2) == "//";
        auto end = line ? source.find('\n', at) : source.find("*/", at + 2);
        if (end == std::string_view::npos) end = source.size();
        else if (!line) end += 2;
        at = end;
        return { source.substr(begin, at - begin) };
    }
    const auto prefix = source.substr(at, 15) == "@!macrofragment" ? std::size_t(15) :
                        source.substr(at, 2) == "@!" ? std::size_t(2) : std::size_t(0);
    if (prefix && at + prefix < source.size() && source[at + prefix] >= '0' && source[at + prefix] <= '9')
    {
        at += prefix;
        std::size_t reference = 0;
        while (at < source.size() && source[at] >= '0' && source[at] <= '9')
        {
            const auto digit = static_cast<std::size_t>(source[at++] - '0');
            if (reference > (std::numeric_limits<std::size_t>::max() - digit) / 10) return { {}, 0, false, false };
            reference = reference * 10 + digit;
        }
        return { source.substr(begin, prefix), reference, true };
    }
    ++at;
    return { source.substr(begin, 1) };
}

/**
 * @brief 比较完整归一模板并建立当前会话的宏引用映射。
 * @param pattern 构建时的未实例化声明。
 * @param source 当前完整源码。
 * @param begin 当前模板起点。
 * @param end 命中时接收结束位置。
 * @param references 接收旧编号到当前编号的映射。
 * @return 除内部编号外所有字节一致时返回 true。
 */
bool MatchTemplatePattern(const Memory::String &pattern, const Memory::String &source, std::size_t begin,
                          std::size_t &end, Memory::Map<std::size_t, std::size_t> &references)
{
    const std::string_view saved(pattern.data(), pattern.size()), current(source.data(), source.size());
    std::size_t left = 0, right = begin;
    while (left < saved.size())
    {
        const auto a = NextTemplateToken(saved, left), b = NextTemplateToken(current, right);
        if (!a.valid || !b.valid || a.marker != b.marker || a.text != b.text) return false;
        if (a.marker)
        {
            const auto inserted = references.emplace(a.reference, b.reference);
            if (!inserted.second && inserted.first->second != b.reference) return false;
        }
    }
    end = right;
    return true;
}

/**
 * @brief 将模板代码中的内部引用换成当前编号，同时更新声明名称偏移。
 * @param source 要重绑定的模板代码。
 * @param references 已验证的宏引用映射。
 * @param nameOffset 声明名称偏移，普通文本字段可传零。
 * @return 每个内部引用均有对应值时返回 true。
 */
bool RebindTemplateReferences(Memory::String &source, const Memory::Map<std::size_t, std::size_t> &references, std::size_t &nameOffset)
{
    const std::string_view view(source.data(), source.size());
    Memory::String result;
    std::ptrdiff_t delta = 0;
    for (std::size_t at = 0; at < view.size();)
    {
        const auto begin = at;
        const auto token = NextTemplateToken(view, at);
        if (!token.valid) return false;
        if (!token.marker) result.append(view.data() + begin, at - begin);
        else
        {
            const auto found = references.find(token.reference);
            if (found == references.end()) return false;
            const auto number = Memory::toString(found->second);
            result.append(token.text.data(), token.text.size());
            result += number;
            if (at <= nameOffset) delta += static_cast<std::ptrdiff_t>(token.text.size() + number.size()) - static_cast<std::ptrdiff_t>(at - begin);
        }
    }
    if (nameOffset > static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max())
        || static_cast<std::ptrdiff_t>(nameOffset) + delta < 0) return false;
    nameOffset = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(nameOffset) + delta);
    source.swap(result);
    return true;
}

/**
 * @brief 首次使用时验证并恢复一个标准模块的只读模板记录。
 * @param module 目录模块名称。
 * @param error 资产损坏时接收原因。
 * @return 只读记录；不存在或损坏时返回空引用。
 */
std::shared_ptr<const Memory::Vector<chali_generic::Generic>> StandardTemplateRecords(const Memory::String &module, Memory::String &error)
{
    static std::mutex mutex;
    static std::map<std::string, std::shared_ptr<const Memory::Vector<chali_generic::Generic>>> cache;
    std::lock_guard<std::mutex> lock(mutex);
    const std::string key(module.data(), module.size());
    const auto found = cache.find(key);
    if (found != cache.end()) return found->second;
    for (const auto &entry : kStandardTemplates)
    {
        if (!entry.name || module != entry.name) continue;
        std::string decoded;
        if (!loadPackage(entry.bytes, entry.size, entry.checksum, module, error, false, &decoded)) return {};
        auto records = std::make_shared<Memory::Vector<chali_generic::Generic>>();
        std::istringstream input(decoded, std::ios::binary);
        if (!chali_generic::readTemplateUnit(input, module, *records))
        {
            error = CHALI_TEXT("内嵌标准库二进制内容损坏或与编译器不兼容");
            return {};
        }
        cache.emplace(key, records);
        return records;
    }
    return {};
}
}  // namespace

/**
 * @brief 写出指定标准模块的未实例化模板包。
 * @param out 包输出流。
 * @param module 标准模块目录名称。
 * @return 成功时返回 true。
 */
bool writeTemplates(std::ostream &out, const Memory::String &module)
{
    return writePackage(out, module, 0, false, true);
}

/**
 * @brief 用只读二进制模板恢复声明，名称冲突与具体实例仍由当前收集器处理。
 * @param module 当前语言模块。
 * @param origin 当前模板原始来源。
 * @param source 完整归一源码。
 * @param begin 模板标记起点。
 * @param end 命中时接收末尾位置。
 * @param item 命中时接收已重绑定的模板副本。
 * @param error 资产损坏时接收诊断。
 * @return 1 为命中，0 为源码回退，-1 为资产错误。
 */
int loadTemplate(const Memory::String &module, const preprocess::LineOrigin &origin, const Memory::String &source,
                 std::size_t begin, std::size_t &end, chali_generic::Generic &item, Memory::String &error)
{
    error.clear();
    const auto key = origin.file == "<内建 range>" ? Memory::String("std::__range") : module;
    if (origin.file != (key == "std::__range" ? Memory::String("<内建 range>") : "<内建 " + key + ">")) return 0;
    const auto records = StandardTemplateRecords(key, error);
    if (!records) return error.empty() ? 0 : -1;
    for (const auto &record : *records)
    {
        if (record.module != module || record.origin.file != origin.file || record.origin.line != origin.line) continue;
        Memory::Map<std::size_t, std::size_t> references;
        std::size_t matchedEnd = 0;
        if (!MatchTemplatePattern(record.sourceTemplate, source, begin, matchedEnd, references)) continue;
        auto restored = record;
        if (!RebindTemplateReferences(restored.declaration, references, restored.nameOffset)) continue;
        bool valid = true;
        for (auto *field : { &restored.requiredQualifier, &restored.derivation, &restored.operatorSwitches })
        {
            std::size_t ignored = 0;
            if (!RebindTemplateReferences(*field, references, ignored)) valid = false;
        }
        for (auto &parameter : restored.params)
            for (auto *field : { &parameter.constraint, &parameter.fallback, &parameter.constantType })
            {
                std::size_t ignored = 0;
                if (!RebindTemplateReferences(*field, references, ignored)) valid = false;
            }
        if (!valid || restored.declaration.compare(restored.nameOffset, restored.sourceName.size(), restored.sourceName) != 0) continue;
        restored.sourceTemplate = source.substr(begin, matchedEnd - begin);
        restored.origin = origin;
        item = std::move(restored);
        end = matchedEnd;
        return 1;
    }
    return 0;
}
