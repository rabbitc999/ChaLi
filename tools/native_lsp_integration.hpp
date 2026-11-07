/**
 * @brief 为有内嵌源码的标准模块构造只读 URI。
 * @param module 限定模块名。
 * @return 虚拟文档 URI。
 */
static std::string standardUri(const std::string &module)
{
    auto path = module;
    for (size_t at = 0; (at = path.find("::", at)) != path.npos; ++at) path.replace(at, 2, "/");
    return "chali-stdlib:/" + path + ".chl";
}
/**
 * @brief 读取真实内嵌源码，不访问任意文件或生成磁盘副本。
 * @param u 只读标准库 URI。
 * @return 模块原文；无对应模块时抛出错误。
 */
static std::string standardText(const std::string &u)
{
    if (!starts(u, "chali-stdlib:/std/") || !ends(u, ".chl")) throw std::runtime_error("无效的标准库源码 URI");
    auto module = u.substr(std::string("chali-stdlib:/").size());
    module.resize(module.size() - 4);
    module = std::regex_replace(module, std::regex("/"), "::");
    Memory::String text;
    if (!chali_module::embedded::find(Memory::String(module.data(), module.size()), &text)) throw std::runtime_error("该模块没有可查看的内嵌源码");
    return std::string(text.data(), text.size());
}
/**
 * @brief 按需建立只读标准库词法索引，仅驻留内存。
 * @param u 标准库 URI。
 * @return 不可变索引。
 */
std::shared_ptr<const LspUnit> standardUnit(const std::string &u)
{
    {
        std::lock_guard<std::mutex> lock(library->sourceMutex);
        const auto found = library->sources.find(u);
        if (found != library->sources.end()) return found->second;
    }
    auto unit = std::make_shared<LspUnit>(standardText(u));
    {
        LspIndexScope scope(*unit);
        unit->records = scan(u, *unit->text);
    }
    std::lock_guard<std::mutex> lock(library->sourceMutex);
    return library->sources.emplace(u, unit).first->second;
}
/**
 * @brief 用实际声明及参考行号选择标准库重载位置。
 * @param record API 声明记录，其 offset 保存参考页的一基行号。
 * @return 实际源码范围；没有源声明的内建函数返回空。
 */
J standardLocation(const Record &record)
{
    const auto unit = standardUnit(record.source);
    const auto &body = *unit->text;
    const auto expected = offsetAt(body, object({ { "line", int64_t(record.offset ? record.offset - 1 : 0) }, { "character", 0 } }));
    const Record *best = nullptr;
    size_t distance = std::numeric_limits<size_t>::max();
    for (const auto &candidate : unit->records)
    {
        if (candidate.name != record.name || tail(candidate.owner) != tail(record.owner)) continue;
        const auto delta = candidate.offset > expected ? candidate.offset - expected : expected - candidate.offset;
        if (delta < distance)
        {
            distance = delta;
            best = &candidate;
        }
    }
    return best ? object({ { "uri", record.source }, { "range", range(body, best->offset, best->offset + best->name.size()) } }) : J();
}
/**
 * @brief 将内部字符串转为编辑器 JSON 使用的标准字符串。
 * @param value 内部字符串。
 * @return 原始 UTF-8 字节副本。
 */
static std::string projectText(const Memory::String &value) { return std::string(value.data(), value.size()); }
/**
 * @brief 读取并验证实际项目与 AOT 目标，不执行构建或网络请求。
 * @param params 源文件 URI。
 * @return 项目、目标、输出和实际编译器变体。
 */
J projectInfo(const J &params)
{
    const auto file = pathOf(str(params, "uri"));
    if (file.empty()) throw std::runtime_error("项目查询需要 file URI");
    chali_package::Project project;
    Memory::String error;
    bool valid = chali_package::discover(Memory::String(file.u8string().c_str()), project, error);
    chali_build::TargetInfo target;
    if (valid) valid = chali_build::describeTarget(project.aotTarget, target, &error);
    const auto manifest = project.file.empty() ? file.parent_path() / "chali.toml" : fs::u8path(projectText(project.file));
    A targets;
    for (const auto name : { "windows-x64", "linux-x64", "linux-arm64" })
    {
        chali_build::TargetInfo supported;
        if (chali_build::describeTarget(name, supported)) targets.push_back(object({ { "label", name }, { "target", projectText(supported.triple) } }));
    }
    return object({ { "valid", valid }, { "error", projectText(error) }, { "manifestUri", uri(manifest) }, { "exists", !project.file.empty() }, { "contents", projectText(project.contents) }, { "main", projectText(project.main) }, { "name", projectText(project.name) }, { "target", projectText(target.triple) }, { "native", target.native }, { "windows", target.windows }, { "hostTarget", projectText(chali_build::hostTarget()) }, { "targets", targets }, { "output", project.aotOutput.empty() ? std::string("executable") : projectText(project.aotOutput) }, { "allocator", project.aotAllocator.empty() ? std::string("malloc") : projectText(project.aotAllocator) }, { "sysroot", projectText(project.aotSysroot) }, { "runtimeDirectory", projectText(project.aotRuntimeDirectory) }, { "compilerVersion", chali_package::compilerVersion }, { "llvm", chali_build::builtin::available() ? "static" : "external" } });
}
/**
 * @brief 使用保留来源的 TOML 库生成配置编辑，不直接写文件。
 * @param params 源文件 URI、当前配置原文、版本、键和值。
 * @return 带版本的工作区编辑；创建配置时不覆盖已有文件。
 */
