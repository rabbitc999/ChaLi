/** @file chali_library_manifest.cpp
 * @brief 库包的规范清单、独立 ABI 编号、条目契约和目标选择。
 */
#include "chali_library.hpp"
#include "chali_aot_target.hpp"
#include "chali_package.hpp"
#include "abi/chali_abi.h"
#include "chali_digest.hpp"
#include "quad_model.hpp"
#include "json/json.hpp"
#include <algorithm>
#include <set>
#include <stdexcept>

namespace chali_library
{
static_assert(runtimeAbi == CHALI_ABI_VERSION, "库包必须同步公开运行时 ABI 编号");
namespace
{
/**
 * @brief 读取必需成员，不接受缺失字段。
 * @param value 当前对象。
 * @param name 字段名。
 * @return 已存在的成员。
 */
const json::Value &field(const json::Value &value, const char *name)
{
    const auto item = value.find(name);
    if (!item) throw std::runtime_error(std::string("库清单缺少字段：") + name);
    return *item;
}
/**
 * @brief 严格读取有界文本，不作数值转换。
 * @param value 字段值。
 * @return 拥有型 UTF-8 字符串。
 */
std::string text(const json::Value &value)
{
    const auto result = value.asString();
    if (!result || result->size() > 4096 || result->find('\0') != std::string_view::npos) throw std::runtime_error("库清单文本字段非法或超长");
    return std::string(*result);
}
/**
 * @brief 严格读取非负三十二位编号。
 * @param value 字段值。
 * @return 整数编号。
 */
std::uint32_t number(const json::Value &value)
{
    const auto result = value.asInt();
    if (!value.isInt() || !result || *result < 0 || *result > UINT32_MAX) throw std::runtime_error("库清单版本或位宽不是有效整数");
    return static_cast<std::uint32_t>(*result);
}
/**
 * @brief 核对对象键集合，不忽略未知的语义字段。
 * @param value JSON 对象。
 * @param keys 格式版本允许的完整键集合。
 */
void keys(const json::Value &value, std::initializer_list<std::string_view> keys)
{
    const auto object = value.asObject();
    if (!object || object->size() != keys.size()) throw std::runtime_error("库清单对象缺少字段或包含未知字段");
    for (const auto &member : *object)
        if (std::find(keys.begin(), keys.end(), member.key) == keys.end()) throw std::runtime_error("库清单包含未知字段：" + member.key);
}
/**
 * @brief 读取不含重复项的有界字符串列表。
 * @param value JSON 数组。
 * @return 按输入顺序的文本列表。
 */
std::vector<std::string> strings(const json::Value &value)
{
    const auto array = value.asArray();
    if (!array || array->size() > 1024) throw std::runtime_error("库清单列表类型或长度非法");
    std::vector<std::string> result;
    std::set<std::string> seen;
    for (const auto &item : *array)
    {
        auto current = text(item);
        if (current.empty() || !seen.insert(current).second) throw std::runtime_error("库清单列表含空值或重复项");
        result.push_back(std::move(current));
    }
    return result;
}
/**
 * @brief 排序并序列化一个文本集合。
 * @param values 不含重复值的文本集合。
 * @return JSON 数组。
 */
json::Value array(std::vector<std::string> values)
{
    std::sort(values.begin(), values.end());
    json::Array result;
    for (const auto &item : values) result.emplace_back(item);
    return json::Value(std::move(result));
}
/**
 * @brief 核对不含路径分隔或选项前缀的原生名称。
 * @param value CPU、特性或库名。
 * @return 由当前可移植字母表组成且非空时为真。
 */
bool nativeName(std::string_view value)
{
    if (value.empty() || value.size() > 256 || value.front() == '-') return false;
    for (const unsigned char c : value)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '+')) return false;
    return true;
}
/**
 * @brief 校验不依赖宿主路径规则的包内名字。
 * @param name 包内相对名字。
 * @return 名字不含越界段、空段和非便携字符时为真。
 */
