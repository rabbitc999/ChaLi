/**
 * @brief 手册字符串的大小写归一化。
 * @param text 文本。
 * @return ASCII 名称归一化文本，保留 UTF-8。
 */
std::string manLower(std::string text)
{
    for (auto &c : text) c = char(std::tolower(static_cast<unsigned char>(c)));
    return text;
}
/**
 * @brief 按工具链语言选择显示语言标识。
 * @return zh、en 或 ja。
 */
std::string manLanguage()
{
    const auto language = magel_i18n::language();
    return language == magel_i18n::Language::Zh ? "zh" : language == magel_i18n::Language::Ja ? "ja" :
                                                                                                "en";
}
/**
 * @brief 手册能力域名称，翻译来自工具链消息目录。
 * @param name 能力域。
 * @return 显示名称。
 */
std::string manGroup(const std::string &name)
{
    if (name == "core") return MAGEL_TEXT("核心与类型");
    if (name == "collections") return MAGEL_TEXT("容器与迭代");
    if (name == "text") return MAGEL_TEXT("文本与数据");
    if (name == "math") return MAGEL_TEXT("数学与数值");
    if (name == "io") return MAGEL_TEXT("输入输出与集成");
    if (name == "concurrency") return MAGEL_TEXT("并发与事件");
    return MAGEL_TEXT("内存与系统");
}
/**
 * @brief 计算候选符号名称距离。
 * @param a 目标。
 * @param b 候选。
 * @return 编辑距离。
 */
size_t manDistance(const std::string &a, const std::string &b)
{
    std::vector<size_t> previous(b.size() + 1), current(b.size() + 1);
    for (size_t at = 0; at <= b.size(); ++at) previous[at] = at;
    for (size_t i = 1; i <= a.size(); ++i)
    {
        current[0] = i;
        for (size_t j = 1; j <= b.size(); ++j) current[j] = std::min({ current[j - 1] + 1, previous[j] + 1, previous[j - 1] + size_t(a[i - 1] != b[j - 1]) });
        previous.swap(current);
    }
    return previous.back();
}
/**
 * @brief 终端颜色与 less 分页；重定向时只写普通 UTF-8 文本。
 */
