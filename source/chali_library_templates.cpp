/** @file chali_library_templates.cpp
 * @brief 独立库泛型定义的规范元数据及受限源码恢复。
 */
#include "chali_library.hpp"
#include "chali_generic.hpp"
#include "chali_package.hpp"
#include "macro/fragments.hpp"
#include "macro/source.hpp"
#include "json/json.hpp"
#include <algorithm>
#include <cctype>
#include <set>
#include <stdexcept>

namespace chali_library
{
namespace
{
/**
 * @brief 读取模板元数据的必需字段。
 * @param value 当前对象。
 * @param name 字段名。
 * @return 已存在的字段；缺失时抛出错误。
 */
const json::Value &templateField(const json::Value &value, const char *name)
{
    const auto field = value.find(name);
    if (!field) throw std::runtime_error(std::string("泛型库元数据缺少字段：") + name);
    return *field;
}
/**
 * @brief 核对模板对象的完整键集合，不接受未知语义字段。
 * @param value 当前对象。
 * @param names 格式一允许的所有字段。
 * @return 无返回值；键集合不一致时抛出错误。
 */
void templateKeys(const json::Value &value, std::initializer_list<std::string_view> names)
{
    const auto object = value.asObject();
    if (!object || object->size() != names.size()) throw std::runtime_error("泛型库元数据字段集合非法");
    for (const auto &member : *object)
        if (std::find(names.begin(), names.end(), member.key) == names.end())
            throw std::runtime_error("泛型库元数据包含未知字段：" + member.key);
}
/**
 * @brief 读取有界文本，拒绝内嵌零字节。
 * @param value 字段值。
 * @param limit 字节预算。
 * @return 拥有型文本。
 */
std::string templateText(const json::Value &value, std::size_t limit = 4096)
{
    const auto text = value.asString();
    if (!text || text->size() > limit || text->find('\0') != std::string_view::npos)
        throw std::runtime_error("泛型库元数据文本非法或超过预算");
    return std::string(*text);
}
/**
 * @brief 判断声明是否含会话宏编号或装载时预处理指令，忽略字符串和注释。
 * @param text 规范化声明。
 * @return 出现不可跨编译会话复用的标记时返回 true。
 */
bool templateHasSessionMarker(const std::string &text)
{
    bool lineHead = true;
    for (std::size_t at = 0; at < text.size(); ++at)
    {
        if (text[at] == '\n') { lineHead = true; continue; }
        if (lineHead && (text[at] == ' ' || text[at] == '\t' || text[at] == '\r')) continue;
        if (lineHead && text[at] == '%') return true;
        lineHead = false;
        if (text[at] == '"' || text[at] == '\'')
        {
            const auto quote = text[at++];
            while (at < text.size() && text[at] != quote)
            {
                if (text[at] == '\\' && at + 1 < text.size()) ++at;
                ++at;
            }
        }
        else if (text.compare(at, 2, "//") == 0)
        {
            const auto end = text.find('\n', at + 2);
            if (end == text.npos) break;
            at = end;
            lineHead = true;
        }
        else if (text.compare(at, 2, "/*") == 0)
        {
            const auto end = text.find("*/", at + 2);
            if (end == text.npos) return true;
            at = end + 1;
        }
        else if (text.compare(at, 2, "@!") == 0) return true;
    }
    return false;
}
/**
 * @brief 将编译器已解析的泛型形参编码为语义字段。
 * @param parameter 类型或编译期整数形参。
 * @return 不含进程状态的规范对象。
 */
json::Value templateParameter(const chali_generic::Parameter &parameter)
{
    json::Object object;
    object.insert("name", parameter.name.c_str());
    object.insert("constraint", parameter.constraint.c_str());
    object.insert("fallback", parameter.fallback.c_str());
    object.insert("hasDefault", parameter.hasDefault);
    object.insert("constant", parameter.constant);
    object.insert("constantType", parameter.constantType.c_str());
    return json::Value(std::move(object));
}
/**
 * @brief 恢复本会话的宏调用实参，以可重新解析的源码保存模板上下文。
 * @param text 尚含稳定宏片段引用的声明。
 * @param label 出错声明的模块内身份。
 * @return 不含会话编号的文本；引用损坏时抛出错误。
 */
std::string templateRestore(Memory::String text, const Memory::String &label)
{
    std::string error;
    if (!chali_macro::sourceFragments.restore(text, error)) throw std::runtime_error(std::string(label.c_str()) + "：" + error);
    return std::string(text.data(), text.size());
}
/**
 * @brief 校验前缀只能是派生请求或无参数运算符开关，禁止普通代码注入。
 * @param text 前缀文本。
 * @param derive 是否只允许派生请求。
 * @return 前缀有效时正常返回，否则抛出错误。
 */
void templatePrefix(const std::string &text, bool derive)
{
    std::size_t at = 0;
    while (at < text.size())
    {
        if (std::isspace(static_cast<unsigned char>(text[at]))) { ++at; continue; }
        if (derive)
        {
            if (text.compare(at, 7, "#derive") != 0) throw std::runtime_error("泛型库派生前缀非法");
            at += 7;
            while (at < text.size() && std::isspace(static_cast<unsigned char>(text[at]))) ++at;
            if (at == text.size() || text[at++] != '(') throw std::runtime_error("泛型库派生请求缺少括号");
            unsigned depth = 1;
            while (at < text.size() && depth)
            {
                const auto ch = text[at++];
                if (ch == '(') ++depth;
                else if (ch == ')') --depth;
                else if (!(std::isalnum(static_cast<unsigned char>(ch)) || std::isspace(static_cast<unsigned char>(ch))
                           || ch == '_' || ch == ':' || ch == ',' || ch == '<' || ch == '>' || ch == '[' || ch == ']'))
                    throw std::runtime_error("泛型库派生请求含非法词素");
                if (depth > 256) throw std::runtime_error("泛型库派生请求嵌套过深");
            }
            if (depth) throw std::runtime_error("泛型库派生请求括号不完整");
        }
        else
        {
            const auto begin = at;
            if (text.compare(at, 3, "#No") != 0) throw std::runtime_error("泛型库运算符开关非法");
            at += 3;
            while (at < text.size() && (std::isalnum(static_cast<unsigned char>(text[at])) || text[at] == '_')) ++at;
            const auto token = text.substr(begin, at - begin);
            if (!((token.size() > 10 && token.compare(token.size() - 7, 7, "Replace") == 0)
                  || (token.size() > 8 && token.compare(0, 8, "#NoRight") == 0)))
                throw std::runtime_error("泛型库运算符开关名称非法");
        }
    }
}
}  // namespace

/**
 * @brief 恢复经过结构和语义元数据核对的未实例化定义。
 * @param manifest 已验证模块身份的库清单。
 * @param bytes 格式一的规范 JSON 模板载荷。
 * @return 可拼接到同模块接口后的模板文本；不包含模块头或普通运行期实现。
 */
std::string templateSource(const Manifest &manifest, std::string_view bytes)
{
    if (bytes.empty() || bytes.size() > 32u * 1024u * 1024u) throw std::runtime_error("泛型库元数据为空或超过预算");
    const auto parsed = json::parse(bytes);
    if (!parsed.ok()) throw std::runtime_error("泛型库元数据不是有效 JSON");
    const auto &root = *parsed.value;
    templateKeys(root, { "format", "module", "imports", "macros", "definitions" });
    const auto &version = templateField(root, "format");
    if (!version.isInt() || version.asInt().value_or(0) != 1) throw std::runtime_error("泛型库元数据版本不兼容");
    if (templateText(templateField(root, "module")) != manifest.module) throw std::runtime_error("泛型库元数据模块身份不匹配");
    const auto definitions = templateField(root, "definitions").asArray();
    const auto macros = templateField(root, "macros").asArray();
    const auto imports = templateField(root, "imports").asArray();
    if (!definitions || !macros || !imports || imports->size() > 1024 || macros->size() > 4096
        || (definitions->empty() && macros->empty()) || definitions->size() > 4096) throw std::runtime_error("泛型库定义数量非法");
    if (root.dump() + "\n" != bytes) throw std::runtime_error("泛型库元数据不是规范编码");
    std::set<std::string> names;
    std::string source;
    std::set<std::string> importedNames;
    for (const auto &entry : *imports)
    {
        const auto imported = templateText(entry);
        if (!chali_package::moduleName(Memory::String(imported.c_str())) || imported == manifest.module || !importedNames.insert(imported).second)
            throw std::runtime_error("泛型库导入上下文非法");
        if (imported.rfind("std::", 0) != 0 && std::none_of(manifest.dependencies.begin(), manifest.dependencies.end(),
                [&](const auto &dependency) { return dependency.module == imported; }))
            throw std::runtime_error("泛型库导入缺少精确库依赖：" + imported);
    }
    for (const auto &entry : *macros)
    {
        const auto macro = templateText(entry, 4u * 1024u * 1024u);
        if (templateHasSessionMarker(macro)) throw std::runtime_error("泛型库宏定义含会话标记或预处理指令");
        Memory::String probe(macro.c_str());
        chali_macro::ProtectedDefinitions hidden;
        std::string error;
        if (!hidden.hide(probe, error) || hidden.records().size() != 1)
            throw std::runtime_error("泛型库宏定义非法：" + error);
        const auto &record = hidden.records().front();
        const auto marker = probe.find(record.first);
        if (marker == Memory::String::npos) throw std::runtime_error("泛型库宏定义占位丢失");
        probe.erase(marker, record.first.size());
        if (probe.find_first_not_of(" \t\r\n") != Memory::String::npos)
            throw std::runtime_error("泛型库宏载荷含非宏实现");
        source += macro + '\n';
    }
    for (const auto &definition : *definitions)
    {
        templateKeys(definition, { "name", "parameters", "declaration", "derivation", "operatorSwitches" });
        const auto derivation = templateText(templateField(definition, "derivation"), 65536);
        const auto switches = templateText(templateField(definition, "operatorSwitches"), 65536);
        templatePrefix(derivation, true);
        templatePrefix(switches, false);
        const auto name = templateText(templateField(definition, "name"));
        const auto declaration = templateText(templateField(definition, "declaration"), 4u * 1024u * 1024u);
        const auto parameters = templateField(definition, "parameters").asArray();
        if (name.empty() || !names.insert(name).second || !parameters || parameters->empty() || parameters->size() > 256)
            throw std::runtime_error("泛型库名称或形参数量非法");
        if (templateHasSessionMarker(declaration)) throw std::runtime_error("泛型库定义仍含编译会话宏片段编号或预处理指令：" + name);
        std::string header = "__chali_template<";
        json::Array canonicalParameters;
        for (const auto &parameter : *parameters)
        {
            templateKeys(parameter, { "name", "constraint", "fallback", "hasDefault", "constant", "constantType" });
            const auto constant = templateField(parameter, "constant").asBool();
            const auto hasDefault = templateField(parameter, "hasDefault").asBool();
            if (!constant || !hasDefault) throw std::runtime_error("泛型库形参标志不是 bool");
            const auto parameterName = templateText(templateField(parameter, "name"));
            const auto constraint = templateText(templateField(parameter, "constraint"));
            const auto fallback = templateText(templateField(parameter, "fallback"));
            const auto constantType = templateText(templateField(parameter, "constantType"));
            if (header.back() != '<') header += ',';
            header += parameterName;
            if (*constant) header += ':' + constantType;
            else if (!constraint.empty()) header += ':' + constraint;
            if (*hasDefault) header += '=' + fallback;
            canonicalParameters.push_back(parameter);
        }
        const auto item = header + "> " + declaration + "\n";
        const Memory::String prefix(("module " + manifest.module + ";\n").c_str());
        Memory::String probe = prefix + Memory::String(item.c_str());
        Memory::Vector<preprocess::LineOrigin> origins;
        const auto lines = std::count(probe.begin(), probe.end(), '\n') + 1;
        for (std::size_t line = 0; line < static_cast<std::size_t>(lines); ++line)
            origins.push_back({ "<库泛型 " + Memory::String(manifest.module.c_str()) + ">", line + 1 });
        Memory::Map<Memory::String, chali_generic::Generic> restored;
        Memory::String error;
        if (!chali_generic::collect(probe, origins, restored, error) || restored.size() != 1
            || probe.substr(prefix.size()).find_first_not_of(" \t\r\n") != Memory::String::npos)
            throw std::runtime_error("泛型库定义包含非模板实现或非法声明：" + name + " " + error.c_str());
        const auto &actual = restored.begin()->second;
        json::Array actualParameters;
        for (const auto &parameter : actual.params) actualParameters.push_back(templateParameter(parameter));
        if (actual.sourceName != name.c_str() || actual.module != manifest.module.c_str()
            || json::Value(std::move(actualParameters)).dump() != json::Value(std::move(canonicalParameters)).dump())
            throw std::runtime_error("泛型库声明与形参元数据不一致：" + name);
        const auto nameEnd = actual.nameOffset + name.size();
        const auto parametersText = header.substr(std::string_view("__chali_template").size()) + ">";
        const auto restoredSource = switches + "\n" + derivation + "\n" + declaration.substr(0, nameEnd) + parametersText + declaration.substr(nameEnd) + "\n";
        if (source.size() + restoredSource.size() > 32u * 1024u * 1024u) throw std::runtime_error("泛型库恢复源码超过预算");
        source += restoredSource;
    }
    return source;
}

/**
 * @brief 按原模块名称导出当前语义会话的未实例化泛型定义。
 * @param module 当前生产模块。
 * @return 规范模板载荷；没有模板时为空，不保存原始实现文件路径。
 */
std::string exportTemplates(const std::string &module)
{
    std::vector<const chali_generic::Generic *> records;
    for (const auto &entry : chali_generic::session.generics)
        if (entry.second.module == module.c_str() && !entry.second.intrinsic) records.push_back(&entry.second);
    json::Array macros;
    for (const auto &entry : chali_generic::session.libraryMacros)
        if (entry.first == module.c_str()) macros.push_back(templateRestore(entry.second, entry.first + " 宏定义"));
    if (records.empty() && macros.empty()) return {};
    std::sort(records.begin(), records.end(), [](const auto *left, const auto *right) { return left->sourceName < right->sourceName; });
    json::Array definitions;
    for (const auto *record : records)
    {
        json::Object item;
        item.insert("name", record->sourceName.c_str());
        json::Array parameters;
        for (const auto &parameter : record->params) parameters.push_back(templateParameter(parameter));
        item.insert("parameters", json::Value(std::move(parameters)));
        item.insert("declaration", templateRestore(record->declaration, record->sourceName));
        item.insert("derivation", templateRestore(record->derivation, record->sourceName));
        item.insert("operatorSwitches", templateRestore(record->operatorSwitches, record->sourceName));
        definitions.push_back(json::Value(std::move(item)));
    }
    json::Object root;
    root.insert("format", 1);
    root.insert("module", module);
    json::Array imports;
    for (const auto &entry : chali_generic::session.libraryImports) imports.push_back(entry.c_str());
    root.insert("imports", json::Value(std::move(imports)));
    root.insert("macros", json::Value(std::move(macros)));
    root.insert("definitions", json::Value(std::move(definitions)));
    const auto bytes = json::Value(std::move(root)).dump() + "\n";
    // 依赖清单由发布入口提供；这里仅核对载荷编码，导入身份在发布前统一检查。
    return bytes;
}
}  // namespace chali_library