bool entryName(std::string_view name)
{
    if (name.empty() || name.size() > 240 || name.front() == '/' || name.back() == '/') return false;
    std::size_t start = 0;
    for (std::size_t index = 0; index <= name.size(); ++index)
    {
        if (index == name.size() || name[index] == '/')
        {
            const auto segment = name.substr(start, index - start);
            if (segment.empty() || segment == "." || segment == "..") return false;
            start = index + 1;
        }
        else
        {
            const auto c = static_cast<unsigned char>(name[index]);
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.')) return false;
        }
    }
    return true;
}
/**
 * @brief 拒绝重复集合项及超长列表。
 * @param values 待检查的文本列表。
 */
void uniqueStrings(const std::vector<std::string> &values)
{
    if (values.size() > 1024 || std::set<std::string>(values.begin(), values.end()).size() != values.size()) throw std::runtime_error("库清单列表重复或超出预算");
}
/**
 * @brief 核对精确身份、独立 ABI 和每个已登记目标。
 * @param manifest 当前清单。
 */
void validateManifest(const Manifest &manifest)
{
    if (manifest.module.size() > 512 || manifest.module.find('\0') != std::string::npos || !chali_package::moduleName(Memory::String(manifest.module.c_str()))) throw std::runtime_error("库模块身份非法");
    if (!nativeName(manifest.version)) throw std::runtime_error("库版本需要非空的精确版本文本");
    if (manifest.compilerVersion != chali_package::compilerVersion || manifest.interfaceVersion != interfaceFormat || manifest.languageAbiVersion != languageAbi || manifest.runtimeAbiVersion != runtimeAbi)
        throw std::runtime_error("库编译器、接口格式、语言 ABI 或运行时 ABI 不兼容");
    for (const auto &name : { manifest.interface, manifest.templates, manifest.bytecode, manifest.source })
        if (!name.empty() && !entryName(name)) throw std::runtime_error("库清单的资产名字非法");
    if (manifest.interface.empty() || (manifest.bytecode.empty() ? manifest.bytecodeVersion != 0 : manifest.bytecodeVersion != 1)) throw std::runtime_error("库接口缺失或字节码格式不兼容");
    uniqueStrings(manifest.requiredFeatures);
    for (const auto &feature : manifest.requiredFeatures)
        if (feature != "native-c-v1" && feature != "scalar-unit-v1" && feature != "borrowed-record-v1" && feature != "plain-record-value-v1" && feature != "plain-enum-value-v1" && feature != "lifetime-contract-v1" && feature != "borrowed-result-v1" && feature != "unit-data-v1" && feature != "shared-type-id-v1" && feature != "owned-string-value-v1") throw std::runtime_error("库要求未支持的能力：" + feature);
    if (std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "borrowed-result-v1") != manifest.requiredFeatures.end()
        && std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "lifetime-contract-v1") == manifest.requiredFeatures.end())
        throw std::runtime_error("语言借用返回缺少 lifetime-contract-v1 必需能力");
    uniqueStrings(manifest.licenseFiles);
    for (const auto &name : manifest.licenseFiles)
        if (!entryName(name)) throw std::runtime_error("许可证资产名字非法");
    if (manifest.license.size() > 4096 || manifest.license.find('\0') != std::string::npos) throw std::runtime_error("许可证文本非法");
    if (manifest.dependencies.size() > 1024 || manifest.targets.size() > 64) throw std::runtime_error("库依赖或目标数量超出预算");
    std::set<std::string> modules, targets;
    for (const auto &dependency : manifest.dependencies)
    {
        if (dependency.module.find('\0') != std::string::npos || !chali_package::moduleName(Memory::String(dependency.module.c_str())) || dependency.module.size() > 512 || dependency.module == manifest.module || !modules.insert(dependency.module).second || !nativeName(dependency.version))
            throw std::runtime_error("库依赖身份、版本或重复关系非法");
        if (dependency.sha256.size() != 64 || dependency.sha256.find_first_not_of("0123456789abcdef") != std::string::npos) throw std::runtime_error("依赖包缺少精确 SHA-256");
    }
    for (const auto &target : manifest.targets)
    {
        chali_aot::TargetInfo known;
        Memory::String error;
        if (!chali_aot::describeTarget(Memory::String(target.triple.c_str()), known, &error) || target.triple != std::string(known.triple.c_str()) || !targets.insert(target.triple).second)
            throw std::runtime_error("库目标不是唯一的规范三元组：" + target.triple);
        if (target.objectFormat != (known.windows ? "coff" : "elf") || target.dataLayout != known.dataLayout.c_str() || !nativeName(target.cpu) || (target.nativeAbi != "c-v1" && target.nativeAbi != "chali-borrow-v1" && target.nativeAbi != "chali-value-v1" && target.nativeAbi != "chali-owned-v1"))
            throw std::runtime_error("库目标格式、数据布局、CPU 或原生 ABI 不兼容");
        if (target.nativeAbi == "chali-borrow-v1" && std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "borrowed-record-v1") == manifest.requiredFeatures.end())
            throw std::runtime_error("语言借用 ABI 缺少必需能力标记");
        if (target.nativeAbi == "chali-value-v1" && std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "plain-record-value-v1") == manifest.requiredFeatures.end())
            throw std::runtime_error("语言记录值 ABI 缺少必需能力标记");
        const bool owned = std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "owned-string-value-v1") != manifest.requiredFeatures.end();
        if ((target.nativeAbi == "chali-owned-v1") != owned)
            throw std::runtime_error("String 拥有值 ABI 与必需能力不匹配");
        if (owned && (std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "plain-record-value-v1") == manifest.requiredFeatures.end()
            || std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "borrowed-record-v1") == manifest.requiredFeatures.end()))
            throw std::runtime_error("String 拥有值 ABI 缺少共用逻辑值能力");
        if (target.nativeAbi == "c-v1" && std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "borrowed-result-v1") != manifest.requiredFeatures.end())
            throw std::runtime_error("语言借用返回能力需要语言聚合 ABI");
        if ((target.implementation.empty() && target.bitcode.empty()) || (!target.implementation.empty() && !entryName(target.implementation)) || (!target.bitcode.empty() && (!entryName(target.bitcode) || !nativeName(target.llvmVersion))))
            throw std::runtime_error("库目标缺少实现或 bitcode 兼容信息");
        if (target.bitcode.empty() && !target.llvmVersion.empty()) throw std::runtime_error("不含 bitcode 的目标不能声明 LLVM 版本要求");
        if (!target.unitData.empty() || !target.unitLayout.empty() || !target.unitSymbol.empty())
        {
            if (!entryName(target.unitData) || !entryName(target.unitLayout) || target.unitSymbol.size() != 80 || target.unitSymbol.rfind("chali_unit_data_", 0) != 0 || target.unitSymbol.substr(16).find_first_not_of("0123456789abcdef") != std::string::npos)
                throw std::runtime_error("单元数据资产或基址符号非法");
            if (std::find(manifest.requiredFeatures.begin(), manifest.requiredFeatures.end(), "unit-data-v1") == manifest.requiredFeatures.end())
                throw std::runtime_error("单元数据缺少必需能力标记");
        }
        for (const auto *values : { &target.features, &target.systemLibraries, &target.runtimeLibraries })
        {
            uniqueStrings(*values);
            for (const auto &name : *values)
                if (!nativeName(name)) throw std::runtime_error("库目标的 CPU 特性或依赖名非法");
        }
    }
    if (manifest.targets.empty() && manifest.bytecode.empty()) throw std::runtime_error("库包没有目标实现或字节码");
    if (manifest.extensions.size() > 1024 * 1024) throw std::runtime_error("库可选元数据超出预算");
    const auto extras = json::parse(manifest.extensions);
    if (!extras.ok() || !extras.value->isObject()) throw std::runtime_error("库可忽略扩展必须是 JSON 对象");
}
}  // namespace
/**
 * @brief 对拥有型条目按精确名字查找，写入前也允许未排序数组。
 * @param name 包内名字。
 * @return 对应条目或空指针。
 */
