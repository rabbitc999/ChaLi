/** @file chali_library_import.cpp
 * @brief 编译会话的库内容快照、精确锁及原生实现恢复。
 */
#include "chali_library.hpp"
#include "chali_aot.hpp"
#include "chali_native_tools.hpp"
#include "chali_package.hpp"
#include "json/json.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>

namespace chali_library
{
/**
 * @brief 按完整模块身份生成不含宿主路径的规范精确锁。
 * @param modules 待排序及校验的锁条目。
 * @return 无换行的规范 JSON 文本。
 */
std::string lockJson(std::vector<Dependency> modules)
{
    if (modules.size() > 128) throw std::runtime_error("库锁模块数量超过 128");
    std::sort(modules.begin(), modules.end(), [](const auto &a, const auto &b) { return a.module < b.module; });
    std::set<std::string> seen;
    json::Array records;
    for (const auto &module : modules)
    {
        if (!chali_package::moduleName(Memory::String(module.module.c_str())) || !seen.insert(module.module).second
            || module.version.empty() || module.version.size() > 128 || module.sha256.size() != 64)
            throw std::runtime_error("库锁模块、版本、摘要或唯一性非法");
        for (const unsigned char c : module.version)
            if (!std::isalnum(c) || c >= 128)
                if (c != '.' && c != '_' && c != '-' && c != '+') throw std::runtime_error("库锁版本字符非法");
        for (const char c : module.sha256)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f')) throw std::runtime_error("库锁摘要必须是小写 SHA-256");
        json::Object record;
        record.insert("module", module.module);
        record.insert("version", module.version);
        record.insert("sha256", module.sha256);
        records.emplace_back(std::move(record));
    }
    json::Object root;
    root.insert("format", json::Value(std::int64_t(1)));
    root.insert("compilerVersion", std::string(chali_package::compilerVersion));
    root.insert("modules", json::Value(std::move(records)));
    return json::Value(std::move(root)).dump();
}
/**
 * @brief 从不超过一 MiB 的普通文件恢复规范锁，拒绝截断和重复键。
 * @param path 库锁路径。
 * @return 验证通过的精确模块集合。
 */
std::vector<Dependency> readLock(const std::filesystem::path &path)
{
    if (!std::filesystem::is_regular_file(path) || std::filesystem::file_size(path) > 1024 * 1024)
        throw std::runtime_error("库锁不是普通文件或超过一 MiB");
    const auto length = std::filesystem::file_size(path);
    if (length > 1024 * 1024) throw std::runtime_error("库锁读取长度超过预算");
    std::ifstream stream(path, std::ios::binary);
    std::string bytes(static_cast<std::size_t>(length), '\0');
    stream.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!stream || stream.peek() != std::char_traits<char>::eof()) throw std::runtime_error("读取库锁失败或内容长度变化");
    while (!bytes.empty() && (bytes.back() == '\r' || bytes.back() == '\n')) bytes.pop_back();
    const auto parsed = json::parse(bytes);
    if (!parsed.ok() || !parsed.value->isObject()) throw std::runtime_error("库锁 JSON 无效");
    const auto &root = *parsed.value;
    const auto format = root.find("format"), compiler = root.find("compilerVersion"), modules = root.find("modules");
    if (!format || !format->isInt() || !format->asInt() || *format->asInt() != 1 || !compiler || !compiler->asString()
        || *compiler->asString() != chali_package::compilerVersion || !modules || !modules->asArray() || modules->asArray()->size() > 128)
        throw std::runtime_error("库锁格式、编译器版本或模块集合不兼容");
    std::vector<Dependency> result;
    for (const auto &record : *modules->asArray())
    {
        const auto module = record.find("module"), version = record.find("version"), digest = record.find("sha256");
        if (!module || !version || !digest || !module->asString() || !version->asString() || !digest->asString())
            throw std::runtime_error("库锁条目缺少文本字段");
        result.push_back({ std::string(*module->asString()), std::string(*version->asString()), std::string(*digest->asString()) });
    }
    if (lockJson(result) != bytes) throw std::runtime_error("库锁不是规范编码、包含重复键或未知字段");
    return result;
}
/** @brief 本次编译独占的目标、内容锁及工作区。 */
struct ImportSession::State
{
    std::string target, cpu, implementation, compiler, llvmVersion;
    std::map<std::string, Package> packages;
    std::vector<std::string> sources, borrowSources, dataSources, typeSources, lifetimeSources, borrowResultSources;
    std::vector<std::filesystem::path> paths;
    std::unique_ptr<chali_native_tools::Workspace> workspace;
    std::uint64_t bytes = 0;
    bool locked = false;
    std::map<std::string, Dependency> locks;
    std::set<std::string> visiting, resolved;
};
/** @brief 初始化目标、CPU 与明确的实现选择。
 * @param target 目标别名或三元组。
 * @param cpu 目标 CPU。
 * @param implementation native 或 bitcode。
 * @param compiler 可选外置 LLVM 路径。
 * @return 无。 */
