/**
 * @brief 根据词法项配对跨行花括号，忽略字符串与注释中的括号。
 * @param source 当前未保存文档的源码快照。
 * @return 按起始行排序的 LSP 折叠区间；未闭合块延伸到文件末尾。
 */
J foldingRanges(const std::string &source)
{
    A result;
    std::vector<int> stack;
    size_t offset = 0;
    int line = 0;
    const auto append = [&](int start, int end) {
        if (end > start)
            result.push_back(object({ { "startLine", start }, { "endLine", end } }));
    };
    for (const auto &token : lex(source, false, true))
    {
        while (offset < token.a)
            if (source[offset++] == '\n') ++line;
        if (token.kind != 0) continue;
        if (token.word == "{") stack.push_back(line);
        else if (token.word == "}" && !stack.empty())
        {
            append(stack.back(), line - 1);
            stack.pop_back();
        }
    }
    while (offset < source.size())
        if (source[offset++] == '\n') ++line;
    for (const auto start : stack) append(start, line);
    std::sort(result.begin(), result.end(), [](const J &a, const J &b) {
        const auto first = integer(get(a, "startLine")), second = integer(get(b, "startLine"));
        return first != second ? first < second : integer(get(a, "endLine")) < integer(get(b, "endLine"));
    });
    result.erase(std::unique(result.begin(), result.end(), [](const J &a, const J &b) {
        return integer(get(a, "startLine")) == integer(get(b, "startLine")) &&
               integer(get(a, "endLine")) == integer(get(b, "endLine"));
    }), result.end());
    return result;
}
