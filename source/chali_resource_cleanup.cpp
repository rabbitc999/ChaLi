#include "chali_resource_cleanup.hpp"

namespace chali_resource_cleanup
{
/**
 * @brief 只合并来源唯一且与先前终结操作精确对应的资源。
 * @param graph 控制流中的释放与初始化操作。
 * @param states 分支合并后的可能失效与必定终结事实。
 * @param reachable 可达节点集合。
 * @return 确定重复的操作及需要沿实际路径守卫的操作。
 * @note 不把字段重叠或动态下标近似当作资源身份相同的证明。
 */
Memory::Vector<Merge> coalesce(const chali_borrow_checker::FlowGraph &graph,
                               const Memory::Vector<chali_borrow_checker::FlowState> &states,
                               const Memory::Vector<bool> &reachable)
{
    using namespace chali_borrow_checker;
    Memory::Vector<Merge> result;
    Memory::Map<int, std::size_t> occurrences;
    for (std::size_t i = 0; i < graph.nodes.size(); ++i)
        if (i < reachable.size() && reachable[i] && graph.nodes[i].operation >= 0
            && (graph.nodes[i].kind == FlowNode::Drop || graph.nodes[i].kind == FlowNode::Destroy))
            ++occurrences[graph.nodes[i].operation];
    for (std::size_t index = 0; index < graph.nodes.size(); ++index)
    {
        if (index >= states.size() || index >= reachable.size() || !reachable[index]) continue;
        const auto &node = graph.nodes[index];
        if (node.kind != FlowNode::Drop && node.kind != FlowNode::Destroy) continue;
        // 首轮与后续迭代共用一条源码释放语句；单个副本的冗余不能删掉整条语句。
        if (node.operation >= 0 && occurrences[node.operation] > 1u) continue;
        const auto &state = states[index];
        const auto targets = resolve(state, node.target);
        if (targets.size() != 1u) continue;
        const auto &target = *targets.begin();
        if (target.root.empty() || std::find(target.path.begin(), target.path.end(), "[*]") != target.path.end()) continue;
        const auto previous = state.invalid.find(target);
        if (previous == state.invalid.end() || previous->second == FlowState::Moved) continue;
        if (node.kind == FlowNode::Destroy && previous->second != FlowState::Freed) continue;
        const auto certain = state.terminated.find(target);
        const bool redundant = certain != state.terminated.end() && (node.kind == FlowNode::Drop || certain->second == FlowState::Freed);
        result.push_back({ node.operation, redundant });
    }
    return result;
}
}  // namespace chali_resource_cleanup
