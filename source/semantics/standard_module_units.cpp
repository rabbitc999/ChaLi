/* 随 chali_semantics.cpp 编译，复用已验证的单元读取器与当前解析池。 */
namespace
{
constexpr std::uint64_t kModuleUnitMagic = 0x31554c444f4d4843ull;

/**
 * @brief 将解析位置解析为标准库内嵌源码的原始文件和行。
 * @param line 合并输入中的一基行号。
 * @return 原始文件及行；无效位置返回空文件。
 */
std::pair<Memory::String, std::size_t> ModuleUnitOrigin(std::size_t line)
{
    if (!line || line > g_sourceFiles.size()) return {};
    return { g_sourceFiles[line - 1], line <= g_sourceLines.size() ? g_sourceLines[line - 1] : line };
}

/**
 * @brief 重定位独立模块中的节点、类型、名称及宏调用编号。
 * @param unit 尚未发布的模块解析状态。
 * @param positions 模块旧行号到当前输入行号的映射。
 * @param fragments 模块旧宏调用编号到当前原始调用编号的映射。
 * @return 编号预算及来源完整时返回 true。
 */
bool RelocateModuleUnit(ParsedStandardUnit &unit, const Memory::Map<std::size_t, std::size_t> &positions,
                        const Memory::Map<std::size_t, std::size_t> &fragments)
{
    const auto limit = static_cast<std::size_t>(std::numeric_limits<int>::max());
    if (unit.nodes.size() > limit - g_nodes.size() || unit.types.size() > limit - g_types.size()
        || unit.names.size() > limit - g_names.size()) return false;
    const auto nodes = static_cast<int>(g_nodes.size()), types = static_cast<int>(g_types.size()), names = static_cast<int>(g_names.size());
    const auto shift = [](int &id, int offset) { if (id >= 0) id += offset; };
    Memory::Set<std::size_t> nameLists;
    for (std::size_t index = 0; index < unit.nodes.size(); ++index)
    {
        const auto &node = unit.nodes[index];
        if ((node.kind == MNode::StructDecl || node.kind == MNode::InterfaceDecl
             || (node.kind == MNode::EnumDecl && node.kids.size() > 1)) && node.i >= 0)
            nameLists.insert(static_cast<std::size_t>(node.i));
        if (node.kind == MNode::List && (node.s == "#borrow" || node.s == "#static")) nameLists.insert(index);
    }
    for (std::size_t index = 0; index < unit.nodes.size(); ++index)
    {
        auto &node = unit.nodes[index];
        for (auto *line : { &node.sourceLine, &node.metadataLine, &node.macroDefinitionLine, &node.expansionLine, &node.deriveLine })
        {
            if (!*line) continue;
            const auto found = positions.find(*line);
            if (found == positions.end()) return false;
            *line = found->second;
        }
        shift(node.type, types);
        if (node.kind == MNode::Params) shift(node.type2, types);
        if (node.kind == MNode::Case && node.type2 > 0) node.type2 += nodes;
        shift(node.guard, nodes);
        shift(node.macroExpansion, nodes);
        for (auto &id : node.kids) shift(id, nameLists.count(index) ? names : nodes);
        for (auto &id : node.annotations) shift(id, nodes);
        for (auto &value : node.annotationValues) shift(value.type, types);
        bool nodeInteger = node.kind == MNode::Block || node.kind == MNode::Declare || node.kind == MNode::Field
                           || node.kind == MNode::NewPtr || node.kind == MNode::Case || node.kind == MNode::StructDecl
                           || node.kind == MNode::InterfaceDecl || (node.kind == MNode::EnumDecl && node.kids.size() > 1)
                           || (node.kind == MNode::Index && node.s2 == "__chali_bare_index");
        if (nodeInteger && node.i >= 0)
        {
            if (node.i > std::numeric_limits<int>::max() - nodes) return false;
            node.i += nodes;
        }
        if (node.kind == MNode::HostImplDecl || (node.kind == MNode::List && node.s == "#borrow"))
        {
            if (node.i < 0 || static_cast<std::size_t>(node.i) >= unit.names.size()) return false;
            node.i += names;
        }
        if (node.kind == MNode::MacroCall && node.i > 0)
        {
            const auto found = fragments.find(static_cast<std::size_t>(node.i - 1));
            if (found == fragments.end()) return false;
            node.i = static_cast<long long>(found->second) + 1;
        }
        if (node.kind == MNode::MacroArgument && node.s.compare(0, 15, "@!macrofragment") == 0)
        {
            const auto separator = node.s.find('_', 15);
            if (separator != Memory::String::npos)
            {
                std::size_t old = 0;
                for (std::size_t i = 15; i < separator; ++i)
                {
                    if (node.s[i] < '0' || node.s[i] > '9' || old > (std::numeric_limits<std::size_t>::max() - 9) / 10) return false;
                    old = old * 10 + static_cast<std::size_t>(node.s[i] - '0');
                }
                const auto found = fragments.find(old);
                if (found != fragments.end()) node.s = "@!macrofragment" + Memory::toString(found->second) + node.s.substr(separator);
            }
        }
    }
    for (auto &id : unit.top) shift(id, nodes);
    Memory::Map<int, Memory::String> strings;
    for (auto &entry : unit.stringSources) strings.emplace(entry.first + nodes, std::move(entry.second));
    unit.stringSources.swap(strings);
    return true;
}
}  // namespace

