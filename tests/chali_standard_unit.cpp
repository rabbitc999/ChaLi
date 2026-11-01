#include "chali_ast.hpp"
#include "chali_precompiled.hpp"
#include "chali_incremental.hpp"
#include "chali_module.hpp"
#include <cassert>
#include <sstream>

/**
 * @brief 核对节点二进制往返、新语义标记和失败装载的原子性。
 * @return 全部断言通过返回零。
 */
int main()
{
    preprocess::Result core;
    core.text = "module std::lang;";
    core.lineMap.push_back({ "core", 1 });
    chali_module::markStandardCore(core);
    assert(chali_module::findStandardCoreBoundary(core.text) == Memory::String("module std::lang;\n").size());
    assert(core.lineMap.size() == 2);
    const int id = M_New(MNode::Int, "snapshot", 37);
    auto &node = const_cast<MNode &>(M_ParsedNode(id));
    node.syntaxContext = 123;
    node.controlContext = 321;
    node.controlTarget = id;
    node.metadataLine = 7;
    node.metadataColumn = 13;
    node.deriveLine = 9;
    node.deriveColumn = 3;
    node.bindingIdentity = 456;
    node.bindingContext = 789;
    node.formattedOutputBridge = true;
    node.moduleLocal = true;
    node.unsafeRequired = true;
    node.unsafeScope = true;
    node.implicitExitMove = true;
    node.s2 = "macro-category";
    node.module = "std::lang";
    node.kids = { id };
    const int integerType = M_TypeInt();
    M_TypeKind(MType::Ptr, "unit-pointer-marker");
    M_TypePtr(integerType);
    const auto count = M_NodeCount();
    std::ostringstream output(std::ios::binary);
    assert(M_WriteStandardUnit(output, count));
    const auto bytes = output.str();
    const auto reject = [&](const std::string &data) {
        std::istringstream broken(data, std::ios::binary);
        assert(!M_ReadStandardUnit(broken));
        assert(M_NodeCount() == count && M_ParsedNode(id).bindingIdentity == 456);
    };
    reject(bytes + "trailing data");
    auto largeCount = bytes;
    largeCount.replace(16, 8, 8, static_cast<char>(0xff));
    reject(largeCount);
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
    std::ostringstream derivePosition(std::ios::binary);
    chali_incremental::writeNumber(derivePosition, 9);
    chali_incremental::writeNumber(derivePosition, 3);
    const auto deriveOffset = bytes.find(derivePosition.str());
    assert(deriveOffset != bytes.npos);
    for (const auto malformed : { false, true })
    {
        auto invalid = bytes;
        invalid.replace(deriveOffset + (malformed ? 0 : 8), 8, 8, malformed ? static_cast<char>(0xff) : '\0');
        std::istringstream brokenDerive(invalid, std::ios::binary);
        assert(!M_ReadStandardUnit(brokenDerive));
        assert(M_NodeCount() == count && M_ParsedNode(id).deriveLine == 9 && M_ParsedNode(id).deriveColumn == 3);
    }
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
    incompatible[8] = 5;
    std::istringstream previousFormat(incompatible, std::ios::binary);
    assert(!M_ReadStandardUnit(previousFormat));
    assert(M_NodeCount() == count);
    node.formattedOutputBridge = false;
    node.bindingIdentity = 0;
    std::istringstream input(bytes, std::ios::binary);
    assert(M_ReadStandardUnit(input));
    std::ostringstream roundtrip(std::ios::binary);
    assert(M_WriteStandardUnit(roundtrip, count) && roundtrip.str() == bytes);
    const auto &restored = M_ParsedNode(id);
    assert(restored.formattedOutputBridge && restored.moduleLocal);
    assert(restored.unsafeRequired && restored.unsafeScope && restored.implicitExitMove);
    assert(restored.syntaxContext == 123 && restored.bindingIdentity == 456 && restored.bindingContext == 789);
    assert(restored.controlContext == 321 && restored.controlTarget == -1);
    assert(restored.metadataLine == 7 && restored.metadataColumn == 13);
    assert(restored.deriveLine == 9 && restored.deriveColumn == 3);
    assert(restored.i == 37 && restored.s2 == "macro-category" && restored.kids[0] == id);
    std::ostringstream package(std::ios::binary);
    const Memory::String text = "standard core identity";
    assert(chali_precompiled::write(package, text, count));
    const auto packed = package.str();
    const auto checksum = chali_incremental::hashBytes(Memory::String(packed.data(), packed.size()));
    Memory::String error;
    assert(chali_precompiled::loadBytes(reinterpret_cast<const unsigned char *>(packed.data()), packed.size(), checksum, text, error));
    assert(error.empty());
    for (const auto offset : { std::size_t(8), std::size_t(16), std::size_t(40), std::size_t(56) })
    {
        auto damaged = packed;
        damaged[offset] ^= 1;
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
    return 0;
}
