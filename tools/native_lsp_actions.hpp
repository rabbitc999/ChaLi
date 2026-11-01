/**
 * @brief 用独立且不驻留缓存的编译检查验证编辑候选。
 * @param u 根文档 URI。
 * @param candidate 替换后的完整源码。
 * @return 只有编译器成功返回且没有错误时为真。
 */
bool acceptsAction(const std::string &u, const std::string &candidate)
{
    auto snapshot = documents;
    snapshot[u] = std::make_shared<const Document>(candidate, documents.at(u)->second);
    Server probe(*this, std::move(snapshot), roots, snapshotRevision);
    probe.ephemeralAnalysis = true;
    probe.sourceSnapshot = sourceSnapshot;
    probe.sourceSnapshot.erase(u);
    probe.requestCancelled = requestCancelled;
    const auto report = probe.checkReport(u, [] { return false; }, 8);
    if (!get(report, "diagnostics").asArray()) return false;
    for (const auto &diagnostic : array(get(report, "diagnostics")))
        if (str(diagnostic, "severity") == "error") return false;
    return revision->load() == snapshotRevision && !(requestCancelled && requestCancelled->load());
}
/**
 * @brief 提供警告修复及由实际编译器核验的有限接口／匹配补全。
 * @param u 根文档 URI。
 * @param text 当前原始源码。
 * @param params 编辑范围和操作类别。
 * @param records 当前可见声明。
 * @return 有版本约束的工作区编辑；不支持的结构不提供候选。
 */
J codeActions(const std::string &u, const std::string &text, const J &params, const std::vector<Record> &records)
{
    auto warnings = warningActions(u, text, params);
    A actions = array(warnings);
    const auto only = array(get(get(params, "context"), "only"));
    if (!only.empty() && std::none_of(only.begin(), only.end(), [](const J &kind) { return str(kind) == "quickfix" || str(kind).empty(); })) return actions;
    const auto tokens = lex(text);
    const int first = integer(get(get(get(params, "range"), "start"), "line"));
    const int last = integer(get(get(get(params, "range"), "end"), "line"));
    size_t attempts = 0;
    for (size_t i = 0; i + 2 < tokens.size() && attempts < 2; ++i)
    {
        const bool structure = tokens[i].word == "struct";
        if (!structure && tokens[i].word != "match") continue;
        if (inMacroTemplate(text, tokens[i].a)) continue;
        size_t open = i + 1;
        if (!structure && tokens[open].word == "(")
            open = groupEnd(tokens, open, "(", ")");
        else if (!structure)
            continue;
        if (structure)
            while (open < tokens.size() && tokens[open].word != "{" && tokens[open].word != ";") ++open;
        if (open >= tokens.size() || tokens[open].word != "{") continue;
        const auto end = groupEnd(tokens, open, "{", "}");
        if (end <= open + 1 || end > tokens.size() || tokens[end - 1].word != "}") continue;
        const int startLine = integer(get(get(range(text, tokens[i].a, tokens[i].b), "start"), "line"));
        const int endLine = integer(get(get(range(text, tokens[end - 1].a, tokens[end - 1].b), "end"), "line"));
        if (last < startLine || first > endLine) continue;
        std::string addition, title;
        if (structure)
        {
            // 首版只处理直接列出的非泛型接口；继承、约束与所有权仍由编译器裁决。
            if (tokens[i + 2].word != ":") continue;
            std::set<std::string> implemented;
            for (const auto &record : records)
                if (record.source == u && record.owner == tokens[i + 1].word) implemented.insert(record.name);
            for (size_t name = i + 3; name < open; ++name)
            {
                if (tokens[name].word == ",") continue;
                if (!identifier(tokens[name].word))
                {
                    addition.clear();
                    break;
                }
                const Record *interface = nullptr;
                for (const auto &record : records)
                    if (record.kind == "interface" && record.name == tokens[name].word)
                    {
                        if (interface)
                        {
                            interface = nullptr;
                            break;
                        }
                        interface = &record;
                    }
                if (!interface)
                {
                    addition.clear();
                    break;
                }
                for (const auto &method : records)
                    if (method.module == interface->module && method.source == interface->source && method.owner == interface->name && method.signature.find('(') != std::string::npos && !implemented.count(method.name))
                    {
                        addition += "\n    " + method.signature + " { panic(\"TODO: implement interface method\"); }\n";
                        implemented.insert(method.name);
                    }
            }
            title = "补齐接口方法（TODO panic）";
        }
        else
        {
            bool wildcard = false;
            for (size_t n = open + 1; n < end - 1; ++n)
                if (tokens[n].word == "_") wildcard = true;
            if (wildcard) continue;
            const auto base = checkReport(u, [] { return false; });
            bool hasError = false;
            for (const auto &diagnostic : array(get(base, "diagnostics")))
                if (str(diagnostic, "severity") == "error") hasError = true;
            if (!hasError) continue;
            addition = "\n    _ => { panic(\"TODO: unhandled match\"); }\n";
            title = "添加 match 兜底（TODO panic）";
        }
        if (addition.empty()) continue;
        ++attempts;
        const auto at = tokens[end - 1].a;
        auto candidate = text;
        candidate.insert(at, addition);
        if (!acceptsAction(u, candidate)) continue;
        const auto edit = object({ { "range", range(text, at, at) }, { "newText", addition } });
        const auto change = object({ { "textDocument", object({ { "uri", u }, { "version", documents.at(u)->second } }) }, { "edits", A{ edit } } });
        actions.push_back(object({ { "title", title }, { "kind", "quickfix" }, { "edit", object({ { "documentChanges", A{ change } } }) } }));
    }
    return actions;
}
