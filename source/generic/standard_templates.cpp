/* 随 chali_generic.cpp 编译；二进制记录只包含未实例化的声明。 */
namespace
{
constexpr std::uint64_t kTemplateUnitMagic = 0x3154504d544843ull;

/**
 * @brief 取得模板目录项允许保存的唯一内嵌来源。
 * @param module 资产目录中的模块名称。
 * @return 编译器内嵌文件身份。
 */
Memory::String TemplateUnitFile(const Memory::String &module)
{
    return module == "std::__range" ? Memory::String("<内建 range>") : "<内建 " + module + ">";
}

/**
 * @brief 检查同一内建辅助文件中的声明是否属于允许的语言模块。
 * @param asset 目录模块名称。
 * @param owner 声明自己的语言模块。
 * @return 目录与定义模块身份一致时返回 true。
 */
bool TemplateUnitOwner(const Memory::String &asset, const Memory::String &owner)
{
    return asset == "std::__range" ? owner == "std::lang" || owner == "std::algorithms" : owner == asset;
}

/**
 * @brief 写出模板原始行位置，不保存用户文件或进程状态。
 * @param out 输出流。
 * @param origin 原始位置。
 */
void WriteTemplateOrigin(std::ostream &out, const preprocess::LineOrigin &origin)
{
    chali_incremental::writeString(out, origin.file);
    chali_incremental::writeNumber(out, origin.line);
}

/**
 * @brief 验证模板位置属于预期内嵌文件且行号有界。
 * @param in 输入流。
 * @param file 允许的文件身份。
 * @param origin 成功时接收来源。
 * @return 字段完整有效时返回 true。
 */
bool ReadTemplateOrigin(std::istream &in, const Memory::String &file, preprocess::LineOrigin &origin)
{
    return chali_incremental::readString(in, origin.file) && origin.file == file
           && chali_incremental::readNumber(in, origin.line) && origin.line > 0 && origin.line <= (1u << 26);
}

/**
 * @brief 按剩余负载限制模板数组长度，避免坏计数触发超额分配。
 * @param in 可定位输入流。
 * @param count 接收元素数量。
 * @param minimum 每个元素至少占用的字节。
 * @return 数量有界且预算足够时返回 true。
 */
bool ReadTemplateCount(std::istream &in, std::uint64_t &count, std::size_t minimum)
{
    if (!chali_incremental::readNumber(in, count) || count > 100000) return false;
    const auto position = in.tellg();
    if (position == std::streampos(-1)) return false;
    in.seekg(0, std::ios::end);
    const auto end = in.tellg();
    in.seekg(position);
    return in && end >= position && count <= static_cast<std::uint64_t>(end - position) / minimum;
}
}  // namespace

/**
 * @brief 导出源自指定标准模块的模板元数据，不包含实例、接口实现表或用户环境。
 * @param out 输出流。
 * @param module 标准模块资产名称。
 * @return 来源一致且写入成功时返回 true。
 */
bool writeTemplateUnit(std::ostream &out, const Memory::String &module)
{
    const auto file = TemplateUnitFile(module);
    Memory::Vector<const Generic *> records;
    for (const auto &entry : session.generics)
        if (entry.second.origin.file == file && !entry.second.sourceTemplate.empty()) records.push_back(&entry.second);
    chali_incremental::writeNumber(out, kTemplateUnitMagic);
    chali_incremental::writeNumber(out, 1);
    chali_incremental::writeString(out, module);
    chali_incremental::writeNumber(out, records.size());
    for (const auto *entry : records)
    {
        const auto &item = *entry;
        if (item.intrinsic || !TemplateUnitOwner(module, item.module)) return false;
        for (const auto *field : { &item.sourceName, &item.module, &item.declaration, &item.sourceTemplate,
                                  &item.requiredQualifier, &item.derivation, &item.operatorSwitches }) chali_incremental::writeString(out, *field);
        chali_incremental::writeNumber(out, item.nameOffset);
        WriteTemplateOrigin(out, item.origin);
        chali_incremental::writeNumber(out, item.params.size());
        for (const auto &parameter : item.params)
        {
            for (const auto *field : { &parameter.name, &parameter.constraint, &parameter.fallback, &parameter.constantType })
                chali_incremental::writeString(out, *field);
            chali_incremental::writeNumber(out, (parameter.hasDefault ? 1u : 0u) | (parameter.constant ? 2u : 0u));
        }
        chali_incremental::writeNumber(out, item.declarationOrigins.size());
        for (std::size_t index = 0; index < item.declarationOrigins.size(); ++index)
        {
            auto origin = item.declarationOrigins[index];
            if (origin.file != file)
            {
                /* 收集器会把声明后的空行包括在最后一个来源中；不固化下一单元的文件。 */
                std::size_t at = 0, line = 0;
                while (line < index && at < item.declaration.size()) if (item.declaration[at++] == '\n') ++line;
                const auto end = item.declaration.find('\n', at);
                const auto part = item.declaration.substr(at, end == Memory::String::npos ? end : end - at);
                if (part.find_first_not_of(" \t\r") != Memory::String::npos) return false;
                origin = item.origin;
            }
            WriteTemplateOrigin(out, origin);
        }
        chali_incremental::writeNumber(out, item.derivationOrigins.size());
        for (const auto &origin : item.derivationOrigins)
        {
            if (origin.file != file) return false;
            WriteTemplateOrigin(out, origin);
        }
    }
    return static_cast<bool>(out);
}