const Entry *Package::find(std::string_view name) const
{
    for (const auto &entry : entries)
        if (entry.name == name) return &entry;
    return nullptr;
}
/**
 * @brief 用固定字段顺序与排序集合生成清单。
 * @param manifest 已登记的模块及实现信息。
 * @return 规范 JSON。
 */
std::string manifestJson(const Manifest &manifest)
{
    validateManifest(manifest);
    json::Object object;
    object.insert("module", manifest.module);
    object.insert("version", manifest.version);
    object.insert("compilerVersion", manifest.compilerVersion);
    object.insert("interfaceFormat", int(manifest.interfaceVersion));
    object.insert("languageAbi", int(manifest.languageAbiVersion));
    object.insert("runtimeAbi", int(manifest.runtimeAbiVersion));
    object.insert("interface", manifest.interface);
    object.insert("templates", manifest.templates);
    object.insert("bytecode", manifest.bytecode);
    object.insert("bytecodeFormat", int(manifest.bytecodeVersion));
    object.insert("source", manifest.source);
    object.insert("license", manifest.license);
    object.insert("licenseFiles", array(manifest.licenseFiles));
    object.insert("requiredFeatures", array(manifest.requiredFeatures));
    auto dependencies = manifest.dependencies;
    std::sort(dependencies.begin(), dependencies.end(), [](const Dependency &a, const Dependency &b) { return a.module < b.module; });
    json::Array records;
    for (const auto &dependency : dependencies)
    {
        json::Object record;
        record.insert("module", dependency.module);
        record.insert("version", dependency.version);
        record.insert("sha256", dependency.sha256);
        records.emplace_back(std::move(record));
    }
    object.insert("dependencies", std::move(records));
    auto targets = manifest.targets;
    std::sort(targets.begin(), targets.end(), [](const Target &a, const Target &b) { return a.triple < b.triple; });
    json::Array implementations;
    for (const auto &target : targets)
    {
        json::Object record;
        record.insert("triple", target.triple);
        record.insert("objectFormat", target.objectFormat);
        record.insert("pointerBits", 64);
        record.insert("endian", "little");
        record.insert("dataLayout", target.dataLayout);
        record.insert("cpu", target.cpu);
        record.insert("features", array(target.features));
        record.insert("nativeAbi", target.nativeAbi);
        record.insert("implementation", target.implementation);
        record.insert("bitcode", target.bitcode);
        record.insert("llvmVersion", target.llvmVersion);
        if (!target.unitData.empty())
        {
            record.insert("unitData", target.unitData);
            record.insert("unitLayout", target.unitLayout);
            record.insert("unitSymbol", target.unitSymbol);
        }
        record.insert("systemLibraries", array(target.systemLibraries));
        record.insert("runtimeLibraries", array(target.runtimeLibraries));
        implementations.emplace_back(std::move(record));
    }
    object.insert("targets", std::move(implementations));
    object.insert("extensions", *json::parse(manifest.extensions).value);
    const auto result = json::Value(std::move(object)).dump();
    if (result.size() > 1024 * 1024) throw std::runtime_error("库清单超出一 MiB 预算");
    return result;
}
/**
 * @brief 从规范清单恢复已验证的元数据，不容忍字段覆盖和隐式转换。
 * @param bytes 完整清单字节。
 * @return 拥有型清单。
 */