ImportSession::ImportSession(std::string target, std::string cpu, std::string implementation, std::string compiler) : state_(new State)
{
    state_->target = std::move(target);
    state_->cpu = std::move(cpu);
    state_->implementation = std::move(implementation);
    state_->compiler = std::move(compiler);
    if (state_->implementation != "native" && state_->implementation != "bitcode") throw std::runtime_error("库实现选择必须是 native 或 bitcode");
}
/** @brief 在消费者执行完毕后释放快照及暂存实现。 */
ImportSession::~ImportSession() = default;
/** @brief 校验模块身份、依赖内容锁和目标后保留拥有型快照。
 * @param path 包文件。
 * @param module 预期完整模块名。
 * @param pin 可选依赖版本与摘要。
 * @return 会话内稳定的已验证包。 */
const Package &ImportSession::open(const std::filesystem::path &path, const std::string &module, const Dependency *pin)
{
    Package package;
    std::string error;
    if (!read(path, package, error)) throw std::runtime_error(path.u8string() + ": " + error);
    const std::string identity = package.manifest.module;
    if (!module.empty() && identity != module) throw std::runtime_error("库模块身份不匹配：" + module);
    if (pin && (package.manifest.version != pin->version || package.contentSha256 != pin->sha256))
        throw std::runtime_error("库依赖版本或摘要不匹配：" + module);
    if (state_->locked)
    {
        const auto expected = state_->locks.find(identity);
        if (expected == state_->locks.end() || expected->second.version != package.manifest.version
            || expected->second.sha256 != package.contentSha256)
            throw std::runtime_error("库不在项目锁中或版本／摘要发生变化：" + identity);
    }
    if (state_->implementation == "bitcode" && state_->llvmVersion.empty())
    {
        chali_aot::Options options;
        options.compiler = state_->compiler.c_str();
        Memory::Vector<Memory::String> diagnostics;
        state_->llvmVersion = chali_aot::llvmVersion(options, &diagnostics).c_str();
        if (state_->llvmVersion.empty()) throw std::runtime_error(diagnostics.empty() ? "无法核对 bitcode 所需 LLVM 版本" : diagnostics.front().c_str());
    }
    const std::vector<std::string> features;
    const auto &selected = select(package, state_->target, state_->cpu, features, state_->implementation, state_->llvmVersion);
    const auto &abi = selected.nativeAbi;
    const bool borrowed = abi == "chali-borrow-v1" || abi == "chali-value-v1";
    const auto existing = state_->packages.find(identity);
    if (existing != state_->packages.end())
    {
        if (existing->second.contentSha256 != package.contentSha256) throw std::runtime_error("同一模块解析到不同库内容：" + module);
        return existing->second;
    }
    std::uint64_t size = 0;
    for (const auto &entry : package.entries) size += entry.bytes.size();
    if (state_->packages.size() >= 128 || size > maximumPackageBytes - state_->bytes)
        throw std::runtime_error("本次编译的库数量或总资产超过预算");
    state_->bytes += size;
    state_->paths.push_back(std::filesystem::weakly_canonical(path));
    state_->sources.push_back(state_->paths.back().u8string() + ".interface.chl");
    if (borrowed) state_->borrowSources.push_back(state_->sources.back());
    if (std::find(package.manifest.requiredFeatures.begin(), package.manifest.requiredFeatures.end(), "lifetime-contract-v1") != package.manifest.requiredFeatures.end())
        state_->lifetimeSources.push_back(state_->sources.back());
    if (borrowed && std::find(package.manifest.requiredFeatures.begin(), package.manifest.requiredFeatures.end(), "borrowed-result-v1") != package.manifest.requiredFeatures.end())
        state_->borrowResultSources.push_back(state_->sources.back());
    if (!selected.unitData.empty()) state_->dataSources.push_back(state_->sources.back());
    if (std::find(package.manifest.requiredFeatures.begin(), package.manifest.requiredFeatures.end(), "shared-type-id-v1") != package.manifest.requiredFeatures.end())
        state_->typeSources.push_back(state_->sources.back());
    return state_->packages.emplace(identity, std::move(package)).first->second;
}
/**
 * @brief 在本次会话首次加载前启用项目级锁。
 * @param path 规范库锁文件。
 */
