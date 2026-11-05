/** @file chali_cas_node_structure.cpp @brief 确定性表达式 DAG 的结构相等、哈希与排序差分驱动。 */
#include "cas/detail/node.h"
#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <string>

/**
 * @brief 产生不依赖平台分布实现的固定伪随机数。
 * @param state 输入并更新的状态。
 * @return 新状态。
 */
static std::uint64_t next(std::uint64_t &state)
{
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}
/**
 * @brief 输出 600 个节点的哈希以及 6000 组比较，供旧 C++ 与 C 内核逐字节对照。
 * @return 所有节点与反对称断言通过返回零。
 */
int main()
{
    using namespace Cas::Detail;
    Cas::Memory::Vector<Ptr> pool;
    pool.reserve(600);
    std::uint64_t state = UINT64_C(0xb7e151628aed2a6b);
    for (int i = 0; i < 600; ++i)
    {
        const auto random = next(state);
        const auto kind = i < 32 ? static_cast<unsigned>(i % 4) : static_cast<unsigned>(random % 8);
        NodePtr node = nullptr;
        if (kind == 0)
            node = Node::makeInteger(BigInt(static_cast<long long>(random % 1000000001) - 500000000));
        else if (kind == 1)
            node = Node::makeRational(BigInt(static_cast<long long>(random % 1000001) - 500000), BigInt(static_cast<long long>((random >> 32) % 127 + 1)));
        else if (kind == 2)
            node = Node::makeSymbol("symbol_" + std::to_string(random % 23));
        else if (kind == 3)
            node = Node::makeConstant(static_cast<ConstantId>(random % 5));
        else
        {
            Cas::Memory::Vector<NodePtr> operands;
            for (unsigned j = 0; j < (kind == 5 ? 2 : (random >> 8) % 4); ++j)
                operands.push_back(pool[next(state) % pool.size()].get());
            if (kind == 4)
                node = Node::makeNary(NodeKind::Add, operands);
            else if (kind == 5)
                node = Node::makePower(operands[0], operands[1]);
            else if (kind == 6)
                node = Node::makeFunction("function_" + std::to_string(random % 11), operands);
            else
                node = Node::makeNary(NodeKind::Mul, operands);
        }
        assert(node);
        pool.emplace_back(node);
    }
    for (std::size_t i = 0; i < pool.size(); ++i)
    {
        assert(pool[i]->equals(pool[i].get()) && pool[i]->compareTo(pool[i].get()) == 0);
        const auto hash = pool[i]->hash();
        assert(hash == pool[i]->hash());
        std::printf("H %zu %" PRIu64 "\n", i, hash);
    }
    for (int sample = 0; sample < 6000; ++sample)
    {
        const auto left = next(state) % pool.size(), right = next(state) % pool.size();
        const auto equal = pool[left]->equals(pool[right].get());
        const auto comparison = pool[left]->compareTo(pool[right].get());
        assert(comparison == -pool[right]->compareTo(pool[left].get()));
        if (equal) assert(pool[left]->hash() == pool[right]->hash());
        std::printf("C %" PRIu64 " %" PRIu64 " %d %d\n", left, right, equal ? 1 : 0, comparison);
    }
}