Manifest parseManifest(std::string_view bytes)
{
    if (bytes.empty() || bytes.size() > 1024 * 1024) throw std::runtime_error("库清单为空或超出预算");
    const auto parsed = json::parse(bytes);
    if (!parsed.ok()) throw std::runtime_error("库清单不是有效 JSON");
    const auto &root = *parsed.value;
    keys(root, { "module", "version", "compilerVersion", "interfaceFormat", "languageAbi", "runtimeAbi", "interface", "templates", "bytecode", "bytecodeFormat", "source", "license", "licenseFiles", "requiredFeatures", "dependencies", "targets", "extensions" });
    Manifest manifest;
    manifest.module = text(field(root, "module"));
    manifest.version = text(field(root, "version"));
    manifest.compilerVersion = text(field(root, "compilerVersion"));
    manifest.interfaceVersion = number(field(root, "interfaceFormat"));
    manifest.languageAbiVersion = number(field(root, "languageAbi"));
    manifest.runtimeAbiVersion = number(field(root, "runtimeAbi"));
    manifest.interface = text(field(root, "interface"));
    manifest.templates = text(field(root, "templates"));
    manifest.bytecode = text(field(root, "bytecode"));
    manifest.bytecodeVersion = number(field(root, "bytecodeFormat"));
    manifest.source = text(field(root, "source"));
    manifest.license = text(field(root, "license"));
    manifest.licenseFiles = strings(field(root, "licenseFiles"));
    manifest.requiredFeatures = strings(field(root, "requiredFeatures"));
    const auto dependencies = field(root, "dependencies").asArray(), targets = field(root, "targets").asArray();
    if (!dependencies || dependencies->size() > 1024 || !targets || targets->size() > 64) throw std::runtime_error("库依赖或目标数组非法");
    for (const auto &item : *dependencies)
    {
        keys(item, { "module", "version", "sha256" });
        manifest.dependencies.push_back({ text(field(item, "module")), text(field(item, "version")), text(field(item, "sha256")) });
    }
    for (const auto &item : *targets)
    {
        if (item.find("unitData") || item.find("unitLayout") || item.find("unitSymbol"))
            keys(item, { "triple", "objectFormat", "pointerBits", "endian", "dataLayout", "cpu", "features", "nativeAbi", "implementation", "bitcode", "llvmVersion", "unitData", "unitLayout", "unitSymbol", "systemLibraries", "runtimeLibraries" });
        else
            keys(item, { "triple", "objectFormat", "pointerBits", "endian", "dataLayout", "cpu", "features", "nativeAbi", "implementation", "bitcode", "llvmVersion", "systemLibraries", "runtimeLibraries" });
        if (number(field(item, "pointerBits")) != 64 || text(field(item, "endian")) != "little") throw std::runtime_error("库目标位宽或字节序不兼容");
        Target target;
        target.triple = text(field(item, "triple"));
        target.objectFormat = text(field(item, "objectFormat"));
        target.dataLayout = text(field(item, "dataLayout"));
        target.cpu = text(field(item, "cpu"));
        target.features = strings(field(item, "features"));
        target.nativeAbi = text(field(item, "nativeAbi"));
        target.implementation = text(field(item, "implementation"));
        target.bitcode = text(field(item, "bitcode"));
        target.llvmVersion = text(field(item, "llvmVersion"));
        if (item.find("unitData"))
        {
            target.unitData = text(field(item, "unitData"));
            target.unitLayout = text(field(item, "unitLayout"));
            target.unitSymbol = text(field(item, "unitSymbol"));
        }
        target.systemLibraries = strings(field(item, "systemLibraries"));
        target.runtimeLibraries = strings(field(item, "runtimeLibraries"));
        manifest.targets.push_back(std::move(target));
    }
    if (!field(root, "extensions").isObject()) throw std::runtime_error("库可忽略扩展不是对象");
    manifest.extensions = field(root, "extensions").dump();
    if (manifestJson(manifest) != bytes) throw std::runtime_error("库清单不是规范编码或包含重复键");
    return manifest;
}
/**
 * @brief 核对条目身份、引用种类和各项载荷预算。
 * @param package 已恢复或待写出的库包。
 */
