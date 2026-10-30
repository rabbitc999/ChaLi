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
    const auto count = M_NodeCount();
    std::ostringstream output(std::ios::binary);
    assert(M_WriteStandardUnit(output, count));
    const auto bytes = output.str();
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
    node.formattedOutputBridge = false;
    node.bindingIdentity = 0;
    std::istringstream input(bytes, std::ios::binary);
    assert(M_ReadStandardUnit(input));
    const auto &restored = M_ParsedNode(id);
    assert(restored.formattedOutputBridge && restored.moduleLocal);
    assert(restored.unsafeRequired && restored.unsafeScope && restored.implicitExitMove);
    assert(restored.syntaxContext == 123 && restored.bindingIdentity == 456 && restored.bindingContext == 789);
    assert(restored.i == 37 && restored.s2 == "macro-category" && restored.kids[0] == id);
    std::ostringstream package(std::ios::binary);
    const Memory::String text = "standard core identity";
    assert(chali_precompiled::write(package, text, count));
    const auto packed = package.str();
    const auto checksum = chali_incremental::hashBytes(Memory::String(packed.data(), packed.size()));
    Memory::String error;
    assert(chali_precompiled::loadBytes(reinterpret_cast<const unsigned char *>(packed.data()), packed.size(), checksum, text, error));
    assert(error.empty());
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
