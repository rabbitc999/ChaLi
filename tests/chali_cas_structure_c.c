/** @file chali_cas_structure_c.c @brief 纯 C 节点视图与结构比较失败原子性检查。 */
#include "cas/detail/node_structure.h"
#include "../source/bignum/chali_bigint.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief 独立于 C++ Node 布局的测试节点。 */
typedef struct TestNode
{
    CasNodeView view;
    ChaliBigInt numerator, denominator;
    const struct TestNode *children[3];
    uint64_t cache;
} TestNode;
/**
 * @brief 借用 C 测试节点字段。
 * @param pointer 节点。
 * @param result 只读视图。
 */
static void view(const void *pointer, CasNodeView *result) { *result = ((const TestNode *)pointer)->view; }
/**
 * @brief 读取 C 测试节点的子节点。
 * @param pointer 节点。
 * @param index 下标。
 * @return 子节点。
 */
static const void *operand(const void *pointer, size_t index) { return ((const TestNode *)pointer)->children[index]; }
/**
 * @brief 借用测试缓存。
 * @param pointer 节点。
 * @return 缓存地址。
 */
static uint64_t *cache(const void *pointer) { return &((TestNode *)pointer)->cache; }
/**
 * @brief 初始化独立节点及其 C 数值。
 * @param node 节点。
 * @param kind 种类。
 * @param numerator 分子。
 * @param denominator 分母。
 */
static void initialize(TestNode *node, unsigned kind, int64_t numerator, int64_t denominator)
{
    memset(node, 0, sizeof(*node));
    assert(chali_bigint_init(&node->numerator, NULL) == CHALI_C_OK);
    assert(chali_bigint_init(&node->denominator, NULL) == CHALI_C_OK);
    assert(chali_bigint_set_i64(&node->numerator, numerator) == CHALI_C_OK);
    assert(chali_bigint_set_i64(&node->denominator, denominator) == CHALI_C_OK);
    node->view.kind = kind;
    node->view.numerator = &node->numerator;
    node->view.denominator = &node->denominator;
}
/** @brief 临时乘积的独立所有者。 */
typedef struct Owner
{
    size_t calls, live;
    long failure;
} Owner;
/**
 * @brief 在指定分配点拒绝临时存储。
 * @param context 所有者。
 * @param bytes 字节数。
 * @return 独占存储或空。
 */
static void *allocate(void *context, size_t bytes)
{
    Owner *owner = (Owner *)context;
    if ((long)owner->calls++ == owner->failure) return NULL;
    void *memory = malloc(bytes);
    assert(memory);
    ++owner->live;
    return memory;
}
/**
 * @brief 归还临时乘积存储。
 * @param context 所有者。
 * @param memory 地址。
 * @param bytes 原字节数。
 */
static void release(void *context, void *memory, size_t bytes)
{
    (void)bytes;
    Owner *owner = (Owner *)context;
    assert(owner->live);
    --owner->live;
    free(memory);
}
/**
 * @brief 验证独立 C 视图、结构种类、名称排序、缓存及所有分配失败点。
 * @return 断言通过返回零。
 */
int main(void)
{
    const CasNodeAccess access = { view, operand, cache };
    TestNode left, right, left_copy, symbol, addition;
    initialize(&left, CAS_NODE_RATIONAL, 17, 3);
    initialize(&right, CAS_NODE_RATIONAL, 22, 7);
    initialize(&left_copy, CAS_NODE_RATIONAL, 17, 3);
    initialize(&symbol, CAS_NODE_SYMBOL, 0, 1);
    symbol.view.name = "alpha";
    symbol.view.symbol = 1;
    initialize(&addition, CAS_NODE_ADD, 0, 1);
    addition.children[0] = &left;
    addition.children[1] = &symbol;
    addition.view.count = 2;
    assert(cas_node_equals(&left, &left_copy, &access));
    assert(!cas_node_equals(&left, &right, &access));
    assert(cas_node_hash(&left, &access) == cas_node_hash(&left_copy, &access));
    uint64_t hash = cas_node_hash(&addition, &access);
    assert(hash && addition.cache == hash && cas_node_hash(&addition, &access) == hash);
    Owner owner = { 0, 0, -1 };
    ChaliCAllocator allocator = { &owner, allocate, release };
    int order = 123;
    assert(cas_node_compare(&left, &right, &access, &allocator, &order) == CHALI_C_OK && order == 1);
    assert(owner.live == 0);
    const size_t failures = owner.calls;
    for (size_t failure = 0; failure < failures; ++failure)
    {
        owner = (Owner){ 0, 0, (long)failure };
        order = 123;
        assert(cas_node_compare(&left, &right, &access, &allocator, &order) == CHALI_C_OUT_OF_MEMORY);
        assert(order == 123 && owner.live == 0);
        assert(cas_node_hash(&addition, &access) == hash);
    }
    assert(cas_node_compare(&left, &symbol, &access, NULL, &order) == CHALI_C_OK && order == -1);
    assert(cas_node_compare(&addition, &symbol, &access, NULL, &order) == CHALI_C_OK && order == 1);
    TestNode *nodes[] = { &left, &right, &left_copy, &symbol, &addition };
    for (size_t i = 0; i < sizeof(nodes) / sizeof(*nodes); ++i)
    {
        chali_bigint_clear(&nodes[i]->numerator);
        chali_bigint_clear(&nodes[i]->denominator);
    }
    printf("C CAS structure: independent C node layout, cached hash and %zu transactional allocation failures passed\n", failures);
}