void validate(const Package &package)
{
    if (package.entries.empty() || package.entries.size() > 4096) throw std::runtime_error("库条目数量为空或超出预算");
    std::set<std::string> names;
    std::uint64_t total = 56 + manifestJson(package.manifest).size();
    for (const auto &entry : package.entries)
    {
        const auto kind = static_cast<std::uint16_t>(entry.kind);
        if (!entryName(entry.name) || !names.insert(entry.name).second || (entry.flags & ~1u) || ((kind < 1 || kind > 9) && !(entry.flags & 1u))) throw std::runtime_error("库条目名字、种类、标志或唯一性非法");
        const std::uint64_t size = 48 + entry.name.size() + entry.bytes.size();
        if (size > maximumPackageBytes || total > maximumPackageBytes - size) throw std::runtime_error("库包超出二百五十六 MiB 预算");
        total += size;
        if ((entry.kind == Kind::Interface || entry.kind == Kind::Templates || entry.kind == Kind::Source || entry.kind == Kind::License)
            && (entry.bytes.find('\0') != std::string::npos || entry.bytes.size() > 32ull * 1024 * 1024)) throw std::runtime_error("库文本资产含 NUL 或超出预算");
    }
    const auto require = [&](const std::string &name, Kind kind) {
        if (name.empty()) return;
        const auto entry = package.find(name);
        if (!entry || entry->kind != kind) throw std::runtime_error("库清单引用缺失或资产种类不符：" + name);
    };
    require(package.manifest.interface, Kind::Interface);
    require(package.manifest.templates, Kind::Templates);
    require(package.manifest.bytecode, Kind::Bytecode);
    require(package.manifest.source, Kind::Source);
    if (package.find(package.manifest.interface)->bytes.empty()) throw std::runtime_error("库接口内容为空");
    for (const auto &name : package.manifest.licenseFiles) require(name, Kind::License);
    for (const auto &target : package.manifest.targets)
    {
        debugDescriptor(package, target);
        require(target.bitcode, Kind::Bitcode);
        require(target.unitData, Kind::UnitData);
        require(target.unitLayout, Kind::UnitData);
        if (!target.unitData.empty())
        {
            const auto &data = package.find(target.unitData)->bytes;
            const auto &layout = package.find(target.unitLayout)->bytes;
            const auto identity = package.manifest.module + "\n" + target.triple + "\n" + layout + "\n" + data;
            if (target.unitSymbol != "chali_unit_data_" + chali_digest::hex(chali_digest::sha256(identity))) throw std::runtime_error("单元数据身份与基址符号不匹配");
            Memory::Vector<stackvm::LayoutRow> rows;
            if (data.empty() || layout.size() > 32ull * 1024 * 1024 || !stackvm::decodeDataLayout(layout, rows) || rows.empty()) throw std::runtime_error("单元数据布局无效");
            std::set<Memory::String> names;
            for (const auto &row : rows)
                if (!names.insert(row.name).second || !row.hasSize || row.offset > data.size() || row.size > data.size() - row.offset)
                    throw std::runtime_error("单元数据布局越界或符号重复");
        }
        if (!target.implementation.empty())
        {
            const auto entry = package.find(target.implementation);
            if (!entry || (entry->kind != Kind::Object && entry->kind != Kind::Archive)) throw std::runtime_error("库目标的原生对象或归档缺失");
            if (entry->kind == Kind::Archive && entry->bytes.compare(0, 8, "!<arch>\n") != 0) throw std::runtime_error("库归档条目不是自包含归档");
        }
    }
}
/**
 * @brief 验证可选符号描述并将其绑定到实际对象及 bitcode 字节。
 * @param package 同一事务恢复的包快照。
 * @param target 本包中已验证结构的目标。
 * @return 规范描述；未携带时为空。
 */