J projectEdit(const J &params)
{
    const auto info = projectInfo(params);
    if (!get(info, "valid").asBool() || !*get(info, "valid").asBool()) throw std::runtime_error(str(info, "error"));
    const auto key = str(params, "key"), value = str(params, "value"), text = str(params, "text");
    if (key == "target")
    {
        chali_build::TargetInfo target;
        Memory::String error;
        if (!chali_build::describeTarget(Memory::String(value.data(), value.size()), target, &error)) throw std::runtime_error(projectText(error));
    }
    else if (key != "output" || (value != "executable" && value != "object" && value != "assembly" && value != "llvm"))
        throw std::runtime_error("仅支持编辑 build target 或 output");
    auto parsed = toml::parse(text);
    if (!parsed.ok()) throw std::runtime_error("配置原文不是有效 TOML，请先修正语法");
    (*parsed.value)["build"][key] = toml::Value(value);
    A changes;
    if (!*get(info, "exists").asBool()) changes.push_back(object({ { "kind", "create" }, { "uri", get(info, "manifestUri") }, { "options", object({ { "overwrite", false }, { "ignoreIfExists", false } }) } }));
    const auto edit = object({ { "range", range(text, 0, text.size()) }, { "newText", parsed.value->dump() } });
    const auto document = object({ { "uri", get(info, "manifestUri") }, { "version", get(params, "version") } });
    changes.push_back(object({ { "textDocument", document }, { "edits", A{ edit } } }));
    return object({ { "documentChanges", changes } });
}
/**
 * @brief 从本次编译报告取得当前文档的安全警告修复。
 * @param u 文档 URI。
 * @param text 原始源码。
 * @param params 请求范围和代码操作上下文。
 * @return 带文档版本的 quickfix 列表。
 */
J warningActions(const std::string &u, const std::string &text, const J &params)
{
    A actions;
    const auto only = array(get(get(params, "context"), "only"));
    if (!only.empty() && std::none_of(only.begin(), only.end(), [](const J &kind) { return str(kind) == "quickfix" || str(kind).empty(); })) return actions;
    const auto report = checkReport(u, [] { return false; });
    const auto first = integer(get(get(get(params, "range"), "start"), "line"));
    const auto last = integer(get(get(get(params, "range"), "end"), "line"));
    std::set<std::string> seen;
    for (const auto &diagnostic : array(get(report, "diagnostics")))
    {
        const auto &span = get(diagnostic, "span");
        const auto row = integer(get(span, "line")) - 1;
        if (row < first || row > last || normalizedPath(fs::u8path(str(span, "file"))) != normalizedPath(pathOf(u))) continue;
        for (const auto &fix : array(get(diagnostic, "fixes")))
        {
            if (str(fix, "file") != str(span, "file") || integer(get(fix, "column")) != 1 || integer(get(fix, "endColumn")) != 1) continue;
            const auto line = integer(get(fix, "line")) - 1;
            const auto editRange = object({ { "start", object({ { "line", line }, { "character", 0 } }) }, { "end", object({ { "line", line }, { "character", 0 } }) } });
            if (line < 0 || size_t(line) > size_t(std::count(text.begin(), text.end(), '\n'))) continue;
            const auto signature = std::to_string(line) + ":" + str(fix, "newText");
            if (!seen.insert(signature).second) continue;
            const auto edit = object({ { "range", editRange }, { "newText", get(fix, "newText") } });
            const auto change = object({ { "textDocument", object({ { "uri", u }, { "version", documents.at(u)->second } }) }, { "edits", A{ edit } } });
            actions.push_back(object({ { "title", str(fix, "title") + " (" + str(diagnostic, "code") + ")" }, { "kind", "quickfix" }, { "edit", object({ { "documentChanges", A{ change } } }) } }));
        }
    }
    return actions;
}
