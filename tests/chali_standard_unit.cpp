#include "chali_ast.hpp"
#include "chali_precompiled.hpp"
#include "chali_incremental.hpp"
#include "chali_module.hpp"
#include "chali_lifetime_contract.hpp"
#include <cassert>
#include <sstream>

/**
 * @brief 核对头部语句边界、注释字面量隔离及工程抽取的原列占位。
 * @return 全部断言通过时正常返回。
 */
void checkModuleHeaders()
{
    preprocess::Result source;
    source.text = "module demo::main; import demo /* gap */ :: lib; import std::memory;void main(){String text=\"/* import fake; */\";}";
    chali_module::UnitInfo info;
    Memory::String error;
    assert(chali_module::inspect(source, info, error));
    assert(info.hasModule && info.name == "demo::main");
    assert(info.imports == Memory::Vector<Memory::String>({ "demo::lib", "std::memory" }));
    assert(info.headers.size() == 1 && info.headers[0].index == 0);
    const auto suffix = source.text.find("void main");
    assert(info.headers[0].rest.size() == source.text.size());
    assert(info.headers[0].rest.substr(suffix) == source.text.substr(suffix));
    assert(info.headers[0].rest.find("/* gap */") != Memory::String::npos);
    assert(info.headers[0].text.find("void main") == Memory::String::npos);
    source.text = "/* lead\nimport ghost;\n*/ module\n demo /* gap */ :: main;\nimport\n demo::one; import demo::two;\nvoid main(){}";
    info = {};
    assert(chali_module::inspect(source, info, error));
    assert(info.name == "demo::main" && info.headers.size() == 4);
    assert(info.imports == Memory::Vector<Memory::String>({ "demo::one", "demo::two" }));
    source.text = R"CHALI(module demo::main;
String text="/*";
String fake="
import ghost;
module hidden;
";
void main(){println(f"{str("import missing; //")}");char quote='\'';}
)CHALI";
    info = {};
    assert(chali_module::inspect(source, info, error));
    assert(info.name == "demo::main" && info.imports.empty());
    source.text = "im/* gap */port ghost;";
    info = {};
    assert(chali_module::inspect(source, info, error) && info.imports.empty());
    for (const auto *invalid : { "int64 value=0;import ghost;", "void main(){} module late;", "module first;module second;" })
    {
        source.text = invalid;
        info = {};
        error.clear();
        assert(!chali_module::inspect(source, info, error) && !error.empty());
    }
}

/**
 * @brief 核对节点二进制往返、新语义标记和失败装载的原子性。
 * @return 全部断言通过返回零。
 */
int main()
{
    checkModuleHeaders();
    preprocess::Result core;
    core.text = "module std::core;";
    core.lineMap.push_back({ "core", 1 });
    chali_module::markStandardCore(core);
    assert(chali_module::findStandardCoreBoundary(core.text) == Memory::String("module std::core;\n").size());
    assert(core.lineMap.size() == 2);
    const int id = M_New(MNode::Int, "snapshot", 37);
    auto &node = const_cast<MNode &>(M_ParsedNode(id));
    node.syntaxContext = 123;
    node.controlContext = 321;
    node.controlTarget = id;
    node.metadataLine = 7;
    node.metadataColumn = 13;
    node.bindingIdentity = 456;
    node.bindingContext = 789;
    node.formattedOutputBridge = true;
    node.moduleLocal = true;
    node.unsafeRequired = true;
    node.unsafeScope = true;
    node.implicitExitMove = true;
    node.s2 = "macro-category";
    node.module = "std::core";
    node.kids = { id };
    const int integerType = M_TypeInt();
    M_TypeKind(MType::Ptr, "unit-pointer-marker");
    M_TypePtr(integerType);
    M_LifetimeContract(id, id);
    const auto members = M_List();
    M_ListAdd(members, M_AssociatedDecl(M_Name("Item")));
    M_ListAdd(members, M_Field(M_TypePtr(M_TypeStruct(M_Name("Item"))), M_Name("value")));
    M_Struct(M_Name("Owner"), members, -1);
    const auto count = M_NodeCount();
    std::ostringstream output(std::ios::binary);
    assert(M_WriteStandardUnit(output, count));
    const auto bytes = output.str();
    assert(bytes.find("Owner::Item") != bytes.npos);
    const auto reject = [&](const std::string &data) {
        std::istringstream broken(data, std::ios::binary);
        assert(!M_ReadStandardUnit(broken));
        assert(M_NodeCount() == count && M_ParsedNode(id).bindingIdentity == 456);
    };
    reject(bytes + "trailing data");
    auto largeCount = bytes;
    largeCount.replace(16, 8, 8, static_cast<char>(0xff));
    reject(largeCount);
    auto invalidContract = bytes;
    invalidContract.replace(invalidContract.size() - 8, 8, 8, static_cast<char>(0xff));
    reject(invalidContract);
    const auto readNumber = [&](std::size_t offset) {
        std::uint64_t value = 0;
        for (unsigned i = 0; i < 8; ++i) value |= std::uint64_t(static_cast<unsigned char>(bytes[offset + i])) << (8 * i);
        return value;
    };
    auto at = bytes.find("unit-pointer-marker");
    assert(at != bytes.npos);
    at += std::string("unit-pointer-marker").size();
    at += 8 + static_cast<std::size_t>(readNumber(at)); // lookupModule
    const auto unresolvedOffset = at;
    at += 8;
    at += 8 + static_cast<std::size_t>(readNumber(at)); // genericBase
    at += 8 + static_cast<std::size_t>(readNumber(at)); // genericMember
    const auto bindingOffset = at;
    at += 24; // macroBinding, bits, isUnsigned
    at += 8 + static_cast<std::size_t>(readNumber(at)) * 8; // dims
    at += 24; // readOnly, variadic, generic argument count
    const auto elementOffset = at;
    for (const auto offset : { unresolvedOffset, bindingOffset, elementOffset })
    {
        auto invalid = bytes;
        invalid.replace(offset, 8, 8, static_cast<char>(0xff));
        reject(invalid);
    }
    std::ostringstream provenance(std::ios::binary);
    chali_incremental::writeNumber(provenance, 123);
    chali_incremental::writeNumber(provenance, 321);
    chali_incremental::writeNumber(provenance, 456);
    const auto provenanceOffset = bytes.find(provenance.str());
    assert(provenanceOffset != bytes.npos);
    std::ostringstream metadata(std::ios::binary);
    chali_incremental::writeNumber(metadata, 7);
    chali_incremental::writeNumber(metadata, 13);
    const auto metadataOffset = bytes.find(metadata.str());
    assert(metadataOffset != bytes.npos);
    auto invalidMetadata = bytes;
    invalidMetadata.replace(metadataOffset, 8, 8, static_cast<char>(0xff));
    std::istringstream invalidMetadataInput(invalidMetadata, std::ios::binary);
    assert(!M_ReadStandardUnit(invalidMetadataInput));
    assert(M_NodeCount() == count && M_ParsedNode(id).metadataLine == 7);
    invalidMetadata = bytes;
    invalidMetadata.replace(metadataOffset + 8, 8, 8, '\0');
    std::istringstream incompleteMetadataInput(invalidMetadata, std::ios::binary);
    assert(!M_ReadStandardUnit(incompleteMetadataInput));
    assert(M_NodeCount() == count && M_ParsedNode(id).metadataColumn == 13);
    auto invalidContext = bytes;
    invalidContext.replace(provenanceOffset + 8, 8, 8, static_cast<char>(0xff));
    std::istringstream invalidProvenance(invalidContext, std::ios::binary);
    assert(!M_ReadStandardUnit(invalidProvenance));
    assert(M_NodeCount() == count && M_ParsedNode(id).controlContext == 321);
    for (const auto size : { std::size_t(0), std::size_t(8), bytes.size() - 1 })
    {
        std::istringstream truncated(bytes.substr(0, size), std::ios::binary);
        assert(!M_ReadStandardUnit(truncated));
        assert(M_NodeCount() == count && M_ParsedNode(id).bindingIdentity == 456);
    }
    auto incompatible = bytes;
    incompatible[8] = static_cast<char>(0xff);
    std::istringstream old(incompatible, std::ios::binary);
    assert(!M_ReadStandardUnit(old));
    assert(M_NodeCount() == count);
    incompatible[8] = 6;
    std::istringstream previousFormat(incompatible, std::ios::binary);
    assert(!M_ReadStandardUnit(previousFormat));
    assert(M_NodeCount() == count);
    node.formattedOutputBridge = false;
    node.bindingIdentity = 0;
    std::istringstream input(bytes, std::ios::binary);
    assert(M_ReadStandardUnit(input));
    assert(chali_lifetime::g_directiveNodes.at(id) == id);
    std::ostringstream roundtrip(std::ios::binary);
    assert(M_WriteStandardUnit(roundtrip, count) && roundtrip.str() == bytes);
    const auto &restored = M_ParsedNode(id);
    assert(restored.formattedOutputBridge && restored.moduleLocal);
    assert(restored.unsafeRequired && restored.unsafeScope && restored.implicitExitMove);
    assert(restored.syntaxContext == 123 && restored.bindingIdentity == 456 && restored.bindingContext == 789);
    assert(restored.controlContext == 321 && restored.controlTarget == -1);
    assert(restored.metadataLine == 7 && restored.metadataColumn == 13);
    assert(restored.i == 37 && restored.s2 == "macro-category" && restored.kids[0] == id);
    std::ostringstream package(std::ios::binary);
    const Memory::String text = "standard core identity";
    assert(chali_precompiled::write(package, text, count));
    const auto packed = package.str();
    const auto checksum = chali_incremental::hashBytes(Memory::String(packed.data(), packed.size()));
    Memory::String error;
    assert(chali_precompiled::loadBytes(reinterpret_cast<const unsigned char *>(packed.data()), packed.size(), checksum, text, error));
    assert(error.empty());
    auto compatibleProducer = packed;
    compatibleProducer[56] ^= 1;
    const auto producerChecksum = chali_incremental::hashBytes(Memory::String(compatibleProducer.data(), compatibleProducer.size()));
    assert(chali_precompiled::loadBytes(reinterpret_cast<const unsigned char *>(compatibleProducer.data()), compatibleProducer.size(), producerChecksum, text, error));
    assert(error.empty());
    for (const auto offset : { std::size_t(8), std::size_t(16), std::size_t(40), std::size_t(56) })
    {
        auto damaged = packed;
        damaged[offset] ^= offset == 56 ? 0x80 : 1;
        const auto updated = chali_incremental::hashBytes(Memory::String(damaged.data(), damaged.size()));
        assert(!chali_precompiled::loadBytes(reinterpret_cast<const unsigned char *>(damaged.data()), damaged.size(), updated, text, error));
        assert(!error.empty() && M_NodeCount() == count);
    }
    for (const auto offset : { std::size_t(0), std::size_t(8), packed.size() - 1 })
    {
        auto damaged = packed;
        damaged[offset] ^= 1;
        assert(!chali_precompiled::loadBytes(reinterpret_cast<const unsigned char *>(damaged.data()), damaged.size(), checksum, text, error));
        assert(!error.empty() && M_NodeCount() == count);
    }
    assert(!chali_precompiled::loadBytes(reinterpret_cast<const unsigned char *>(packed.data()), packed.size(), checksum, "other core", error));
    assert(error.empty() && M_NodeCount() == count);
    Memory::Vector<Memory::String> moduleFiles(64, "<内建 std::test>");
    Memory::Vector<std::size_t> moduleLines;
    for (std::size_t i = 0; i < moduleFiles.size(); ++i) moduleLines.push_back(i + 1);
    M_SetSourceFiles(moduleFiles, "/temporary/user-build-path.chl");
    M_SetSourceLines(moduleLines);
    std::ostringstream moduleOutput(std::ios::binary);
    assert(M_WriteModuleUnit(moduleOutput));
    const auto moduleBytes = moduleOutput.str();
    assert(moduleBytes.find("user-build-path") == moduleBytes.npos);
    moduleFiles.insert(moduleFiles.begin(), 3, "user.chl");
    moduleLines.insert(moduleLines.begin(), { 1, 2, 3 });
    M_SetSourceFiles(moduleFiles, "user.chl");
    M_SetSourceLines(moduleLines);
    for (const auto size : { std::size_t(0), moduleBytes.size() - 1 })
    {
        std::istringstream damaged(moduleBytes.substr(0, size), std::ios::binary);
        assert(!M_AppendModuleUnit(damaged));
        assert(M_NodeCount() == count);
    }
    std::istringstream moduleInput(moduleBytes, std::ios::binary);
    assert(M_AppendModuleUnit(moduleInput));
    assert(M_NodeCount() == count * 2);
    assert(M_ParsedNode(static_cast<int>(count) + id).kids[0] == static_cast<int>(count) + id);
    assert(M_ParsedNode(static_cast<int>(count) + id).metadataLine == 10);
    assert(chali_lifetime::g_directiveNodes.at(static_cast<int>(count) + id) == static_cast<int>(count) + id);
    const Memory::String templateSource = "module std::unit;\nstruct Demo<T=int64>{T value;};\n";
    const Memory::Vector<preprocess::LineOrigin> templateOrigins = {
        { "<内建 std::unit>", 1 }, { "<内建 std::unit>", 2 }, { "<内建 std::unit>", 3 }
    };
    chali_generic::session = chali_generic::Session{};
    auto templateInput = templateSource;
    chali_generic::normalizeGenericSyntax(templateInput);
    assert(chali_generic::collect(templateInput, templateOrigins, chali_generic::session.generics, error));
    std::ostringstream templates(std::ios::binary);
    assert(chali_generic::writeTemplateUnit(templates, "std::unit"));
    const auto templateBytes = templates.str();
    Memory::Vector<chali_generic::Generic> records;
    std::istringstream templateStream(templateBytes, std::ios::binary);
    assert(chali_generic::readTemplateUnit(templateStream, "std::unit", records));
    assert(records.size() == 1 && records[0].sourceName == "Demo" && records[0].params[0].fallback == "int64");
    for (const auto &invalid : { templateBytes.substr(0, templateBytes.size() - 1), templateBytes + "trailing" })
    {
        std::istringstream input(invalid, std::ios::binary);
        assert(!chali_generic::readTemplateUnit(input, "std::unit", records));
        assert(records.size() == 1 && records[0].sourceName == "Demo");
    }
    static chali_generic::Generic cachedTemplate;
    cachedTemplate = records[0];
    chali_generic::setTemplateLoader(+[](const Memory::String &module, const preprocess::LineOrigin &, const Memory::String &source,
                                        std::size_t begin, std::size_t &end, chali_generic::Generic &item, Memory::String &) -> int {
        if (module != "std::unit" || source.compare(begin, cachedTemplate.sourceTemplate.size(), cachedTemplate.sourceTemplate) != 0) return 0;
        item = cachedTemplate;
        end = begin + cachedTemplate.sourceTemplate.size();
        return 1;
    });
    Memory::Map<Memory::String, chali_generic::Generic> current;
    auto caller = cachedTemplate;
    caller.module = "user";
    current.emplace("Demo", caller);
    templateInput = templateSource;
    chali_generic::normalizeGenericSyntax(templateInput);
    assert(chali_generic::collect(templateInput, templateOrigins, current, error));
    assert(current.at("Demo").module == "user" && current.at("__module_std__unit_Demo").module == "std::unit");
    assert(chali_generic::session.templateHits == 1);
    chali_generic::setTemplateLoader(nullptr);
    return 0;
}