class ManTerminal
{
    bool terminal_ = false, color_ = false;
#ifdef _WIN32
    HANDLE handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode_ = 0;
#endif
public:
    /**
     * @brief 按终端能力启用 ANSI；Windows 使用 WinAPI 虚拟终端模式。
     */
    ManTerminal()
    {
#ifdef _WIN32
        terminal_ = GetConsoleMode(handle_, &mode_) != 0;
        color_ = terminal_ && SetConsoleMode(handle_, mode_ | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
        terminal_ = isatty(STDOUT_FILENO) != 0;
        color_ = terminal_;
#endif
        if (std::getenv("NO_COLOR")) color_ = false;
    }
    /**
     * @brief 恢复 Windows 控制台模式。
     */
    ~ManTerminal()
    {
#ifdef _WIN32
        if (terminal_) SetConsoleMode(handle_, mode_);
#endif
    }
    /**
     * @brief 输出有颜色的标题。
     * @param text 标题。
     * @return 终端能力适配后的文本。
     */
    std::string title(const std::string &text) const { return color_ ? "\x1b[1;36m" + text + "\x1b[0m" : text; }
    /**
     * @brief 输出文本；长文且存在 less 时分页。
     * @param text 完整手册。
     */
    void print(const std::string &text) const
    {
        bool pager = false;
        if (terminal_ && std::count(text.begin(), text.end(), '\n') > 24)
        {
            const char *environment = std::getenv("PATH");
            std::istringstream paths(environment ? environment : "");
            std::string path;
#ifdef _WIN32
            const char separator = ';';
            const char *program = "less.exe";
#else
            const char separator = ':';
            const char *program = "less";
#endif
            while (std::getline(paths, path, separator))
            {
                std::error_code ec;
                if (fs::is_regular_file(fs::u8path(path) / program, ec))
                {
                    pager = true;
                    break;
                }
            }
        }
        if (pager)
        {
#ifdef _WIN32
            auto *pipe = _popen("less -R", "w");
#else
            auto *pipe = popen("less -R", "w");
#endif
            if (pipe)
            {
                std::fwrite(text.data(), 1, text.size(), pipe);
#ifdef _WIN32
                _pclose(pipe);
#else
                pclose(pipe);
#endif
                return;
            }
        }
        std::cout << text;
    }
};
/**
 * @brief 命令行手册，查询同一份 LSP 内嵌数据。
 * @param argc 参数数。
 * @param argv 参数。
 * @param compiler 工具链路径。
 * @return 正常为零，查找或示例失败为一，参数错误为二。
 */
int man(int argc, char **argv, const fs::path &compiler)
{
    bool machine = false, exact = false, runExample = false, update = false;
    std::string target, query;
    for (int at = 2; at < argc; ++at)
    {
        const std::string arg = argv[at];
        if (arg == "--json")
            machine = true;
        else if (arg == "--exact")
            exact = true;
        else if (arg == "--run")
            runExample = true;
        else if (arg == "--update")
            update = true;
        else if (arg == "--search")
        {
            if (at + 1 >= argc)
            {
                std::cerr << MAGEL_TEXT("--search 后需要关键词") << '\n';
                return 2;
            }
            query = argv[++at];
        }
        else if (arg == "--help" || arg == "-h")
        {
            std::cout << MAGEL_TEXT("用法：magel man [目标] [--search 关键词] [--exact] [--json] [--run] [--lang zh|en|ja] [--update]\n");
            return 0;
        }
        else if (starts(arg, "--") || !target.empty())
        {
            std::cerr << MAGEL_TEXT("手册参数无效：") << arg << '\n';
            return 2;
        }
        else
            target = arg;
    }
    if ((!query.empty() && !target.empty()) || (update && (runExample || !query.empty() || !target.empty())))
    {
        std::cerr << MAGEL_TEXT("手册参数不能组合使用") << '\n';
        return 2;
    }
    if (update)
    {
        fs::path repository;
        for (auto start : { fs::current_path(), compiler.parent_path() })
            for (auto path = start; !path.empty();)
            {
                if (fs::is_regular_file(path / "tools" / "library_reference.py"))
                {
                    repository = path;
                    break;
                }
                const auto parent = path.parent_path();
                if (parent == path) break;
                path = parent;
            }
        if (repository.empty())
        {
            std::cerr << MAGEL_TEXT("--update 需要 Magel 源码仓库和 Python 构建环境") << '\n';
            return 1;
        }
        const char *python = std::getenv("MAGEL_PYTHON");
        const auto result = execute({ python ? python : "python", "tools/library_reference.py" }, repository, "", 120);
        if (machine)
            std::cout << object({ { "schemaVersion", 1 }, { "kind", "update" }, { "language", manLanguage() }, { "exitCode", result.code }, { "stdout", result.output }, { "stderr", result.error }, { "diagnostics", A{} } }).dump() << '\n';
        else
            std::cout << result.output << result.error << (result.code == 0 ? MAGEL_TEXT("手册数据已生成；重新构建后进入可执行文件。\n") : "");
        return result.code;
    }
    const auto data = parse(magel_development_tool("tools/magel_stdlib_api.json"));
    const auto &reference = get(data, "reference"), &modules = get(reference, "modules"), &examples = get(reference, "examples");
    A catalog = array(get(data, "functions"));
    for (auto &symbol : catalog)
        if (!symbol.find("kind")) symbol["kind"] = "function";
    for (const auto &symbol : array(get(data, "symbols"))) catalog.push_back(symbol);
    const auto qualified = [](const J &symbol) { return str(symbol, "module") + "::" + str(symbol, "name"); };
    const auto expanded = [&](const J &symbol) {J result=symbol;result["qualifiedName"]=qualified(symbol);A programs;
        for(const auto &id:array(get(symbol,"examples")))if(const auto *example=examples.find(str(id)))programs.push_back(*example);
        result["examples"]=programs;return result; };
    J result = object({ { "schemaVersion", 1 }, { "language", manLanguage() }, { "sourceLanguage", "zh" }, { "target", target }, { "diagnostics", A{} } });
    A selected, selectedModules, programIds;
    std::string kind;
    int status = 0;
    if (!query.empty())
    {
        kind = "search";
        result["query"] = query;
        const auto matches = [&](const std::string &text) { return exact ? text == query : manLower(text).find(manLower(query)) != std::string::npos; };
        if (const auto *entries = modules.asObject())
            for (const auto &entry : entries->members())
                if (matches(std::string(entry.key)) || matches(str(entry.value, "title")) || matches(str(entry.value, "documentation"))) selectedModules.push_back(entry.value);
        for (const auto &symbol : catalog)
        {
            bool match = matches(qualified(symbol)) || matches(tail(str(symbol, "name"))) || matches(str(symbol, "module")) || matches(str(symbol, "documentation")) || matches(str(symbol, "comment"));
            for (const auto &param : array(get(symbol, "parameters"))) match = match || matches(str(param, "name")) || matches(str(param, "documentation"));
            for (const auto &id : array(get(symbol, "examples")))
                if (const auto *example = examples.find(str(id))) match = match || matches(str(*example, "code"));
            if (match) selected.push_back(expanded(symbol));
        }
    }
    else if (target.empty())
    {
        kind = "modules";
        if (const auto *entries = modules.asObject())
            for (const auto &entry : entries->members()) selectedModules.push_back(entry.value);
    }
    else
    {
        if (!starts(target, "std") && modules.find("std::" + target)) target = "std::" + target;
        if (const auto *module = modules.find(target))
        {
            kind = "module";
            selectedModules.push_back(*module);
            programIds = array(get(*module, "examples"));
            for (const auto &symbol : catalog)
                if (str(symbol, "module") == target) selected.push_back(expanded(symbol));
        }
        else
        {
            kind = "symbol";
            for (const auto &symbol : catalog)
                if (qualified(symbol) == target)
                {
                    selected.push_back(expanded(symbol));
                    for (const auto &id : array(get(symbol, "examples"))) programIds.push_back(id);
                }
            if (!selected.empty() && (str(selected[0], "kind") == "struct" || str(selected[0], "kind") == "enum" || str(selected[0], "kind") == "interface"))
                for (const auto &symbol : catalog)
                    if (starts(qualified(symbol), target + "::")) selected.push_back(expanded(symbol));
        }
        if (selected.empty() && selectedModules.empty())
        {
            status = 1;
            std::vector<std::pair<size_t, std::string>> candidates;
            const auto separator = target.rfind("::");
            const auto parent = separator == target.npos ? "" : target.substr(0, separator);
            std::set<std::string> names;
            if (const auto *entries = modules.asObject())
                for (const auto &entry : entries->members()) names.insert(std::string(entry.key));
            for (const auto &symbol : catalog) names.insert(qualified(symbol));
            for (const auto &name : names) candidates.push_back({ manDistance(manLower(tail(target)), manLower(tail(name))) + (parent.empty() || starts(name, parent + "::") ? 0 : 100), name });
            std::sort(candidates.begin(), candidates.end());
            A suggestions;
            Memory::Vector<Memory::String> notes;
            for (size_t at = 0; at < std::min<size_t>(8, candidates.size()); ++at)
            {
                suggestions.push_back(candidates[at].second);
                notes.push_back(candidates[at].second.c_str());
            }
            result["candidates"] = suggestions;
            result["diagnostics"] = A{ parse(magel_diagnostics::toolDiagnostic("MGL1701", Memory::String(MAGEL_TEXT("未找到手册目标：")) + target.c_str(), notes).c_str()) };
        }
    }
    result["kind"] = kind;
    result["target"] = target;
    result["modules"] = selectedModules;
    result["symbols"] = selected;
    if (kind == "modules")
    {
        A groups;
        for (const auto name : { "core", "collections", "text", "math", "io", "concurrency", "system" })
        {
            A entries;
            for (const auto &module : selectedModules)
                if (str(module, "capability") == name) entries.push_back(module);
            groups.push_back(object({ { "name", name }, { "title", manGroup(name) }, { "modules", entries } }));
        }
        result["groups"] = groups;
    }
    if (runExample && !status)
    {
        if (programIds.empty())
        {
            status = 1;
            result["diagnostics"] = A{ parse(magel_diagnostics::toolDiagnostic("MGL1702", MAGEL_TEXT("该目标没有可运行示例")).c_str()) };
        }
        else
        {
            const auto *program = examples.find(str(programIds[0]));
            if (!program) throw std::runtime_error("内嵌示例引用不存在");
            Temporary temporary;
            const auto source = temporary.path / "example.mgl";
            write(source, str(*program, "code"));
            std::vector<std::string> command{ compiler.u8string(), "run", source.u8string(), "--out", (temporary.path / "example").u8string(), "--lang", manLanguage() };
            for (const auto &flag : array(get(*program, "flags"))) command.push_back(str(flag));
            if (!str(*program, "input").empty())
            {
                command.push_back("--input");
                command.push_back(str(*program, "input"));
            }
            if (!array(get(*program, "arguments")).empty())
            {
                command.push_back("--");
                for (const auto &arg : array(get(*program, "arguments"))) command.push_back(str(arg));
            }
            const auto execution = execute(command, temporary.path, "", 60);
            status = execution.code ? 1 : 0;
            result["execution"] = object({ { "example", str(*program, "id") }, { "exitCode", execution.code }, { "stdout", execution.output }, { "stderr", execution.error } });
            if (status) result["diagnostics"] = A{ parse(magel_diagnostics::toolDiagnostic("MGL1703", MAGEL_TEXT("手册示例执行失败")).c_str()) };
        }
    }
    if (machine)
    {
        std::cout << result.dump() << '\n';
        return status;
    }
    ManTerminal terminal;
    std::ostringstream output;
    if (kind == "modules")
    {
        output << terminal.title(MAGEL_TEXT("Magel 标准库手册")) << '\n';
        for (const auto &group : array(get(result, "groups")))
        {
            output << '\n'
                   << terminal.title(str(group, "title")) << '\n';
            for (const auto &module : array(get(group, "modules"))) output << "  " << str(module, "name") << " — " << str(module, "title") << '\n';
        }
    }
    else
    {
        for (const auto &module : selectedModules) output << terminal.title(str(module, "name")) << " — " << str(module, "title") << '\n'
                                                          << str(module, "documentation") << "\n\n";
        for (const auto &symbol : selected)
        {
            if (kind == "module" || kind == "search")
            {
                const auto description = str(symbol, "documentation");
                output << qualified(symbol) << "  " << str(symbol, "signature") << " — " << description.substr(0, description.find('\n')) << '\n';
                continue;
            }
            output << terminal.title(qualified(symbol)) << '\n'
                   << str(symbol, "signature") << "\n\n"
                   << MAGEL_TEXT("说明") << ":\n"
                   << str(symbol, "documentation") << "\n\n";
            if (manLanguage() != "zh") output << MAGEL_TEXT("说明原文语言：zh") << "\n\n";
            if (!array(get(symbol, "parameters")).empty())
            {
                output << terminal.title(MAGEL_TEXT("参数")) << '\n';
                for (const auto &param : array(get(symbol, "parameters"))) output << "  " << str(param, "label") << " — " << str(param, "documentation") << (str(param, "default").empty() ? "" : " [" + std::string(MAGEL_TEXT("默认值")) + ": " + str(param, "default") + "]") << '\n';
            }
            if (symbol.find("returns")) output << '\n'
                                               << terminal.title(MAGEL_TEXT("返回值")) << "\n"
                                               << str(get(symbol, "returns"), "type") << " — " << str(get(symbol, "returns"), "documentation") << '\n';
            if (symbol.find("errors"))
            {
                const auto &errors = get(symbol, "errors");
                output << '\n'
                       << terminal.title(MAGEL_TEXT("错误与边界")) << '\n'
                       << str(errors, "documentation") << '\n';
                for (const auto &check : array(get(errors, "preconditions"))) output << "  assert(" << str(check) << ")\n";
            }
            if (symbol.find("usage")) output << '\n'
                                             << terminal.title(MAGEL_TEXT("调用形式")) << '\n'
                                             << str(symbol, "usage") << '\n';
            for (const auto &program : array(get(symbol, "examples"))) output << '\n'
                                                                              << terminal.title(MAGEL_TEXT("完整示例")) << " (" << str(program, "source") << ")\n"
                                                                              << str(program, "code") << '\n';
            output << '\n';
        }
    }
    for (const auto &diagnostic : array(get(result, "diagnostics"))) output << str(diagnostic, "code") << ": " << str(diagnostic, "message") << '\n';
    if (result.find("candidates"))
    {
        output << MAGEL_TEXT("候选目标") << ":\n";
        for (const auto &candidate : array(get(result, "candidates"))) output << "  " << str(candidate) << '\n';
    }
    if (result.find("execution")) output << '\n'
                                         << terminal.title(MAGEL_TEXT("示例输出")) << '\n'
                                         << str(get(result, "execution"), "stdout") << str(get(result, "execution"), "stderr");
    terminal.print(output.str());
    return status;
}