std::string debugDescriptor(const Package &package, const Target &target)
{
    const auto extras = json::parse(package.manifest.extensions);
    if (!extras.value || !extras.value->isObject()) throw std::runtime_error("库调试可选元数据不是有效对象");
    const auto debug = extras.value->find("nativeDebug");
    if (!debug) return {};
    if (!debug->isObject()) throw std::runtime_error("库调试目录不是目标对象");
    for (const auto &item : *debug->asObject())
        if (std::none_of(package.manifest.targets.begin(), package.manifest.targets.end(), [&](const auto &known) { return known.triple == item.key; }))
            throw std::runtime_error("库调试目录含未知目标");
    const auto descriptor = debug->find(target.triple);
    if (!descriptor) return {};
    keys(*descriptor, { "format", "prefix", "optimized", "implementationSha256", "bitcodeSha256" });
    const auto prefix = "chali_debug_" + chali_digest::hex(chali_digest::sha256(package.manifest.module));
    if (number(field(*descriptor, "format")) != 1 || text(field(*descriptor, "prefix")) != prefix || !field(*descriptor, "optimized").isBool())
        throw std::runtime_error("库调试目录格式、模块身份或优化状态无效");
    for (const auto &pair : { std::make_pair(target.implementation, "implementationSha256"), std::make_pair(target.bitcode, "bitcodeSha256") })
    {
        const auto declared = text(field(*descriptor, pair.second));
        const auto payload = pair.first.empty() ? nullptr : package.find(pair.first);
        if ((!pair.first.empty() && !payload) || declared != (payload ? chali_digest::hex(chali_digest::sha256(payload->bytes)) : ""))
            throw std::runtime_error("库调试符号与实现 SHA-256 不匹配");
    }
    return descriptor->dump();
}
/**
 * @brief 按明确选择核对原生或 bitcode 实现，不作缺失实现回退。
 * @param package 已验证的库。
 * @param requested 目标或别名。
 * @param cpu 调用方 CPU。
 * @param enabledFeatures 明确允许的特性。
 * @param implementation native 或 bitcode。
 * @param llvmVersion 实际 LLVM 工具版本，bitcode 路径必须提供。
 * @return 对应目标，bitcode 同时核对载荷内目标、布局及生产版本。
 */