/**
 * @brief 写出仅包含当前标准模块的解析状态及可重定位来源，不保存用户映射表。
 * @param out 模块资产输出流。
 * @return 全部节点和宏调用均有合法内嵌来源时返回 true。
 */
bool M_WriteModuleUnit(std::ostream &out)
{
    std::ostringstream body(std::ios::binary);
    if (!M_WriteStandardUnit(body, g_nodes.size())) return false;
    Memory::Map<std::pair<Memory::String, std::size_t>, std::size_t> canonical;
    for (std::size_t i = 1; i <= g_sourceFiles.size(); ++i) canonical.emplace(ModuleUnitOrigin(i), i);
    Memory::Map<std::size_t, std::pair<Memory::String, std::size_t>> origins;
    Memory::Map<std::size_t, chali_macro::SourceInvocation> fragments;
    for (const auto &node : g_nodes)
    {
        for (const auto line : { node.sourceLine, node.metadataLine, node.macroDefinitionLine, node.expansionLine, node.deriveLine })
            if (line)
            {
                const auto origin = ModuleUnitOrigin(line);
                origins.emplace(canonical.at(origin), origin);
            }
        if (node.kind == MNode::MacroCall && node.i > 0)
        {
            const auto index = static_cast<std::size_t>(node.i - 1);
            fragments.emplace(index, chali_macro::sourceFragments.at(index));
        }
    }
    chali_incremental::writeNumber(out, kModuleUnitMagic);
    const auto bytes = body.str();
    chali_incremental::writeString(out, Memory::String(bytes.data(), bytes.size()));
    chali_incremental::writeNumber(out, origins.size());
    for (const auto &entry : origins)
    {
        if (entry.second.first.compare(0, 8, "<内建 ") != 0) return false;
        chali_incremental::writeNumber(out, entry.first);
        chali_incremental::writeString(out, entry.second.first);
        chali_incremental::writeNumber(out, entry.second.second);
    }
    chali_incremental::writeNumber(out, fragments.size());
    for (const auto &entry : fragments)
    {
        const auto origin = ModuleUnitOrigin(entry.second.line);
        if (origin.first.empty()) return false;
        chali_incremental::writeNumber(out, entry.first);
        chali_incremental::writeString(out, origin.first);
        chali_incremental::writeNumber(out, origin.second);
        chali_incremental::writeNumber(out, entry.second.column);
        chali_incremental::writeString(out, Memory::String(entry.second.name.data(), entry.second.name.size()));
    }
    return static_cast<bool>(out);
}

