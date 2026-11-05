// SPDX-License-Identifier: MIT
// Copyright (c) 2026 小兔糖糖_OvO

#include "cas/detail/node.h"
#include "cas/allocator.h"
#include "cas/detail/symbol_table.h"
#include "cas/detail/node_structure.h"
#include <stdexcept>

#include <algorithm>
#include <cstring>
#include <new>
#include <string>

namespace Cas::Detail {
/** @brief 无分配借用既有 C++ 节点字段，算法在独立 C 单元中执行。 */
struct CNodeAccess {
    /**
     * @brief 借用不可变节点字段及共享 C 数值视图。
     * @param pointer 节点。
     * @param result 接收视图。
     */
    static void view(const void *pointer, CasNodeView *result) noexcept {
        const auto *node = static_cast<const Node *>(pointer);
        *result = { static_cast<unsigned>(node->m_kind), node->m_symbolId,
                    static_cast<std::uint32_t>(node->m_constant), node->m_name,
                    &node->m_numerator.backend().cValue(), &node->m_denominator.backend().cValue(),
                    node->m_operands.size() };
    }
    /**
     * @brief 借用已验证下标的子节点。
     * @param pointer 父节点。
     * @param index 子节点下标。
     * @return 子节点。
     */
    static const void *operand(const void *pointer, std::size_t index) noexcept {
        return static_cast<const Node *>(pointer)->m_operands[index];
    }
    /**
     * @brief 借用原有可变哈希缓存。
     * @param pointer 节点。
     * @return 缓存地址。
     */
    static std::uint64_t *cache(const void *pointer) noexcept {
        return &static_cast<const Node *>(pointer)->m_hash;
    }
};
static_assert(static_cast<unsigned>(NodeKind::Integer) == CAS_NODE_INTEGER &&
              static_cast<unsigned>(NodeKind::Rational) == CAS_NODE_RATIONAL &&
              static_cast<unsigned>(NodeKind::Symbol) == CAS_NODE_SYMBOL &&
              static_cast<unsigned>(NodeKind::Add) == CAS_NODE_ADD &&
              static_cast<unsigned>(NodeKind::Mul) == CAS_NODE_MUL &&
              static_cast<unsigned>(NodeKind::Pow) == CAS_NODE_POW &&
              static_cast<unsigned>(NodeKind::Function) == CAS_NODE_FUNCTION &&
              static_cast<unsigned>(NodeKind::Constant) == CAS_NODE_CONSTANT);

namespace {
const ::CasNodeAccess nodeAccess{ CNodeAccess::view, CNodeAccess::operand, CNodeAccess::cache };

/* ------------------------------------------------------------- 节点内存池 */

/**
 * @brief 节点分配走 CAS 内存池；池未初始化时退化为普通堆分配。
 *
 * 退化路径刻意用 malloc/free 而不是 operator new/delete：
 * 分配与释放必须是同一对函数，混用 nothrow new 与普通 delete 会破坏堆。
 */
void *nodeAllocate(std::size_t bytes) {
    if (!Allocator::ready()) return std::malloc(bytes);
    return Allocator::allocate(bytes, alignof(std::max_align_t));
}

void nodeFree(void *pointer) {
    if (!pointer) return;
    if (!Allocator::release(pointer)) std::free(pointer);
}

/* --------------------------------------------------------------- 符号表 */

} // namespace

/* --------------------------------------------------------------- 构造 */

Node::~Node() {
    for (NodePtr child : m_operands) Node::release(child);
}

NodePtr Node::makeInteger(const BigInt &value) {
    void *raw = nodeAllocate(sizeof(Node));
    if (!raw) return nullptr;
    NodePtr node = new (raw) Node();
    node->m_kind = NodeKind::Integer;
    node->m_numerator = value;
    node->m_denominator = BigInt(1);
    return node;
}

NodePtr Node::makeRational(const BigInt &numerator, const BigInt &denominator) {
    if (denominator.isZero()) return nullptr;
    BigInt p = numerator, q = denominator;
    if (q.isNegative()) {
        p = p.negated();
        q = q.negated();
    }
    const BigInt divisor = BigInt::gcd(p.abs(), q);
    if (!divisor.isOne() && !divisor.isZero()) {
        p = p / divisor;
        q = q / divisor;
    }
    if (q.isOne()) return makeInteger(p);

    void *raw = nodeAllocate(sizeof(Node));
    if (!raw) return nullptr;
    NodePtr node = new (raw) Node();
    node->m_kind = NodeKind::Rational;
    node->m_numerator = p;
    node->m_denominator = q;
    return node;
}

NodePtr Node::makeSymbol(const std::string &name) { return makeSymbolById(internSymbol(name)); }

NodePtr Node::makeSymbolById(std::uint32_t id) {
    void *raw = nodeAllocate(sizeof(Node));
    if (!raw) return nullptr;
    NodePtr node = new (raw) Node();
    node->m_kind = NodeKind::Symbol;
    node->m_symbolId = id;
    node->m_name = symbolName(id);
    node->m_denominator = BigInt(1);
    return node;
}

NodePtr Node::makeNary(NodeKind kind, const Memory::Vector<NodePtr> &operands) {
    void *raw = nodeAllocate(sizeof(Node));
    if (!raw) return nullptr;
    NodePtr node = new (raw) Node();
    node->m_kind = kind;
    node->m_denominator = BigInt(1);
    node->m_operands = operands;
    for (NodePtr child : node->m_operands) Node::retain(child);
    return node;
}

NodePtr Node::makePower(NodePtr base, NodePtr exponent) {
    Memory::Vector<NodePtr> operands;
    operands.push_back(base);
    operands.push_back(exponent);
    return makeNary(NodeKind::Pow, operands);
}

NodePtr Node::makeFunction(const std::string &name, const Memory::Vector<NodePtr> &arguments) {
    void *raw = nodeAllocate(sizeof(Node));
    if (!raw) return nullptr;
    NodePtr node = new (raw) Node();
    node->m_kind = NodeKind::Function;
    node->m_symbolId = internSymbol(name);
    node->m_name = symbolName(node->m_symbolId);
    node->m_denominator = BigInt(1);
    node->m_operands = arguments;
    for (NodePtr child : node->m_operands) Node::retain(child);
    return node;
}

NodePtr Node::makeConstant(ConstantId id) {
    void *raw = nodeAllocate(sizeof(Node));
    if (!raw) return nullptr;
    NodePtr node = new (raw) Node();
    node->m_kind = NodeKind::Constant;
    node->m_constant = id;
    node->m_denominator = BigInt(1);
    return node;
}

/* ---------------------------------------------------------- 引用计数 */

void Node::retain(NodePtr node) {
    if (node) ++node->m_refs;
}

void Node::release(NodePtr node) {
    if (!node) return;
    if (--node->m_refs) return;
    node->~Node();
    nodeFree(node);
}

/* ------------------------------------------------------------- 查询 */

bool Node::isSmallInteger(long long *out) const {
    if (m_kind != NodeKind::Integer) return false;
    if (!m_numerator.fitsLongLong()) return false;
    if (out) *out = m_numerator.toLongLong();
    return true;
}

bool Node::isInteger() const {
    if (m_kind == NodeKind::Integer) return true;
    if (m_kind == NodeKind::Rational) return m_denominator.isOne();
    return false;
}

bool Node::isZero() const {
    if (m_kind != NodeKind::Integer) return false;
    return m_numerator.isZero();
}

bool Node::isOne() const {
    if (m_kind != NodeKind::Integer) return false;
    return m_numerator.isOne();
}

bool Node::isNegativeOne() const {
    if (m_kind != NodeKind::Integer) return false;
    return m_numerator == BigInt(-1);
}

bool Node::isNegative() const {
    if (!isNumber()) return false;
    return m_numerator.isNegative();
}

/* ------------------------------------------------------------ 比较 */

/**
 * @brief 通过 C 结构内核判断相等，保留不可变节点的身份快速路径。
 * @param other 另一节点。
 * @return 原结构相等协议的结果。
 */
bool Node::equals(const NodePtr &other) const {
    if (this == other) return true;
    return cas_node_equals(this, other, &nodeAccess) != 0;
}
/**
 * @brief 通过 C 结构内核生成哈希，沿用原缓存与混合顺序。
 * @return 非零结构哈希。
 */
std::uint64_t Node::hash() const {
    return m_hash ? m_hash : cas_node_hash(this, &nodeAccess);
}
/**
 * @brief 在原当前预算下调用 C 结构排序及精确有理数交叉乘积。
 * @param other 另一节点。
 * @return 负一、零或一；失败沿原异常边界报告。
 */
int Node::compareTo(const NodePtr &other) const {
    if (this == other) return 0;
    if (!other) return 1;
    BigInt allocationContext;
    const auto &allocator = allocationContext.backend().cValue().limbs.allocator;
    int order = 0;
    const int status = cas_node_compare(this, other, &nodeAccess, &allocator, &order);
    if (status == CHALI_C_OUT_OF_MEMORY) throw std::bad_alloc();
    if (status != CHALI_C_OK) throw std::length_error("CAS 结构比较数值超出范围");
    return order;
}

/* ------------------------------------------------------------ 符号表 */

/**
 * @brief 通过 C 服务驻留完整名称，保留既有编号与稳定地址。
 * @param name 名称字节。
 * @return 已有或新符号编号；分配失败沿原异常边界报告。
 */
std::uint32_t internSymbol(const std::string &name) {
    const auto id = cas_symbols_intern(cas_symbols_default(), name.data(), name.size());
    if (id == UINT32_MAX) throw std::bad_alloc();
    return id;
}
/**
 * @brief 借用 C 表内不可变名称。
 * @param id 符号编号。
 * @return 名称指针；无效编号返回空字符串。
 */
const char *symbolName(std::uint32_t id) { return cas_symbols_name(cas_symbols_default(), id, nullptr); }
/**
 * @brief 将当前编号范围复制为兼容接口的快照。
 * @return 按驻留顺序排列的编号。
 */
Memory::Vector<std::uint32_t> allSymbols() {
    const auto count = cas_symbols_count(cas_symbols_default());
    Memory::Vector<std::uint32_t> result;
    result.reserve(count);
    for (std::size_t i = 0; i < count; ++i) result.push_back(static_cast<std::uint32_t>(i));
    return result;
}
/**
 * @brief 在 C 驻留表中按完整字节名称查询。
 * @param name 名称。
 * @return 编号；不存在返回 UINT32_MAX。
 */
std::uint32_t lookupSymbol(const std::string &name) {
    return cas_symbols_lookup(cas_symbols_default(), name.data(), name.size());
}

} // namespace Cas::Detail