const Target &select(const Package &package, const std::string &requested, const std::string &cpu, const std::vector<std::string> &enabledFeatures,
                     const std::string &implementation, const std::string &llvmVersion)
{
    if (implementation != "native" && implementation != "bitcode") throw std::runtime_error("库实现选择必须是 native 或 bitcode");
    chali_aot::TargetInfo known;
    Memory::String error;
    if (!chali_aot::describeTarget(Memory::String(requested.c_str()), known, &error)) throw std::runtime_error(error.c_str());
    for (const auto &target : package.manifest.targets)
    {
        if (target.triple != known.triple.c_str()) continue;
        const auto actualCpu = cpu.empty() ? std::string(known.cpu.c_str()) : cpu;
        if (target.cpu != actualCpu && target.cpu != known.cpu.c_str()) throw std::runtime_error("库目标 CPU 不兼容：" + target.cpu);
        for (const auto &feature : target.features)
            if (std::find(enabledFeatures.begin(), enabledFeatures.end(), feature) == enabledFeatures.end()) throw std::runtime_error("库要求未启用的 CPU 特性：" + feature);
        if (implementation == "native")
        {
            if (target.implementation.empty()) throw std::runtime_error("库只有 bitcode，当前选择需要原生实现");
        }
        else
        {
            if (target.bitcode.empty()) throw std::runtime_error("库缺少明确选择的 bitcode 实现");
            if (llvmVersion.empty()) throw std::runtime_error("bitcode 选择缺少可核对的实际 LLVM 版本");
            if (target.llvmVersion != llvmVersion) throw std::runtime_error("库 bitcode LLVM 版本不兼容：需要 " + target.llvmVersion + "，实际 " + llvmVersion);
            const auto *entry = package.find(target.bitcode);
            if (!entry || entry->kind != Kind::Bitcode) throw std::runtime_error("库 bitcode 实现条目缺失");
            const auto actual = inspectBitcode(entry->bytes);
            chali_aot::TargetInfo module;
            if (!chali_aot::describeTarget(actual.triple.c_str(), module, &error) || module.triple != known.triple)
                throw std::runtime_error("bitcode 内部目标与库清单不一致：" + actual.triple);
            if (actual.dataLayout != target.dataLayout) throw std::runtime_error("bitcode 内部数据布局与库清单不一致");
            if (actual.llvmVersion != target.llvmVersion) throw std::runtime_error("bitcode 实际生产版本与清单不一致：" + actual.llvmVersion);
        }
        return target;
    }
    std::string available;
    for (const auto &target : package.manifest.targets) available += (available.empty() ? "" : ", ") + target.triple;
    throw std::runtime_error("库缺少目标 " + std::string(known.triple.c_str()) + "；可用目标：" + available);
}
}  // namespace chali_library