void ImportSession::loadLock(const std::filesystem::path &path)
{
    if (!state_->packages.empty() || state_->locked) throw std::runtime_error("库锁必须在加载任何库之前设置一次");
    const auto modules = readLock(path);
    for (const auto &module : modules) state_->locks.emplace(module.module, module);
    state_->locked = true;
}
/** @brief 判断项目锁是否指定该模块为库包。
 * @param module 完整模块名。
 * @return 锁中存在时为真。 */
bool ImportSession::requiresPackage(const std::string &module) const { return state_->locked && state_->locks.count(module); }
/**
 * @brief 独立链接时按清单解析精确传递依赖；不执行接口或初始化。
 * @param path 根包文件。
 * @param searchRoots 调用方提供的库根。
 * @return 会话持有的根包快照。
 */
const Package &ImportSession::openRecursive(const std::filesystem::path &path, const std::vector<std::filesystem::path> &searchRoots)
{
    const auto &package = open(path, {});
    const auto &module = package.manifest.module;
    if (state_->visiting.count(module)) throw std::runtime_error("库依赖循环：" + module);
    if (state_->resolved.count(module)) return package;
    state_->visiting.insert(module);
    auto root = std::filesystem::absolute(path).parent_path();
    for (std::size_t at = 0; (at = module.find("::", at)) != std::string::npos; at += 2) root = root.parent_path();
    std::vector<std::filesystem::path> roots{ root, std::filesystem::absolute(path).parent_path() };
    roots.insert(roots.end(), searchRoots.begin(), searchRoots.end());
    for (const auto &dependency : package.manifest.dependencies)
    {
        std::string relative = dependency.module;
        for (std::size_t at = 0; (at = relative.find("::", at)) != std::string::npos;) relative.replace(at, 2, "/");
        bool found = false;
        for (const auto &directory : roots)
        {
            const auto candidate = directory / std::filesystem::u8path(relative + ".chlib");
            if (!std::filesystem::is_regular_file(candidate)) continue;
            open(candidate, dependency.module, &dependency);
            openRecursive(candidate, roots);
            found = true;
            break;
        }
        if (!found) throw std::runtime_error("找不到锁定的库依赖：" + dependency.module);
    }
    state_->visiting.erase(module);
    state_->resolved.insert(module);
    return package;
}
/** @brief 返回接口来源标签。
 * @return 本次编译全部已加载接口的标签。 */
std::vector<std::string> ImportSession::sources() const { return state_->sources; }
/** @brief 返回已验证语言借用接口来源。
 * @return 与当前目标原生 ABI 对应的稳定标签。 */
std::vector<std::string> ImportSession::borrowSources() const { return state_->borrowSources; }
/** @brief 返回允许公开生命周期契约的已验证来源。
 * @return 当前快照的契约接口标签。 */
std::vector<std::string> ImportSession::lifetimeSources() const { return state_->lifetimeSources; }
/** @brief 返回允许借用返回的已验证来源。
 * @return 当前快照的借用返回接口标签。 */
std::vector<std::string> ImportSession::borrowResultSources() const { return state_->borrowResultSources; }
/** @brief 返回当前目标携带已验证数据映像的来源。
 * @return 对应本次快照的稳定接口标签。 */
std::vector<std::string> ImportSession::dataSources() const { return state_->dataSources; }
/** @brief 返回要求统一类型编号的已验证来源。
 * @return 当前快照的共享类型接口标签。 */