/**
 * @brief 验证独立标准模块并按当前源码和宏调用身份追加解析节点。
 * @param in 完整模块资产输入流。
 * @return 来源、编号及负载有效且成功发布时返回 true；失败保留当前状态。
 */
bool M_AppendModuleUnit(std::istream &in)
{
    std::uint64_t magic = 0, count = 0;
    Memory::String body;
    if (!chali_incremental::readNumber(in, magic) || magic != kModuleUnitMagic || !chali_incremental::readString(in, body)) return false;
    ParsedStandardUnit unit;
    std::istringstream unitInput(std::string(body.data(), body.size()), std::ios::binary);
    if (!ReadStandardUnitState(unitInput, unit)) return false;
    Memory::Map<std::pair<Memory::String, std::size_t>, std::size_t> current;
    for (std::size_t i = 1; i <= g_sourceFiles.size(); ++i) current.emplace(ModuleUnitOrigin(i), i);
    Memory::Map<std::size_t, std::size_t> positions, fragments;
    if (!chali_incremental::readNumber(in, count) || !UnitPayloadFits(in, count, 24)) return false;
    for (std::uint64_t i = 0; i < count; ++i)
    {
        std::size_t old = 0, row = 0;
        Memory::String file;
        if (!chali_incremental::readNumber(in, old) || !chali_incremental::readString(in, file) || !chali_incremental::readNumber(in, row)) return false;
        const auto found = current.find({ file, row });
        if (found == current.end() || !positions.emplace(old, found->second).second) return false;
    }
    if (!chali_incremental::readNumber(in, count) || !UnitPayloadFits(in, count, 40)) return false;
    for (std::uint64_t i = 0; i < count; ++i)
    {
        std::size_t old = 0, row = 0, column = 0;
        Memory::String file, name;
        if (!chali_incremental::readNumber(in, old) || !chali_incremental::readString(in, file)
            || !chali_incremental::readNumber(in, row) || !chali_incremental::readNumber(in, column) || !chali_incremental::readString(in, name)) return false;
        bool matched = false;
        for (std::size_t index = 0; index < chali_macro::sourceFragments.size(); ++index)
        {
            const auto &invocation = chali_macro::sourceFragments.at(index);
            if (invocation.column == column && invocation.name == std::string(name.data(), name.size())
                && ModuleUnitOrigin(invocation.line) == std::make_pair(file, row))
            {
                if (!fragments.emplace(old, index).second) return false;
                matched = true;
                break;
            }
        }
        if (!matched) return false;
    }
    if (in.peek() != std::char_traits<char>::eof() || !RelocateModuleUnit(unit, positions, fragments)) return false;
    /* 先准备完整的新状态。复制或分配失败不会留下半个模块。 */
    auto nodes = g_nodes;
    auto types = g_types;
    auto names = g_names;
    auto top = g_top;
    auto strings = g_stringSources;
    auto directives = chali_lifetime::g_directiveNodes;
    const auto offset = static_cast<int>(g_nodes.size());
    for (const auto &entry : unit.directiveNodes) directives.emplace(entry.first + offset, entry.second + offset);
    for (auto &node : unit.nodes) nodes.push_back(std::move(node));
    types.insert(types.end(), unit.types.begin(), unit.types.end());
    names.insert(names.end(), std::make_move_iterator(unit.names.begin()), std::make_move_iterator(unit.names.end()));
    top.insert(top.end(), unit.top.begin(), unit.top.end());
    strings.insert(std::make_move_iterator(unit.stringSources.begin()), std::make_move_iterator(unit.stringSources.end()));
    g_nodes.swap(nodes);
    g_types.swap(types);
    g_names.swap(names);
    g_top.swap(top);
    g_stringSources.swap(strings);
    chali_lifetime::g_directiveNodes.swap(directives);
    g_parseModule.swap(unit.parseModule);
    return true;
}