/**
 * @brief 在局部拥有容器中验证模板记录，失败不修改调用方模板表。
 * @param in 完整模板二进制输入。
 * @param module 期望的目录模块名称。
 * @param records 成功时接收未绑定当前实例的模板。
 * @return 格式、数量、来源和名称偏移有效时返回 true。
 */
bool readTemplateUnit(std::istream &in, const Memory::String &module, Memory::Vector<Generic> &records)
{
    std::uint64_t magic = 0, version = 0, count = 0;
    Memory::String name;
    if (!chali_incremental::readNumber(in, magic) || magic != kTemplateUnitMagic
        || !chali_incremental::readNumber(in, version) || version != 1
        || !chali_incremental::readString(in, name) || name != module || !ReadTemplateCount(in, count, 96)) return false;
    const auto file = TemplateUnitFile(module);
    Memory::Vector<Generic> restored;
    Memory::Set<std::pair<Memory::String, Memory::String>> names;
    for (std::uint64_t index = 0; index < count; ++index)
    {
        Generic item;
        for (auto *field : { &item.sourceName, &item.module, &item.declaration, &item.sourceTemplate,
                            &item.requiredQualifier, &item.derivation, &item.operatorSwitches })
            if (!chali_incremental::readString(in, *field)) return false;
        item.name = item.sourceName;
        if (!TemplateUnitOwner(module, item.module) || item.sourceName.empty()
            || !names.emplace(item.module, item.sourceName).second || !wordAt(item.sourceTemplate, 0, "__chali_template")
            || !chali_incremental::readNumber(in, item.nameOffset) || item.nameOffset > item.declaration.size()
            || !wordAt(item.declaration, item.nameOffset, item.sourceName)
            || !ReadTemplateOrigin(in, file, item.origin)) return false;
        std::uint64_t parameters = 0;
        if (!ReadTemplateCount(in, parameters, 40)) return false;
        Memory::Set<Memory::String> parameterNames;
        for (std::uint64_t i = 0; i < parameters; ++i)
        {
            Parameter parameter;
            std::uint64_t flags = 0;
            for (auto *field : { &parameter.name, &parameter.constraint, &parameter.fallback, &parameter.constantType })
                if (!chali_incremental::readString(in, *field)) return false;
            if (parameter.name.empty() || !parameterNames.insert(parameter.name).second
                || !chali_incremental::readNumber(in, flags) || flags > 3) return false;
            parameter.hasDefault = (flags & 1) != 0;
            parameter.constant = (flags & 2) != 0;
            if ((parameter.hasDefault && parameter.fallback.empty()) || (parameter.constant && parameter.constantType.empty())) return false;
            item.params.push_back(std::move(parameter));
        }
        for (auto *origins : { &item.declarationOrigins, &item.derivationOrigins })
        {
            std::uint64_t positions = 0;
            if (!ReadTemplateCount(in, positions, 16)) return false;
            for (std::uint64_t i = 0; i < positions; ++i)
            {
                preprocess::LineOrigin origin;
                if (!ReadTemplateOrigin(in, file, origin)) return false;
                origins->push_back(std::move(origin));
            }
        }
        if (item.declarationOrigins.size() != static_cast<std::size_t>(std::count(item.declaration.begin(), item.declaration.end(), '\n')) + 1) return false;
        restored.push_back(std::move(item));
    }
    if (in.peek() != std::char_traits<char>::eof()) return false;
    records.swap(restored);
    return true;
}