std::vector<std::string> ImportSession::typeSources() const { return state_->typeSources; }
/** @brief 返回成功加载的真实来源路径。
 * @return 本次会话持有快照对应的规范包路径。 */
std::vector<std::filesystem::path> ImportSession::paths() const { return state_->paths; }

/** @brief 只将已验证快照写入私有工作区，避免重新打开包时发生内容替换。
 * @param options 接收对象路径和系统库的链接选项。 */
void ImportSession::appendTo(chali_aot::Options &options)
{
    if (state_->packages.empty()) return;
    if (!state_->workspace) state_->workspace.reset(new chali_native_tools::Workspace);
    std::size_t index = 0;
    for (const auto &item : state_->packages)
    {
        const std::vector<std::string> features;
        const auto &target = select(item.second, state_->target, state_->cpu, features, state_->implementation, state_->llvmVersion);
        const bool bitcode = state_->implementation == "bitcode";
        const auto &entry = *item.second.find(bitcode ? target.bitcode : target.implementation);
        const auto file = state_->workspace->path / (std::to_string(index++) + (bitcode && options.lto ? ".bc" : entry.kind == Kind::Archive ? ".a" :
                                                                                                                                               ".o"));
        const bool rewrite = bitcode && options.lto;
        if (bitcode && (!options.lto || rewrite))
        {
            chali_aot::Options materialize;
            materialize.target = state_->target.c_str();
            materialize.cpu = state_->cpu.c_str();
            materialize.compiler = state_->compiler.c_str();
            materialize.sysroot = options.sysroot;
            materialize.debug = options.debug;
            materialize.lto = options.lto;
            materialize.thinLto = options.thinLto;
            Memory::Vector<Memory::String> diagnostics;
            const auto compile = rewrite ? chali_aot::bitcodeModule : chali_aot::bitcodeObject;
            if (compile(Memory::String(entry.bytes.data(), entry.bytes.size()), materialize, Memory::String(file.u8string().c_str()), &diagnostics))
                throw std::runtime_error("无法物化库 bitcode：" + item.first + (diagnostics.empty() ? "" : "：" + std::string(diagnostics.front().c_str())));
            if (rewrite)
            {
                const auto size = std::filesystem::file_size(file);
                if (size > maximumPackageBytes) throw std::runtime_error("重编码 bitcode 超过包预算");
                std::ifstream input(file, std::ios::binary);
                std::string bytes(static_cast<std::size_t>(size), '\0');
                input.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
                if (!input || input.peek() != std::char_traits<char>::eof()) throw std::runtime_error("无法读取重编码 bitcode");
                const auto info = inspectBitcode(bytes);
                if (info.summary != options.thinLto || info.triple != target.triple || info.dataLayout != target.dataLayout || info.llvmVersion != state_->llvmVersion)
                    throw std::runtime_error("重编码 bitcode 的模式或兼容性头不匹配");
            }
        }
        else
        {
            std::ofstream stream(file, std::ios::binary | std::ios::trunc);
            stream.write(entry.bytes.data(), static_cast<std::streamsize>(entry.bytes.size()));
            stream.close();
            if (!stream) throw std::runtime_error("无法暂存库实现：" + item.first);
        }
        if (bitcode && options.lto)
            options.linkBitcode.push_back(Memory::String(file.u8string().c_str()));
        else
            options.linkObjects.push_back(Memory::String(file.u8string().c_str()));
        if (!target.unitData.empty())
        {
            const auto &data = item.second.find(target.unitData)->bytes;
            const auto &layout = item.second.find(target.unitLayout)->bytes;
            options.unitData.push_back({ Memory::String(target.unitSymbol.c_str()), Memory::String(data.data(), data.size()), Memory::String(layout.data(), layout.size()) });
        }
        for (const auto &library : target.systemLibraries)
            options.linkLibraries.push_back(Memory::String(("-l" + library).c_str()));
    }
}
/** @brief 导出快照身份，避免再次读取路径导致内容竞态。
 * @return 本次会话全部包的精确身份记录。 */
std::vector<Dependency> ImportSession::records() const
{
    std::vector<Dependency> records;
    for (const auto &item : state_->packages) records.push_back({ item.first, item.second.manifest.version, item.second.contentSha256 });
    return records;
}
}  // namespace chali_library
